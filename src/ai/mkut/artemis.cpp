#include <vector>
#include <map>
#include <iostream>
#include <queue>
#include <set>
#include <algorithm>
#include <fstream>
#include <cmath>

using namespace std;

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
        os << g.mine[i] << ((i == g.mines - 1) ? "": " ");
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
    int n; is >> n;
    s.dist.resize(n);
    for (int i = 0; i < n; ++i) {
        int m; is >> m;
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
        set<int> rests; for (int j = 0; j < game.n; j++) rests.insert(j);
        queue<int> q; q.push(game.mine[i]); q.push(-1);
        int d = 0;
        while (q.size() > 1) {
            int x = q.front(); q.pop();
            if (x == -1) {
                d++;
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

double getMinPathScore(vector<double>& score, map<int, double>& minPathScore, map<int, int>& pathNum, map<int, int>& dist, vector<double>& dist2, vector<vector<int> >& es, Game& game, int x) {
    if (minPathScore.find(x) != minPathScore.end()) {
        return minPathScore[x];
    }
    //cerr << "IN" << x << endl;
    double ret = 0;
    ret += dist2[x];
    for (int i = 0; i < es[x].size(); i++) {
        Edge& e = game.edge[es[x][i]];
        int to = e.from == x ? e.to : e.from;
        //cerr << dist[x] << "," << dist[to] << endl;
        if (dist[x] >= dist[to]) continue;
        double nscore = getMinPathScore(score, minPathScore, pathNum, dist, dist2, es, game, to) * pathNum[x] / pathNum[to];
        ret += nscore / game.punter;
        score[es[x][i]] += nscore * pow(1. / game.punter, dist[x]);
        //cerr << "edge" << es[x][i] << "/" << score[es[x][i]] << endl;
    }
    //cerr << "OUT" << x << " " << ret << endl;
    return minPathScore[x] = ret;
}

Result move(Game &game, State &state) {
    vector<vector<double> > potentials(game.n, vector<double>(game.mines));
    for (int i = 0; i < game.mines; i++) {
        potentials[game.mine[i]][i] = 1.;
    }
    vector<vector<set<int> > > sources(game.n, vector<set<int> >(game.mines));
    for (int i = 0; i < game.mines; i++) {
        sources[game.mine[i]][i].insert(game.mine[i]);
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

    vector<vector<int> > es(game.n);
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

//    cerr << "ES" << endl;
//    for (int i = 0; i < es.size(); i++) {
//        for (int j = 0; j < es[i].size(); j++) {
//            cerr << es[i][j] << " ";
//        }
//        cerr << endl;
//    }

    vector<vector<double> > dist2(game.mines, vector<double>(game.n));
    for (int i = 0; i < game.mines; i++) {
        for (map<int, vector<int> >::iterator it = unions.begin(); it != unions.end(); ++it) {
            int x = it->first;
            double score = 0;
            for (int j = 0; j < it->second.size(); j++) {
                int y = it->second[j];
                score += state.dist[i][y] * state.dist[i][y];
            }
            dist2[i][x] = score;
        }
    }

//    cerr << "DIST2" << endl;
//    for (int i = 0; i < dist2.size(); i++) {
//        for (int j = 0; j < dist2[i].size(); j++) {
//            cerr << dist2[i][j] << " ";
//        }
//        cerr << endl;
//    }

    for (int i = 0; i < game.mines; i++) {
        int mine = uf.find(game.mine[i]);
        map<int, int> pathNum;
        map<int, int> dist;
        set<int> vis;
        set<int> q; q.insert(mine);
        pathNum[mine] = 1;
        int d = 0;
        for (int d = 0; !q.empty(); d++) {
            set<int> nq;
            for (set<int>::iterator it = q.begin(); it != q.end(); ++it) {
                vis.insert(*it);
            }
            for (set<int>::iterator it = q.begin(); it != q.end(); ++it) {
                int x = *it;
                //cerr << x << endl;
                dist[x] = d;
                for (int j = 0; j < es[x].size(); j++) {
                    Edge &e = game.edge[es[x][j]];
                    int to = uf.find(e.from) == x ? e.to : e.from;
                    to = uf.find(to);
                    //cerr << " " << to << endl;
                    if (vis.find(to) == vis.end()) {
                        pathNum[to] += pathNum[x];
                        nq.insert(to);
                    }
                }
            }
            //cerr << "===" << endl;
            q = nq;
        }

//        cerr << "PATHNUM" << endl;
//        for (map<int, int>::iterator it = pathNum.begin(); it != pathNum.end(); ++it) {
//            cerr << it->first << ":" << it->second << endl;
//        }
//
//        cerr << "DIST" << endl;
//        for (map<int, int>::iterator it = dist.begin(); it != dist.end(); ++it) {
//            cerr << it->first << ":" << it->second << endl;
//        }

        map<int, double> minPathScore;
        getMinPathScore(score, minPathScore, pathNum, dist, dist2[i], es, game, mine);
    }

    double maxScore = 0;
    int maxIdx = -1;

//    cerr << "SCORE" << endl;
//    for (int i = 0; i < game.m; i++) {
//        if (game.edge[i].owner == -1) {
//            cerr << i << ":" << score[i] << endl;
//            if (maxScore < score[i]) {
//                maxScore = score[i];
//                maxIdx = i;
//            }
//        }
//    }

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
            cout << "artemis" << endl;
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