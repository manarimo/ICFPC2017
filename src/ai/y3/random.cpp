#include <cstdio>
#include <cstdlib>
#include <vector>
#include <queue>
#include <algorithm>
#include <ctime>

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

vector<vector<Edge>> graph;
vector<int> degree;
vector<int> used;

void input() {
    int n, m, mine, setting;
    
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
      int from, to, owner, owner2;
      scanf("%d %d %d %d", &from, &to, &owner, &owner2);
        
        graph[from].push_back(Edge(to, i, owner));
        graph[to].push_back(Edge(from, i, owner));
        
        if (owner == -1) {
            degree[from]++;
            degree[to]++;
        } else if (owner == punter_id) {
            used[from] = used[to] = 1;
        }
    }
    
    scanf("%d", &setting);
    for (int i = 0; i < setting; i++) {
        char option[10];
        scanf("%s", option);
    }
}

void output(const vector<pair<int, int>>& futures) {
    printf("%d", futures.size());
    for (int i = 0; i < futures.size(); i++) printf(" %d %d", futures[i].first, futures[i].second);
    puts("");
    exit(0);
}

void output(int edge_id) {
    printf("%d\n", edge_id);
    exit(0);
}

void handshake() {
    puts("random");
}

void init() {
    output(vector<pair<int, int>>());
}

void move() {
  vector<Edge> free_edges;
  for (auto &v: graph) {
    for (auto &e: v) {
      if (e.is_free()) {
	free_edges.push_back(e);
      }
    }
  }
  random_shuffle(free_edges.begin(), free_edges.end());
  output(free_edges[0].id);
}

void end() {
}

int main() {
  srand(clock());
    char protocol[10];
    scanf("%s", protocol);
    
    if (protocol[0] == 'H') {
        handshake();
    } else if (protocol[0] == 'I') {
        // Init
        input();
        init();
    } else if (protocol[0] == 'M') {
        // Move
        input();
        move();
    } else {
        // End
        end();
    }
    
    return 0;
}
