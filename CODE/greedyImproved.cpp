#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h> 
#include <sstream>
#include <string>
#include <fstream>
#include <numeric>
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
	if (rows.empty() || cols.empty()) return 0;
     float res = 0;
     int n = rows.size();
     int m = cols.size();
     for (int i = 0; i < n; ++i) {
          for (int j = 0; j < m; ++j) {
               if (cells[rows[i]][cols[j]] == 1) res += 1;
          }
     }
     return res / (n * m);
}

int sumSolutionCells(vector<vector<int> > cells, vector<int> rows, vector<int> cols) {
	if (rows.empty() || cols.empty()) return 0;
     float res = 0;
     int n = rows.size();
     int m = cols.size();
     for (int i = 0; i < n; ++i) {
          for (int j = 0; j < m; ++j) {
               if (cells[rows[i]][cols[j]] == 1) res += 1;
               else res -= 1;
          }
     }
     return res;
}

bool meetsRowAdditionCriteria(vector<vector<int> > cells, vector<int> cols, int row, int criteria) {
	//cout << "Trying to add row " << row << endl;
	bool b = false;
	int cnt = 0;
	for (int i = 0; i < cols.size(); ++i) {
		if (cells[row][cols[i]] == 1) cnt += 1;
		//cout << "For column " << cols[i] << " count = " << cnt << endl;
	}
	float res = float(cnt) / float(cols.size());
	if (res >= float(criteria) / float(100)) b = true;
	return b;
}

bool meetsColAdditionCriteria(vector<vector<int> > cells, vector<int> rows, int col, int criteria) {
	//cout << "Trying to add col " << col << endl;
	bool b = false;
	int cnt = 0;
	for (int i = 0; i < rows.size(); ++i) {
		if (cells[rows[i]][col] == 1) cnt += 1;
		//cout << "For row " << rows[i] << " count = " << cnt << endl;
	}
	float res = float(cnt) / float(rows.size());
	if (res >= float(criteria) / float(100)) b = true;
	return b;
}

// BSC algorithm for finding maximal bicliques
int main() {
	srand(time(NULL));
	
	// Generate matrix with random values
    	vector<vector<int> > cells = generate_random_matrix();
	
	// Read matrix from CSV/TXT file
    	//vector<vector<int> > cells = read_csv_file();
    	//vector<vector<int> > cells = read_txt_file();
    	//vector<vector<int> > cells = read_csv_file_PERMEPSY();

	int n = cells.size();
	int m = cells[0].size();

	int min_rows = 20;
	int min_cols = 20;

	//Show matrix
	show_matrix(cells);

	// Inicialize N and |randomRows| (tunning parameters)
	//int N = rand() % n + 1; // N --> times the algorithm will found a biclique
	int N = 10;
	//cout << N << " ";
	int Pn = rand() % (n/3) + 3; // Pn --> number of random rows to start
	//cout << Pn << endl;

	vector<int> bestSolRows(n);
     vector<int> bestSolCols(m);
     iota(begin(bestSolRows), end(bestSolRows), 0);
    	iota(begin(bestSolCols), end(bestSolCols), 0);

    	vector<int> bestSolRows2(n);
     vector<int> bestSolCols2(m);
     iota(begin(bestSolRows2), end(bestSolRows2), 0);
    	iota(begin(bestSolCols2), end(bestSolCols2), 0);

	//loop N times
	for (int i = 0; i < N; ++i) {
		vector<int> rows; 
		vector<int> cols;
		while (rows.empty() || cols.empty()) { // To ensure we find a solution every iteration of the N times loop 
			vector<int> randomRows; 
			//Seeding phase
			while (randomRows.size() != Pn) {
				int row = rand() % n;
				int cnt = count(randomRows.begin(), randomRows.end(), row);
				if (cnt == 0) randomRows.push_back(row);
			}
			/*
			//Show |randomRows| random rows
			cout << "randomRows: ";
			for (int i = 0; i < randomRows.size(); ++i) {
				cout << randomRows[i] << " ";
			}
			cout << endl;
			*/
			//Column addition phase
			for (int j = 0; j < m; ++j) {
				bool b = true;
				int i = 0;
				while (b && i < randomRows.size()) {
					if (cells[randomRows[i]][j] == 0) b = false;
					++i;
				}
				if (b) cols.push_back(j);
			}
			//Row addition phase
			if (not cols.empty()) {
				for (int i = 0; i < n; ++i) {
					bool b = true;
					int j = 0;
					while (b == true && j < cols.size()) {
						if (cells[i][cols[j]] == 0) b = false;
						++j;
					}
					if (b == true) rows.push_back(i);
				}
			}
		}
		/*
		cout << "rows before improvement:";
		for (int i = 0; i < rows.size(); ++i) {
			cout << " " << rows[i];
		}
		cout << endl;

		cout << "cols before improvement:";
		for (int i = 0; i < cols.size(); ++i) {
			cout << " " << cols[i];
		}
		cout << endl << endl;
		*/
		// NEW Improvement phase --> add row that meets criteria to the solution
		int criteria = 90;
		while ((rows.size() < min_rows) && (criteria != -10)) {
			for(int i = 0; i < n; ++i) {
				int cnt = count(rows.begin(), rows.end(), i);
				if (cnt == 0) { // row "i" not in solution rows
					//cout << "Trying to add row " << i << endl;
					bool b = meetsRowAdditionCriteria(cells, cols, i, criteria);
					//cout << b << endl;
					if (b) rows.push_back(i);
				}
			}
			criteria -= 10;
		}
		
		// NEW Improvement phase --> add column that meets criteria to the solution
		criteria = 90;
		while ((cols.size() < min_cols) && (criteria != -10)) {
			for(int i = 0; i < m; ++i) {
				int cnt = count(cols.begin(), cols.end(), i);
				if (cnt == 0) { // col "i" not in solution cols
					//cout << "Trying to add col " << i << endl;
					bool b = meetsColAdditionCriteria(cells, rows, i, criteria);
					//cout << b << endl;
					if (b) cols.push_back(i);
				}
			}
			criteria -= 10;
		}
		/*
		cout << "rows after improvement:";
		for (int i = 0; i < rows.size(); ++i) {
			cout << " " << rows[i];
		}
		cout << endl;

		cout << "cols after improvement:";
		for (int i = 0; i < cols.size(); ++i) {
			cout << " " << cols[i];
		}
		cout << endl << endl;
		*/

		//Update Best Solution (Completeness criteria)
		if (completeness_function(cells, rows, cols) > completeness_function(cells, bestSolRows, bestSolCols)) {
			bestSolRows = rows;
			bestSolCols = cols;
			sort(bestSolRows.begin(), bestSolRows.end());
			sort(bestSolCols.begin(), bestSolCols.end());
		}
		//Update Best Solution (Sum of Solution Cells criteria)
		if (sumSolutionCells(cells, rows, cols) > sumSolutionCells(cells, bestSolRows2, bestSolCols2)) {
			bestSolRows2 = rows;
			bestSolCols2 = cols;
			sort(bestSolRows2.begin(), bestSolRows2.end());
			sort(bestSolCols2.begin(), bestSolCols2.end());
		}
		/*
		//Show list of (rows,cols)
		cout << "rows:";
		for (int i = 0; i < rows.size(); ++i) {
			cout << " " << rows[i];
		}
		cout << endl;

		cout << "cols:";
		for (int i = 0; i < cols.size(); ++i) {
			cout << " " << cols[i];
		}
		cout << endl << endl;
		*/
	}

	//Show Best Solution (Completeness criteria)
	cout << "Best Solution Rows (Completeness criteria):";
	for (int i = 0; i < bestSolRows.size(); ++i) {
		cout << " " << bestSolRows[i];
	}
	cout << endl;
	cout << "Best Solution Columns:";
	for (int i = 0; i < bestSolCols.size(); ++i) {
		cout << " " << bestSolCols[i];
	}
	cout << endl;
	/*
	//Show Best Solution (Sum of Solution Cells criteria)
	cout << "Best Solution Rows (Sum of Solution Cells criteria):";
	for (int i = 0; i < bestSolRows2.size(); ++i) {
		cout << " " << bestSolRows2[i];
	}
	cout << endl;
	cout << "Best Solution Columns:";
	for (int i = 0; i < bestSolCols2.size(); ++i) {
		cout << " " << bestSolCols2[i];
	}
	cout << endl;
	*/
	
	// Print completeness
	vector<int> allRows(n);
     vector<int> allCols(m);
     iota(begin(allRows), end(allRows), 0);
    	iota(begin(allCols), end(allCols), 0);
    	cout << "Rows size: " << n << endl;
    	cout << "Cols size: " << m << endl;
     cout << "Completeness original dataset: " << completeness_function(cells, allRows, allCols) << endl;
	cout << "Completeness of solution dataset (Completeness criteria): " << completeness_function(cells, bestSolRows, bestSolCols) << endl;
	cout << "Sum Solution Cells (Completeness criteria): " << sumSolutionCells(cells, bestSolRows, bestSolCols) << endl;
	//cout << "Completeness of solution dataset (Sum of Solution Cells criteria): " << completeness_function(cells, bestSolRows2, bestSolCols2) << endl;
	//cout << "Sum Solution Cells (Sum of Solution Cells criteria): " << sumSolutionCells(cells, bestSolRows2, bestSolCols2) << endl;
}