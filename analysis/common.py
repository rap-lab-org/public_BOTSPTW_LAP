import numpy as np
import os
from itertools import product
import pandas as pd

def normal_fn(fn: str) -> str:
    if fn.endswith('.txt') or fn.endswith('.tw'):
        return fn.removesuffix('.' + fn.split('.')[-1])
    else:
        return fn


def parse_fn(fn: str) -> tuple[str, str, int]:
    # fn, e.g.,:
    # - ../mo-data/Dumas_10/n20w20.001.txt
    # - ../mo-data/AFG_10/rbg017.tw
    # - ../mo-data/SolomonPotvinBengio_10/rc_201.1.txt
    instance: str = ""
    dataset: str = ""
    k: int = 0
    instance = normal_fn(fn.split('/')[-1])
    dataset = fn.split("/")[-2].split("/")[0].split("_")[0]
    k = int(fn.split("/")[-2].split("/")[0].split("_")[-1].strip())
    return dataset, instance, k


def get_bestknown(dataset: str, instname: str) -> float:
    # "../data/best-known/Dumas-makespan.txt"
    fn = f"../data/best-known/{dataset}-makespan.txt"
    res = 0.0

    print (dataset, instname)

    with open(fn, "r") as f:
        for line in f.readlines():
            if line.startswith("#"):
                continue
            name = normal_fn(line.strip().split()[0].strip())
            cost = float(line.strip().split()[1].strip())
            if name == instname:
                return cost
    assert False
    return res


def get_best_makespan_sol(solfile: str) -> float:
    if os.path.exists(solfile):
        with open(solfile, 'r') as f:
            penalty, makespan = map(float, f.readline().strip().split())
            return makespan
    return np.inf


def get_frontier_avg(labelfile: str) -> float:
    if os.path.exists(labelfile):
        df = pd.read_csv(labelfile)
        return float(df['labels'].mean())
    return np.inf


def get_NDs_avg(labelfile: str) -> float:
    if os.path.exists(labelfile):
        df = pd.read_csv(labelfile)
        return float(df['NDs'].mean())
    return np.inf


class Instance:
    tw: list[tuple[float, float]]
    key_nodes: list[int]
    cost: list[list[float]]
    instname: str
    dataset: str
    k: int
    bestknown: float

    def __init__(self, fn: str):
        print ("fn:", fn)
        self.dataset, self.instname, self.k = parse_fn(fn)
        self.bestknown = get_bestknown(self.dataset, self.instname)
        with open(fn, "r") as f:
            n = int(f.readline().strip())
            self.cost = []
            self.tw = []
            self.key_nodes = []
            for r in range(n):
                row = list(map(float, f.readline().strip().split()))
                self.cost.append(row)
            for r in range(n):
                l, r = map(float, f.readline().strip().split())
                self.tw.append((l, r))
            k = int(f.readline().strip())
            for i in range(k):
                v = int(f.readline().strip())
                self.key_nodes.append(v)

    def to_dict(self):
        return dict(
            instname=self.instname,
            dataset=self.dataset,
            otw=self.mean_overlap(),
            ltw=self.mean_tw(),
            mcost=self.mean_cost(),
            n=len(self.cost),
            bestknown=self.bestknown,
        )

    def mean_cost(self):
        costs: list[float] = []
        for c in self.cost:
            costs += c
        return np.mean(costs)

    def mean_tw(self):
        lens: list[float] = []
        for l, r in self.tw:
            lens.append(r - l)
        return np.mean(lens)

    def mean_overlap(self):
        n = len(self.cost)
        otw: list[float] = []
        for i, j in product(range(n), range(n)):
            if i <= j:
                continue
            li, ri = self.tw[i]
            lj, rj = self.tw[j]
            overlap = max(min(ri, rj) - max(li, lj), 0)
            otw.append(overlap)
        return np.mean(otw)
