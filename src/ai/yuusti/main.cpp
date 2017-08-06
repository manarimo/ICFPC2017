#include <vector>
#include <map>
#include <iostream>
#include <queue>
#include <set>
#include <algorithm>
#include <fstream>
#include <cmath>

using namespace std;

bool debug = false;
//ofstream ofs;
ostream &ofs = cerr;
//ofstream ofs("out", ios_base::out | ios_base::app);

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
    os << g.punter << endl << g.punter_id << endl << g.n << endl << g.mines << endl;
    for (int i = 0; i < g.mines; ++i) {
        os << g.mine[i] << ((i == g.mines - 1) ? "" : " ");
    }
    os << endl << g.m << endl;
    for (int i = 0; i < g.m; ++i) {
        os << g.edge[i].from << ' ' << g.edge[i].to << ' ' << g.edge[i].owner << endl;
        auto &e = g.edge[i];
    }
    return os;
}

struct State {
    vector<vector<int> > dist;
};

istream &operator>>(istream &is, State &s) {
    int n;
    is >> n;
    s.dist.resize(n);
    for (int i = 0; i < n; ++i) {
        int m;
        is >> m;
        s.dist[i].resize(m);
        for (int j = 0; j < m; ++j) {
            is >> s.dist[i][j];
        }
    }
    return is;
}

ostream &operator<<(ostream &os, const State &s) {
    os << s.dist.size();
    for (int i = 0; i < s.dist.size(); ++i) {
        os << ' ' << s.dist[i].size();
        for (int j = 0; j < s.dist[i].size(); ++j) {
            os << ' ' << s.dist[i][j];
        }
    }
    return os;
}

struct Result {
    int edge;
    State state;
};

State init(Game &game) {
    vector<vector<Edge> > es(game.n);
    for (int i = 0; i < game.m; i++) {
        Edge &e = game.edge[i];
        es[e.from].push_back(Edge{e.from, e.to, e.owner});
        es[e.to].push_back(Edge{e.to, e.from, e.owner});
    }

    vector<vector<int> > dist(game.mines, vector<int>(game.n));
    for (int i = 0; i < game.mines; i++) {
        set<int> rests;
        for (int j = 0; j < game.n; j++) rests.insert(j);
        queue<int> q;
        q.push(game.mine[i]);
        q.push(-1);
        int d = 0;
        while (q.size() > 1) {
            int x = q.front();
            q.pop();
            if (x == -1) {
                d++;
                q.push(-1);
                continue;
            }
            dist[i][x] = d;
            for (int j = 0; j < es[x].size(); j++) {
                int y = es[x][j].to;
                if (rests.find(y) != rests.end()) {
                    q.push(y);
                    rests.erase(rests.find(y));
                }
            }
        }
    }
    return State{dist};
}

struct UnionFind {
    vector<int> data;

    UnionFind(int n) : data(n, -1) {}

    void unite(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) return;
        if (data[x] < data[y]) swap(x, y);
        data[x] += data[y];
        data[y] = x;
    }

    int find(int x) {
        return data[x] < 0 ? x : data[x] = find(data[x]);
    }
};

double
getMinPathScore(vector<double> &score, map<pair<int, int>, double> &minPathScore, map<pair<int, int>, int> &pathNum,
                map<int, int> &dist, vector<double> &dist_sq, vector<vector<int> > &es, Game &game, UnionFind &uf,
                vector<vector<int>> is_bridge, vector<int> bridge_cnt, vector<set<int>> enemy, pair<int, int> xdame) {
    int x = xdame.first;
    int dame = xdame.second;
    if (minPathScore.find(xdame) != minPathScore.end()) {
        return minPathScore[xdame];
    }
    if (debug) ofs << "IN" << x << "(" << dame << ")" << endl;
    double ret = 0;
    ret += dist_sq[x];
    for (int i = 0; i < es[x].size(); i++) {
        Edge &e = game.edge[es[x][i]];
        int to = uf.find(e.from) == x ? e.to : e.from;
        to = uf.find(to);
        if (debug) ofs << dist[x] << "," << dist[to] << endl;
        double nscore;
        if (dist[x] < dist[to]) {
            pair<int, int> next = make_pair(to, dame);
            nscore = getMinPathScore(score, minPathScore, pathNum, dist, dist_sq, es, game, uf, is_bridge, bridge_cnt, enemy, next) * pathNum[xdame] /
                     pathNum[next];
        } else if (dist[x] == dist[to] && dame == 0) {
            pair<int, int> next = make_pair(to, 1);
            nscore = getMinPathScore(score, minPathScore, pathNum, dist, dist_sq, es, game, uf, is_bridge, bridge_cnt, enemy, next) * pathNum[xdame] /
                     pathNum[next];
        } else {
            continue;
        }
        double sc = max(enemy[e.from].size(), enemy[e.to].size());
        if (!is_bridge[game.punter_id][i]) sc /= 10;
        double div = game.punter - sc;
        ret += nscore / div;
        score[es[x][i]] += nscore * pow(1. / game.punter, dist[x]);
        if (debug) ofs << "edge" << es[x][i] << "/" << score[es[x][i]] << endl;
    }
    if (debug) ofs << "OUT" << x << " " << ret << endl;
    return minPathScore[xdame] = ret;
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

vector<int> calc_dist(const Game &game, int v, int punter_id = -1) {
    vector<vector<int>> G(static_cast<unsigned long>(game.n));
    for (auto &e : game.edge) {
        if (punter_id != -1 || e.owner == punter_id || e.owner == -1) {
            G[e.from].push_back(e.to);
            G[e.to].push_back(e.from);
        }
    }
    return bfs(G, v);
}

vector<vector<int>> calc_dist_all(const Game &game, int punter_id = -1) {
    vector<vector<int>> G(static_cast<unsigned long>(game.n));
    for (auto &e : game.edge) {
        if (punter_id != -1 || e.owner == punter_id || e.owner == -1) {
            G[e.from].push_back(e.to);
            G[e.to].push_back(e.from);
        }
    }

    vector<vector<int>> d;
    for (int i = 0; i < game.n; ++i) {
        d.push_back(bfs(G, i));
    }
    return d;
}

Result move(Game &game, State &state) {
    vector<vector<int>> is_bridge(game.punter, vector<int>(game.edge.size()));
    vector<int> bridge_cnt(game.edge.size());
    vector<set<int>> enemy(static_cast<unsigned long>(game.n));

    int threshold = game.edge.size() / 10;
    // keisanryou zako
    for (int i = 0; i < game.punter; ++i) {
        auto d = calc_dist_all(game, i);
        for (int j = 0; j < game.edge.size(); ++j) {
            auto &e = game.edge[j];
            int tmp = e.owner;
            e.owner = 1e9;
            auto d1 = calc_dist(game, e.from, i);
            auto d2 = calc_dist(game, e.to, i);
            e.owner = tmp;

            int cnt1 = 0;
            int cnt2 = 0;
            for (int j = 0; j < game.n; ++j) {
                if (d[e.from][j] != INF && d1[j] - d[e.from][j] > threshold) ++cnt1;
                if (d[e.to][j] != INF && d2[j] - d[e.from][j] > threshold) ++cnt2;
            }
            if ((double) min(cnt1, cnt2) > game.n / 10 + 1) {
                is_bridge[i][j];
                ++bridge_cnt[j];
            }
            e.owner = tmp;

        }
    }

    for (auto &e : game.edge) {
        if (e.owner == -1 || e.owner == game.punter_id) continue;
        enemy[e.from].insert(e.owner);
        enemy[e.to].insert(e.owner);
    }

    UnionFind uf(game.n);
    for (int i = 0; i < game.m; i++) {
        if (game.edge[i].owner == game.punter_id) {
            uf.unite(game.edge[i].to, game.edge[i].from);
        }
    }

    map<int, vector<int> > unions;
    for (int i = 0; i < game.n; i++) {
        unions[uf.find(i)].push_back(i);
    }

    vector<double> score(game.m);

    vector<vector<int>> es(game.n);
    for (int i = 0; i < game.m; i++) {
        Edge &e = game.edge[i];
        int from = uf.find(e.from);
        int to = uf.find(e.to);
        if (from == to) {
            continue;
        }
        if (e.owner == -1) {
            es[from].push_back(i);
            es[to].push_back(i);
        }
    }

    if (debug) {
        ofs << "ES" << endl;
        for (int i = 0; i < es.size(); i++) {
            for (int j = 0; j < es[i].size(); j++) {
                ofs << es[i][j] << " ";
            }
            ofs << endl;
        }
    }

    vector<vector<double>> dist_sq(game.mines, vector<double>(game.n));
    for (int i = 0; i < game.mines; i++) {
        for (auto &p: unions) {
            int x = p.first;
            double score = 0;
            for (int j = 0; j < p.second.size(); j++) {
                int y = p.second[j];
                score += state.dist[i][y] * state.dist[i][y];
            }
            dist_sq[i][x] = score;
        }
    }

    if (debug) {
        ofs << "DIST2" << endl;
        for (int i = 0; i < dist_sq.size(); i++) {
            for (int j = 0; j < dist_sq[i].size(); j++) {
                ofs << dist_sq[i][j] << " ";
            }
            ofs << endl;
        }
    }

    for (int i = 0; i < game.mines; i++) {
        int mine = uf.find(game.mine[i]);
        pair<int, int> start = make_pair(mine, 0);
        map<pair<int, int>, int> path_num;
        map<int, int> dist;
        dist[mine] = 0;
        set<pair<int, int>> q;
        q.insert(start);
        path_num[start] = 1;
        for (int d = 0; !q.empty(); d++) {
            set<pair<int, int>> nq;
            for (auto &p: q) {
                int x = p.first;
                int dame = p.second;
                if (debug) ofs << x << "," << dame << endl;
                for (int j = 0; j < es[x].size(); j++) {
                    Edge &e = game.edge[es[x][j]];
                    int to = uf.find(e.from) == x ? e.to : e.from;
                    to = uf.find(to);
                    if (debug) ofs << " " << to << endl;
                    if (dist.find(to) == dist.end() || dist[to] == dist[x] + 1) {
                        pair<int, int> next = make_pair(to, dame);
                        path_num[next] += path_num[p];
                        nq.insert(next);
                        dist[to] = dist[x] + 1;
                    } else if (dist[to] == d && dame == 0) {
                        pair<int, int> next = make_pair(to, 1);
                        path_num[next] += path_num[p];
                        nq.insert(next);
                    }
                }
            }
            if (debug) ofs << "===" << endl;
            q = nq;
        }

        if (debug) {
            ofs << "PATHNUM" << endl;
            for (map<pair<int, int>, int>::iterator it = path_num.begin(); it != path_num.end(); ++it) {
                ofs << it->first.first << "(" << it->first.second << "):" << it->second << endl;
            }

            ofs << "DIST" << endl;
            for (map<int, int>::iterator it = dist.begin(); it != dist.end(); ++it) {
                ofs << it->first << ":" << it->second << endl;
            }
        }

        map<pair<int, int>, double> minPathScore;
        getMinPathScore(score, minPathScore, path_num, dist, dist_sq[i], es, game, uf, is_bridge, bridge_cnt, enemy, start);
    }

    double maxScore = 0;
    int maxIdx = -1;

    if (debug) ofs << "SCORE" << endl;
    for (int i = 0; i < game.m; i++) {
        if (game.edge[i].owner == -1) {

            if (debug) ofs << i << ":" << score[i] << endl;
            if (maxScore < score[i]) {
                maxScore = score[i];
                maxIdx = i;
            }
        }
    }

    if (maxIdx == -1) {
        for (int i = 0; i < game.m; i++) {
            if (game.edge[i].owner == -1) {
                maxIdx = i;
                break;
            }
        }
    }

    return Result {maxIdx, state};
}

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
    Settings settings;
    switch (state_map.find(command)->second) {
        case HANDSHAKE:
            cout << "pakuri-mu" << endl;
            break;
        case INIT:
            cin >> game >> settings;
            //cerr << game << endl;
            cout << 0 << endl;  // futures
            cout << init(game) << endl;
            break;
        case MOVE:
            cin >> game >> settings >> state;
            result = move(game, state);
            cout << result.edge << '\n' << result.state;
            break;
        case END:
            break;
    }
}