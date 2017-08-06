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
    bool used;
    
    Edge(int to, int id, bool used) : to(to), id(id), used(used) {}
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

class UnionFind {
    public:
    void init(int n) {
        parent.resize(n);
        weight.resize(n);
        for (int i = 0; i < n; i++) parent[i] = -1;
    }
    
    void add_weight(int x, int w) {
        weight[find(x)] += w;
    }
    
    int find(int x) {
        if (parent[x] < 0) return x;
        return parent[x] = find(parent[x]);
    }
    
    void unite(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) return;
        if (parent[x] > parent[y]) swap(x, y);
        parent[x] += parent[y];
        parent[y] = x;
        weight[x] += weight[y];
    }
    
    bool same(int x, int y) {
        return find(x) == find(y);
    }
    
    bool used(int x) {
        return get_size(x) > 1;
    }
    
    int get_size(int x) {
        return -parent[find(x)];
    }
    
    int get_weight(int x) {
        return weight[find(x)];
    }
    
    private:
    vector<int> parent;
    vector<int> weight;
} uf;

vector<vector<Edge>> graph;
vector<int> degree;
vector<vector<int>> all_dist;

void input(bool read_state) {
    int n, m, mine, setting;
    
    scanf("%d", &punter);
    scanf("%d", &punter_id);
    
    scanf("%d", &n);
    scanf("%d", &mine);
    mines.init(n);
    uf.init(n);
    for (int i = 0; i < mine; i++) {
        int v;
        scanf("%d", &v);
        mines.add_mine(v);
        uf.add_weight(v, 1);
    }
    
    scanf("%d", &m);
    graph.resize(n);
    degree.resize(n);
    for (int i = 0; i < m; i++) {
        int from, to, owner;
        scanf("%d %d %d", &from, &to, &owner);
        
        if (owner == -1) {
            degree[from]++;
            degree[to]++;
            graph[from].push_back(Edge(to, i, false));
            graph[to].push_back(Edge(from, i, false));
        } else if (owner == punter_id) {
            graph[from].push_back(Edge(to, i, true));
            graph[to].push_back(Edge(from, i, true));
            uf.unite(from, to);
        }
    }
    
    scanf("%d", &setting);
    for (int i = 0; i < setting; i++) {
        char option[10];
        scanf("%s", option);
    }
    
    all_dist = vector<vector<int>>(mine, vector<int>(n));
    if (read_state) {
        for (int i = 0; i < mine; i++) {
            for (int j = 0; j < n; j++) {
                int d;
                scanf("%d", &d);
                all_dist[i][j] = d;
            }
        }
    }
}

void output(const vector<pair<int, int>>& futures) {
    printf("%d", futures.size());
    for (int i = 0; i < futures.size(); i++) printf(" %d %d", futures[i].first, futures[i].second);
    puts("");
    for (int i = 0; i < mines.get_count(); i++) {
        for (int j = 0; j < graph.size(); j++) {
            printf("%d ", all_dist[i][j]);
        }
    }
    puts("");
    exit(0);
}

void output(int id) {
    printf("%d\n", id);
    for (int i = 0; i < mines.get_count(); i++) {
        for (int j = 0; j < graph.size(); j++) {
            printf("%d ", all_dist[i][j]);
        }
    }
    puts("");
    exit(0);
}

void handshake() {
    puts("kawatea-bridge");
}

void init() {
    int num = 0;
    queue<int> q;
    
    for (int mine : mines.get_mines()) {
        for (int i = 0; i < graph.size(); i++) all_dist[num][i] = INF;
        all_dist[num][mine] = 0;
        q.push(mine);
        
        while (!q.empty()) {
            int last = q.front();
            q.pop();
            
            for (const Edge& edge : graph[last]) {
                int next = edge.to;
                if (all_dist[num][next] == INF) {
                    all_dist[num][next] = all_dist[num][last] + 1;
                    q.push(next);
                }
            }
        }
        
        num++;
    }
    
    output(vector<pair<int, int>>());
}

vector<double> calc_prob() {
    vector<bool> visited(graph.size());
    vector<double> prob(graph.size());
    deque<int> q;
    
    for (int mine : mines.get_mines()) {
        if (uf.used(mine)) {
            prob[mine] = 1;
        } else {
            prob[mine] = 0.3;
        }
        q.push_back(mine);
    }
    
    while (!q.empty()) {
        int last = q.front(), count = 1;
        q.pop_front();
        
        if (visited[last]) continue;
        visited[last] = true;
        if (prob[last] < 0.1) continue;
        
        for (const Edge& edge : graph[last]) {
            if (!visited[edge.to] && !edge.used) count++;
        }
        
        for (const Edge& edge : graph[last]) {
            int next = edge.to;
            if (visited[next]) continue;
            if (edge.used) {
                prob[next] = min(prob[next] + prob[last], 1.0);
                q.push_front(next);
            } else {
                prob[next] = min(prob[next] + prob[last] / count, 0.99);
                q.push_back(next);
            }
        }
    }
    
    return prob;
}

pair<int, int> calc_bridge(int from, int to) {
    int count = 0;
    vector<bool> visited1(graph.size()), visited2(graph.size());
    vector<int> dist1(graph.size()), dist2(graph.size());
    deque<int> q1, q2;
    
    for (int i = 0; i < graph.size(); i++) dist1[i] = dist2[i] = INF;
    dist1[from] = 0;
    visited1[to] = true;
    q1.push_back(from);
    dist2[to] = 0;
    visited2[from] = true;
    q2.push_back(to);
    
    while (!q1.empty() || !q2.empty()) {
        if (!q1.empty()) {
            int last = q1.front();
            q1.pop_front();
            
            if (visited1[last]) continue;
            visited1[last] = true;
            if (visited2[last]) return make_pair(dist1[last] + dist2[last], count);
            
            for (const Edge& edge : graph[last]) {
                int next = edge.to;
                if (visited1[next]) continue;
                if (edge.used) {
                    if (dist1[next] > dist1[last]) {
                        dist1[next] = dist1[last];
                        q1.push_front(next);
                    }
                } else {
                    if (dist1[next] > dist1[last] + 1) {
                        dist1[next] = dist1[last] + 1;
                        q1.push_back(next);
                    }
                }
            }
        }
        
        if (!q2.empty()) {
            int last = q2.front();
            q2.pop_front();
            
            if (visited2[last]) continue;
            visited2[last] = true;
            if (visited1[last]) return make_pair(dist1[last] + dist2[last], count);
            if (!uf.used(last)) count++;
            if (mines.is_mine(last)) count += graph.size();
            
            for (const Edge& edge : graph[last]) {
                int next = edge.to;
                if (visited2[next]) continue;
                if (edge.used) {
                    if (dist2[next] > dist2[last]) {
                        dist2[next] = dist2[last];
                        q2.push_front(next);
                    }
                } else {
                    if (dist2[next] > dist2[last] + 1) {
                        dist2[next] = dist2[last] + 1;
                        q2.push_back(next);
                    }
                }
            }
        }
    }
    
    return make_pair(INF, count);
}

void bridge(const vector<pair<double, int>>& order) {
    int id = -1;
    double best = 0;
    vector<bool> used(graph.size());
    
    for (int i = 0; i < order.size(); i++) {
        int last = order[i].second;
        used[last] = true;
        for (const Edge& edge : graph[last]) {
            int next = edge.to;
            if (edge.used || used[next] || degree[next] <= 2) continue;
            pair<int, int> bridge = calc_bridge(last, next);
            if (bridge.first * order[i].first > 3) {
                double now = bridge.second * order[i].first;
                if (now > best) {
                    best = now;
                    id = edge.id;
                }
            }
        }
    }
    
    if (best > 0) output(id);
}

void connect(const vector<pair<double, int>>& order) {
    vector<bool> visited(graph.size());
    vector<int> dist(graph.size());
    vector<int> parent(graph.size());
    deque<int> q;
    
    for (const pair<double, int>& start : order) {
        if (visited[start.second]) continue;
        
        for (int i = 0; i < graph.size(); i++) dist[i] = INF;
        dist[start.second] = 0;
        parent[start.second] = -1;
        q.push_back(start.second);
        
        while (!q.empty()) {
            int last = q.front();
            q.pop_front();
            
            if (visited[last]) continue;
            visited[last] = true;
            
            if (dist[last] > 0 && (mines.is_mine(last) || uf.used(last))) output(parent[last]);
            
            for (const Edge& edge : graph[last]) {
                int next = edge.to;
                if (visited[next]) continue;
                if (edge.used) {
                    if (dist[next] > dist[last]) {
                        dist[next] = dist[last];
                        parent[next] = parent[last];
                        q.push_front(next);
                    }
                } else {
                    if (dist[next] > dist[last] + 1) {
                        dist[next] = dist[last] + 1;
                        if (parent[last] == -1) {
                            parent[next] = edge.id;
                        } else {
                            parent[next] = parent[last];
                        }
                        q.push_back(next);
                    }
                }
            }
        }
    }
}

void extend() {
    int id = -1;
    long long best = 0;
    
    for (int i = 0; i < graph.size(); i++) {
        int last = i;
        if (!uf.used(last)) continue;
        
        for (const Edge& edge : graph[last]) {
            int next = edge.to, num = 0;
            long long profit = 0;
            if (edge.used || uf.same(last, next)) continue; 
            for (int mine : mines.get_mines()) {
                if (uf.same(last, mine)) profit += all_dist[num][last] * all_dist[num][last];
                num++;
            }
            if (profit > best) {
                best = profit;
                id = edge.id;
            }
        }
    }
    
    if (best > 0) output(id);
}

void prevent() {
    for (int i = 0; i < graph.size(); i++) {
        for (const Edge& edge : graph[i]) {
            if (!edge.used) output(edge.id);
        }
    }
}

void move() {
    vector<double> prob = calc_prob();
    vector<pair<double, int>> order;
    for (int i = 0; i < graph.size(); i++) {
        if (prob[i] >= 0.1) order.push_back(make_pair(prob[i], i));
    }
    sort(order.begin(), order.end(), greater<pair<double, int>>());
    
    bridge(order);
    connect(order);
    extend();
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
