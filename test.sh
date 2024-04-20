#!/bin/bash

# 设置运行时间上限（以秒为单位）
time_limit=60

# 检查输入参数
if [ $# -ne 1 ]; then
    echo "Usage: $0 <folder_path>"
    exit 1
fi

# 确保文件夹存在
if [ ! -d "$1" ]; then
    echo "Folder not found: $1"
    exit 1
fi

# 遍历文件夹中的文件
for file in "$1"/*; do
    if [ -f "$file" ]; then
        echo "Running run_motsptw on file: $file"
        
        # 记录开始时间（毫秒为单位）
        start_time=$(($(date +%s%N)/1000000))
        
        # 执行程序，并将输出和运行时间写入另一个文件
        timeout -s 9 $time_limit ./build/run_motsptw "$file" &> temp_output.log
        return_code=$?
        
        # 记录结束时间（毫秒为单位）
        end_time=$(($(date +%s%N)/1000000))
        runtime=$((end_time - start_time))
        
        # 如果超时，记录日志并跳过到下一个文件
        if [ $return_code -eq 124 ]; then
            echo "Timeout occurred for file: $file" >> output.log
            continue
        fi
        
        # 记录运行时间和输出
        echo "File: $file, Runtime: $runtime milliseconds" >> output.log
        cat temp_output.log >> output.log
    fi
done

