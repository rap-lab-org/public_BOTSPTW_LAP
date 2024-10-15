#!/usr/bin/env python
from itertools import product
import os
from os import path
import pandas as pd

def run_config(n: int, w: int, k: int) -> bool:
    dirname = path.join('./data', f'Dumas_{k}')
    ksize: int = n * k // 100
    all_timeout = True
    for i in range(5):
        instance = f'n{n}w{w}.00{i+1}'
        fname = path.join(dirname, f'{instance}.txt')
        cmd = f"./build/run_motsptw {fname}"
        os.system(cmd)
        df = pd.read_csv('output/res.csv')

        istimeout = df[(df['name'] == instance) & (df['k'] == ksize)]['timeout'].max()
        if istimeout == 0:
            all_timeout = False
    return all_timeout

def is_skip(n: int, w: int, k: int, failed: list[tuple[int, int, int]]):
    for fn, fw, fk in failed:
        if n >= fn and w >= fw and k >= fk:
            return True
    return False


def main():
    paras = {
            20: [20, 40, 60, 80, 100],
            40: [20, 40, 60, 80, 100],
            60: [20, 40, 60, 80, 100],
            80: [20, 40, 60, 80],
            100: [20, 40, 60],
    }
    failed = []
    ks = [10, 20, 30, 40, 50, 60, 70, 80, 90, 100]
    cols = ['name', 'solver', 'k', 'time', 'timeout', 'numsol', 'expd', 'gen', 'qsize', 'pr-frontier', 'pr-sol',
            'pr-fea', 'pr-post', 'pr-extra']
    header = ",".join(cols)
    with open('output/res.csv', 'w') as f:
        f.write(header + "\n")
    for n, ws in paras.items():
        for w, k in product(ws, ks):
            if is_skip(n, w, k, failed):
                continue
            all_timeout = run_config(n, w, k)
            if all_timeout:
                failed.append((n, w, k))

if __name__ == "__main__":
    """
    python expr.py
    """
    main()
