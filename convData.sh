#!/bin/bash

input_folder="./data/Dumas"
output_folder="./data/Dumas_100"

mkdir -p $output_folder

for file in $input_folder/*; do
    filename=$(basename "$file")
    output_file="$output_folder/$filename"
    python3 ./python/convertTestData.py "$file" "$output_file" "100"
done
