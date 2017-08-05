from pathlib import Path
import json
from collections import Counter


LOGS_DIR = Path("/var/local/logs/")


def main():
    print("meta analysis tool konohazuku")
    kati = Counter()
    per_win_agg = Counter()
    per_match_agg = Counter()
    for meta_path in LOGS_DIR.iterdir():
        if not meta_path.name.endswith("meta.json"):
            continue
        with meta_path.open() as f:
            meta_json = json.load(f)
        if "scores" not in meta_json or len(meta_json["scores"]) != 2:
            continue
        scores = [0, 0]
        names = meta_json["names"]
        for sc in meta_json["scores"]:
            scores[sc["panter"]] = sc["rank_score"]
        if scores[0] > scores[1]:
            kati[(names[0], names[1])] += 1
            per_win_agg[names[0]] += 1
        elif scores[0] > scores[1]:
            kati[(names[1], names[0])] += 1
            per_win_agg[names[1]] += 1
        per_match_agg[names[0]] += 1
        per_match_agg[names[1]] += 1
    all_names = list(per_win_agg.keys())
    all_names.sort(key=lambda name: (per_win_agg[name] / per_match_agg[name]), reverse=True)
    for name in all_names:
        win, all = per_win_agg[name], per_match_agg[name]
        print("{}: {:.5f}% ({} / {})".format(name, win / all, win, all))
        for opponent in all_names:
            win, lose = kati[(name, opponent)], kati[(opponent, kati)]
            all = win + lose
            print(" - vs {}: {:.5f}% ({} / {})".format(opponent, win / all, win, all))


if __name__ == '__main__':
    main()