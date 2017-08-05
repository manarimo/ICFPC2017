#include <cstdio>
#include <cstdlib>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

const int INF = 1e9;
int punter, punter_id;

struct Edge {
    int to;
    int id;
    int owner;
    
    Edge(int to, int id, int owner) : to(to), id(id), owner(owner) {}
    
    bool is_free() const {
        return owner == -1;
    }
    
    bool belongs() const {
        return owner == punter_id;
    }
};

class Mine {
    public:
    void init(int n) {
        mine.resize(n);
    }
    
    void add_mine(int v) {
        mine[v] = true;
        mines.push_back(v);
    }
    
    bool is_mine(int v) {
        return mine[v];
    }
    
    int get_count() {
        return mines.size();
    }
    
    const vector<int>& get_mines() {
        return mines;
    }
    
    private:
    vector<bool> mine;
    vector<int> mines;
} mines;

class State {
    public:
    void next_stage() {
        switch (stage) {
            case 0: stage = 1; break;
            case 1: stage = 2; break;
            case 2: index++; index == mines.size() ? stage = 3 : stage = 1; break;
        }
    }
    
    void set_stage(int stage) {
        this->stage = stage;
    }
    
    int get_stage() {
        return stage;
    }
    
    void set_index(int index) {
        this->index = index;
    }
    
    int get_index() {
        return index;
    }
    
    void add_mine(int v) {
        mines.push_back(v);
    }
    
    int get_mine() {
        return mines[index];
    }
    
    const vector<int>& get_mines() {
        return mines;
    }
    
    void output() {
        printf("%d %d", stage, index);
        for (int i = 0; i < mines.size(); i++) printf(" %d", mines[i]);
        puts("");
    }
    
    private:
    int stage = 0;
    int index = 0;
    vector<int> mines;
} state;

vector<vector<Edge>> graph;
vector<int> degree;
vector<int> used;

void input(bool read_state) {
    int n, m, mine;
    
    scanf("%d", &punter);
    scanf("%d", &punter_id);
    
    scanf("%d", &n);
    
    scanf("%d", &mine);
    mines.init(n);
    for (int i = 0; i < mine; i++) {
        int v;
        scanf("%d", &v);
        mines.add_mine(v);
    }
    
    graph.resize(n);
    degree.resize(n);
    used.resize(n);
    scanf("%d", &m);
    for (int i = 0; i < m; i++) {
        int from, to, owner;
        scanf("%d %d %d", &from, &to, &owner);
        
        graph[from].push_back(Edge(to, i, owner));
        graph[to].push_back(Edge(from, i, owner));
        
        if (owner == -1) {
            degree[from]++;
            degree[to]++;
        } else if (owner == punter_id) {
            used[from] = used[to] = 1;
        }
    }
    
    if (read_state) {
        int stage, index;
        scanf("%d %d", &stage, &index);
        state.set_stage(stage);
        state.set_index(index);
        
        for (int i = 0; i < mine; i++) {
            int v;
            scanf("%d", &v);
            state.add_mine(v);
        }
    }
}

void output(int edge_id) {
    if (edge_id != -1) printf("%d\n", edge_id);
    state.output();
    exit(0);
}

void handshake() {
    puts("kawatea-cherry-pick");
}

void init() {
    vector<pair<int, pair<int, int>>> order;
    vector<int> dist(graph.size());
    queue<int> q;
    
    for (int mine : mines.get_mines()) {
        int last = -1, connected = 0;
        
        for (int i = 0; i < graph.size(); i++) dist[i] = INF;
        dist[mine] = 0;
        q.push(mine);
        
        while (!q.empty()) {
            last = q.front();
            connected++;
            q.pop();
            
            for (const Edge& edge : graph[last]) {
                int next = edge.to;
                if (dist[next] == INF) {
                    dist[next] = dist[last] + 1;
                    q.push(next);
                }
            }
        }
        
        order.push_back(make_pair(-connected, make_pair(dist[last], mine)));
    }
    
    sort(order.begin(), order.end());
    for (int i = 0; i < order.size(); i++) state.add_mine(order[i].second.second);
    
    output(-1);
}

void cherry_pick() {
    for (int mine : state.get_mines()) {
        int best = 0, id = -1;
        if (used[mine] == 1) continue;
        
        for (const Edge& edge: graph[mine]) {
            if (!edge.is_free()) continue;
            if (degree[edge.to] > best) {
                best = degree[edge.to];
                id = edge.id;
            }
        }
        
        if (best - 2 >= degree[mine]) output(id);
    }
    
    state.next_stage();
}

void color(int time) {
    int start = state.get_mine();
    vector<int> dist(graph.size());
    queue<int> q;
    
    for (int i = 0; i < graph.size(); i++) dist[i] = INF;
    dist[start] = 0;
    used[start] = time;
    q.push(start);
    
    while (!q.empty()) {
        int last = q.front();
        q.pop();
        
        for (const Edge& edge : graph[last]) {
            int next = edge.to;
            if (dist[next] == INF && edge.belongs()) {
                dist[next] = 0;
                used[next] = time;
                q.push(next);
            }
        }
    }
}

void connect(int time) {
    vector<int> dist(graph.size());
    vector<int> parent(graph.size());
    queue<int> q;
    
    for (int i = 0; i < graph.size(); i++) dist[i] = INF;
    for (int i = 0; i < graph.size(); i++) {
        if (used[i] == time) {
            dist[i] = 0;
            parent[i] = -1;
            q.push(i);
        }
    }
    
    while (!q.empty()) {
        int last = q.front();
        q.pop();
        
        if (used[last] != time && used[last] != 0) output(parent[last]);
        if (used[last] == 0 && mines.is_mine(last)) output(parent[last]);
        
        for (const Edge& edge : graph[last]) {
            int next = edge.to;
            if (dist[next] == INF && edge.is_free()) {
                if (used[last] == time) {
                    parent[next] = edge.id;
                } else {
                    parent[next] = parent[last];
                }
                dist[next] = dist[last] + 1;
                q.push(next);
            }
        }
    }
    
    state.next_stage();
}

void extend(int time) {
    int id = -1;
    long long best_profit = 0, best_sum = 0;
    vector<int> dist(graph.size());
    vector<long long> profit(graph.size());
    queue<int> q;
    
    for (int mine : mines.get_mines()) {
        if (used[mine] != time) continue;
        
        for (int i = 0; i < graph.size(); i++) dist[i] = INF;
        dist[mine] = 0;
        q.push(mine);
        
        while (!q.empty()) {
            int last = q.front();
            q.pop();
            
            profit[last] += (long long)dist[last] * dist[last];
            
            for (const Edge& edge : graph[last]) {
                int next = edge.to;
                if (dist[next] == INF) {
                    dist[next] = dist[last] + 1;
                    q.push(next);
                }
            }
        }
    }
    
    for (int i = 0; i < graph.size(); i++) {
        int last = i;
        if (used[last] != time) continue;
        
        for (const Edge& edge : graph[last]) {
            int next = edge.to;
            if (used[next] == 0 && edge.is_free()) {
                long long sum = 0;
                for (const Edge& edge : graph[next]) {
                    if (used[edge.to] == 0 && edge.is_free()) sum += profit[edge.to];
                }
                if (profit[next] > best_profit || (profit[next] == best_profit && sum > best_sum)) {
                    best_profit = profit[next];
                    best_sum = sum;
                    id = edge.id;
                }
            }
        }
    }
    
    if (best_profit > 0) {
        output(id);
    } else {
        state.next_stage();
    }
}

void prevent() {
    for (int i = 0; i < graph.size(); i++) {
        for (const Edge& edge : graph[i]) {
            if (edge.is_free()) output(edge.id);
        }
    }
}

void move() {
    if (state.get_stage() == 0) cherry_pick();
    for (int time = 2; state.get_stage() != 3; time++) {
        color(time);
        if (state.get_stage() == 1) connect(time);
        if (state.get_stage() == 2) extend(time);
    }
    prevent();
}

void end() {
}

int main() {
    char protocol[10];
    scanf("%s", protocol);
    
    if (protocol[0] == 'H') {
        // Handshake
        handshake();
    } else if (protocol[0] == 'I') {
        // Init
        input(false);
        init();
    } else if (protocol[0] == 'M') {
        // Move
        input(true);
        move();
    } else {
        // End
        end();
    }
    
    return 0;
}
