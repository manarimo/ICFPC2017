from konohazuku.db import fetch_all_logs
from utils.const import ROOT_DIR
import pandas as pd
from pathlib import Path
import json
import random
from argparse import ArgumentParser
from sklearn.linear_model import LinearRegression
from sklearn.externals import joblib
from washimimizuku.eval_model import feature_vector, edge_key


PATH_LENGTH = 3
TO_SAMPLE = 10000


def get_punters_data(log):
    if "names" not in log:
        return {}
    handshake_names = log["names"]
    punter_count = len(handshake_names)
    tag_names = log.get("tag_names", [None] * punter_count)
    raw_scores = {score["punter"]: score["score"] for score in log["scores"]}
    punters = []
    for i in range(punter_count):
        punter = {
            "id": i,
            "handshake_name": handshake_names[i],
            "tag_name": tag_names[i],
            "raw_score": raw_scores[i],
            "rank_score": len([s for s in raw_scores if s <= raw_scores[i]])
        }
        punters.append(punter)
    return punters


def get_map(log):
    nodes = log["map"]["sites"]
    edges = log["map"]["rivers"]
    mines = log["map"]["mines"]
    adj = {node["id"]: [] for node in nodes}
    edge_owner = {}
    for edge in edges:
        fr, to = edge["source"], edge["target"]
        adj[fr].append(to)
        adj[to].append(fr)
        edge_owner[edge_key(fr, to)] = None
    return adj, mines, edge_owner



def process_log(log, sample_rate=0.1):
    punters = get_punters_data(log)
    graph, mines, edge_owner = get_map(log)
    if "history" not in log:
        return []
    vectors = []
    for update in log["history"]:
        move = update["move"]
        punter_id = move["claim"]["punter"]
        if random.random() < sample_rate:
            vec = feature_vector(graph, mines, edge_owner, punter_id)
            target = punters[punter_id]["raw_score"]
            vec.append(target)
            vectors.append(vec)
        claim_edge = edge_key(move["claim"]["source"], move["claim"]["target"])
        edge_owner[claim_edge] = punter_id
    print("processed", len(vectors), "states")
    return vectors


def train(df):
    X = df[df.columns[:-1]]
    y = df[df.columns[-1]]
    clf = LinearRegression(n_jobs=-1)
    clf.fit(X, y)
    return clf


def main():
    parser = ArgumentParser()
    parser.add_argument("--sample-rate", type=float, default=0.05)
    parser.add_argument("--points", type=int, default=50000)
    args = parser.parse_args()

    states = []
    for row in fetch_all_logs():
        log = json.loads(row["log"])
        states += process_log(log, args.sample_rate)
        if len(states) > args.points:
            break
    print("generated", len(states), "training data.")
    df = pd.DataFrame(states)
    clf = train(df)
    print("trained a linear model.")
    artifact_dir = Path(ROOT_DIR / "mimi_artifacts")
    if not artifact_dir.exists():
        artifact_dir.mkdir()
    joblib.dump(clf, Path(artifact_dir / "clf.pkl"))
    print("saved a model")


if __name__ == '__main__':
    main()
