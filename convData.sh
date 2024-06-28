#!/bin/bash

input_folder="./data/test_"
output_folder="./data/test__"

for file in $input_folder/*; do
    filename=$(basename "$file")
    output_file="$output_folder/$filename"
    python3 ./python/convertTestData.py "$file" "$output_file"
done
