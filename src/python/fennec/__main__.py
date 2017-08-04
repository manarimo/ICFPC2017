from argparse import ArgumentParser
from pathlib import Path
import random
import subprocess


ROOT_DIR = Path(__file__).absolute().parent.parent.parent.parent


def list_ais():
    return [d.name for d in Path("/var/ai/").iterdir() if d.is_dir()]


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
    parser.add_argument("--map", type=Path, default=None, help="map json. if absent, randomly selected from ./map")

    args = parser.parse_args()
    map_path = args.map or random.choice(list_map_paths())
    ai_commits = []
    if args.ais is not None:
        ai_commits += args.ais.split(',')
    ai_commits += random.sample(list_ais(), args.random_ai_num)
    ai_commands = [ai_command(commit) for commit in ai_commits]
    exe(map_path, ai_commands)

