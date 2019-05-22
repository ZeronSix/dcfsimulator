#!/usr/bin/env python3

from scipy.optimize import root
import numpy as np
from matplotlib import pyplot as plt
from scipy import stats
import pandas as pd
import numpy as np


def durations(sigma, SIFS, DIFS, data, ack):
    T_id = sigma
    T_c = data + DIFS + sigma
    T_s = data + SIFS + sigma + ack + DIFS + sigma
    return T_id, T_c, T_s


def eq(CW_min, T_id, T_c, T_s, m, N, tau, p):
    f = 1 / (1 - p)
    omega = 0.5 * (CW_min * (1 - (2 * p)**m) / (1 - 2 * p) +
                   (CW_min * (2 * p)**m - 1) / (1 - p))
    return tau - f / (f + omega), p - 1 + (1 - tau)**(N - 1)


def confidenceInterval(a, alpha=0.01):
    n = len(a)
    return np.sqrt(np.var(a, ddof=1)) / np.sqrt(n) * \
        stats.t.ppf(1 - alpha / 2, n - 1)


df = pd.read_table("output.txt", sep='\t', index_col=None, header=0)

# average over runs
df = df.groupby(['nodes'])['throughput'].agg(
    [np.mean, confidenceInterval]).reset_index()

print(df)
plt.errorbar(
    df['nodes'],
    df['mean'],
    df['confidenceInterval'],
    marker='x',
    linestyle='None',
    color='k',
    label='simulation')

plt.xlabel(r'Nodes')
plt.ylabel('Throughput')
plt.grid()
plt.legend(loc='best')


DIFS = 34
SIFS = 16
slot = 9
data = 1430
ack = 60
l = 1000
CW_min = 15
T_id, T_c, T_s = durations(slot, SIFS, DIFS, data, ack)
for m in range(7, 8):
    N = range(1, 51, 1)
    throughput = []
    for n in N:
        tau, p = root(
            lambda z: eq(
                CW_min, T_id, T_c, T_s, m, n, tau=z[0], p=z[1]), [
                0.9, 0.1]).x
        p_e = (1 - tau)**n
        p_s = n * tau * (1 - tau)**(n - 1)
        p_c = 1 - p_e - p_s
        throughput.append(p_s * l / (
            p_e * slot + p_s * T_s + p_c * T_c) * 8)
    plt.plot(N, throughput, label="m={}".format(m))
plt.ylim(3, 6)
plt.savefig("throughput.png")
