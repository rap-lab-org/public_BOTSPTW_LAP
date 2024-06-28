def read_data(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    # Number of locations
    n = int(lines[0].strip())

    # Travel time matrix
    c = []
    for i in range(1, n + 1):
        row = list(map(int, lines[i].strip().split()))
        c.append(row)

    # Ready time and Deadline
    a = []
    b = []
    for i in range(n + 1, 2 * n + 1):
        ready_time, deadline = map(int, lines[i].strip().split())
        a.append(ready_time)
        b.append(deadline)

    return n, a, b, c

if __name__ == '__main__':
    # 使用文件路径调用函数
    file_path = '../data/Dumas/n20w20.001.txt'
    n, a, b, c = read_data(file_path)

    # 打印结果以验证是否正确
    print("Number of locations (n):", n)
    print("Ready time (a):", a)
    print("Deadline (b):", b)
    print("Travel time matrix (c):", c)
