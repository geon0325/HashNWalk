#include <cstdio>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <ctime>
#include <unordered_set>
#include <unordered_map>
#include "read_data.cpp"
#include "HyperAnom.cpp"

using namespace std;

int main(int argc, char *argv[])
{
	clock_t start;

	string dataname = argv[1];
    	int K = stoi(argv[2]);
    	int M = stoi(argv[3]);
    	double alpha = stod(argv[4]);
    	string aggregate = argv[5];
	bool edge_dependent = true;
	
	string graphFile = "../data/" + dataname + ".txt";
	string resultFile = "../results/" +  dataname + "_" + to_string(K) + "_" + to_string(M) + "_" + to_string(alpha) + "_" + aggregate + ".txt";

	// Read data
	start = clock();
	vector< vector<int> > node2hyperedge;
	vector< vector<int> > hyperedge2node;
	vector<double> hyperedge2time;
	vector<int> hyperedge2anomaly;
	read_data(graphFile, node2hyperedge, hyperedge2node, hyperedge2time, hyperedge2anomaly);
	
	int V = (int)node2hyperedge.size();
	int E = (int)hyperedge2node.size();
	cout << "# of nodes: " << V << endl;
	cout << "# of hyperedges: " << E << endl;
	cout << "Reading data done: "
	     << (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl;
	cout << "--------------------------------------------------" << endl << endl;

	// Sort by timestamp
	start = clock();
	int x = 0;
	vector<int> hyperedge2index(E);
	iota(hyperedge2index.begin(), hyperedge2index.end(), x++);
	sort(hyperedge2index.begin(), hyperedge2index.end(), [&](long long i, long long j)
		{return hyperedge2time[i] < hyperedge2time[j];});

	vector< vector<int> > hyperedges;
	vector<double> timestamps;
	vector<int> anomalies;
	for (int i = 0; i < E; i++){
		int index = hyperedge2index[i];
		hyperedges.push_back(hyperedge2node[index]);
		timestamps.push_back(hyperedge2time[index]);
		anomalies.push_back(hyperedge2anomaly[index]);
	}

	cout << "Sorting done: "
	     << (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl;
	cout << "--------------------------------------------------" << endl << endl;

	// Detect hyperedge anomalies
	start = clock();
	vector<pair<double,double> > scores;

	HyperAnom hyperanom(K, M, V, alpha, hyperedges, timestamps, edge_dependent, aggregate);
	hyperanom.init_hash();
	scores = hyperanom.run();

	cout << "Hyperedge anomaly detection done: "
	     << (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl;
	cout << "--------------------------------------------------" << endl << endl;

	// File output
	start = clock();
	ofstream outFile(resultFile.c_str());
	
	for (int i = 0; i < E; i++){
		string line = "";
		for (const int &v: hyperedges[i])
			line += to_string(v) + ",";
		line = line.substr(0, line.size()-1);

		outFile << line << "\t"
			<< to_string(timestamps[i]) << "\t"
			<< to_string(anomalies[i]) << "\t"
			<< to_string(scores[i].first) << "\t"
			<< to_string(scores[i].second) << endl;
	}

	outFile.close();
	cout << "File output done: "
	     << (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl;
	cout << "--------------------------------------------------" << endl << endl;

	return 0;
}
