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
        for (int i = 0; i < n; i++) mine[i] = -1;
    }
    
    void add_mine(int v, int num) {
        mine[v] = num;
        mines.push_back(v);
    }
    
    bool is_mine(int v) {
        return mine[v] >= 0;
    }
    
    int get_num(int v) {
        return mine[v];
    }
    
    int get_count() {
        return mines.size();
    }
    
    const vector<int>& get_mines() {
        return mines;
    }
    
    private:
    vector<int> mine;
    vector<int> mines;
} mines;

class UnionFind {
    public:
    void init(int n) {
        parent.resize(n);
        mines.resize(n);
        for (int i = 0; i < n; i++) parent[i] = -1;
    }
    
    void add_mine(int x, int num) {
        mines[x].push_back(num);
    }
    
    int find(int x) {
        if (parent[x] < 0) return x;
        return parent[x] = find(parent[x]);
    }
    
    void unite(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) return;
        if (mines[x].size() < mines[y].size()) swap(x, y);
        parent[x] += parent[y];
        parent[y] = x;
        for (int mine : mines[y]) mines[x].push_back(mine);
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
    
    const vector<int>& get_mines(int x) {
        return mines[find(x)];
    }
    
    private:
    vector<int> parent;
    vector<vector<int>> mines;
} uf;

vector<vector<Edge>> graph;
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
        mines.add_mine(v, i);
        uf.add_mine(v, i);
    }
    
    scanf("%d", &m);
    graph.resize(n);
    for (int i = 0; i < m; i++) {
        int from, to, owner;
        scanf("%d %d %d", &from, &to, &owner);
        
        if (owner == -1) {
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

void output_state() {
    for (int i = 0; i < mines.get_count(); i++) {
        for (int j = 0; j < graph.size(); j++) {
            printf("%d ", all_dist[i][j]);
        }
    }
    puts("");
    exit(0);
}

void output(const vector<pair<int, int>>& futures) {
    printf("%d", futures.size());
    for (int i = 0; i < futures.size(); i++) printf(" %d %d", futures[i].first, futures[i].second);
    puts("");
    output_state();
}

void output(int id) {
    printf("%d\n", id);
    output_state();
}

void handshake() {
    puts("kawatea-careful");
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

int calc_dist(int v1, int v2, int from, int to) {
    vector<bool> visited(graph.size());
    vector<int> dist(graph.size());
    deque<int> q;
    
    for (int i = 0; i < graph.size(); i++) dist[i] = INF;
    dist[v1] = 0;
    q.push_back(v1);
    
    while (!q.empty()) {
        int last = q.front();
        q.pop_front();
        
        if (last == v2) return dist[last];
        if (visited[last]) continue;
        visited[last] = true;
        
        for (const Edge& edge : graph[last]) {
            int next = edge.to;
            if ((last == from && next == to) || (last == to && next == from)) continue;
            if (visited[next]) continue;
            if (edge.used) {
                if (dist[next] > dist[last]) {
                    dist[next] = dist[last];
                    q.push_front(next);
                }
            } else {
                if (dist[next] > dist[last] + 1) {
                    dist[next] = dist[last] + 1;
                    q.push_back(next);
                }
            }
        }
    }
    
    return INF;
}

void connect(int v1, int v2) {
    int base, best = 0, best_near = 0, id = -1;
    vector<bool> visited(graph.size());
    vector<int> dist(graph.size());
    deque<int> q;
    
    for (int i = 0; i < graph.size(); i++) dist[i] = INF;
    dist[v1] = 0;
    q.push_back(v1);
    
    while (!q.empty()) {
        int last = q.front();
        q.pop_front();
        
        if (visited[last]) continue;
        visited[last] = true;
        
        for (const Edge& edge : graph[last]) {
            int next = edge.to;
            if (visited[next]) continue;
            if (edge.used) {
                if (dist[next] > dist[last]) {
                    dist[next] = dist[last];
                    q.push_front(next);
                }
            } else {
                if (dist[next] > dist[last] + 1) {
                    dist[next] = dist[last] + 1;
                    q.push_back(next);
                }
            }
        }
    }
    
    base = dist[v2];
    for (int i = 0; i < graph.size(); i++) visited[i] = false;
    q.push_back(v2);
    
    while (!q.empty()) {
        int last = q.front();
        q.pop_front();
        
        visited[last] = true;
        
        for (const Edge& edge : graph[last]) {
            int next = edge.to;
            if (visited[next]) continue;
            if (edge.used) {
                q.push_front(next);
            } else {
                if (dist[next] == dist[last] - 1) {
                    int d = calc_dist(v1, v2, last, next);
                    if (d > best || (d == best && min(dist[next], base - dist[last]) < best_near)) {
                        best = d;
                        best_near = min(dist[next], base - dist[last]);
                        id = edge.id;
                    }
                    if (dist[next] > 0) q.push_back(next);
                }
            }
        }
    }
    
    output(id);
}

void connect() {
    int v1 = -1, v2 = -1;
    double best = 0;
    vector<bool> used(mines.get_count());
    vector<bool> visited(graph.size());
    vector<bool> checked(graph.size());
    vector<int> dist(graph.size());
    deque<int> q;
    
    for (int mine : mines.get_mines()) {
        if (used[mines.get_num(mine)]) continue;
        
        for (int i = 0; i < graph.size(); i++) visited[i] = false;
        for (int i = 0; i < graph.size(); i++) checked[i] = false;
        for (int i = 0; i < graph.size(); i++) dist[i] = INF;
        checked[uf.find(mine)] = true;
        dist[mine] = 0;
        q.push_back(mine);
        
        while (!q.empty()) {
            int last = q.front();
            q.pop_front();
            
            if (visited[last]) continue;
            visited[last] = true;
            if (dist[last] == 0 && mines.is_mine(last)) used[mines.get_num(mine)] = true;
            if (uf.get_mines(last).size() > 0 && !checked[uf.find(last)]) {
                double score = (double)uf.get_size(last) * uf.get_size(mine) / dist[last];
                if (score > best) {
                    best = score;
                    v1 = mine;
                    v2 = last;
                }
                checked[uf.find(last)] = true;
            }
            
            for (const Edge& edge : graph[last]) {
                int next = edge.to;
                if (visited[next]) continue;
                if (edge.used) {
                    if (dist[next] > dist[last]) {
                        dist[next] = dist[last];
                        q.push_front(next);
                    }
                } else {
                    if (dist[next] > dist[last] + 1) {
                        dist[next] = dist[last] + 1;
                        q.push_back(next);
                    }
                }
            }
        }
    }
    
    if (best > 0) connect(v1, v2);
}

void extend() {
    int id = -1;
    long long best = 0;
    vector<long long> profit(graph.size());
    vector<int> parent(graph.size());
    queue<int> q;
    
    for (int i = 0; i < graph.size(); i++) parent[i] = -1;
    for (int mine : mines.get_mines()) {
        parent[mine] = mine;
        q.push(mine);
    }
    
    while (!q.empty()) {
        int last = q.front();
        q.pop();
        
        if (!uf.used(last)) {
            for (int mine : uf.get_mines(parent[last])) {
                profit[last] += (long long)all_dist[mine][last] * all_dist[mine][last];
            }
        }
        
        for (const Edge& edge : graph[last]) {
            int next = edge.to;
            if (parent[next] == -1) {
                parent[next] = parent[last];
                q.push(next);
            }
        }
    }
    
    for (int i = 0; i < graph.size(); i++) {
        for (const Edge& edge : graph[i]) {
            int from = i, to = edge.to;
            if (edge.used || uf.same(from, to)) continue;
            if (!uf.used(from)) swap(from, to);
            if (uf.used(from)) {
                long long score = profit[to];
                for (const Edge& edge : graph[to]) {
                    score += profit[edge.to] / punter;
                }
                if (score > best) {
                    best = score;
                    id = edge.id;
                }
            }
        }
    }
    
    if (best > 0) output(id);
}

void prevent() {
    int best = INF, id = -1;
    
    for (int i = 0; i < graph.size(); i++) {
        for (const Edge& edge : graph[i]) {
            if (!edge.used) {
                int degree = (graph[i].size() + 1) * (graph[edge.to].size() + 1);
                if (degree < best) {
                    best = degree;
                    id = edge.id;
                }
            }
        }
    }
    
    output(id);
}

void move() {
    connect();
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
