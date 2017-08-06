#include <iostream>
#include <vector>
#include <set>

using namespace std;

struct River {
    int id, from, to, owner;
};

int P, V, E, M;
int myId;
set<int> mines;
vector<vector<River>> graph;
vector<bool> owned;
bool useFuture, useSplurge;
int timeCounter;

void readMap() {
    cin >> P >> myId >> V;
    cin >> M;
    for (int i = 0; i < M; ++i) {
        int m;
        cin >> m;
        mines.insert(m);
    }

    graph.resize(V);
    cin >> E;
    for (int i = 0; i < E; ++i) {
        River r;
        r.id = i;
        cin >> r.from >> r.to >> r.owner;
        graph[r.from].push_back(r);
        graph[r.to].push_back(River{r.id, r.to, r.from, r.owner});
    }
}

void init() {
    readMap();
    int C;
    cin >> C;
    for (int i = 0; i < C; ++i) {
        string s;
        cin >> s;
        cerr << s << endl;
        if (s == "splurge") {
            useSplurge = true;
        }
    }
}

void dump() {
    cout << (useSplurge ? 1 : 0) << endl;
    cout << timeCounter << endl;
}

void readState() {
    cin >> useSplurge >> timeCounter;
}

void calcOwned() {
    owned.resize(V);
    for (auto mine : mines) {
        owned[mine] = true;
    }
    for (const auto &row : graph) {
        for (const River &r : row) {
            if (r.owner == myId) {
                owned[r.from] = owned[r.to] = true;
            }
        }
    }
}

vector<int> tap(int start) {
    vector<int> dist(V, -1);
    vector<River> prev(V);
    vector<int> q[2];
    q[0].push_back(start);
    dist[start] = 0;
    while (q[0].size() > 0) {
        q[1].clear();
        for (const auto from : q[0]) {
            for (const auto &r : graph[from]) {
                if (r.owner != -1) continue;
                if (dist[r.to] != -1) continue;
                dist[r.to] = dist[from] + 1;
                prev[r.to] = r;
                q[1].push_back(r.to);
            }
        }
        swap(q[0], q[1]);
    }
    int maxMine = -1;
    for (int mine = 0; mine < V; ++mine) {
        if (start == mine) continue;
        if (!owned[mine]) continue;
        if (dist[mine] != -1 && (maxMine == -1 || dist[mine] > dist[maxMine])) {
            maxMine = mine;
        }
    }

    vector<int> res;
    if (maxMine != -1) {
        int curr = maxMine;
        res.push_back(curr);
        while (dist[curr] > 0) {
            res.push_back(prev[curr].from);
            curr = prev[curr].from;
        }
    }
    return res;
}

int greedy() {
    vector<int> score(V, 0);
    vector<int> q[2];
    for (auto mine : mines) {
        vector<bool> visited(V, false);
        q[0].clear();
        q[0].push_back(mine);
        visited[mine] = true;
        int dist = 0;
        while (q[0].size() > 0) {
            q[1].clear();
            ++dist;
            for (auto from : q[0]) {
                for (const River &r : graph[from]) {
                    if (r.owner != -1 && r.owner != myId) continue;
                    if (visited[r.to]) continue;
                    score[r.to] += dist * dist;
                    visited[r.to] = true;
                    q[1].push_back(r.to);
                }
            }
            swap(q[0], q[1]);
        }
    }

    int ans = -1;
    int ansScore = 0;
    for (const auto &row : graph) {
        for (const River &r : row) {
            if (r.owner != -1) continue;
            if (owned[r.from] && !owned[r.to] && score[r.to] > ansScore) {
                ans = r.id;
                ansScore = score[r.to];
            }
        }
    }
    cout << ans << endl;
    return ans;
}

void play() {
    calcOwned();
    if (useSplurge) {
        ++timeCounter;

        vector<int> answer;
        for (const auto start : mines) {
            vector<int> tmp = tap(start);
            cerr << "timeCounter " << timeCounter << " size " << tmp.size() << endl;
            if (tmp.size() > answer.size()) {
                answer = tmp;
            }
            if ((int)answer.size() - 1 > timeCounter) {
                cout << -1 << endl;
                return;
            }
        }

        if (answer.size() >= 2) {
            cerr << "Time to splurge!!!" << endl;
            cout << -2 << endl;
            for (int id : answer) {
                cout << id << ' ';
            }
            cout << endl;
            timeCounter = 0;
            return;
        }
    }
    if (greedy() != -1) {
        timeCounter = 0;
    }
}

int main() {
    string s;
    cin >> s;
    if (s == "HANDSHAKE") {
        cout << "Time Vault" << endl;
    } else if (s == "INIT") {
        init();
        cout << 0 << endl; // Number of futures
        dump();
    } else if (s == "MOVE") {
        init();
        readState();
        play();
        dump();
    }
}