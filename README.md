# Bi-Objective Search for TSPTW-VP

## How to use

### Generate dataset

Define your TSPTW data with the following format:

- first line: number of nodes `n`
- next `n` lines: the distance matrix of size `n x n`
- next `n` lines: the time window matrix of size `n x 2`

You can also refer to [this link](https://lopez-ibanez.eu/tsptw-instances) for some TSPTW instances.

### Convert the dataset

- TL;DR: 
    ```bash
    ./convData.sh
    ```
The script above will read `TSPTW` instances from `data/` and generate `TSPTW-VP` instances in `mo-data/`.

Specifically, in our problem, we also define some `key nodes` with the total waiting time should also be minimized. 
You can generate the dataset by running the following command:

```bash
python3 ./python/convertTestData.py <input_file> <output_file> <load>
```

where `<input_file>` is the path to the TSPTW data, `<output_file>` is the path to the MO-TSPTW data, and `<load>` is the percentage of key nodes.

You can also manually change the number of key nodes and choose the key nodes by modifying the generated file.

### Compile and Run

```bash
make fast
```

This will compile the executable `build/run_motsptw`, then you can run the code as follows

```bash
./build/run_motsptw <data>
# e.g.,
# ./build/run_motsptw ./mo-data/Dumas_10/n20w20.001.txt
```
where `<data>` is the path to the TSPTW-VP data.

One can also run the following scripts to reproduce all experiments mentioned in the paper:

```bash
python full-expr.py
```
### Cite
```bibtex
@inproceedings{zhao2025botsptwvp,
  title={Bi-Objective Search for the Traveling Salesman Problem with Time Windows and Vacant Penalties},
  author={Zhao, Shizhe and Wu, Yancheng and Ren, Zhongqiang},
  booktitle={Proceedings of the International Symposium on Combinatorial Search},
  volume={18},
  pages={171--179},
  year={2025}
}
```
