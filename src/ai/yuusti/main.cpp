#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
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
    vector<vector<pair<int, int>>> graph;
};

enum Command {
    HANDSHAKE,
    INIT,
    MOVE,
    END
};

istream &operator>>(istream &input, Game &g) {
    input >> g.punter >> g.punter_id >> g.n >> g.mines;
    g.mine.resize(g.mines);
    for (int i = 0; i < g.mines; ++i) {
        input >> g.mine[i];
    }
    input >> g.m;
    g.edge.resize(g.m);
    g.graph.resize(g.n);
    for (int i = 0; i < g.m; ++i) {
        input >> g.edge[i].from >> g.edge[i].to >> g.edge[i].owner;
        auto &e = g.edge[i];

        if (e.owner != -1 || e.owner != g.punter_id) continue;
        g.graph[e.from].push_back(make_pair(e.to, i));
        g.graph[e.to].push_back(make_pair(e.from, i));
    }
}

ostream& operator<<(ostream& os, const Game& g) {
    cout << "game!!!" << endl;
}

struct State {
    int cur, goal, start;
};

istream &operator>>(istream &input, State &s) {
    input >> s.cur >> s.goal >> s.start;
}

ostream& operator<<(ostream& os, const State &s) {
    cout << s.cur << ' ' << s.goal << ' ' << s.start << endl;
}

struct Result {
    int edge;
    State state;
};

const int INF = 1e9;

vector<int> bfs(Game &game, int v) {
    int n = game.n;
    vector<int> dist(n, INF);
    auto &e = game.edge;
    auto &G = game.graph;

    queue<pair<int, int>> q;
    q.push(make_pair(v, 0));
    dist[v] = 0;
    while (!q.empty()) {
        auto &p = q.front();
        q.pop();

        for (auto &p: G[p.first]) {
            int v = p.first;
            if (dist[v] != INF) {
                dist[v] = p.second + 1;
                q.emplace(v, p.second);
            }
        }
    }

    return dist;
}

State init(Game &game) {
    pair<int, pair<int, int>> max_dist = make_pair(-1, make_pair(0, 0));
    for (auto &x : game.mine) {
        auto d = bfs(game, x);
        auto y = max_element(d.begin(), d.end());
        max_dist = max(max_dist, make_pair(d[y - d.begin()], make_pair(x, y - d.begin())));
    }

    auto &p = max_dist.second;
    return {p.first, p.second};
}

Result doMove(Game &game, State &state) {
    auto dist = bfs(game, state.cur);
    auto dist_from_goal = bfs(game, state.goal);
    if (dist[state.goal] == INF) {
        auto &e = game.edge;
        for (int i = 0; i < e.size(); ++i) {
            if (e[i].owner == -1) {
                state.cur = i;
                return {i, state};
            }
        }
    } else if (state.cur != state.goal) {
        auto &g = game.graph;
        for (auto &to : g[state.cur]) {
            if (dist_from_goal[to.first] < dist_from_goal[state.cur]) {
                state.cur = to.first;
                return {to.second, state};
            }
        }
    } else {
        state.cur = state.start;
        state.goal = max_element(dist_from_goal.begin(), dist_from_goal.end()) - dist_from_goal.begin();
    }
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
    switch (state_map.find(command)->second) {
        case HANDSHAKE:
            cout << "yunter" << endl;
            break;
        case INIT:
            cin >> game;
            cout << "hello" << endl;
            break;
        case MOVE:
            cin >> game >> state;
            result = doMove(game, state);
            cout << result.edge << ' ' << result.state;
            break;
        case END:
            break;
    }
}