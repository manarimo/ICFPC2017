#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <algorithm>
#include <cmath>

using namespace std;

const int create_node_count = 20;    // required count to create a new node
const int playout_count = 1000;        // times of playout
double C = 1.1;


struct Edge {
    int from;
    int to;
    int owner;
};

struct Game {
    int punter;
    int punter_id;
    int n;
    int mines;
    vector<int> mine;
    int m;
    vector<Edge> edge;
};

enum Command {
    HANDSHAKE,
    INIT,
    MOVE,
    END
};

map<int, int> is_mine;

istream &operator>>(istream &is, Game &g) {
    is >> g.punter >> g.punter_id >> g.n >> g.mines;
    g.mine.resize(g.mines);
    for (int i = 0; i < g.mines; ++i) {
        is >> g.mine[i];
    }
    is >> g.m;
    g.edge.resize(g.m);
    for (int i = 0; i < g.m; ++i) {
        is >> g.edge[i].from >> g.edge[i].to >> g.edge[i].owner;
        auto &e = g.edge[i];
    }
    return is;
}

ostream &operator<<(ostream &os, const Game &g) {
    cout << "game!!!" << endl;
    return os;
}

struct State {
    string s;
};

istream &operator>>(istream &is, State &s) {
    is >> s.s;
    return is;
}

ostream &operator<<(ostream &os, const State &s) {
    cout << s.s;
    return os;
}

struct Result {
    int edge;
    State state;
};

const int X = 10007;
long long hash_edge(const vector<Edge> &edge) {
    // TODO: too slow
    const int MOD = static_cast<const int>(1e9 + 7);
    long long hash = 0;
    for (int i = 0; i < edge.size(); ++i) {
        (hash += X * hash + edge[i].owner) %= MOD;
    }
    return hash;
}

inline double calc_ucb(double ex, int ni, int n) {
    return ex + C * sqrt(2 * log2(n) / ni);
}

struct UCBchild {
    double ex;
    int cnt;
    UCBchild() : ex(0), cnt(0) {}
};

struct UCBnode {
    map<int, UCBchild> ch;
    double ex;
    int cnt;
    UCBnode() : ex(0), cnt(0) {}
};

map<long long, int> game_freq;
map<long long, UCBnode> game_to_node;

// get candidate moves
vector<int> get_candidate(const Game &game, int turn) {
    vector<int> vacant_edge, cand;
    for (int i = 0; i < game.edge.size(); ++i) {
        if (game.edge[i].owner == -1) {
            vacant_edge.push_back(i);
            if (game.edge[i].owner == (game.punter_id + turn) % game.punter
                || is_mine[game.edge[i].from] || is_mine[game.edge[i].to]) {
                cand.push_back(i);
            }
        }
    }
    if (cand.empty()) return vacant_edge;
    return cand;
}

const int INF = static_cast<const int>(1e9);

vector<int> bfs(const vector<vector<int>> &G, int v) {
    int n = static_cast<int>(G.size());
    vector<int> dist(n, INF);

    queue<pair<int, int>> q;
    q.push(make_pair(v, 0));
    dist[v] = 0;
    while (!q.empty()) {
        auto &p = q.front();
        q.pop();

        for (auto nv: G[p.first]) {
            if (dist[nv] == INF) {
                dist[nv] = p.second + 1;
                q.emplace(nv, p.second);
            }
        }
    }

    return dist;
}

// get the score of the game
long long calc_score(const Game &game, const vector<Edge> &edge) {
    // TODO: too slow
    long long score = 0;
    for (auto &mine : game.mine) {
        vector<vector<int>> org(game.n), res(game.n);
        for (auto &e : edge) {
            org[e.from].push_back(e.to);
            org[e.to].push_back(e.from);
            if (e.owner == game.punter_id) {
                res[e.from].push_back(e.to);
                res[e.to].push_back(e.from);
            }
        }
        auto d1 = bfs(org, mine);
        auto d2 = bfs(res, mine);
        for (int i = 0; i < game.n; ++i) {
            if (d2[i] != INF) {
                score += d1[i] * d1[i];
            }
        }
    }

    return score;
}

long long calc_score(const Game &game) {
    return calc_score(game, game.edge);
}

long long random_play(const Game &game, int turn) {
    auto edge = game.edge;
    auto cand = get_candidate(game, turn);
    random_shuffle(cand.begin(), cand.end());
    for (int e: cand) {
        edge[e].owner = (game.punter_id + turn++) % game.punter;
    }

    // score
    return calc_score(game, edge);
}

long long uct_search(Game &game, int turn) {
    // 辿った回数がcreate_node_count回未満ならMC
    long long hash_value = hash_edge(game.edge);
    int &cnt = game_freq[hash_value];
    if (cnt < create_node_count) {
        ++cnt;
        long long res = random_play(game, turn);
        return res;
    }

    UCBnode &v = game_to_node[hash_value];
    int idx = -1;
    double best = turn == 0 ? -1 : 1;

    // find the best move so far
    for (auto &e : get_candidate(game, turn)) {
        if (!v.ch[e].cnt) {
            idx = e;
            break;
        }
        double ucb = calc_ucb(v.ch[e].ex, v.ch[e].cnt, v.cnt);
        if (turn == 0) {
            if (best < ucb) {
                best = ucb;
                idx = e;
            }
        } else if (best > ucb) {
            best = ucb;
            idx = e;
        }
    }

    if (idx < 0) return calc_score(game);
    game.edge[idx].owner = (game.punter_id + turn) % game.punter;
    long long res = uct_search(game, (turn + 1) % game.punter);
    game.edge[idx].owner = -1;

    // propagate
    v.ch[idx].ex = ((v.ch[idx].ex * v.ch[idx].cnt) + res) / (v.ch[idx].cnt + 1);
    ++v.ch[idx].cnt;

    v.ex = ((v.ex * v.cnt) + res) / (v.cnt + 1);
    ++v.cnt;

    return res;
}

Result search(Game &game, int playout) {
    for (auto &m: game.mine) {
        is_mine[m] = 1;
    }

    long long hash = hash_edge(game.edge);
    UCBnode &root = game_to_node[hash];
    for (int i = 0; i < playout; ++i) uct_search(game, 0);

    int idx = -1;
    double best = -1;
    for (auto &e : get_candidate(game, 0)) {
        double ucb1 = calc_ucb(root.ch[e].ex, root.ch[e].cnt, root.cnt);
        if (best >= ucb1) continue;
        best = ucb1, idx = e;
    }
    return Result{idx, {}};
}

/////////////////////////////////////////////////////////////////////////////////////////////


int main() {
    string command;
    cin >> command;

    map<string, Command> state_map{
            {"HANDSHAKE", HANDSHAKE},
            {"INIT",      INIT},
            {"MOVE",      MOVE},
            {"END",       END}
    };

    Game game;
    State state;
    Result result;
    switch (state_map.find(command)->second) {
        case HANDSHAKE:
            cout << "tsurapoyo" << endl;
            break;
        case INIT:
            cin >> game;
            cout << 0 << endl;
            cout << "tsurapoyo~" << endl;
            break;
        case MOVE:
            cin >> game >> state;
            result = search(game, playout_count);
            cout << result.edge << '\n' << result.state;
            break;
        case END:
            break;
    }
}