from sklearn.externals import joblib
from washimimizuku.eval_model import feature_vector, edge_key
from pathlib import Path
import sys
import base64
import pickle
from io import StringIO


# TODO: embed this model within the script because the punter to read a file is prohibited.
CLASSIFIER_PATH = Path(Path(__file__).parent / "clf.pkl")


class Game(object):
    def __init__(self, graph, mines, edge_owner, punters, punter_id, edges):
        self.graph = graph
        self.mines = mines
        self.edge_owner = edge_owner
        self.punters = punters
        self.punter_id = punter_id
        self.edges = edges

    def feature_vector(self):
        sample = 1000
        fv = feature_vector(self.graph, self.mines, self.edge_owner, self.punter_id, 3, sample)
        return [v * 10000 / sample for v in fv]


class State(object):
    def __init__(self, game):
        self.x = 1

    def dumps(self):
        return base64.b64encode(pickle.dumps(self)).decode()


def read_game(f):
    def read_int():
        return int(f.readline())
    punters = read_int()
    punter_id = read_int()
    nodes = read_int()
    mine_count = read_int()
    mines = [int(c) for c in f.readline().split()]
    edge_count = read_int()
    graph = {i: [] for i in range(nodes)}
    edge_owner = {}
    edges = []
    for i in range(edge_count):
        items = [int(c) for c in f.readline().split()]
        if len(items) == 4:
            fr, to, ow1, ow2 = items
        else:
            fr, to, ow1 = items
        graph[fr].append(to)
        graph[to].append(fr)
        if ow1 == -1:
            ow1 = None
        edge_owner[edge_key(fr, to)] = ow1
        edges.append(edge_key(fr, to))
    f.readline()  # skip empty line
    extension_count = read_int()
    extensions = []
    for i in range(extension_count):
        extensions.append(f.readline().strip())
    f.readline()
    return Game(graph, mines, edge_owner, punters, punter_id, edges)


def read_state(f):
    encoded = f.readline().strip().encode()
    return pickle.loads(base64.b64decode(encoded))


def handshake(f):
    print("jaguar")


def init(f):
    game = read_game(f)
    state = State(game)
    print(0)  # futures
    print(state.dumps())


def move(f):
    game = read_game(f)
    state = read_state(f)
    clf = joblib.load(CLASSIFIER_PATH)
    hands = [(-1e100, -1)]
    for i, edge in enumerate(game.edges):
        if game.edge_owner[edge] is not None:
            continue
        game.edge_owner[edge] = game.punter_id
        fv = game.feature_vector()
        hands.append((clf.predict([fv])[0], i))
        game.edge_owner[edge] = None
    print(max(hands)[1])
    print(state.dumps())


def end(f):
    pass


def main(f=sys.stdin):
    mode = f.readline().strip()
    if mode == "HANDSHAKE":
        handshake(f)
    elif mode == "INIT":
        init(f)
    elif mode == "MOVE":
        move(f)
    elif mode == "END":
        end(f)


TEST_INIT = """\
INIT
2
0
3
1
0
2
0 1 0
1 2 -1
"""

TEST_MOVE = """\
MOVE
2
0
3
1
0
2
0 1 -1
1 2 0
gANjX19tYWluX18KU3RhdGUKcQApgXEBfXECWAEAAAB4cQNLAXNiLg==
"""


if __name__ == '__main__':
    main()
