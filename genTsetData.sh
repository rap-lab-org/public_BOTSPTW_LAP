#!/bin/bash

input_folder="./data/SolomonPotvinBengio"
output_folder="./data"

for file in $input_folder/*; do
    filename=$(basename "$file")
    output_file="$output_folder/$filename"
    python3 ./python/genTestData.py "$file" "$output_file"
done

