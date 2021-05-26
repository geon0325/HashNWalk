#dataset_list="email-Enron-full"
#dataset_list="email-Eu-full"
#dataset_list="amazon-beauty"
#dataset_list="congress-bills-full"
dataset_list="coauth-DBLP-full"
#dataset_list="coauth-MAG-History-full"
#dataset_list="tags-ask-ubuntu"
#dataset_list="tags-stack-overflow"
#dataset_list="tags-math-sx"
#dataset_list="DAWN"
#dataset_list="patent"

g++ -O3 -std=c++11 main.cpp -o run;
IFS=,
for dataset in $dataset_list;
do
	./run $dataset;
done
rm run;
