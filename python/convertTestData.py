import sys
import random
def transform_file(input_filename, output_filename, load):
    random.seed(0)
    with open(input_filename, 'r') as fin, open(output_filename, 'w') as fout:
        lines = fin.readlines()
        n = int(lines[0])
        fout.write(f"{n + 1}\n")
        # for i in range(1, n + 1):
        #     fout.write(lines[i].strip() + " 0\n")
        # fout.write("0 " * n + "0\n")
        # fout.write(lines[n + 1].strip() + " 999\n")
        # for i in range(n + 2, n * 2 + 1):
        #     fout.write(lines[i].strip() + " " + lines[i].split()[0] + "\n")
        # fout.write("999 " * n + "0\n" )
        # for i in range(n * 2 + 1, n * 3 + 1):
        #     fout.write(lines[i])
        # fout.write(lines[n * 2 + 1])
        # for i in range(n * 3 + 1, n * 4 + 1):
        #     fout.write(lines[i])
        # fout.write("0\n")
        fout.write(lines[1].strip() + " 999\n")
        for i in range(2, n + 1):
            fout.write(lines[i].strip() + " " + lines[i].split()[0] + "\n")
        fout.write("999 " * n + "0\n" )
        for i in range(n + 1, n * 2 + 1):
            fout.write(lines[i])
        fout.write(lines[n + 1])
        num_rand = int(n * load // 100)
        random_nums = random.sample(range(n), num_rand)
        fout.write(str(num_rand) + "\n")
        for i in random_nums:
            fout.write(str(i) + " \n")
        


if __name__ == '__main__':
    if len(sys.argv) != 4:
        print("Usage: python script.py input_file output_file")
    else:
        input_file = sys.argv[1]
        output_file = sys.argv[2]
        num_rand = int(sys.argv[3])
        transform_file(input_file, output_file, num_rand)
