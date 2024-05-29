#!/usr/bin/env python3
import os
import subprocess
import sys
import re
import json
from datetime import datetime

# 设置运行时间上限（以秒为单位）
time_limit = 60

# 存储结果的列表
results = []

def save_results(results, filename):
    with open(filename, 'w') as f:
        json.dump(results, f, indent=4)

def parse_output(output):
    match = re.search(r'number of generated labels = (\d+)', output)
    return int(match.group(1)) if match else None

def main():
    # 检查输入参数
    if len(sys.argv) != 3:
        print("Usage: {} <folder_path> <result_path>".format(sys.argv[0]))
        sys.exit(1)

    # 确保文件夹存在
    folder_path = os.path.abspath(sys.argv[1])  # 使用绝对路径
    result_path = os.path.abspath(sys.argv[2])  # 使用绝对路径
    if not os.path.isdir(folder_path):
        print("Folder not found: {}".format(folder_path))
        sys.exit(1)

    # 遍历文件夹中的文件
    for filename in os.listdir(folder_path):
        if os.path.isfile(os.path.join(folder_path, filename)):
            full_path = os.path.abspath(os.path.join(folder_path, filename))  # 获取文件的绝对路径
            print("Running run_motsptw on file: {}".format(full_path))

            # 记录开始时间
            start_time = datetime.now()

            # 执行程序
            try:
                result = subprocess.run(
                    ['./build/run_motsptw', full_path],  # 使用文件的绝对路径
                    timeout=time_limit,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT,
                    text=True,
                    check=True
                )
                return_code = 0  # 正常完成
            except subprocess.TimeoutExpired:
                return_code = 124  # 超时
                result = None
            except subprocess.CalledProcessError as e:
                print("An error occurred while running run_motsptw on file: {}".format(full_path))
                print(e)
                continue

            # 记录结束时间
            end_time = datetime.now()
            runtime = (end_time - start_time).total_seconds() * 1000

            # 存储结果
            if result:
                labels = parse_output(result.stdout)
            else:
                labels = None

            results.append({
                'file': full_path,  # 使用绝对路径
                'runtime': runtime,
                'number_of_generated_labels': labels,
                'return_code': return_code
            })

            # 输出结果
            if return_code == 124:
                print("Timeout occurred for file: {}".format(full_path))
            else:
                print("File: {}, Runtime: {} milliseconds, Number of generated labels: {}".format(
                    full_path, runtime, labels))

    # 打印所有结果
    for res in results:
        print(res)
    save_results(results, result_path)
if __name__ == "__main__":
    main()