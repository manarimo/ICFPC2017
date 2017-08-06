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
    vector<vector<double> > potentials(game.n, vector<double>(game.mines));
    for (int i = 0; i < game.mines; i++) {
        potentials[game.mine[i]][i] = 1.;
    }
    vector<vector<set<int> > > sources(game.n, vector<set<int> >(game.mines));
    for (int i = 0; i < game.mines; i++) {
        sources[game.mine[i]][i].insert(game.mine[i]);
    }

    int depth = 10;
    while (depth--) {
        vector<vector<double> > nextPotentials = potentials;
        vector<vector<set<int> > > nextSources = sources;
        for (int i = 0; i < game.mines; i++) {
            nextPotentials[game.mine[i]][i] = 1.;
        }
        for (int i = 0; i < game.m; i++) {
            Edge &e = game.edge[i];
            for (int j = 0; j < game.mines; j++) {
                double p = 0;
                if (e.owner == game.punter_id) {
                    p = 1;
                } else if (e.owner == -1) {
                    p = 1. / game.punter;
                }
                int a = e.from;
                int b = e.to;
                if (sources[a][j].find(b) == sources[a][j].end()) {
                    if (sources[a][j].find(game.mine[j]) != sources[a][j].end()) {
                        nextSources[a][j].insert(b);
                        nextSources[b][j].insert(b);
                    }
                    nextPotentials[b][j] = 1 - (1 - nextPotentials[b][j]) * (1 - potentials[a][j] * p);
                }
                if (sources[b][j].find(a) == sources[b][j].end()) {
                    if (sources[b][j].find(game.mine[j]) != sources[b][j].end()) {
                        nextSources[b][j].insert(a);
                        nextSources[a][j].insert(a);
                    }
                    nextPotentials[a][j] = 1 - (1 - nextPotentials[a][j]) * (1 - potentials[b][j] * p);
                }
                nextSources[b][j].insert(sources[a][j].begin(), sources[a][j].end());
                nextSources[a][j].insert(sources[b][j].begin(), sources[b][j].end());
            }
        }
        potentials = nextPotentials;
        sources = nextSources;
    }

    double maxScore = 0;
    int maxIdx = -1;

    for (int i = 0; i < game.m; i++) {
        if (game.edge[i].owner == -1) {
            double score = 0;
            for (int j = 0; j < game.mines; j++) {
                int a = game.edge[i].from;
                int b = game.edge[i].to;
                double pa = (1 - potentials[a][j]) * potentials[b][j];
                double pb = potentials[a][j] * (1 - potentials[b][j]);
                int da = state.dist[j][a];
                int db = state.dist[j][b];
                score += pa * da * da + pb * db * db;
            }
            //cerr << i << ":" << score;
            if (maxScore < score) {
                maxScore = score;
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
            cout << "artemis" << endl;
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