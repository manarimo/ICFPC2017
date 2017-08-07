#include <vector>
#include <map>
#include <iostream>
#include <queue>
#include <set>
#include <algorithm>
#include <fstream>
#include <cmath>

using namespace std;

bool debug  = false;
//ofstream ofs;
ostream &ofs = cerr;
//ofstream ofs("out", ios_base::out | ios_base::app);

struct Edge {
    int from;
    int to;
    int owner;
    int option;
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
        is >> g.edge[i].from >> g.edge[i].to >> g.edge[i].owner >> g.edge[i].option;
        auto &e = g.edge[i];
    }
    return is;

}

ostream &operator<<(ostream &os, const Game &g) {
    os << g.punter << endl << g.punter_id << endl << g.n << endl << g.mines << endl;
    for (int i = 0; i < g.mines; ++i) {
        os << g.mine[i] << ((i == g.mines - 1) ? "": " ");
    }
    os << endl << g.m << endl;
    for (int i = 0; i < g.m; ++i) {
        os << g.edge[i].from << ' ' << g.edge[i].to << ' ' << g.edge[i].owner << ' ' << g.edge[i].option << endl;
        auto &e = g.edge[i];
    }
    return os;
}

struct State {
    vector<vector<int> > dist;
    int numOption;
    int rTurn;
    vector<int> enemyRTurn;
};

istream &operator>>(istream &is, State &s) {
    int n; is >> n;
    s.dist.resize(n);
    for (int i = 0; i < n; ++i) {
        int m; is >> m;
        s.dist[i].resize(m);
        for (int j = 0; j < m; ++j) {
            is >> s.dist[i][j];
        }
    }
    is >> s.numOption >> s.rTurn;
    int punter; is >> punter;
    s.enemyRTurn.resize(punter);
    for (int i = 0; i < punter; i++) is >> s.enemyRTurn[i];
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
    os << ' ' << s.numOption << ' ' << s.rTurn;
    os << ' ' << s.enemyRTurn.size();
    for (int i = 0; i < s.enemyRTurn.size(); i++) {
        os << ' ' << s.enemyRTurn[i];
    }
    return os;
}

struct Result {
    int edge;
    State state;
};

struct Settings {
    int n;
    bool futures;
    bool splurges;
    bool options;
};

istream &operator>>(istream &is, Settings &settings) {
    is >> settings.n;
    for (int i = 0; i < settings.n; ++i) {
        string str; is >> str;
        if (str == "futures") {
            settings.futures = true;
        } else if (str == "splurges") {
            settings.splurges = true;
        } else if (str == "options") {
            settings.options = true;
        }
    }
    return is;
}

bool canClaim(Edge& e, Settings& settings, int numOption, int punterId) {
    if (e.owner == -1) return true;
    if (!settings.options) return false;
    if (e.option != -1) return false;
    if (e.owner == punterId) return false;
    if (numOption <= 0) return false;
    return true;
}

double openProb(Game& game, Settings& settings, int numOption, int rTurn) {
    if (settings.options) {
        return (1 - (1 - 1. / game.punter) * (1 - 1. / max(1, (game.punter - 1)) * min(1., 1. * numOption / rTurn)));
    } else {
        return 1. / game.punter;
    }
}

double occupiedProb(Game& game, Settings& settings, int numOption, int rTurn) {
    if (settings.options) {
        return 1. / max(1, (game.punter - 1)) * min(1., 1. * numOption / rTurn);
    } else {
        return 0;
    }
}

double edgeProb(Game& game, Settings& settings, int numOption, int rTurn, bool open) {
    return open ? openProb(game, settings, numOption, rTurn) : occupiedProb(game, settings, numOption, rTurn);
}

State init(Game &game) {
    vector<vector<Edge> > es(game.n);
    for (int i = 0; i < game.m; i++) {
        Edge &e = game.edge[i];
        es[e.from].push_back(Edge{e.from, e.to, e.owner});
        es[e.to].push_back(Edge{e.to, e.from, e.owner});
    }

    vector<vector<int> > dist(game.mines, vector<int>(game.n));
    for (int i = 0; i < game.mines; i++) {
        set<int> rests; for (int j = 0; j < game.n; j++) rests.insert(j);
        queue<int> q; q.push(game.mine[i]); q.push(-1);
        int d = 0;
        while (q.size() > 1) {
            int x = q.front(); q.pop();
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
    int expectedTurns = game.m / game.punter + (game.punter_id < game.m % game.punter ? 1 : 0);
    vector<int> enemyNumOption(game.punter, game.mines);
    vector<int> enemyRTurn(game.punter);
    for (int i = 0; i < game.punter; i++) {
        enemyRTurn[i] = game.m / game.punter + (i < game.m % game.punter ? 1 : 0);
    }
    return State{dist, game.mines, expectedTurns, enemyRTurn};
}

struct UnionFind {
    vector<int> data;
    UnionFind(int n) : data(n, -1) {}

    void unite(int x, int y) {
        x = find(x); y = find(y);
        if (x == y) return;
        if (data[x] < data[y]) swap(x, y);
        data[x] += data[y];
        data[y] = x;
    }

    int find(int x) {
        return data[x] < 0 ? x : data[x] = find(data[x]);
    }
};

double getMinPathScore(vector<double>& score, map<pair<int, int>, double>& minPathScore, map<pair<int, int>, double>& pathNum, map<int, int>& dist, vector<double>& dist2, vector<vector<int> >& es, Game& game, Settings& settings, int numOption, int rTurn, bool enemy, UnionFind& uf, pair<int, int> xdame) {
    int x = xdame.first;
    int dame = xdame.second;
    if (minPathScore.find(xdame) != minPathScore.end()) {
        return minPathScore[xdame];
    }
    if (debug) ofs << "IN" << x << "(" << dame << ")" << endl;
    double ret = 0;
    ret += dist2[x];
    if (dist[x] + dame < rTurn) {
        for (int i = 0; i < es[x].size(); i++) {
            Edge& e = game.edge[es[x][i]];
            int to = uf.find(e.from) == x ? e.to : e.from;
            to = uf.find(to);
            if (debug) ofs << dist[x] << "," << dist[to] << endl;
            pair<int, int> next;
            if (dist[x] < dist[to]) {
                next = make_pair(to, dame);
            } else if (dist[x] == dist[to] && dame == 0) {
                next = make_pair(to, 1);
            } else {
                continue;
            }
            double ep = edgeProb(game, settings, numOption, rTurn, e.owner == -1);
            double nscore = getMinPathScore(score, minPathScore, pathNum, dist, dist2, es, game, settings, numOption, rTurn, enemy, uf, next) * pathNum[xdame] * ep / pathNum[next];
            ret += nscore * ep;
            double probEffect;
            if (!enemy) {
                probEffect = 1 - ep;
            } else if (e.owner == -1) {
                probEffect = ep - occupiedProb(game, settings, numOption, rTurn);
            } else {
                probEffect = ep;
            }
            score[es[x][i]] += nscore * pathNum[xdame] * probEffect;
            if (debug) ofs << "edge" << es[x][i] << "/" << score[es[x][i]] << endl;
        }
    }
    if (debug) ofs << "OUT" << x << " " << ret << endl;
    return minPathScore[xdame] = ret;
}

vector<double> edgeScore(Game &game, Settings& settings, vector<vector<int> >& stateDist, int numOption, int rTurn, bool enemy, int punterId) {
    UnionFind uf(game.n);
    for (int i = 0; i < game.m; i++) {
        if (game.edge[i].owner == punterId || game.edge[i].option == punterId) {
            uf.unite(game.edge[i].to, game.edge[i].from);
        }
    }

    map<int, vector<int> > unions;
    for (int i = 0; i < game.n; i++) {
        unions[uf.find(i)].push_back(i);
    }

    vector<double> score(game.m);

    vector<vector<int> > es(game.n);
    for (int i = 0; i < game.m; i++) {
        Edge &e = game.edge[i];
        int from = uf.find(e.from);
        int to = uf.find(e.to);
        if (from == to) {
            continue;
        }
        if (canClaim(e, settings, numOption, punterId)) {
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

    vector<vector<double> > dist2(game.mines, vector<double>(game.n));
    for (int i = 0; i < game.mines; i++) {
        for (map<int, vector<int> >::iterator it = unions.begin(); it != unions.end(); ++it) {
            int x = it->first;
            double score = 0;
            for (int j = 0; j < it->second.size(); j++) {
                int y = it->second[j];
                score += stateDist[i][y] * stateDist[i][y];
            }
            dist2[i][x] = score;
        }
    }

    if (debug) {
        ofs << "DIST2" << endl;
        for (int i = 0; i < dist2.size(); i++) {
            for (int j = 0; j < dist2[i].size(); j++) {
                ofs << dist2[i][j] << " ";
            }
            ofs << endl;
        }
    }

    for (int i = 0; i < game.mines; i++) {
        int mine = uf.find(game.mine[i]);
        pair<int, int> start = make_pair(mine, 0);
        map<pair<int, int>, double> pathNum;
        map<int, int> dist; dist[mine] = 0;
        set<pair<int, int>> q; q.insert(start);
        pathNum[start] = 1;
        int d = 0;
        for (int d = 0; !q.empty(); d++) {
            set<pair<int, int>> nq;
            for (set<pair<int, int> >::iterator it = q.begin(); it != q.end(); ++it) {
                int x = it->first;
                int dame = it->second;
                if (debug) ofs << x << "," << dame << endl;
                for (int j = 0; j < es[x].size(); j++) {
                    Edge &e = game.edge[es[x][j]];
                    int to = uf.find(e.from) == x ? e.to : e.from;
                    to = uf.find(to);
                    if (debug) ofs << " " << to << endl;
                    if (dist.find(to) == dist.end() || dist[to] == dist[x] + 1) {
                        pair<int, int> next = make_pair(to, dame);
                        pathNum[next] += pathNum[*it] * edgeProb(game, settings, numOption, rTurn, e.owner == -1);
                        nq.insert(next);
                        dist[to] = dist[x] + 1;
                    } else if (dist[to] == dist[x] && dame == 0) {
                        pair<int, int> next = make_pair(to, 1);
                        pathNum[next] += pathNum[*it] * edgeProb(game, settings, numOption, rTurn, e.owner == -1);
                        nq.insert(next);
                    }
                }
            }
            if (debug) ofs << "===" << endl;
            q = nq;
        }

        if (debug) {
            ofs << "PATHNUM" << endl;
            for (map<pair<int, int>, double>::iterator it = pathNum.begin(); it != pathNum.end(); ++it) {
                ofs << it->first.first << "(" << it->first.second << "):" << it->second << endl;
            }

            ofs << "DIST" << endl;
            for (map<int, int>::iterator it = dist.begin(); it != dist.end(); ++it) {
                ofs << it->first << ":" << it->second << endl;
            }
        }

        map<pair<int, int>, double> minPathScore;
        getMinPathScore(score, minPathScore, pathNum, dist, dist2[i], es, game, settings, numOption, rTurn, enemy, uf, start);
    }
    return score;
}

Result move(Game &game, Settings& settings, State &state) {
    vector<double> score = edgeScore(game, settings, state.dist, state.numOption, state.rTurn, false, game.punter_id);

    vector<int> enemyNumOption(game.punter, game.mines);
    for (int i = 0; i < game.m; i++) {
        if (game.edge[i].option != -1) {
            enemyNumOption[game.edge[i].option]--;
        }
    }

    if (debug) ofs << "SCORE (me)" << endl;
    for (int i = 0; i < game.m; i++) {
        if (canClaim(game.edge[i], settings, state.numOption, game.punter_id)) {
            if (debug) ofs << i << ":" << score[i] << endl;
        }
    }

    for (int i = 0; i < game.punter; i++) {
        if (i == game.punter_id) continue;
        vector<double> enemyScore = edgeScore(game, settings, state.dist, enemyNumOption[i], state.enemyRTurn[i], true, i);

        if (debug) ofs << "SCORE (" << i << ")" << endl;
        for (int j = 0; j < game.m; j++) {
            if (canClaim(game.edge[i], settings, enemyNumOption[i], i)) {
                if (debug) ofs << j << ":" << enemyScore[j] << endl;
            }
        }

        for (int j = 0; j < game.m; j++) {
            double probEffect;
            if (game.edge[j].owner == -1) {
                probEffect = openProb(game, settings, enemyNumOption[i], state.enemyRTurn[i]) - occupiedProb(game, settings, enemyNumOption[i], state.enemyRTurn[i]);
            } else {
                probEffect = occupiedProb(game, settings, enemyNumOption[i], state.enemyRTurn[i]);
            }
            score[j] += enemyScore[j] * probEffect / max(1, game.punter - 1);
        }
    }

    double maxScore = 0;
    int maxIdx = -1;

    if (debug) ofs << "SCORE" << endl;
    for (int i = 0; i < game.m; i++) {
        if (canClaim(game.edge[i], settings, state.numOption, game.punter_id)) {
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

    if (game.edge[maxIdx].owner != -1) {
        state.numOption -= 1;
    }

    state.rTurn = max(1, state.rTurn - 1);

    for (int i = 0; i < game.punter; i++) {
        state.enemyRTurn[i] = max(1, state.enemyRTurn[i] - 1);
    }

    if (debug) ofs << "numOption=" << state.numOption << endl;

    return Result {maxIdx, state};
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
            cout << "artemis-v4.1" << endl;
            break;
        case INIT:
            cin >> game >> settings;
            cout << 0 << endl;  // futures
            cout << init(game) << endl;
            break;
        case MOVE:
            cin >> game >> settings >> state;
            result = move(game, settings, state);
            cout << result.edge << '\n' << result.state;
            break;
        case END:
            break;
    }
}