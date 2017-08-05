from scipy.optimize import minimize
import numpy as np


def estimate_rating(win_matrix):
    n = len(win_matrix)
    def loglikelihood(r):
        lp = 0
        for i in range(n):
            for j in range(n):
                lp += np.log(1. / (1. + np.exp(r[j] - r[i]))) * win_matrix[i][j]
        return lp
    def objective(r):
        return -loglikelihood(r)
    res = minimize(objective, np.array([0] * n))
    return res.x


if __name__ == '__main__':
    print(estimate_rating([
        [0, 8, 7, 13, 10, 11, 12],
        [8, 0, 8, 6, 9, 10, 10],
        [8, 6, 0, 7, 9, 10, 9],
        [4, 7, 9, 0, 10, 10, 6],
        [4, 8, 5, 6, 0, 9, 9],
        [5, 6, 5, 6, 7, 0, 5],
        [6, 8, 9, 12, 9, 12, 0]
    ]))