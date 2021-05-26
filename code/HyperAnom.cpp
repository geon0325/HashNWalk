#include <iostream>
#include <random>
#include <numeric>
#include <cmath>
#include <ctime>
#include <queue>
#include <unordered_map>
#define EPSILON 1e-5

using namespace std;

class HyperAnom{
public:
	HyperAnom(int K, int M, int V, double alpha,
		  vector< vector<int> >& hyperedges, 
		  vector<double>& timestamps,
		  bool edge_dependent,
		  string agg);
	void init_hash();
	void update(vector<int>& hyperedge, double timestamp);
	pair<double,double> anomaly_score(vector<int>& hyperedge, double timestamp, string agg);
	vector<pair<double,double> > run();
	void print_matrix();
private:
	bool edge_dependent;
	int K, M, V;
	double alpha;
	string agg;
	vector< vector<int> > node2hash, hyperedges;
	vector<double> timestamps;
	vector< vector<double> > T_agg, T_pre, D_agg, D_pre;
	vector< vector< vector<double> > > S_agg, S_pre;
};

HyperAnom::HyperAnom(int K, int M, int V, double alpha,
		     vector< vector<int> >& hyperedges,
		     vector<double>& timestamps,
		     bool edge_dependent, 
		     string agg)
{	
	this->edge_dependent = edge_dependent;
	this->agg = agg;
	this->K = K;
	this->M = M;
	this->V = V;
	this->alpha = alpha;
	this->hyperedges = hyperedges;
	this->timestamps = timestamps;
	this->D_agg.resize(this->K);
	this->D_pre.resize(this->K);
	this->T_agg.resize(this->K);
	this->T_pre.resize(this->K);
	this->S_agg.resize(this->K);
	this->S_pre.resize(this->K);
	for (int k = 0; k < this->K; k++){
		this->D_agg[k].resize(this->M, 0);
		this->D_pre[k].resize(this->M, 0);
		this->T_agg[k].resize(this->M, 0);
		this->T_pre[k].resize(this->M, 0);
		this->S_agg[k].resize(this->M, vector<double>(this->M, 0.0));
		this->S_pre[k].resize(this->M, vector<double>(this->M, 0.0));
	}
}

void HyperAnom::init_hash()
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dist(0, this->M-1);

	for (int k = 0; k < this->K; k++){
		vector<int> n2h;
		for (int v = 0; v < this->V; v++)
			n2h.push_back(dist(gen));
		this->node2hash.push_back(n2h);
	}
}

void HyperAnom::update(vector<int>& hyperedge, double timestamp)
{
	for (int k = 0; k < this->K; k++){
		int R = 0;
		unordered_map<int,int> supernodes;
		for (const int &v: hyperedge){
			int supernode = this->node2hash[k][v];
			if (this->edge_dependent){
				R++;
				supernodes[supernode] += 1;
			} else{
				if (supernodes[supernode] == 0) R++;
				supernodes[supernode] = 1;
			}
		}
		
		for (auto& i: supernodes){
			int vi = i.first;
			this->D_agg[k][vi] += 1.0;
			this->T_agg[k][vi] += pow(this->alpha, -timestamp);
			for (auto& j: supernodes){
				int vj = j.first;
				this->S_agg[k][vi][vj] += pow(this->alpha, -timestamp) * ((double)j.second / R);
			}
		}
	}
}

pair<double,double> HyperAnom::anomaly_score(vector<int>& hyperedge, double timestamp, string agg)
{
	double ret_scoreU = -1e8, ret_scoreB = -1e8;
	for (int k = 0; k < this->K; k++){
		int R = 0;
		unordered_map<int,int> supernodes;
		vector<int> _keys;
		for (const int &v: hyperedge){
			int supernode = this->node2hash[k][v];
			if (this->edge_dependent){
				R++;
				if (supernodes[supernode] == 0) _keys.push_back(supernode);
				supernodes[supernode] += 1;
			} else{
				if (supernodes[supernode] == 0){
				       R++;
				       _keys.push_back(supernode);
				}
				supernodes[supernode] = 1;
			}
		}

		double scoreU = 0, scoreB = 0;
		if (agg.compare("max") == 0){
			scoreU = -1e8; 
			scoreB = -1e8;
		} else if (agg.compare("gmean") == 0){
			scoreU = 1;
			scoreB = 1;
		}

		for (auto vi : _keys){
			double d_i = this->D_agg[k][vi] - this->D_pre[k][vi];
			for (auto vj : _keys){		
				double p_ij = (double)supernodes[vj] / R;
				double p_ex = this->T_pre[k][vi] > 0 ? 
				  	      max(this->S_pre[k][vi][vj] / this->T_pre[k][vi], EPSILON) : EPSILON;
				if (agg.compare("mean") == 0 or agg.compare("sum") == 0){
					scoreU += log(p_ij / p_ex);
					scoreB += d_i * log(p_ij / p_ex);
				} else if (agg.compare("max") == 0){
					scoreU = max(scoreU, log(p_ij / p_ex));
					scoreB = max(scoreB, d_i * log(p_ij / p_ex));
				} else if (agg.compare("gmean") == 0){
					scoreU *= log(p_ij / p_ex);
					scoreB *= d_i * log(p_ij / p_ex);
				}
			}
		}
		if (agg.compare("mean") == 0){
			scoreU /= (supernodes.size() * supernodes.size());
			scoreB /= (supernodes.size() * supernodes.size());
		} else if (agg.compare("gmean") == 0){
			scoreU = pow(scoreU, 1/(supernodes.size() * supernodes.size()));
			scoreB = pow(scoreB, 1/(supernodes.size() * supernodes.size()));
		}
		ret_scoreU = max(ret_scoreU, scoreU);
		ret_scoreB = max(ret_scoreB, scoreB);
	}
	return make_pair(ret_scoreU, ret_scoreB);
}

vector<pair<double,double> > HyperAnom::run()
{
	clock_t start = clock();
	pair<double,double> score;
	vector<pair<double,double> > scores;
	queue<int> index;

	for (int i = 0; i < this->hyperedges.size(); i++){
		
		if (i % 100000 == 0 and i > 0){
			double runtime = (double)(clock() - start) / CLOCKS_PER_SEC;
			double expected = (double)this->hyperedges.size() * runtime / i;
			cout << i << " / " << this->hyperedges.size()
			     << "\t" << runtime << "\t" << expected 
			     << "\t(" << runtime / expected << ")" <<  endl;
		}
	

		if (i > 0 and this->timestamps[i] != this->timestamps[i-1]){
			while (!index.empty()){
				int j = index.front(); index.pop();
				score = this->anomaly_score(this->hyperedges[j], this->timestamps[j], this->agg);
				scores.push_back(score);
			}
			this->D_pre = this->D_agg;
			this->T_pre = this->T_agg;
			this->S_pre = this->S_agg;
		}
		this->update(this->hyperedges[i], this->timestamps[i]);
		index.push(i);
	}
	while (!index.empty()){
		int j = index.front(); index.pop();
		score = this->anomaly_score(this->hyperedges[j], this->timestamps[j], this->agg);
		scores.push_back(score);
	}

	double sparsity = 0;
	for (int k = 0; k < this->K; k++){
		for (int i = 0; i < this->M; i++){
			for (int j = 0; j < this->M; j++)
				if (this->S_agg[k][i][j] > 0) sparsity += 1;
		}
	}
	cout << "sparsity:\t" << sparsity / (this->K * this->M * this->M) << endl;

	return scores;
}

void HyperAnom::print_matrix()
{
	for (int i = 0; i < this->M; i++){
		for (int j = 0; j < this->M; j++){
			cout << this->S_agg[0][i][j] / this->T_agg[0][i] << endl;
		}	
		cout << endl;
	}
	cout << endl;
}
