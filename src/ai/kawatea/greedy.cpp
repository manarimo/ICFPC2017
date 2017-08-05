#include <cstdio>
#include <cstdlib>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

const int inf = 1e9;

struct edge {
    int to;
    int id;
    int owner;
    
    edge(int to, int id, int owner) : to(to), id(id), owner(owner) {}
};

char protocol[10];
int punter, punter_id, n, m;
vector <int> mine;
vector <int> mines;
vector <vector <edge> > graph;
vector <int> dist;
vector <int> parent;
vector <int> used;
vector <long long> sum;

void input(bool state) {
    int num_mine, i;
    
    scanf("%d", &punter);
    scanf("%d", &punter_id);
    scanf("%d", &n);
    
    mine.resize(n);
    graph.resize(n);
    dist.resize(n);
    parent.resize(n);
    used.resize(n);
    sum.resize(n);
    
    scanf("%d", &num_mine);
    
    for (i = 0; i < num_mine; i++) {
        int x;
        
        scanf("%d", &x);
        
        mine[x] = 1;
        if (!state) mines.push_back(x);
    }
    
    scanf("%d", &m);
    
    for (i = 0; i < m; i++) {
        int x, y, z;
        
        scanf("%d %d %d", &x, &y, &z);
        
        graph[x].push_back(edge(y, i, z));
        graph[y].push_back(edge(x, i, z));
    }
    
    if (state) {
        int num;
        
        scanf("%d", &num);
        
        for (i = 0; i < num; i++) {
            int x;
            
            scanf("%d", &x);
            
            mines.push_back(x);
        }
    }
}

void output(int edge_id) {
    int i;
    
    if (edge_id != -1) printf("%d\n", edge_id);
    printf("%d", mines.size());
    for (i = 0; i < mines.size(); i++) printf(" %d", mines[i]);
    puts("");
    exit(0);
}

void handshake() {
    puts("kawatea-greedy");
}

void init() {
    int i, j;
    vector <pair<int, pair<int, int> > > order;
    queue <int> q;
    
    input(false);
    
    for (i = 0; i < mines.size(); i++) {
        int last = -1, connected = 0;
        
        for (j = 0; j < n; j++) dist[j] = inf;
        
        dist[mines[i]] = 0;
        q.push(mines[i]);
        
        while (!q.empty()) {
            last = q.front();
            connected++;
            
            q.pop();
            
            for (j = 0; j < graph[last].size(); j++) {
                int next = graph[last][j].to;
                
                if (dist[next] == inf) {
                    dist[next] = dist[last] + 1;
                    q.push(next);
                }
            }
        }
        
        order.push_back(make_pair(-connected, make_pair(dist[last], mines[i])));
    }
    
    sort(order.begin(), order.end());
    
    for (i = 0; i < order.size(); i++) mines[i] = order[i].second.second;
    
    puts("0");
    output(-1);
}

void near_mine() {
    int start = mines[0], i, j;
    queue <int> q;
    
    for (i = 0; i < n; i++) dist[i] = inf;
    
    dist[start] = 0;
    q.push(start);
    
    while (!q.empty()) {
        int last = q.front();
        
        q.pop();
        
        for (i = 0; i < graph[last].size(); i++) {
            int next = graph[last][i].to;
            
            if (dist[next] == inf && graph[last][i].owner == punter_id) {
                dist[next] = 0;
                q.push(next);
            }
        }
    }
    
    for (i = 0; i < n; i++) {
        if (dist[i] == 0) {
            used[i] = 1;
            parent[i] = -1;
            q.push(i);
        }
    }
    
    while (!q.empty()) {
        int last = q.front();
        
        q.pop();
        
        if (used[last] == 0 && mine[last] == 1) output(parent[last]);
        
        for (i = 0; i < graph[last].size(); i++) {
            int next = graph[last][i].to;
            
            if (dist[next] == inf && graph[last][i].owner == -1) {
                if (used[last] == 1) {
                    parent[next] = graph[last][i].id;
                } else {
                    parent[next] = parent[last];
                }
                dist[next] = dist[last] + 1;
                q.push(next);
            }
        }
    }
}

void greedy() {
    int start = mines[0], id = -1, i, j, k;
    long long best = 0;
    vector <int> connected;
    queue <int> q;
    
    for (i = 0; i < mines.size(); i++) {
        if (used[mines[i]] == 1) connected.push_back(mines[i]);
    }
    
    for (i = 0; i < connected.size(); i++) {
        for (j = 0; j < n; j++) dist[j] = inf;
        
        dist[connected[i]] = 0;
        q.push(connected[i]);
        
        while (!q.empty()) {
            int last = q.front();
            
            q.pop();
            
            sum[last] += (long long)dist[last] * dist[last];
            
            for (j = 0; j < graph[last].size(); j++) {
                int next = graph[last][j].to;
                
                if (dist[next] == inf) {
                    dist[next] = dist[last] + 1;
                    q.push(next);
                }
            }
        }
    }
    
    for (i = 0; i < n; i++) {
        int from = i;
        
        if (used[from] == 0) continue;
        
        for (j = 0; j < graph[from].size(); j++) {
            int to = graph[from][j].to;
            
            if (graph[from][j].owner == -1 && used[to] == 0) {
                long long score = sum[to];
                
                for (k = 0; k < graph[to].size(); k++) {
                    int next = graph[to][k].to;
                    
                    if (graph[to][k].owner == -1 && used[next] == 0) score += (long long)dist[next] * dist[next] / 2;
                }
                
                if (score > best) {
                    best = score;
                    id = graph[from][j].id;
                }
            }
        }
    }
    
    if (best > 0) output(id);
}

void next_mine() {
    int i;
    
    while (mines.size() > 0) {
        int start = mines[0];
        
        for (i = 0; i < graph[start].size(); i++) {
            if (graph[start][i].owner == -1) output(graph[start][i].id);
        }
        
        mines.erase(mines.begin());
    }
}

void eager() {
    int i, j;
    
    for (i = 0; i < n; i++) {
        for (j = 0; j < graph[i].size(); j++) {
            if (graph[i][j].owner == -1) output(graph[i][j].id);
        }
    }
}

void move() {
    input(true);
    
    if (mines.size() > 0) near_mine();
    if (mines.size() > 0) greedy();
    if (mines.size() > 0) next_mine();
    eager();
}

void end() {
}

int main() {
    scanf("%s", protocol);
    
    if (protocol[0] == 'H') {
        // Handshake
        handshake();
    } else if (protocol[0] == 'I') {
        // Init
        init();
    } else if (protocol[0] == 'M') {
        // Move
        move();
    } else {
        // End
        end();
    }
    
    return 0;
}
