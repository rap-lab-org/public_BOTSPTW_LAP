#!/usr/bin/env python
from itertools import product
import os
import subprocess
from pathlib import Path


def run_instance(file: Path, respath: Path, frontierpath: Path, solver: str) -> bool:
    cmd = ["./build/run_motsptw", str(file), solver]
    print(f">> {' '.join(cmd)}")
    subprocess.run(cmd)
    hasSol = False
    res = Path("./solutions.txt")
    if res.exists():
        hasSol = True
        res.rename(respath)
    frontier = Path("./frontiers.csv")
    if frontier.exists():
        frontier.rename(frontierpath)
    return hasSol


def main():
    datasets = [
            "AFG",
            "Dumas", 
            "OhlmannThomas",
            "SolomonPotvinBengio"
    ]
    ks = [0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100]
    datadir = Path("./mo-data")
    solvers = [
            "combine",
            "gap_prune", 
            "fastdom", 
            "default", 
    ]
    cols = [
        "dataset",
        "name",
        "k",
        "solver",
        "time",
        "timeout",
        "numsol",
        "expd",
        "gen",
        "qsize",
        "pr-frontier",
        "pr-sol",
        "pr-fea",
        "pr-post",
        "pr-extra",
    ]
    header = ",".join(cols)
    os.makedirs("output/solutions", exist_ok=True)
    os.makedirs("output/frontiers", exist_ok=True)
    with open("output/res.csv", "w") as f:
        f.write(header + "\n")
    outdir = Path("./output/")
    failed: dict[tuple[str, str], int] = {}
    for ds, k in product(datasets, ks):
        basedir = datadir.joinpath(f"{ds}_{k}")
        for instance in os.listdir(basedir):
            file = basedir.joinpath(instance)
            for solver in solvers:
                if k > failed.get((instance, solver), 100):
                    break
                respath = outdir.joinpath("solutions", f"{solver}-k{k}-{instance}")
                frontierpath = outdir.joinpath(
                    "frontiers", f"{solver}-k{k}-{instance[:-4]}.csv"
                )
                hasSol = run_instance(file, respath, frontierpath, solver)
                if not hasSol:
                    failed[(instance, solver)] = k
                    if solver == 'combine':
                        break


if __name__ == "__main__":
    """
    python full-expr.py
    """
    main()
