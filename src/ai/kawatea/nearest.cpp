#include <cstdio>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

char protocol[10];
int punter, id, n, mine, m;
vector <int> mines;
vector <vector <pair<int, int> > > graph;
vector <int> used;
vector <int> dist;
vector <int> parent;

void input() {
    int i;
    
    scanf("%d", &punter);
    scanf("%d", &id);
    scanf("%d", &n);
    
    mines.resize(n);
    graph.resize(n);
    
    scanf("%d", &mine);
    
    for (i = 0; i < mine; i++) {
        int x;
        
        scanf("%d", &x);
        
        mines[i] = 1;
    }
    
    scanf("%d", &m);
    
    for (i = 0; i < m; i++) {
        int x, y;
        
        scanf("%d %d", &x, &y);
        
        graph[x].push_back(make_pair(y, i));
        graph[y].push_back(make_pair(x, i));
    }
    
    used.resize(m);
    
    for (i = 0; i < m; i++) scanf("%d", &used[i]);
}

void handshake() {
    puts("kawatea-nearest");
}

void init() {
    int best = 1e9, index = -1, i, j;
    
    input();
    
    dist.resize(n);
    
    for (i = 0; i < n; i++) {
        if (mines[i] == 1) {
            int last = -1;
            queue <int> q;
            
            for (j = 0; j < n; j++) dist[j] = 1e9;
            
            dist[i] = 0;
            q.push(i);
            
            while (!q.empty()) {
                last = q.front();
                
                q.pop();
                
                for (j = 0; j < graph[last].size(); j++) {
                    int next = graph[last][j].first;
                    
                    if (dist[next] == 1e9) {
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
    
    printf("%d\n", index);
}

void move() {
    int start, last, i, j;
    queue <int> q;
    
    input();
    
    scanf("%d", &start);
    
    dist.resize(n);
    
    for (i = 0; i < n; i++) dist[i] = 1e9;
    
    dist[start] = 0;
    q.push(start);
    
    while (!q.empty()) {
        last = q.front();
        
        q.pop();
        
        for (i = 0; i < graph[last].size(); i++) {
            int next = graph[last][i].first;
            
            if (dist[next] == 1e9 && used[graph[last][i].second] == id) {
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
            int next = graph[last][i].first;
            
            if (dist[next] == 1e9 && used[graph[last][i].second] == -1) {
                if (dist[last] == 0) {
                    parent[next] = graph[last][i].second;
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
                if (used[graph[i][j].second] == -1) {
                    printf("%d\n", graph[i][j].second);
                    printf("%d\n", i);
                    return;
                }
            }
        }
    }
    
    // pass
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
