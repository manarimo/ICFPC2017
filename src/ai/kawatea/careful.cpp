#include <cstdio>
#include <cstdlib>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

const int INF = 1e9;
const long long INF2 = (long long)INF * INF;
int punter, punter_id;

struct Edge {
    int to;
    int id;
    bool used;
    bool option;
    
    Edge(int to, int id, bool used, bool option) : to(to), id(id), used(used), option(option) {}
};

struct Edge2 {
    int from;
    int to;
    bool option;
    bool bridge = false;
    
    Edge2(int from, int to, bool option) : from(from), to(to), option(option) {}
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
};

int stage = 0;
int options = 0;
vector<Edge2> edges;
vector<vector<Edge>> graph;
vector<vector<int>> degree;
vector<UnionFind> uf;
vector<vector<int>> all_dist;

class Bridge {
    public:
    void bridge(int n) {
        int count = 0;
        
        ord.resize(n);
        low.resize(n);
        for (int i = 0; i < n; i++) ord[i] = -1;
        
        for (int i = 0; i < n; i++) {
            if (ord[i] == -1) dfs(i, -1, count);
        }
    }
    
    private:
    vector<int> ord;
    vector<int> low;
    
    void dfs(int now, int parent, int& count) {
        ord[now] = low[now] = count++;
        
        for (const Edge& edge : graph[now]) {
            int next = edge.to;
            if (next == parent) continue;
            if (ord[next] == -1) {
                dfs(next, now, count);
                low[now] = min(low[now], low[next]);
                if (ord[now] < low[next]) edges[edge.id].bridge = true;
            } else {
                low[now] = min(low[now], ord[next]);
            }
        }
    }
} bridge;

void input(bool read_state) {
    int n, m, mine, setting;
    vector<pair<int, pair<int, int>>> option_edges;
    
    scanf("%d", &punter);
    scanf("%d", &punter_id);
    
    scanf("%d", &n);
    scanf("%d", &mine);
    mines.init(n);
    uf.resize(punter);
    for (int i = 0; i < punter; i++) uf[i].init(n);
    for (int i = 0; i < mine; i++) {
        int v;
        scanf("%d", &v);
        mines.add_mine(v, i);
        for (int j = 0; j < punter; j++) uf[j].add_mine(v, i);
    }
    
    scanf("%d", &m);
    graph.resize(n);
    degree = vector<vector<int>>(punter, vector<int>(n));
    for (int i = 0; i < m; i++) {
        int from, to, owner1, owner2;
        scanf("%d %d %d %d", &from, &to, &owner1, &owner2);
        if (owner1 != -1 && owner1 != punter_id && owner2 == -1) {
            edges.push_back(Edge2(from, to, true));
        } else {
            edges.push_back(Edge2(from, to, false));
        }
        
        if (owner1 == -1) {
            graph[from].push_back(Edge(to, i, false, false));
            graph[to].push_back(Edge(from, i, false, false));
            for (int j = 0; j < punter; j++) {
                if (j == punter_id) continue;
                uf[j].unite(from, to);
            }
        } else if (owner1 == punter_id || owner2 == punter_id) {
            graph[from].push_back(Edge(to, i, true, false));
            graph[to].push_back(Edge(from, i, true, false));
            uf[punter_id].unite(from, to);
        } else if (owner2 == -1) {
            option_edges.push_back(make_pair(i, make_pair(from, to)));
        }
        
        if (owner1 != punter_id && owner1 != -1) {
            degree[owner1][from]++;
            degree[owner1][to]++;
            uf[owner1].unite(from, to);
        }
        if (owner2 != punter_id && owner2 != -1) {
            degree[owner2][from]++;
            degree[owner2][to]++;
            uf[owner2].unite(from, to);
        }
    }
    
    scanf("%d", &setting);
    for (int i = 0; i < setting; i++) {
        char option[10];
        scanf("%s", option);
        if (option[0] == 'o') options = mine;
    }
    
    all_dist = vector<vector<int>>(mine, vector<int>(n));
    if (read_state) {
        scanf("%d", &stage);
        scanf("%d", &options);
        for (int i = 0; i < mine; i++) {
            for (int j = 0; j < n; j++) {
                int d;
                scanf("%d", &d);
                all_dist[i][j] = d;
            }
        }
    }
    
    if (options > 0) {
        for (int i = 0; i < option_edges.size(); i++) {
            int id = option_edges[i].first;
            int from = option_edges[i].second.first;
            int to = option_edges[i].second.second;
            graph[from].push_back(Edge(to, id, false, true));
            graph[to].push_back(Edge(from, id, false, true));            
        }
        
        if (stage == 0) bridge.bridge(n);
    }
}

void output_state() {
    printf("%d\n", stage);
    printf("%d\n", options);
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

void output(int id, bool option = false) {
    if (option) options--;
    printf("%d\n", id);
    output_state();
}

void handshake() {
    puts("kawatea-careful-forecast");
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

int calc_dist(int v1, int v2, int id) {
    int base = INF, level = 0;
    vector<vector<bool>> visited(graph.size(), vector<bool>(options + 1));
    vector<vector<int>> dist(graph.size(), vector<int>(options + 1));
    deque<pair<int, int>> q;
    
    for (int i = 0; i < graph.size(); i++) {
        for (int j = 0; j <= options; j++) {
            dist[i][j] = INF;
        }
    }
    dist[v1][0] = 0;
    q.push_back(make_pair(v1, 0));
    
    while (!q.empty()) {
        int last = q.front().first, count = q.front().second;
        q.pop_front();
        
        if (visited[last][count]) continue;
        visited[last][count] = true;
        
        if (last == v2) {
            if (dist[v2][count] + count < base) {
                base = dist[v2][count] + count;
                level = count;
            }
        }
        
        if (dist[last][count] >= base) return base;
        
        for (const Edge& edge : graph[last]) {
            int next = edge.to;
            if (edge.id == id) continue;
            if (uf[punter_id].same(next, v2)) next = v2;
            if (edge.used) {
                if (dist[next][count] > dist[last][count]) {
                    dist[next][count] = dist[last][count];
                    q.push_front(make_pair(next, count));
                }
            } else if (edge.option) {
                if (count < options) {
                    if (dist[next][count + 1] > dist[last][count] + 1) {
                        dist[next][count + 1] = dist[last][count] + 1;
                        q.push_back(make_pair(next, count + 1));
                    }
                }
            } else {
                if (dist[next][count] > dist[last][count] + 1) {
                    dist[next][count] = dist[last][count] + 1;
                    q.push_back(make_pair(next, count));
                }
            }
        }
    }
    
    return INF;
}

void connect(int v1, int v2) {
    bool option = false;
    int base = INF, best = -1, id = -1;
    long long sum = 0, best_status = 0;
    vector<vector<bool>> visited(graph.size(), vector<bool>(options + 1));
    vector<vector<int>> dist1(graph.size(), vector<int>(options + 1));
    vector<vector<int>> dist2(graph.size(), vector<int>(options + 1));
    vector<vector<long long>> sum1(graph.size(), vector<long long>(options + 1));
    vector<vector<long long>> sum2(graph.size(), vector<long long>(options + 1));
    vector<long long> root(edges.size());
    deque<pair<int, int>> q;
    
    for (int i = 0; i < graph.size(); i++) {
        for (int j = 0; j <= options; j++) {
            dist1[i][j] = INF;
        }
    }
    dist1[v1][0] = 0;
    sum1[v1][0] = 1;
    q.push_back(make_pair(v1, 0));
    
    while (!q.empty()) {
        int last = q.front().first, count = q.front().second;
        q.pop_front();
        
        if (visited[last][count]) continue;
        visited[last][count] = true;
        
        for (const Edge& edge : graph[last]) {
            int next = edge.to;
            if (uf[punter_id].same(next, v2)) next = v2;
            if (edge.used) {
                if (dist1[next][count] > dist1[last][count]) {
                    dist1[next][count] = dist1[last][count];
                    sum1[next][count] = sum1[last][count];
                    q.push_front(make_pair(next, count));
                } else if (dist1[next][count] == dist1[last][count]) {
                    if (!visited[next][count]) sum1[next][count] = min(sum1[next][count] + sum1[last][count], INF2);
                }
            } else if (edge.option) {
                if (count < options) {
                    if (dist1[next][count + 1] > dist1[last][count] + 1) {
                        dist1[next][count + 1] = dist1[last][count] + 1;
                        sum1[next][count + 1] = sum1[last][count];
                        q.push_back(make_pair(next, count + 1));
                    } else if (dist1[next][count + 1] == dist1[last][count] + 1) {
                        sum1[next][count + 1] = min(sum1[next][count + 1] + sum1[last][count], INF2);
                    }
                }
            } else {
                if (dist1[next][count] > dist1[last][count] + 1) {
                    dist1[next][count] = dist1[last][count] + 1;
                    sum1[next][count] = sum1[last][count];
                    q.push_back(make_pair(next, count));
                } else if (dist1[next][count] == dist1[last][count] + 1) {
                    sum1[next][count] = min(sum1[next][count] + sum1[last][count], INF2);
                }
            }
        }
    }
    
    for (int i = 0; i <= options; i++) {
        for (int j = 0; j < i; j++) {
            if (dist1[v2][i] > dist1[v2][j] + j - i) dist1[v2][i] = INF;
        }
        base = min(base, dist1[v2][i] + i);
        if (dist1[v2][i] < INF) sum = min(sum + sum1[v2][i], INF2);
    }
    
    for (int i = 0; i < graph.size(); i++) {
        for (int j = 0; j <= options; j++) {
            visited[i][j] = false;
            dist2[i][j] = INF;
        }
    }
    dist2[v2][0] = 0;
    sum2[v2][0] = 1;
    q.push_back(make_pair(v2, 0));
    
    while (!q.empty()) {
        int last = q.front().first, count = q.front().second;
        q.pop_front();
        
        if (visited[last][count]) continue;
        visited[last][count] = true;
        
        for (const Edge& edge : graph[last]) {
            int next = edge.to;
            if (uf[punter_id].same(next, v2)) next = v2;
            if (edge.used) {
                if (dist2[next][count] > dist2[last][count]) {
                    dist2[next][count] = dist2[last][count];
                    sum2[next][count] = sum2[last][count];
                    q.push_front(make_pair(next, count));
                } else if (dist2[next][count] == dist2[last][count]) {
                    if (!visited[next][count]) sum2[next][count] = min(sum2[next][count] + sum2[last][count], INF2);
                }
            } else if (edge.option) {
                for (int i = 0; i + count + 1 <= options; i++) {
                    if (dist1[next][i] + dist2[last][count] + 1 == dist1[v2][i + count + 1]) {
                        if (INF2 / sum1[next][i] >= sum2[last][count]) {
                            root[edge.id] = min(root[edge.id] + sum1[next][i] * sum2[last][count], INF2);
                        } else {
                            root[edge.id] = INF2;
                        }
                    }
                }
                if (count < options) {
                    if (dist2[next][count + 1] > dist2[last][count] + 1) {
                        dist2[next][count + 1] = dist2[last][count] + 1;
                        sum2[next][count + 1] = sum2[last][count];
                        q.push_back(make_pair(next, count + 1));
                    } else if (dist2[next][count + 1] == dist2[last][count] + 1) {
                        sum2[next][count + 1] = min(sum2[next][count + 1] + sum2[last][count], INF2);
                    }
                }
            } else {
                for (int i = 0; i + count <= options; i++) {
                    if (dist1[next][i] + dist2[last][count] + 1 == dist1[v2][i + count]) {
                        if (INF2 / sum1[next][i] >= sum2[last][count]) {
                            root[edge.id] = min(root[edge.id] + sum1[next][i] * sum2[last][count], INF2);
                        } else {
                            root[edge.id] = INF2;
                        }
                    }
                }
                if (dist2[next][count] > dist2[last][count] + 1) {
                    dist2[next][count] = dist2[last][count] + 1;
                    sum2[next][count] = sum2[last][count];
                    q.push_back(make_pair(next, count));
                } else if (dist2[next][count] == dist2[last][count] + 1) {
                    sum2[next][count] = min(sum2[next][count] + sum2[last][count], INF2);
                }
            }
        }
    }
    
    for (int i = 0; i < edges.size(); i++) {
        int d, from = edges[i].from, to = edges[i].to;
        long long status = root[i];
        if (root[i] == 0) continue;
        
        if (edges[i].bridge) {
            d = INF;
        } else if (root[i] == sum) {
            d = calc_dist(v1, v2, i);
        } else {
            d = 0;
        }
        
        if (uf[punter_id].same(from, v1) || uf[punter_id].same(from, v2) || uf[punter_id].same(to, v1) || uf[punter_id].same(to, v2)) status++;
        if (!edges[i].option) status *= 2;
        
        if (d > best || (d == best && status > best_status)) {
            best = d;
            best_status = status;
            id = i;
            option = edges[i].option;
        }
    }
    
    output(id, option);
}

void connect() {
    int component = 0, v1 = -1, v2 = -1;
    double best = 0;
    vector<bool> used(mines.get_count());
    vector<vector<bool>> visited(graph.size(), vector<bool>(options + 1));
    vector<vector<int>> dist(graph.size(), vector<int>(options + 1));
    deque<pair<int, int>> q;
    
    for (int mine : mines.get_mines()) {
        component = max(component, (int)uf[punter_id].get_mines(mine).size());
    }
    
    for (int mine : mines.get_mines()) {
        if (used[mines.get_num(mine)]) continue;
        if (component > mines.get_count() / 2 && uf[punter_id].get_mines(mine).size() != component) continue;
        
        for (int i = 0; i < graph.size(); i++) {
            for (int j = 0; j <= options; j++) {
                visited[i][j] = false;
                dist[i][j] = INF;
            }
        }
        dist[mine][0] = 0;
        q.push_back(make_pair(mine, 0));
        
        while (!q.empty()) {
            int last = q.front().first, count = q.front().second;
            q.pop_front();
            
            if (visited[last][count]) continue;
            visited[last][count] = true;
            if (dist[last][count] == 0 && mines.is_mine(last)) used[mines.get_num(mine)] = true;
            if (!uf[punter_id].same(last, mine) && uf[punter_id].get_mines(last).size() > 0) {
                double score = (double)uf[punter_id].get_size(last) * uf[punter_id].get_size(mine) / (dist[last][count] + count);
                if (score > best) {
                    best = score;
                    v1 = mine;
                    v2 = last;
                }
            }
            
            for (const Edge& edge : graph[last]) {
                int next = edge.to;
                if (edge.used) {
                    if (dist[next][count] > dist[last][count]) {
                        dist[next][count] = dist[last][count];
                        q.push_front(make_pair(next, count));
                    }
                } else if (edge.option) {
                    if (count < options && dist[next][count + 1] > dist[last][count] + 1) {
                        dist[next][count + 1] = dist[last][count] + 1;
                        q.push_back(make_pair(next, count + 1));
                    }
                } else {
                    if (dist[next][count] > dist[last][count] + 1) {
                        dist[next][count] = dist[last][count] + 1;
                        q.push_back(make_pair(next, count));
                    }
                }
            }
        }
    }
    
    if (best > 0) {
        connect(v1, v2);
    } else {
        stage++;
    }
}

void surround() {
    int best = 0, id = -1;
    vector<pair<int, int>> order;
    
    for (int mine : mines.get_mines()) {
        int rest = 0;
        for (const Edge& edge : graph[mine]) {
            if (!edge.used && !edge.option) rest++;
        }
        if (rest > 0) order.push_back(make_pair(rest, mine));
    }
    sort(order.begin(), order.end());
    
    for (int i = 0; i < order.size(); i++) {
        int mine = order[i].second;
        for (const Edge& edge : graph[mine]) {
            int count1 = punter, count2 = 0;
            if (edge.used || edge.option) continue;
            for (int j = 0; j < punter; j++) {
                if (j == punter_id) continue;
                if (uf[j].get_mines(mine).size() == 1) continue;
                if (degree[j][mine] == 0) {
                    if (degree[j][edge.to] > 0) {
                        count1++;
                    } else {
                        count2++;
                    }
                }
            }
            if ((count1 - punter > 0 && count1 > best) || count2 > best) {
                best = max(count1, count2);
                id = edge.id;
            }
        }
    }
    
    if (best > 0) {
        output(id);
    } else {
        stage++;
    }
}

void extend() {
    int id = -1;
    bool option = false;
    long long best = 0;
    vector<bool> visited(graph.size());
    vector<int> dist(graph.size());
    vector<int> parent(graph.size());
    vector<int> order;
    vector<long long> profit(graph.size());
    deque<int> q;
    
    for (int i = 0; i < graph.size(); i++) dist[i] = INF;
    for (int i = 0; i < graph.size(); i++) parent[i] = -1;
    for (int mine : mines.get_mines()) {
        dist[mine] = 0;
        parent[mine] = mine;
        q.push_back(mine);
    }
    
    while (!q.empty()) {
        int last = q.front();
        q.pop_front();
        
        if (visited[last]) continue;
        visited[last] = true;
        
        if (dist[last] > 0) {
            order.push_back(last);
            for (int mine : uf[punter_id].get_mines(parent[last])) {
                profit[last] += (long long)all_dist[mine][last] * all_dist[mine][last];
            }
        }
        
        for (const Edge& edge : graph[last]) {
            int next = edge.to;
            if (visited[next]) continue;
            if (edge.used) {
                if (dist[next] > dist[last]) {
                    dist[next] = dist[last];
                    parent[next] = parent[last];
                    q.push_front(next);
                }
            } else if (!edge.option || options > 0) {
                if (dist[next] > dist[last] + 1) {
                    dist[next] = dist[last] + 1;
                    parent[next] = parent[last];
                    q.push_back(next);
                }
            }
        }
    }
    
    reverse(order.begin(), order.end());
    for (int last : order) {
        for (const Edge& edge : graph[last]) {
            int next = edge.to;
            if (edge.used || (!edge.option && dist[next] == dist[last] + 1)) {
                profit[last] += profit[next] / 2;
            } else if (options > 0 && dist[next] == dist[last] + 1) {
                profit[last] += profit[next] / 4;
            }
        }
    }
    
    for (int i = 0; i < graph.size(); i++) {
        int last = i;
        if (dist[last] > 0) continue;
        for (const Edge& edge : graph[last]) {
            int next = edge.to;
            if (edge.used || (edge.option && options == 0) || uf[punter_id].same(last, next)) continue;
            if (profit[next] > best) {
                best = profit[next];
                id = edge.id;
                option = edge.option;
            }
        }
    }
    
    if (best > 0) {
        output(id, option);
    } else {
        stage++;
    }
}

void prevent() {
    int best = INF, id = -1;
    
    for (int mine : mines.get_mines()) {
        for (const Edge& edge : graph[mine]) {
            if (!edge.used && !edge.option) output(edge.id);
        }
    }
    
    for (int i = 0; i < graph.size(); i++) {
        for (const Edge& edge : graph[i]) {
            if (!edge.used && !edge.option) {
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
    if (stage == 0) connect();
    if (stage == 1) surround();
    if (stage == 2) extend();
    if (stage == 3) prevent();
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
