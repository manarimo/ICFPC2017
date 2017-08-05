from argparse import ArgumentParser
from pathlib import Path
import random
import subprocess
import itertools
import time
import shutil
import json
import numpy as np


ROOT_DIR = Path(__file__).absolute().parent.parent.parent.parent
LOG_DIR = Path(ROOT_DIR / "logs")


def valid_ai(ai_dir):
    punter_path = Path(ai_dir / "punter")
    return punter_path.exists()


def list_ais():
    return [d.name for d in Path("/var/ai/").iterdir() if d.is_dir() and valid_ai(d)]


def list_map_paths():
    return list(Path(ROOT_DIR / "map").iterdir())


def exe(map_path: Path, ai_commands, ruleset=None):
    ruleset = ruleset or []
    print("ruleset:", ruleset)
    ruleset_args = ["-{}".format(rule) for rule in ruleset]
    cmd = ["java", "-cp", "/var/icfpc/zeus/build:/var/icfpc/zeus/lib/*", "Main"]
    cmd += ruleset_args
    cmd.append(str(map_path.absolute()))
    cmd.append(str(len(ai_commands)))
    cmd += ai_commands
    print(cmd)
    out = subprocess.check_output(cmd)
    if not LOG_DIR.exists():
        LOG_DIR.mkdir()
    filename = "{}.json".format(int(time.time() * 10 ** 6))
    log_path = Path(LOG_DIR / filename)
    with log_path.open("wb") as f:
        f.write(out)
    print("alpaca link: http://alpaca.adlersprung.osak.jp/index.html#{}".format(filename.replace(".json", "")))


def ai_command(commit):
    runner = Path(ROOT_DIR / "bin" / "run_ai.sh")
    return "bash {} {}".format(str(runner), commit)


def tag_names():
    names = dict()
    tag_script_path = Path(ROOT_DIR / "bin" / "get_git_tags.sh")
    out = subprocess.check_output(["bash", str(tag_script_path)]).decode()
    out_lines = out.splitlines()
    for i in range(1, len(out_lines), 2):
        names[out_lines[i - 1]] = out_lines[i]
    print(out)
    print(out_lines)
    print(names)
    return names


def main_all(args):
    ais = list_ais()
    if args.only_tagged:
        tagged_commits = set(tag_names().values())
        ais = [commit for commit in ais if commit in tagged_commits]
    print("ais:", ais)
    print("maps", list_map_paths())
    for aip in itertools.combinations(ais, 2):
        for map_path in list_map_paths():
            ai_commands = [ai_command(commit) for commit in aip]
            exe(map_path, ai_commands, ruleset)


def ruleset(ruleset_str):
    if not ruleset_str:
        return []
    if ruleset_str == "lightning_random":
        return random.choice([[], ["x1"]])
    if ruleset_str == "random":
        full_rules = ["x1"]
        subsets = sum((list(itertools.combinations(full_rules, r)) for r in range(len(full_rules) + 1)), [])
        return random.choice(subsets)
    return ruleset_str.strip().split(',')


def choice(collection, weights):
    acc = 0
    thresh = [0]
    for w in weights:
        acc += w
        thresh.append(acc)
    r = random.uniform(0, thresh[-1])
    for i in range(collection):
        if thresh[i] <= r <= thresh[i+1]:
            return collection[i]


def konoha_scores():
    konoha_json_path = Path(ROOT_DIR / "konoha_artifacts" / "ratings.json")
    with konoha_json_path.open() as f:
        return json.load(f)


def win_rate(me, opponent):
    return 1 / (1 + np.exp(opponent - me))


def entropy(p):
    return -p * np.log2(p) - (1 - p) * np.log2(1 - p)


def unpredictability(me, opponent):
    return entropy(win_rate(me, opponent))


def sample_ais(n):
    tags = list(tag_names().values())
    weights = []
    tag_tuples = itertools.combinations(tags, n)
    ratings = konoha_scores()
    average_rate = np.average(sc["rating"] for sc in ratings)
    for tup in tag_tuples:
        matches = [ratings[name]["match_count"] for name in tup]
        rates = [ratings[name]["rating"] for name in tup]
        aged_penalty = np.prod([1 / (50 + c) for c in matches])
        predictability_penalty = np.prod([unpredictability(*rate_pair) for rate_pair in itertools.combinations(rates, 2)])
        rating_penalty = win_rate(np.average(rates), average_rate)
        weight = aged_penalty * predictability_penalty * rating_penalty
        weights.append(weight)
        print(tup, weight, aged_penalty, predictability_penalty, rating_penalty)
    return list(choice(tag_tuples, weights))


def main():
    print(ROOT_DIR)
    if LOG_DIR.exists():
        shutil.rmtree(str(LOG_DIR))
    parser = ArgumentParser()
    parser.add_argument("--do-all", action="store_true")
    parser.add_argument("--only-tagged", action="store_true", help="if --do-all is given. use only ais with tagged. if --do-all is absent, random sampling is done from tagged")
    parser.add_argument("--ais", type=str, help="comma separated AI commit hashes")
    parser.add_argument("--duplicate", type=int, default=1, help="the number of instances for each AIs.")
    parser.add_argument("--random-ai-num", type=int, default=0, help="the number of AIs that will be randomly added as participants")
    parser.add_argument("--map", type=str, nargs='?', help="map json. if absent, randomly selected from ./map")
    parser.add_argument("--repeat", type=int, default=1, help="the number of match to do. ai order is shuffled")
    parser.add_argument("--ruleset", type=ruleset, nargs='?', help="comma separated additional rule set. if empty, rule set will be initial one. currently supported by zeus: x1=futures. Or, this argument supports special value lightning-random, random")
    args = parser.parse_args()
    if args.do_all:
        main_all(args)
        return

    if args.map == "random" or args.map is None:
        map_path = random.choice(list_map_paths())
    else:
        map_path = Path(args.map)
    ai_commits = []
    if args.ais is not None:
        ai_commits += args.ais.split(',')
    ai_commits += sample_ais(args.random_ai_num)
    tags = tag_names()
    ai_commands = [ai_command(tags.get(commit, commit)) for commit in ai_commits]
    ai_commands = ai_commands * args.duplicate
    for i in range(args.repeat):
        print("match #{}".format(i + 1))
        exe(map_path, ai_commands, args.ruleset)
        random.shuffle(ai_commands)


if __name__ == '__main__':
    main()