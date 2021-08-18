#include <iostream>
#include <fstream>
#include <cmath>
using namespace std; 

class NFA{
public:
	uint32_t _n, _m;
	uint32_t _sstates = 0;
	uint32_t _fstates = 0;
	uint32_t _fstatescount = 0;
	uint32_t **_delta;

	NFA(){}
	NFA(const NFA &nfa){}
	~NFA(){}

	friend ostream& operator<<(ostream& os, NFA& nfa);
};

ostream& operator<< (ostream &os, NFA& nfa){

	int flag = 1;
	os << "+++ Input NFA" << endl;
	os << "	Number of states: "<< nfa._n << endl;
	os << "	Input alphabet: {";   for(int i = 0; i < nfa._m; i++){ if(flag-- == 1) os << i;	else os << "," << i;}	os << "}" << endl;	flag = 1;
	os << "	Start states: {";	for(int i = 0; i < nfa._n; i++){ if(nfa._sstates & (1U << i)){ if(flag-- == 1) os << i;	else os << "," << i;}}	os << "}" << endl;	flag = 1;
	os << "	Final states: {";	for(int i = 0; i < nfa._n; i++){ if(nfa._fstates & (1U << i)){ if(flag-- == 1) os << i;	else os << "," << i;}}	os << "}" << endl;	flag = 1;
	os << "	Transition function" << endl;

	for(int i = 0; i < nfa._n; i++){
		for(int j = 0; j < nfa._m; j++){
			os << "		Delta(" << i << "," << j << ") = {";	for(int k = 0; k < nfa._n; k++){ if(nfa._delta[i][j] & (1U << k)){ if(flag-- == 1) os << k;	else os << "," << k;}}	os << "}" << endl;	flag = 1;
		}
	}
	os << endl;
	return os;
}

class DFA{
public:
	uint32_t _n, _m;
	uint32_t _sstate = 0;
	uint32_t *_fstates;
	uint32_t _fstatescount = 0;
	uint32_t **_delta;

	DFA(){}
	DFA(const DFA &dfa){}

	friend ostream& operator<<(ostream& os, DFA& dfa);
};

ostream& operator<< (ostream &os, DFA& dfa){

	int flag = 1;
	os << "	Number of states: "<< dfa._n << endl;
	os << "	Input alphabet: {";   for(int i = 0; i < dfa._m; i++){ if(flag-- == 1) os << i;	else os << "," << i;}	os << "}" << endl;	flag = 1;
	os << "	Start state: " << dfa._sstate << endl;
	if (dfa._fstatescount >= 64)	os << "	" << dfa._fstatescount << " final states" << endl;
	else {os << "	Final states: {";	for(int i = 0; i < dfa._n; i++){ if(dfa._fstates[i] == 1){ if(flag-- == 1) os << i;	else os << "," << i;}}	os << "}" << endl;	flag = 1;}
	if (dfa._n > 64)	os << "	Transition function: Skipped" << endl;
	else{
		os << "	Transition function" << endl;
		os << "	a/p|";	for(int i = 0; i < dfa._n; ++i)	if(i<=9)os << "  " << i; else os << " " << i;	os << endl << "	---+";	for(int i = 0; i < dfa._n; ++i)	os << "---"; os << endl;
		for(int i = 0; i < dfa._m; ++i){
			os << "	 " << i <<" |";	for(int j = 0; j < dfa._n; ++j){ if(dfa._delta[j][i]<=9) os << "  "; else os << " "; os << dfa._delta[j][i];}	os << endl;
		}
	}

	os << endl;
	return os;
}

NFA readNFA(string filename){
	fstream file;
	NFA nfa;
	int it, x, q;

	file.open(filename);
	file >> nfa._n >> nfa._m;

	while(1){
		file >> it;
		if(it == -1){it = 0;	break;}
		nfa._sstates |= (1U << it);
	}
	while(1){
		file >> it;
		if(it == -1){it = 0;	break;}
		nfa._fstates |= (1U << it);
		nfa._fstatescount++;
	}

	nfa._delta = new uint32_t*[nfa._n]; 
	for (int i = 0; i < nfa._n; ++i)	nfa._delta[i] = new uint32_t[nfa._m]();
		
	while(1){
		file >> it;
		if(it == -1)break;
		file >> x >> q;
		nfa._delta[it][x] |= (1U << q);
	}

	file.close();

	cout << nfa;

	return nfa;
}

DFA subsetcons(const NFA& nfa){
	DFA dfa;

	dfa._n = pow(2, nfa._n);
	dfa._m = nfa._m;
	dfa._sstate = nfa._sstates;

	dfa._fstates = new uint32_t[dfa._n]();
	for (uint32_t i = 0; i < dfa._n; ++i){
		for (uint32_t j = 0; j < nfa._n; ++j){
			if ((nfa._fstates & (1U << j)) && (i & (1U << j))){	dfa._fstates[i] = 1;	dfa._fstatescount++;	break;}
		}
	}

	dfa._delta = new uint32_t*[dfa._n];
	for (uint32_t i = 0; i < dfa._n; ++i){
		dfa._delta[i] = new uint32_t[dfa._m]();
		for (int j = 0; j < dfa._m; ++j){
			for (int x = 0; x < nfa._n; ++x)
				if ( i & (1U << x))		dfa._delta[i][j] |= nfa._delta[x][j];
		}
	}

	cout << "+++ Converted DFA" << endl;
	cout << dfa;

	return dfa;
}

void dfs(const DFA& dfa, uint32_t *visited, uint32_t state){
	visited[state] = 1;
	for (int i = 0; i < dfa._m; ++i)
		if (visited[dfa._delta[state][i]] == 0)	dfs(dfa, visited, dfa._delta[state][i]);
}

void findreachable(DFA& dfa, uint32_t* visited){
    dfs(dfa, visited, dfa._sstate);

    int flag = 1;
    cout << "+++ Reachable states: {";	for(int i = 0; i < dfa._n; ++i){if(visited[i] == 1){if(flag-- == 1)	cout << i; else cout << "," << i;}}	cout << "}" << endl;
}

DFA rmunreachable(DFA& dfa, uint32_t *visited){
	DFA r_dfa;
	r_dfa._m = dfa._m;

	uint32_t state_count = 0;
	uint32_t *states_ind = new uint32_t[dfa._n]();
	for(uint32_t i = 0; i < dfa._n; ++i)
		if(visited[i] == 1)	states_ind[i] = state_count++;

	r_dfa._n = state_count;

	uint32_t *states_val = new uint32_t[r_dfa._n]();
	for(uint32_t i = 0; i < dfa._n; ++i)
		if(visited[i] == 1)	states_val[states_ind[i]] = i;
	

	r_dfa._sstate = states_ind[dfa._sstate];
	
	
	r_dfa._fstates = new uint32_t[r_dfa._n]();
	for(int i = 0; i < dfa._n; ++i)
		if((visited[i] == 1) && (dfa._fstates[i] == 1)){	r_dfa._fstates[states_ind[i]] = 1;	r_dfa._fstatescount++;}

	r_dfa._delta = new uint32_t*[r_dfa._n];
	for (uint32_t i = 0; i < r_dfa._n; ++i){
		r_dfa._delta[i] = new uint32_t[r_dfa._m]();
		for (int j = 0; j < r_dfa._m; ++j){
			r_dfa._delta[i][j] = states_ind[dfa._delta[states_val[i]][j]];
		}
	}	

	delete[] states_ind;
	delete[] states_val;
	delete[] visited;

	cout << endl << "+++ Reduced DFA after removing unreachable states" << endl;
	cout << r_dfa;

	return r_dfa;
}

void findequiv(DFA& dfa, int** M){
	
	int count;
	for(int i = 0;i < dfa._n; ++i)
        for(int j = 0; j < dfa._n ; ++j)
            if( ((dfa._fstates[i] == 1) && (dfa._fstates[j] == 0)) || ((dfa._fstates[i] == 0) && (dfa._fstates[j] == 1)))
                {M[i][j] = 1;	count++;}

    int flag = 0;
    while(1){
    	flag = 0;
        for(int i = 0; i < dfa._n; ++i){
            for(int j = 0; j < dfa._n; ++j){
                if(M[i][j])	continue;
                for(int x = 0; x < dfa._m; ++x){
                    if( M[dfa._delta[i][x]][dfa._delta[j][x]] ){	M[i][j] = 1;	flag = 1;	count++;}
                }
            }
        }
        if(flag == 0)	break;
    }
}

DFA collapse(DFA& dfa, int** M){
	DFA c_dfa;

	cout<<M[1][2];
	int check_array[dfa._n] = {0};

	uint32_t colapsed_states[dfa._n];
	int grp_id = 0;
	cout << "+++ Equivalent states" << endl;
	for(int i = 0; i < dfa._n; ++i){
		int flag = 1;
		if(check_array[i] == 1)	continue;
		cout << "Group " << grp_id++ <<": {";
		for(int j = i; j < dfa._n; ++j){
			if((M[j][i] == 0) && (check_array[j] == 0)){
				check_array[j] = 1;	colapsed_states[j] = grp_id - 1;
				if(flag-- == 1)cout << j;
				else	cout << "," << j;
			}
		}	cout << "}" << endl;
	}

	c_dfa._n = grp_id;
	c_dfa._m = dfa._m;

	c_dfa._fstates = new uint32_t[c_dfa._n]();
	for (int i = 0; i < dfa._n; ++i)
		if(dfa._fstates[i] == 1){c_dfa._fstates[colapsed_states[i]] = 1;	c_dfa._fstatescount++;}

	c_dfa._delta = new uint32_t*[c_dfa._n];
	for (int i = 0; i < c_dfa._n; ++i)
		c_dfa._delta[i] = new uint32_t[c_dfa._m]();

	for (int i = 0; i < dfa._n; ++i){
		for (int j = 0; j < dfa._m; ++j){
			c_dfa._delta[colapsed_states[i]][j] = colapsed_states[dfa._delta[i][j]];
		}

	}

	cout<<c_dfa;

	return c_dfa;
}

int main(int argc, char const *argv[]){

	NFA nfa;
	DFA dfa, r_dfa, c_dfa;

	if(argc == 2)	nfa = readNFA(argv[1]);
	else	nfa = readNFA(argv[2]);

	dfa = subsetcons(nfa);

	uint32_t *visited = new uint32_t[dfa._n]();
	findreachable(dfa, visited);

	r_dfa = rmunreachable(dfa, visited);

	int **M = new int*[r_dfa._n];
	for(int i = 0; i < r_dfa._n; ++i)	M[i] = new int[r_dfa._n]();
	findequiv(r_dfa, M);

	c_dfa = collapse(r_dfa, M);

	return 0;
}