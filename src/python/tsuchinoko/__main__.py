from argparse import ArgumentParser
from pathlib import Path
import random
import subprocess
from subprocess import CalledProcessError
import itertools
import time
import shutil
import json

ROOT_DIR = Path(__file__).absolute().parent.parent.parent.parent
LOG_DIR = Path(ROOT_DIR / "logs")

def do_until_success(cmd):
    for i in range(5):
        try:
            out = subprocess.check_output(cmd, stderr=subprocess.DEVNULL)
            return out
        except CalledProcessError as e:
            print("Failed. Try again.")

def exe(map_path: Path, ai_commands, ruleset=None):
    ruleset = ruleset or []
    ruleset_args = ["-{}".format(rule) for rule in ruleset]
    cmd = ["java", "-cp", "/var/icfpc/zeus/build:/var/icfpc/zeus/lib/*", "Main"]
    cmd += ruleset_args
    cmd.append(str(map_path.absolute()))
    cmd.append(str(len(ai_commands)))
    cmd += ai_commands
    out = do_until_success(cmd)
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
    return names

def ruleset(ruleset_str):
    if not ruleset_str:
        return []
    return ruleset_str.strip().split(',')

REPEAT = 10
NUM_PLAYERS = [2, 4, 8]
MAPS = ["lambda", "randomMedium", "randomSparse", "rand1", "rand3"]
RANDOM_AI = "6758e6b36e9b185501ea5d2731b98a5f396f2c67"

BENCHMARK_VERSION = 2

def main():
    print(ROOT_DIR)
    if LOG_DIR.exists():
        shutil.rmtree(str(LOG_DIR))
    parser = ArgumentParser()
    parser.add_argument("--ai", type=str, help="AI commit hash/tag")
    parser.add_argument("--ruleset", type=ruleset, nargs='?', help="comma separated additional rule set. if empty, rule set will be initial one. currently supported by zeus: x1=futures.")
    args = parser.parse_args()
    tags = tag_names()

    ai_commit = tags.get(args.ai, args.ai)
    log = {"ai": args.ai, "ai_commit": ai_commit, "version": BENCHMARK_VERSION, "performances": []}
    print("Benchmark Version: {}".format(BENCHMARK_VERSION))
    print("AI: {}, Ruleset: {}".format(args.ai, args.ruleset))
    for punters in NUM_PLAYERS:
        ai_commits = [ai_commit] + [RANDOM_AI] * (punters - 1)
        ai_commands = [ai_command(commit) for commit in ai_commits]
        for map in MAPS:
            print("punters: {}, map: {}".format(punters, map), flush=True)
            total_score = 0
            scores = []
            for i in range(REPEAT):
                score = exe(Path("map/{}.json".format(map)), ai_commands, args.ruleset)
                scores.append(score)
                total_score += score
                print("Score: {}".format(score), flush=True)
            print("Total Score: {}".format(total_score), flush=True)
            testcase_name = "{}-{}".format(map, punters)
            log["performances"].append({"name": testcase_name, "map": map, "punters": punters, "total": total_score, "scores": scores})
    for performance in log["performances"]:
        print("{}:\t\t{}".format(performance["name"], performance["total"]))
    log_string = json.dumps(log)
    filename = "report-{}-{}.json".format(args.ai, int(time.time() * 10 ** 6))
    log_path = Path(LOG_DIR / filename)
    if not LOG_DIR.exists():
        LOG_DIR.mkdir()
    with log_path.open("wb") as f:
        f.write(log_string.encode("utf-8"))

if __name__ == '__main__':
    main()
