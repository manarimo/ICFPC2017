from konohazuku.db import fetch_all_logs
from utils.const import ROOT_DIR
import pandas as pd
from pathlib import Path
import json
from collections import deque
import random


MAX_DISTANCE = 100


def fetch_all_logs():
    with Path(Path(__file__).parent / "sample.json").open() as f:
        return [{"log": f.read()}]


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


def edge_key(fr, to):
    if fr > to:
        fr, to = to, fr
    return fr, to


def zero_one_bfs(graph, starting_nodes, zero_edges, prohibit_edges):
    distances = dict()
    deq = deque()
    for starting_node in starting_nodes:
        deq.append((starting_node, 0))
        distances[starting_node] = 0
    while len(deq) > 0:
        pos, dist = deq.popleft()
        for next in graph[pos]:
            if next in distances:
                continue
            edge = edge_key(pos, next)
            if edge in prohibit_edges:
                continue
            if edge in zero_edges:
                next_dist = dist
                deq.appendleft((next, next_dist))
                distances[next] = next_dist
            else:
                next_dist = dist + 1
                deq.append((next, next_dist))
                distances[next] = next_dist
    return distances


def feature_vector(graph, mines, edge_owner, punter_id, punter_num):
    poseidon_feature = [0] * MAX_DISTANCE
    current_score = 0

    opponent_edges = set(ek for ek, owner in edge_owner.items() if owner != punter_id and owner is not None)
    my_edges = set(ek for ek, owner in edge_owner.items() if owner == punter_id)
    non_my_edges = set(ek for ek, owner in edge_owner.items() if owner != punter_id)
    starting_nodes = set(sum([list(edge) for edge in my_edges], []))
    for mine_id in mines:
        distances = zero_one_bfs(graph, [mine_id], set(), set())
        own_nodes = list(zero_one_bfs(graph, [mine_id], set(), non_my_edges).keys())
        current_score += sum(distances[node_id] ** 2 for node_id in own_nodes)
        costs = zero_one_bfs(graph, starting_nodes, my_edges, opponent_edges)
        if mine_id in costs:
            base_cost = costs[mine_id]
            for node_id, cost in costs.items():
                p = (1 / punter_num) ** (base_cost + cost)
                dist = distances[node_id]
                if dist < MAX_DISTANCE:
                    poseidon_feature[dist] += p
    return [current_score] + poseidon_feature


def process_log(log, sample_rate=0.01):
    punters = get_punters_data(log)
    graph, mines, edge_owner = get_map(log)
    if "history" not in log:
        return []
    vectors = []
    for update in log["history"]:
        move = update["move"]
        punter_id = move["claim"]["punter"]
        if random.random() < sample_rate + 1:
            vec = feature_vector(graph, mines, edge_owner, punter_id, len(punters))
            target = punters[punter_id]["raw_score"]
            vec.append(target)
            vectors.append(vec)
        claim_edge = edge_key(move["claim"]["source"], move["claim"]["target"])
        edge_owner[claim_edge] = punter_id
    return vectors


def main():
    logs = list(json.loads(row["log"]) for row in fetch_all_logs())
    states = sum([process_log(log) for log in logs], [])
    df = pd.DataFrame(states)
    artifact_dir = Path(ROOT_DIR / "mimi_artifacts")
    if not artifact_dir.exists():
        artifact_dir.mkdir()
    with Path(artifact_dir / "states.csv").open("w") as f:
        df.to_csv(f)


if __name__ == '__main__':
    main()
