#include <bits/stdc++.h>

using namespace std;

const int create_node_count = 50;
const int playout_count = 50000;
double C = 1.2;

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

struct UnionFind{
    vector<int> par;

    UnionFind(){}
    UnionFind(int n) {
        par.resize(n);
        iota(par.begin(), par.end(), 0);
    }

    int find(int x) {
        if (par[x] == x) return x;
        return par[x] = find(par[x]);
    }

    void unite(int x, int y) {
        par[find(x)] = find(y);
    }

    bool same(int x, int y) {
        return find(x) == find(y);
    }
};

vector<int> is_mine, is_bridge, mine_connector;
vector<vector<int>> base_dist, current_dist, possess, group_dist;

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
    os << g.punter << '\n' << g.punter_id << '\n' << g.n << '\n' << g.mines << '\n';
    for (int i = 0; i < g.mines; ++i) {
        os << g.mine[i] << (i == g.mines - 1 ? "": " ");
    }
    os << '\n' << g.m << '\n';
    for (int i = 0; i < g.m; ++i) {
        os << g.edge[i].from << ' ' << g.edge[i].to << ' ' << g.edge[i].owner << '\n';
        auto &e = g.edge[i];
    }
    return os;
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
                q.emplace(nv, p.second + 1);
            }
        }
    }

    return dist;
}

vector<vector<int>> calc_dist(const Game &game, bool current = true) {
    vector<vector<int>> G(game.n);
    for (auto &e : game.edge) {
        if (!current || e.owner == game.punter_id || e.owner == -1) {
            G[e.from].push_back(e.to);
            G[e.to].push_back(e.from);
        }
    }
    vector<vector<int>> dist(game.n);
    for (int i = 0; i < game.n; ++i) {
        dist[i] = bfs(G, i);
    }
    return dist;
}

vector<int> calc_dist(const Game &game, int v, bool current = true) {
    vector<vector<int>> G(game.n);
    for (auto &e : game.edge) {
        if (!current || e.owner == game.punter_id || e.owner == -1) {
            G[e.from].push_back(e.to);
            G[e.to].push_back(e.from);
        }
    }
    return bfs(G, v);
}

struct State {
    vector<vector<int>> dist;
};

istream &operator>>(istream &is, State &s) {
    int n;
    is >> n;
    s.dist.resize(n);
    for (int i = 0; i < n; ++i) {
        s.dist[i].resize(n);
        for (int j = 0; j < n; ++j) {
            is >> s.dist[i][j];
        }
    }
    return is;
}

ostream &operator<<(ostream &os, const State &s) {
    int n = s.dist.size();
    os << n;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            os << ' ' << s.dist[i][j];
        }
        cout << ' ';
    }
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
    int cnt;

    UCBnode() : cnt(0) {}
};

map<long long, int> game_freq;
map<long long, UCBnode> game_to_node;

int get_player_id(int punter_id, int punter, int turn) {
    return (turn + punter_id) % punter;
}

mt19937 mt = mt19937(static_cast<unsigned int>(1));
double mt_rand() {
    static auto rand = bind(uniform_real_distribution<double>(0.0, 100.0), mt);
    return rand();
}

struct Candidate {
    int idx;
    double modifier;
};

// get candidate moves
vector<Candidate> get_candidate(Game &game, int turn, bool all) {
    auto &edge = game.edge;
    vector<Candidate> rest, cand;
    vector<int> visited(game.n);
    UnionFind uf(game.n);
    vector<set<int>> color(game.n);

    int current_punter = get_player_id(game.punter_id, game.punter, turn);
    for (int i = 0; i < edge.size(); ++i) {
        auto &e = edge[i];
        if (e.owner == current_punter) {
            visited[e.from] = visited[e.to] = 1;
            uf.unite(e.from, e.to);
        } else if (e.owner != -1){
            color[e.from].insert(e.owner);
            color[e.to].insert(e.owner);
        }
    }
    for (int i = 0; i < edge.size(); ++i) {
        if (edge[i].owner == -1) {
            rest.push_back({i, 1.0});

            int a = edge[i].from;
            int b = edge[i].to;
            if (uf.same(a, b) || all) continue;
            if (is_bridge[i] || visited[a] || visited[b] || is_mine[a] || is_mine[b]) {
                double modifier = 1.0;
                if (is_bridge[i]) modifier *= 1.2;
                if (mine_connector[i]) modifier *= 1.2;
                if (possess[current_punter][a] && possess[current_punter][b]) modifier *= 1.5;
                else if (!possess[current_punter][a] && !possess[current_punter][b]) modifier *= 0.9;
                modifier *= (1.0 + (color[a].size() + color[b].size()) / 20.0);

                cand.push_back({i, modifier});
            }
        }
    }
    if (all || cand.empty()) return rest;
    return cand;
}

using Score = long long;

// get the score of the game
Score calc_score(const Game &game, const vector<Edge> &edge, int turn) {
    // TODO: too slow
    Score score = 0;
    for (auto &mine : game.mine) {
        vector<vector<int>> org(game.n), res(game.n);
        for (auto &e : edge) {
            org[e.from].push_back(e.to);
            org[e.to].push_back(e.from);
            if (e.owner == get_player_id(game.punter_id, game.punter, turn)) {
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

Score calc_score(const Game &game, int turn) {
    return calc_score(game, game.edge, turn);
}

vector<Score> win_rate(const Game &game, vector<Score> score) {
    vector<pair<Score, int>> score_id;
    for (int i = 0; i < game.punter; ++i) {
        score_id.emplace_back(score[i], i);
    }
    vector<Score> result(game.punter);
    sort(score.rbegin(), score.rend());
    for (int i = 0; i < game.punter; ++i) {
        if (i <= game.punter / 2) {
            result[score_id[i].second] = 100.0 / (i + 1);
        }
    }

    return result;
}

vector<Score> random_play(Game &game, int turn) {
    auto edge = game.edge;
    auto cand = get_candidate(game, turn, true);
    // 隣接辺からランダム
    priority_queue<pair<double, int>> q;

    vector<int> inqueue(game.m);
    for (auto c: cand) {
        auto e = c.idx;
        inqueue[e] = 1;
        q.emplace(mt_rand(), e);
    }

    while (!q.empty()) {
        auto p = q.top();
        q.pop();
        auto &e1 = edge[p.second];

        e1.owner = get_player_id(game.punter_id, game.punter, turn++);
        // TODO: reduce order..
        for (int i = 0; i < edge.size(); ++i) {
            auto &e2 = edge[i];
            if (e2.owner == -1 && !inqueue[i] && (e1.from == e2.from || e1.to == e2.to || e1.from == e2.to || e1.to == e2.from))  {
                inqueue[i] = 1;
                q.emplace(mt_rand(), i);
            }
        }
    }

    // score for punters
    vector<Score> result;
    for (int i = 0; i < game.punter; ++i) {
        result.push_back(calc_score(game, edge, i));
    }
    return result;
}

vector<Score> uct_search(Game &game, int turn) {
    long long hash_value = hash_edge(game.edge);
    int &cnt = game_freq[hash_value];
    if (cnt < create_node_count) {
        ++cnt;
        return random_play(game, turn);
    }

    UCBnode &v = game_to_node[hash_value];
    int idx = -1;
    double best = -1;

    // find the best move so far
    for (auto &e : get_candidate(game, turn, false)) {
        if (!v.ch[e.idx].cnt) {
            idx = e.idx;
            break;
        }
        double ucb = calc_ucb(v.ch[e.idx].ex, v.ch[e.idx].cnt, v.cnt) * e.modifier;
        if (best < ucb) {
            best = ucb;
            idx = e.idx;
        }
    }

    if (idx < 0) {
        vector<Score> score;
        score.push_back(calc_score(game, turn));

        return score;
    }
    game.edge[idx].owner = (game.punter_id + turn) % game.punter;
    // This res should be the score of the player playing the turn
    vector<Score> res = uct_search(game, (turn + 1) % game.punter);
    game.edge[idx].owner = -1;

    // propagate
    v.ch[idx].ex = ((v.ch[idx].ex * v.ch[idx].cnt) + res[turn]) / (v.ch[idx].cnt + 1);
    ++v.ch[idx].cnt;
    ++v.cnt;

    return res;
}

Result move(Game &game, State state, int playout) {
    base_dist = state.dist;
    current_dist = calc_dist(game);

    possess.resize(game.punter);
    for (int i = 0; i < game.punter; ++i) {
        possess[i].resize(game.n);
    }

    UnionFind uf(game.n);
    for (auto &e: game.edge) {
        if (e.owner != -1) possess[e.owner][e.from] = possess[e.owner][e.to] = 1;
        if (e.owner == game.punter_id) uf.unite(e.from, e.to);
    }

    group_dist.resize(game.n);
    for (int i = 0; i < game.n; ++i) {
        group_dist[i].resize(game.n, INF);
        for (int j = 0; j < game.n; ++j) {
            group_dist[uf.find(i)][uf.find(j)] = group_dist[uf.find(j)][uf.find(i)]
                    = min(group_dist[uf.find(i)][uf.find(j)], current_dist[i][j]);
        }
    }

    mine_connector.resize(game.edge.size());
    is_bridge.resize(game.edge.size());
    for (int i = 0; i < game.edge.size(); ++i) {
        auto &e = game.edge[i];

        int tmp = e.owner;
        e.owner = 1e9;
        int cnt1 = 0;
        int cnt2 = 0;
        auto d1 = calc_dist(game, e.from);
        auto d2 = calc_dist(game, e.to);

        int threshold = game.edge.size() / 10;
        bool good = false;
        for (int m1 : game.mine) {
            for (int m2 : game.mine) {
                int m1g = uf.find(m1);
                int m2g = uf.find(m2);
                int from = uf.find(e.from);
                int to = uf.find(e.to);
                if(group_dist[m1g][m2g] < threshold
                    && (group_dist[m1g][from] + group_dist[to][m2g] < group_dist[m1g][m2g]
                        || group_dist[m1g][to] + group_dist[from][m2g] < group_dist[m1g][m2g])) {
                    mine_connector[i] = 1;
                }
            }
        }

        for (int j = 0; j < game.n; ++j) {
            if (current_dist[e.from][j] != INF && d1[j] == INF) ++cnt1;
            if (current_dist[e.to][j] != INF && d2[j] == INF) ++cnt2;
        }
        if ((double)min(cnt1, cnt2) > game.n / 10 + 1 || (mine_connector[i] && d1[e.to] >= threshold)) {
            is_bridge[i] = 1;
        }
        e.owner = tmp;
    }

    is_mine.resize(game.n);
    for (auto &m: game.mine) {
        is_mine[m] = 1;
    }

    long long hash = hash_edge(game.edge);
    UCBnode &root = game_to_node[hash];
    for (int i = 0; i < playout; ++i) uct_search(game, 0);

    int idx = -1;
    double best = -1;
    for (auto &e : get_candidate(game, 0, false)) {
//        cerr << game.edge[e.idx].from << ' ' << game.edge[e.idx].to << ' ' << root.ch[e.idx].ex << ' ' << e.modifier << endl;
        double ucb1 = calc_ucb(root.ch[e.idx].ex, root.ch[e.idx].cnt, root.cnt) * e.modifier;
        if (best >= ucb1) continue;
        best = ucb1, idx = e.idx;
    }
    return Result{idx, state};
}

/////////////////////////////////////////////////////////////////////////////////////////////

struct Settings {
    int n;
    vector<string> content;
};

istream &operator>>(istream &is, Settings &settings) {
    is >> settings.n;
    settings.content.resize(settings.n);
    for (int i = 0; i < settings.n; ++i) {
        is >> settings.content[i];
    }
    return is;
}

State init(const Game &game) {
    auto d = calc_dist(game);
    return State{d};
}

int main() {
    cin.tie();
    ios::sync_with_stdio();

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
    Settings settings;
    switch (state_map.find(command)->second) {
        case HANDSHAKE:
            cout << "new-latest-mu" << endl;
            break;
        case INIT:
            cin >> game >> settings;
//            cerr << game << endl;
            cout << 0 << '\n';
            cout << init(game) << endl;
            break;
        case MOVE:
            cin >> game >> settings >> state;
            result = move(game, state, playout_count / (game.edge.size() * game.punter));
            cout << result.edge << '\n' << result.state << endl;
            break;
        case END:
            break;
    }
}