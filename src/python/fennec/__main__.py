from argparse import ArgumentParser
from pathlib import Path
import random
import subprocess


ROOT_DIR = Path(__file__).absolute().parent.parent.parent.parent


def valid_ai(ai_dir):
    punter_path = Path(ai_dir / "punter")
    return punter_path.exists()


def list_ais():
    return [d.name for d in Path("/var/ai/").iterdir() if d.is_dir() and valid_ai(d)]


def list_map_paths():
    return list(Path(ROOT_DIR / "map").iterdir())


def exe(map_path: Path, ai_commands):
    cmd = ["/var/icfpc/zeus/zeus"]
    cmd.append(str(map_path.absolute()))
    cmd.append(str(len(ai_commands)))
    cmd += ai_commands
    print(cmd)
    subprocess.call(cmd)


def ai_command(commit):
    runner = Path(ROOT_DIR / "bin" / "run_ai.sh")
    return "{} {}".format(str(runner), commit)


if __name__ == '__main__':
    print(ROOT_DIR)
    parser = ArgumentParser()
    parser.add_argument("--ais", type=str, help="comma separated AI commit hashes")
    parser.add_argument("--random-ai-num", type=int, default=0, help="the number of AIs that will be randomly added as participants")
    parser.add_argument("--map", type=str, default=None, help="map json. if absent, randomly selected from ./map")

    args = parser.parse_args()
    if args.map == "random" or args.map is None:
        map_path = random.choice(list_map_paths())
    else:
        map_path = Path(args.map)
    ai_commits = []
    if args.ais is not None:
        ai_commits += args.ais.split(',')
    ai_commits += random.sample(list_ais(), args.random_ai_num)
    ai_commands = [ai_command(commit) for commit in ai_commits]
    exe(map_path, ai_commands)

