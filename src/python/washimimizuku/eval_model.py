import random
from collections import deque


def edge_key(fr, to):
    if fr > to:
        fr, to = to, fr
    return fr, to


def zero_one_bfs(graph, starting_nodes, zero_edges, prohibit_edges):
    distances = dict()
    deq = deque()
    for starting_node in starting_nodes:
        deq.append((starting_node, 0))
        distances[starting_node] = 0
    while len(deq) > 0:
        pos, dist = deq.popleft()
        for next in graph[pos]:
            if next in distances:
                continue
            edge = edge_key(pos, next)
            if edge in prohibit_edges:
                continue
            if edge in zero_edges:
                next_dist = dist
                deq.appendleft((next, next_dist))
                distances[next] = next_dist
            else:
                next_dist = dist + 1
                deq.append((next, next_dist))
                distances[next] = next_dist
    return distances


def sample_paths(graph, own_edges, own_nodes, length, path_num):
    path_hashes = []
    nodes = list(graph.keys())
    mask = (1 << (2 * length + 1)) - 1
    while True:
        pos = random.choice(nodes)
        prev = None
        deq = int(pos in own_nodes)
        path_len = 0
        while (path_len - length) * 10 < path_num:
            adj = graph[pos]
            if len(adj) < 2:
                break
            next = prev
            while next == prev:
                next = random.choice(adj)
            edge_info = int(edge_key(pos, next) in own_edges)
            node_info = int(next in own_nodes)
            deq = ((deq << 2) & mask) | (2 * edge_info) | node_info
            path_len += 1
            if path_len >= length:
                path_hashes.append(deq)
            if len(path_hashes) >= path_num:
                return path_hashes
        if path_len < length:
            path_hashes.append(deq)
        if len(path_hashes) >= path_num:
            return path_hashes


def quad_vector(v):
    return [i * i for i in v]


def feature_vector(graph, mines, edge_owner, punter_id, length, to_sample):
    dims = 1 << (2 * length + 1)
    current_path = [0] * dims
    possible_path = [0] * dims
    current_score = 0

    all_edges = set(ek for ek, owner in edge_owner.items())
    my_edges = set(ek for ek, owner in edge_owner.items() if owner == punter_id)
    free_edges = set(ek for ek, owner in edge_owner.items() if owner is None)
    possible_edges = my_edges | free_edges
    non_my_edges = all_edges ^ my_edges
    for mine_id in mines:
        distances = zero_one_bfs(graph, [mine_id], set(), set())
        own_nodes = set(zero_one_bfs(graph, [mine_id], set(), non_my_edges).keys())
        current_score += sum(distances[node_id] ** 2 for node_id in own_nodes)
        for edge in sample_paths(graph, my_edges, own_nodes, length, to_sample // len(mines)):
            current_path[edge] += 1
        for edge in sample_paths(graph, possible_edges, own_nodes, length, to_sample // len(mines)):
            possible_path[edge] += 1

    path_feature = current_path + possible_path
    normalizer = len(graph) / to_sample
    path_feature = [v * normalizer for v in path_feature]
    return [current_score] + path_feature + quad_vector(path_feature)
