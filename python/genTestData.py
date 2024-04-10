import sys

def modify_file(input_file, output_file):
    # 读取原始文件
    with open(input_file, 'r') as file:
        lines = file.readlines()

    # 获取矩阵的维度
    dimension = int(lines[0])

    # 获取原始矩阵和向量
    matrix_lines = lines[1:dimension+1]
    vector_lines = lines[dimension+1:2*dimension+1]

    # 修改矩阵
    new_matrix_lines = [' '.join(['0'] * dimension) + '\n'] * dimension + matrix_lines

    # 修改向量
    new_vector_lines = vector_lines + ['0\n'] * dimension

    # 将修改后的内容写入新文件
    with open(output_file, 'w') as file:
        file.write(str(dimension) + '\n')
        file.writelines(new_matrix_lines)
        file.writelines(new_vector_lines)

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python script.py input_file output_file")
    else:
        input_file = sys.argv[1]
        output_file = sys.argv[2]
        modify_file(input_file, output_file)
