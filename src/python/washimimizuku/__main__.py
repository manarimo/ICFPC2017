from konohazuku.db import fetch_all_logs
from utils.const import ROOT_DIR
import pandas as pd
from pathlib import Path


def turns(match_json):
    return len(match_json["history"])


def tag_name(punter_json):
    return punter_json["tag_name"]


def punters(logs):
    for log in logs:
        if "names" not in log:
            continue
        handshake_names = log["names"]
        punter_count = len(handshake_names)
        tag_names = log.get("tag_names", [None] * punter_count)
        for i in range(punter_count):
            punter = {
                "id": i,
                "handshake_name": handshake_names[i],
                "tag_name": tag_names[i]
            }
            yield punter


def to_df(items, metrics):
    table = [[func(item) for name, func in metrics] for item in items]
    columns = [name for name, func in metrics]
    return pd.DataFrame(table, columns=columns)


def main():
    logs = list(row["log"] for row in fetch_all_logs())

    match_metrics = [
        ("turns", turns)
    ]
    match_df = to_df(logs, match_metrics)

    punter_metrics = [
        ("tag_name", tag_name)
    ]
    punter_df = to_df(punters(logs), punter_metrics)

    artifact_dir = Path(ROOT_DIR / "mimi_artifacts")
    if not artifact_dir.exists():
        artifact_dir.mkdir()
    with Path(artifact_dir / "match.csv").open("w") as f:
        match_df.to_csv(f)
    with Path(artifact_dir / "punter.csv").open("w") as f:
        punter_df.to_csv(f)


if __name__ == '__main__':
    main()
