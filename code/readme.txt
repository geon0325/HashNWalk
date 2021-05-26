The datasets should be prepared in following format. Each hyperedge is represented as:
	[nodes] <tab> [timestamp]
	ex) 0,1,3,5\t2345

To run the code, execute:
	./run.sh

You can set the hyperparamters in lines 20-22 in main.cpp.
	- K: number of hash functions
	- M: number of supernodes
	- alpha: time decaying parameter

