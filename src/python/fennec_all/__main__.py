import itertools
from ..fennec import list_ais, list_map_paths, exe, ai_command


if __name__ == '__main__':
    print("ais:", list_ais())
    print("maps", list_map_paths())
    for ais in itertools.combinations(list_ais(), 2):
        for map_path in list_map_paths():
            ai_commands = [ai_command(commit) for commit in ais]
            exe(map_path, ai_commands)