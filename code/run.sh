dataset="credit-card"
K=4
M=350
alpha=0.98
agg=max

g++ -O3 -std=c++11 main.cpp -o run;
./run $dataset $K $M $alpha $agg;
rm run;
