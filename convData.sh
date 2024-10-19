#!/bin/bash


conv_data() {
	input_folder=$1
	output_folder=$2
	ratio=$3

	echo "${input_folder} ${output_folder} ${ratio}"


	mkdir -p $output_folder

	for file in $input_folder/*; do
			filename=$(basename "$file")
			output_file="$output_folder/$filename"
			python3 ./python/convertTestData.py "$file" "$output_file" "$ratio"
	done
}

dataset=(
	"./data/Dumas"
	"./data/SolomonPotvinBengio"
	"./data/AFG"
	"./data/OhlmannThomas"
)

newdir="./mo-data"

for ds in ${dataset[@]}; do
	for ratio in `seq 0 10 100`; do
		conv_data $ds "$newdir/$(basename $ds)_${ratio}" $ratio
	done
done
