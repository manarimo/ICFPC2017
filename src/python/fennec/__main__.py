from argparse import ArgumentParser
from pathlib import Path
import random
import subprocess
import itertools


ROOT_DIR = Path(__file__).absolute().parent.parent.parent.parent


def valid_ai(ai_dir):
    punter_path = Path(ai_dir / "punter")
    return punter_path.exists()


def list_ais():
    return [d.name for d in Path("/var/ai/").iterdir() if d.is_dir() and valid_ai(d)]


def list_map_paths():
    return list(Path(ROOT_DIR / "map").iterdir())


def exe(map_path: Path, ai_commands):
    cmd = ["java", "-cp", "/var/icfpc/zeus/build:/var/icfpc/zeus/lib/*", "Main"]
    cmd.append(str(map_path.absolute()))
    cmd.append(str(len(ai_commands)))
    cmd += ai_commands
    print(cmd)
    subprocess.call(cmd)


def ai_command(commit):
    runner = Path(ROOT_DIR / "bin" / "run_ai.sh")
    return "bash {} {}".format(str(runner), commit)


def tag_names():
    names = dict()
    for commit in list_ais():
        name_file = Path(Path("/var/ai/") / commit / "tag_name")
        if name_file.exists():
            with name_file.open() as f:
                name = f.read().strip()
            names[name] = commit
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
            exe(map_path, ai_commands)


def main():
    print(ROOT_DIR)
    parser = ArgumentParser()
    parser.add_argument("--do-all", action="store_true")
    parser.add_argument("--only-tagged", action="store_true", help="effective only if --do-all is given. use only ais with tagged.")
    parser.add_argument("--ais", type=str, help="comma separated AI commit hashes")
    parser.add_argument("--random-ai-num", type=int, default=0, help="the number of AIs that will be randomly added as participants")
    parser.add_argument("--map", type=str, default=None, help="map json. if absent, randomly selected from ./map")
    parser.add_argument("--repeat", type=int, default=1, help="the number of match to do")
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
    tags = tag_names()
    ai_commits += random.sample(list_ais(), args.random_ai_num)
    ai_commands = [ai_command(tags.get(commit, commit)) for commit in ai_commits]
    for i in range(args.repeat):
        print("match #{}".format(i + 1))
        exe(map_path, ai_commands)


if __name__ == '__main__':
    main()