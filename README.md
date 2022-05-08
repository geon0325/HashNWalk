# HashNWalk: Hash and Random Walk Based Anomaly Detection in Hyperedge Streams

Source code for the paper [HashNWalk: Hash and Random Walk Based Anomaly Detection in Hyperedge Streams](https://arxiv.org/pdf/2204.13822.pdf), Geon Lee, Minyoung Choe, and Kijung Shin, [IJCAI 2022](https://ijcai-22.org/).

*Sequences of group interactions, such as emails, online discussions, and co-authorships, are ubiquitous; and they are naturally represented as a stream of hyperedges (i.e.,  sets of nodes). Despite its broad potential applications, anomaly detection in hypergraphs (i.e., sets of hyperedges) has received surprisingly little attention, compared to anomaly detection in graphs. While it is tempting to reduce hypergraphs to graphs and apply existing graph-based methods, according to our experiments, taking higher-order structures of hypergraphs into consideration is worthwhile.*
*We propose HashNWalk, an incremental algorithm that detects anomalies in a stream of hyperedges. It maintains and updates a constant-size summary of the structural and temporal information about the input stream. Using the summary, which is the form of a proximity matrix, HashNWalk measures the anomalousness of each new hyperedge as it appears. HashNWalk is **(a) Fast:** it processes each hyperedge in near real-time and billions of hyperedges within a few hours, **(b) Space Efficient:** the size of the maintained summary is a user-specific constant, **(c) Effective:** it successfully detects anomalous hyperedges in real-world hypergraphs.*

## Datasets
* Datasets used in the paper is in [data](https://github.com/geonlee0325/HashNWalk/tree/main/data).
* Input file should be formatted as (it does not need to be sorted):
```
(nodes) <tab> (timestamp) <tab> (0 = normal/1 = anomaly)

e.g.,
126     9986.0  0
126,95  10000.0 0
40,133  4008.0  1
```

## How to Run
* To run demos, execute following commend in [code](https://github.com/geonlee0325/HashNWalk/tree/main/code):
```
./run.sh
```
* In HashNWalk, there are several hyperparameters:
```
K:	# of hash functions
M:	# of supernodes
alpha:	time-decaying parameter
agg:	aggregation function (max or mean)
```
* To run with specified hyperparameters, execute following commends in [code](https://github.com/geonlee0325/HashNWalk/tree/main/code):
```
g++ -O3 -std=c++11 main.cpp -o run;
./run (dataset) (K) (M) (alpha) (agg)
```
* Results will be saved in [results](https://github.com/geonlee0325/HashNWalk/tree/main/results) as:
```
Filename: (dataset)_(K)_(M)_(alpha)_(agg).txt

(nodes) <tab> (timestamp) <tab> (0 = normal/1 = anomaly) <tab> score_U <tab> score_B

e.g.,
370,852,173,795,815,261761      9269.468494     0       9.721166        9.721166
651,497,747,461,699,261762      9269.526367     1       10.414313       10.414313
602,415,890,382,11,261763       9269.526367     0       9.721166        9.721166
```
