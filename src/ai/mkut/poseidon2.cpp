#include <vector>
#include <map>
#include <iostream>
#include <queue>
#include <set>
#include <algorithm>

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

void connectivity(Game &game, vector<vector<int> >& es, vector<double>& danger, vector<double> &conn, double p, int mine, int site, set<int>& visited, double limit, Edge& newEdge, int punterId) {
    if (p < limit) return;
    if (visited.find(site) != visited.end()) return;
    queue<int> q; q.push(site);
    map<int, double> newSites;
    while (!q.empty()) {
        int x = q.front(); q.pop();
        if (visited.find(x) != visited.end()) {
            continue;
        }
        conn[x] += p;
        visited.insert(x);
        for (int i = 0; i < es[x].size(); i++) {
            Edge& edge = game.edge[es[x][i]];
            int edgeTo = edge.from == x ? edge.to : edge.from;
            if (edge.owner == punterId || (x == newEdge.from && edgeTo == newEdge.to) || (x == newEdge.to && edgeTo == newEdge.from)) {
                q.push(edgeTo);
            } else if (edge.owner == -1) {
                newSites[edgeTo] = 1 - (1 - newSites[edgeTo]) * (1 - 1. / game.punter) * (1 - danger[es[x][i]]);
            }
        }
    }
    for (map<int, double>::iterator it = newSites.begin(); it != newSites.end(); it++) {
        connectivity(game, es, danger, conn, p * it->second, mine, it->first, visited, limit, newEdge, punterId);
    }
}

double score(Game &game, vector<vector<int> >& dist, vector<vector<int> >& es, vector<double>& danger, Edge& newEdge, int punterId) {
    double s = 0;
    for (int i = 0; i < game.mines; i++) {
        int mine = game.mine[i];
        vector<double> conn(game.n);
        set<int> visited;
        connectivity(game, es, danger, conn, 1, mine, mine, visited, 1e-4, newEdge, punterId);
        //for (int j = 0; j < game.n; j++) cerr << conn[j] << " "; cerr << endl;
        for (int j = 0; j < game.n; j++) {
            s += conn[j] * dist[i][j] * dist[i][j];
        }
    }
    return s;
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

Result move(Game &game, State &state) {

    vector<vector<int> > es(game.n);
    for (int i = 0; i < game.m; i++) {
        Edge &e = game.edge[i];
        es[e.from].push_back(i);
        es[e.to].push_back(i);
    }

    double maxScore = 0;
    int maxIdx = -1;

    vector<double> noDanger(game.m, 0);
    vector<double> danger(game.m, 0);
    for (int p = 0; p < game.punter; p++) {
        if (p == game.punter_id) continue;
        vector<double> pDanger(game.m);
        double sumDanger = 0;
        for (int i = 0; i < game.m; i++) {
            if (game.edge[i].owner == -1) {
                double s = score(game, state.dist, es, noDanger, game.edge[i], p);
                sumDanger += s * s;
                pDanger[i] = s * s;
            }
        }
        for (int i = 0; i < game.m; i++) {
            danger[i] = 1 - (1 - danger[i]) * (1 - pDanger[i] / sumDanger);
        }
    }

    for (int i = 0; i < game.m; i++) {
        if (game.edge[i].owner == -1) {
            double s = score(game, state.dist, es, danger, game.edge[i], game.punter_id);
            //cerr << i << ":" << s << endl;
            if (maxScore < s) {
                maxScore = s;
                maxIdx = i;
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
            cout << "poseidon" << endl;
            break;
        case INIT:
            cin >> game >> settings;
            cerr << game << endl;
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