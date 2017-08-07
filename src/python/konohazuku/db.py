import MySQLdb
from MySQLdb.cursors import DictCursor
import json
from collections import defaultdict


def fetch_all_logs():
    try:
        conn = MySQLdb.connect(host="35.194.126.173", user="root", passwd="kaban", db="adlersprung")

        cursor = conn.cursor(DictCursor)
        while True:
            offset = 0
            items = cursor.execute("SELECT * FROM match_log LIMIT 1000 OFFSET ?", offset)
            if items == 0:
                break
            offset += items
            for row in cursor.fetchall():
                yield row
    finally:
        conn.close()


def fetch_results_db():
    for row in fetch_all_logs():
        log_data = json.loads(row["log"])
        if "tag_names" not in log_data:
            continue
        all_names = log_data["tag_names"]
        if "scores" not in log_data:
            continue
        raw_scores = [-1] * len(all_names)
        for sc in log_data["scores"]:
            raw_scores[sc["punter"]] = sc["score"]
        rank_scores = [len([s for s in raw_scores if s <= score]) for score in raw_scores]
        punter_rank_scores = defaultdict(list)
        for name, rank_score in zip(all_names, rank_scores):
            punter_rank_scores[name].append(rank_score)
        yield punter_rank_scores
