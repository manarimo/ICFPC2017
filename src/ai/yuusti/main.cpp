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
    os << g.punter << '\n' << g.punter_id << '\n' << g.n << '\n' << g.mines << '\n';
    for (int i = 0; i < g.mines; ++i) {
        os << g.mine[i] << (i == g.mines - 1) ? "": " ";
    }
    os << '\n' << g.m << '\n';
    for (int i = 0; i < g.m; ++i) {
        os << g.edge[i].from << ' ' << g.edge[i].to << ' ' << g.edge[i].owner << '\n';
        auto &e = g.edge[i];
    }
    return os;
}

struct State {
    int n;
    vector<int> edge_index;
};

istream &operator>>(istream &is, State &s) {
    is >> s.n;
    s.edge_index.resize(s.n);
    for (int i = 0; i < s.n; ++i) {
        is >> s.edge_index[i];
    }
    return is;
}

ostream &operator<<(ostream &os, const State &s) {
    cout << s.n;
    for (int i = 0; i < s.n; ++i) {
        os << ' ' << s.edge_index[i];
    }
    return os;
}

struct Result {
    int edge;
    State state;
};

State init(Game &game) {
    int size = game.edge.size();
    vector<int> a(size);
    iota(a.begin(), a.end(), 0);
    shuffle(a.begin(), a.end(), mt19937(time(nullptr)));
    return State{size, a};
}

Result move(Game &game, State &state) {
    for (int i = 0; i < state.n; ++i) {
        int idx = state.edge_index[i];
        if (game.edge[idx].owner == -1) {
            return Result {idx, State{state.n - i - 1, vector<int>(state.edge_index.begin() + i + 1, state.edge_index.end())} };
        }
    }

    // Cannot reach here
    return Result{};
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
    cin.tie(0);
    cin.sync_with_stdio(false);

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
            cout << 0 << '\n';  // futures
            cout << init(game) << endl;
            break;
        case MOVE:
            cin >> game >> settings >> state;
            result = move(game, state);
            cout << result.edge << '\n' << result.state << endl;
            break;
        case END:
            break;
    }
}