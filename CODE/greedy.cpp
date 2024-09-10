#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h> 
#include <sstream>
#include <string>
#include <fstream>
#define CSV_FILE_NAME "/Users/laiabonilla/Desktop/TFG/CODI/inputs/permepsy.csv"
#define TXT_FILE_NAME "/Users/laiabonilla/Desktop/TFG/CODI/inputs/mat100x20.txt"
using namespace std;

vector<vector<int> > read_csv_file() {
	ifstream file(CSV_FILE_NAME);
    	string line;
    	vector<vector<int> > v;
    	while(getline(file, line, '\n')) {
     	string word;
        	double value;
        	vector<int> row;
        	stringstream ss(line);
        	while(getline(ss, word, ';')) {
         		value = stod(word);
            	if (value != -66666) row.push_back(1);
            	else row.push_back(0);
        	}
        	v.push_back(row);
    	}
    	return v;
}

vector<vector<int> > read_csv_file_PERMEPSY() {
	ifstream file(CSV_FILE_NAME);
    	string line;
    	vector<vector<int> > v;
    	getline(file, line, '\n'); // we don't want variable names
    	while(getline(file, line, '\n')) {
     	string word;
        	double value;
        	vector<int> row;
        	stringstream ss(line);
        	while(getline(ss, word, ',')) {
         		//value = stod(word);
            	if (word != "NA") row.push_back(1);
            	else row.push_back(0);
        	}
        	v.push_back(row);
    	}
    	return v;
}

vector<vector<int> > read_txt_file() {
     ifstream file(TXT_FILE_NAME);
     string line;
     vector<vector<int> > v;
     while(getline(file, line, '\n')) {
          string word;
          double value;
          vector<int> row;
          stringstream ss(line);
          while(getline(ss, word, ' ')) {
               value = stod(word);
               if (value != 0) row.push_back(1);
               else row.push_back(0);
          }
          v.push_back(row);
     }
     return v;
}

vector<vector<int> > generate_random_matrix() {
     int n, m;
     cin >> n >> m;
     srand(time(NULL));
     vector<vector<int> > v(n, vector<int>(m));
     for (int i = 0; i < n; ++i) {
          for (int j = 0; j < m; ++j) {
               int value = rand() % 2;
               v[i][j] = value;
          }
     }
     return v;
}

void show_matrix(vector<vector<int> > v) {
     int n = v.size();
     int m = v[0].size();
     for (int i = 0; i < n; ++i) {
          for (int j = 0; j < m; ++j) {
               cout << v[i][j] << " ";
          }
          cout << endl;
     }
     cout << endl;
}

float completeness_function(vector<vector<int> > cells, vector<int> rows, vector<int> cols) {
     float res = 0;
     int n = cells.size();
     int m = cells[0].size();
     for (int i = 0; i < rows.size(); ++i) {
          for (int j = 0; j < cols.size(); ++j) {
               if (cells[rows[i]][cols[j]] == 1) res += 1;
          }
     }
     return res / (n * m);
}

// BSC algorithm for finding maximal bicliques
int main() {
	srand(time(NULL));
	
	// Generate matrix with random values
    	//vector<vector<int> > X = generate_random_matrix();
	
	// Read matrix from CSV/TXT file
    	//vector<vector<int> > X = read_csv_file();
    	//vector<vector<int> > X = read_txt_file();
    	vector<vector<int> > X = read_csv_file_PERMEPSY();

	int n = X.size();
	int m = X[0].size();

	int min_rows = 2;
	int min_cols = 2;

	//Show matrix
	show_matrix(X);

	// Inicialize N and |P| (tunning parameters)
	int N = rand() % n; // N --> times the algorithm will found a biclique
	cout << N << " ";
	int Pn = rand() % (n/3) + 3; // Pn --> number of random rows to start
	cout << Pn << endl;

	vector<int> maxI; 
	vector<int> maxJ;

	//loop N times
	for (int i = 0; i < N; ++i) {
		vector<int> P; 
		//Seeding phase
		while (P.size() != Pn) {
			int row = rand() % n;
			int cnt = count(P.begin(), P.end(), row);
			if (cnt == 0) P.push_back(row);
		}
		/*
		//Show |P| random rows
		cout << "P: ";
		for (int i = 0; i < P.size(); ++i) {
			cout << P[i] << " ";
		}
		cout << endl;
		*/
		vector<int> I; 
		vector<int> J;
		//Column addition phase
		for (int j = 0; j < m; ++j) {
			bool b = true;
			int i = 0;
			while (b == true && i < P.size()) {
				if (X[P[i]][j] == 0) b = false;
				++i;
			}
			if (b == true) J.push_back(j);
		}
		//Row addition phase
		if (not J.empty()) {
			for (int i = 0; i < n; ++i) {
				bool b = true;
				int j = 0;
				while (b == true && j < J.size()) {
					if (X[i][J[j]] == 0) b = false;
					++j;
				}
				if (b == true) I.push_back(i);
			}
		}
		//Upload MAX
		if (I.size() >= min_rows && J.size() >= min_cols) {
			if (I.size() + J.size() > maxI.size() + maxJ.size()) {
				maxI = I;
				maxJ = J;
			}
		}
		/*
		//Show list of (I,J)
		cout << "I:";
		for (int i = 0; i < I.size(); ++i) {
			cout << " " << I[i];
		}
		cout << endl;

		cout << "J:";
		for (int i = 0; i < J.size(); ++i) {
			cout << " " << J[i];
		}
		cout << endl << endl;
		*/
	}
	//Show MAX
	cout << "Rows:";
	for (int i = 0; i < maxI.size(); ++i) {
		cout << " " << maxI[i];
	}
	cout << endl;
	cout << "Columns:";
	for (int i = 0; i < maxJ.size(); ++i) {
		cout << " " << maxJ[i];
	}
	cout << endl;
	cout << "Completeness: " << completeness_function(X, maxI, maxJ) << endl;
}