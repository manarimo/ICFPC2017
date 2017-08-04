#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace std;

struct Game {
    int punter;
    int punter_id;
    int n;
    int mines;
    vector<int> mine;
    int m;
    vector<pair<int, int>> edge;
    vector<int> edge_state;
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
    for (int i = 0; i < g.m; ++i) {
        input >> g.edge[i].first >> g.edge[i].second;
    }
    g.edge_state.resize(g.m);
    for (int i = 0; i < g.m; ++i) {
        input >> g.edge_state[i];
    }
}

ostream& operator<<(ostream& os, const Game& g) {
    cout << "game!!!" << endl;
}

struct State {
    string s;
};

istream &operator>>(istream &input, State &s) {
    input >> s.s;
}

ostream& operator<<(ostream& os, const State &s) {
    cout << s.s << endl;
}

struct Result {
    int edge;
    State state;
};

Result doMove(Game &g, State &s) {
    for (int i = 0; i < g.edge_state.size(); ++i) {
        if (g.edge_state[i] != -1) return {i, s};
    }
    return {-1, s};
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
            cout << "eager" << endl;
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