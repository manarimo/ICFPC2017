#include <bits/stdc++.h>

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
        os << g.mine[i] << (i == g.mines - 1) ? "": " ";
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
            is >> dist[i][j];
        }
    }
    return is;
}

ostream &operator<<(ostream &os, const State &s) {
    cout << s.dist.size();
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

double connectivity(Game &game, vector<vector<Edge> >& es, vector<double> &connectivity, double p, int mine, int site, set<int> visited, int depth) {
    if (depth <= 0) return;
    if (visited.find(site) != visited.end()) return;
    connectivity[site] += p;
    queue<int> q; q.push_back(site);
    vector<int> newSites;
    while (!q.empty()) {
        int x = q.pop_front();
        if (newVisited.find(x) != newVisited.end()) {
            continue;
        }
        newVisited.insert(x);
        for (int i = 0; i < es[x].size(); i++) {
            if (es[x][i].owner == game.punter_id) {
                q.push_back(es[x][i].to);
            } else {
                newSites.push_back(es[x][i].to);
            }
        }
    }
    for (int i = 0; i < newSites.size(); i++) {
        score2(game, es, score, p / game.punter, mine, newSites[i], newVisited, depth - 1);
    }
}

double score(Game &game, vector<vector<int> >& dist, vector<vector<Edge> >& es) {
    double s = 0;
    for (int i = 0; i < game.mines; i++) {
        int mine = game.mines[i];
        vector<double> connectivity;
        connectivity(game, es, connectivity, 1, mine, mine, set<int>(), 3);
        for (int j = 0; j < game.n; j++) {
            s += connectivity[j] * dist[i][j] * dist[i][j];
        }
    }
    return s;
}

State init(Game &game) {
    vector<vector<Edge> > es(game.n);
    for (int i = 0; i < game.m; i++) {
        Edge &e = game.edge[i];
        es[e.from] = Edge{e.from, e.to, e.owner};
        es[e.to] = Edge{e.to, e.from, e.owner};
    }

    vector<vector<int> > dist(game.mines, vector<int>(game.n));
    for (int i = 0; i < game.mines; i++) {
        set<int> rests; for (int j = 0; j < game.n; j++) rests.insert(j);
        queue<int> q; q.push_back(game.mines[i]); q.push_back(-1);
        int d = 0;
        while (q.size() > 1) {
            int x = q.pop_front();
            if (x == -1) {
                d++;
                continue;
            }
            dist[i][x] = d;
            for (int j = 0; j < es[x].size(); j++) {
                int y = es[x][j].to;
                if (rests.find(y) != rests.end()) {
                    q.push_back(y);
                    rests.remove(y);
                }
            }
        }
    }
    return State{dist};
}

Result move(Game &game, State &state) {

    vector<vector<Edge> > es(game.n);
    for (int i = 0; i < game.m; i++) {
        Edge &e = game.edge[i];
        es[e.from] = Edge{e.from, e.to, e.owner};
        es[e.to] = Edge{e.to, e.from, e.owner};
    }

    double maxScore = 0;
    int maxIdx = 0;

    for (int i = 0; i < game.m; i++) {
        if (game.edge[i].owner == -1) {
            Game newGame = game;
            newGame.edge[i].owner = game.punter_id;
            double s = score(newGame, state.dist, es);
            if (maxScore < s) {
                maxScore = s;
                maxIdx = i;
            }
        }
    }

    return Result {maxIdx, State{state.dist}};
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
            cout << "random" << endl;
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