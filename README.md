# MO-TSPTW

## How to use

### Clone the repository

```bash
git clone https://github.com/eggeek/mo-tsptw.git
cd mo-tsptw
```

### Generate dataset

Define your TSPTW data with the following format:

- first line: number of nodes `n`
- next `n` lines: the distance matrix of size `n x n`
- next `n` lines: the time window matrix of size `n x 2`

You can also refer to [this link](https://lopez-ibanez.eu/tsptw-instances) for some TSPTW instances.

### Convert the dataset

In our problem, we also define some `key nodes` with the total waiting time should also be minimized. You can generate the dataset by running the following command:

```bash
python3 ./python/convertTestData.py <input_file> <output_file> <load>
```

where `<input_file>` is the path to the TSPTW data, `<output_file>` is the path to the MO-TSPTW data, and `<load>` is the percentage of key nodes.

You can also manually change the number of key nodes and choose the key nodes by modifying the generated file.

### Compile the code

```bash
mkdir build
cd build
cmake ..
make
```

### Run the code

```bash
./run_motsptw <data>
```

where `<data>` is the path to the MO-TSPTW data.

