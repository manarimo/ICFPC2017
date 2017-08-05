from argparse import ArgumentParser
from pathlib import Path
import random
import subprocess
import itertools
import time
import shutil
import json

ROOT_DIR = Path(__file__).absolute().parent.parent.parent.parent
LOG_DIR = Path(ROOT_DIR / "logs")

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
    out_obj = json.loads(out.decode("utf-8"))
    scores = {}
    for score in out_obj["scores"]:
        scores[score["punter"]] = score["score"]
    return scores[0]

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

def ruleset(ruleset_str):
    if not ruleset_str:
        return []
    return ruleset_str.strip().split(',')

REPEAT = 3
NUM_PLAYERS = [2, 4]
MAPS = ["map/sample.json", "map/lambda.json"]
RANDOM_AI = "6758e6b36e9b185501ea5d2731b98a5f396f2c67"

def main():
    print(ROOT_DIR)
    if LOG_DIR.exists():
        shutil.rmtree(str(LOG_DIR))
    parser = ArgumentParser()
    parser.add_argument("--ai", type=str, help="AI commit hash/tag")
    parser.add_argument("--ruleset", type=ruleset, nargs='?', help="comma separated additional rule set. if empty, rule set will be initial one. currently supported by zeus: x1=futures.")
    args = parser.parse_args()
    
    tags = tag_names()

    for punters in NUM_PLAYERS:
        ai_commits = [args.ai] + [RANDOM_AI] * (punters - 1)
        ai_commands = [ai_command(tags.get(commit, commit)) for commit in ai_commits]
        for map in MAPS:
            for i in range(REPEAT):
                print("punters: {}, map: {}, i: {}".format(punters, map, i))
                score = exe(Path(map), ai_commands, args.ruleset)
                print("Score: {}".format(score))

if __name__ == '__main__':
    main()
