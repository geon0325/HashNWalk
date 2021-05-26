#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>

using namespace std;

vector<string> split(string str, char delimiter)
{
	vector<string> internal;
	stringstream ss(str);
	string temp;
	while (getline(ss, temp, delimiter)){
		internal.push_back(temp);
	}
	return internal;
}

void read_data(string path,
	       vector< vector<int> >& node2hyperedge,
	       vector< vector<int> >& hyperedge2node,
	       vector<double>& hyperedge2time,
	       vector<int>& hyperedge2anomaly)
{
	ifstream graphFile(path.c_str());
	string line;
	int num_hyperedge = 0;

	while (getline(graphFile, line)){
		vector<string> term = split(line, '\t');
		vector<string> nodes = split(term[0], ',');
		double timestamp = stod(term[1]);
		int anomaly = -1;
		if (term.size() > 2) anomaly = stoi(term[2]);
	
		vector<int> tokens;
		for (int i = 0; i < nodes.size(); i++){
			tokens.push_back(stoi(nodes[i]));
			while (node2hyperedge.size() <= stoi(nodes[i]))
				node2hyperedge.push_back(vector<int>());
			node2hyperedge[stoi(nodes[i])].push_back(num_hyperedge);
		}
		hyperedge2node.push_back(tokens);
		hyperedge2time.push_back(timestamp);
		hyperedge2anomaly.push_back(anomaly);
		num_hyperedge++;
	}
}
