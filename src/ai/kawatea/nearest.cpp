#include <cstdio>
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
int punter, id, n, mine, m;
vector <int> mines;
vector <vector <edge> > graph;
vector <int> dist;
vector <int> parent;

void input() {
    int setting, i;
    
    scanf("%d", &punter);
    scanf("%d", &id);
    scanf("%d", &n);
    
    mines.resize(n);
    graph.resize(n);
    
    scanf("%d", &mine);
    
    for (i = 0; i < mine; i++) {
        int x;
        
        scanf("%d", &x);
        
        mines[x] = 1;
    }
    
    scanf("%d", &m);
    
    for (i = 0; i < m; i++) {
        int x, y, z;
        
        scanf("%d %d %d", &x, &y, &z);
        
        graph[x].push_back(edge(y, i, z));
        graph[y].push_back(edge(x, i, z));
    }
    
    scanf("%d", &setting);
    
    for (int i = 0; i < setting; i++) {
        char option[10];
        
        scanf("%s", option);
    }
}

void handshake() {
    puts("kawatea-nearest");
}

void init() {
    int best = inf, index = -1, i, j;
    
    input();
    
    dist.resize(n);
    
    for (i = 0; i < n; i++) {
        if (mines[i] == 1) {
            int last = -1;
            queue <int> q;
            
            for (j = 0; j < n; j++) dist[j] = inf;
            
            dist[i] = 0;
            q.push(i);
            
            while (!q.empty()) {
                last = q.front();
                
                q.pop();
                
                for (j = 0; j < graph[last].size(); j++) {
                    int next = graph[last][j].to;
                    
                    if (dist[next] == inf) {
                        dist[next] = dist[last] + 1;
                        q.push(next);
                    }
                }
            }
            
            if (dist[last] < best) {
                best = dist[last];
                index = i;
            }
        }
    }
    
    puts("0");
    printf("%d\n", index);
}

void move() {
    int start, last, i, j;
    queue <int> q;
    
    input();
    
    scanf("%d", &start);
    
    dist.resize(n);
    
    for (i = 0; i < n; i++) dist[i] = inf;
    
    dist[start] = 0;
    q.push(start);
    
    while (!q.empty()) {
        last = q.front();
        
        q.pop();
        
        for (i = 0; i < graph[last].size(); i++) {
            int next = graph[last][i].to;
            
            if (dist[next] == inf && graph[last][i].owner == id) {
                dist[next] = 0;
                q.push(next);
            }
        }
    }
    
    parent.resize(n);
    
    for (i = 0; i < n; i++) {
        if (dist[i] == 0) {
            parent[i] = -1;
            q.push(i);
        }
    }
    
    while (!q.empty()) {
        last = q.front();
        
        q.pop();
        
        if (dist[last] > 0 && mines[last] == 1) {
            printf("%d\n", parent[last]);
            printf("%d\n", start);
            return;
        }
        
        for (i = 0; i < graph[last].size(); i++) {
            int next = graph[last][i].to;
            
            if (dist[next] == inf && graph[last][i].owner == -1) {
                if (dist[last] == 0) {
                    parent[next] = graph[last][i].id;
                } else {
                    parent[next] = parent[last];
                }
                dist[next] = dist[last] + 1;
                q.push(next);
            }
        }
    }
    
    if (parent[last] != -1) {
        printf("%d\n", parent[last]);
        printf("%d\n", start);
        return;
    }
    
    for (i = 0; i < n; i++) {
        if (mines[i] == 1 && i != start) {
            for (j = 0; j < graph[i].size(); j++) {
                if (graph[i][j].owner == -1) {
                    printf("%d\n", graph[i][j].id);
                    printf("%d\n", i);
                    return;
                }
            }
        }
    }
    
    for (i = 0; i < n; i++) {
        for (j = 0; j < graph[i].size(); j++) {
            if (graph[i][j].owner == -1) {
                printf("%d\n", graph[i][j].id);
                printf("%d\n", i);
                return;
            }
        }
    }
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
