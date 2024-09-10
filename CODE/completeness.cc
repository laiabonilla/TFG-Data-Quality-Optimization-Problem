#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h> 
using namespace std;


float completeness_function(vector<vector<int> > cells, vector<int> rows, vector<int> cols) {
     float res = 0;
     int n = cells.size();
     int m = cells[0].size();
     for (int i = 0; i < rows.size(); ++i) {
          for (int j = 0; j < cols.size(); ++j) {
               if (cells[rows[i]][cols[j]] == 1) res += 1;
               //else res -= 1;
          }
     }
     return res / (n * m);
}

int main() {
	srand(time(NULL));
	
	int n, m;
	cin >> n >> m;
	vector<vector<int> > cells(n, vector<int>(m));

	//Fill matrix with random values
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			int value = rand() % 2;
			cells[i][j] = value;
		}
	}
	//Show matrix
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			cout << cells[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;

	//Select random rows and cols
	int nrows = rand() % n;
	int ncols = rand() % m;

	vector<int> rows;
	vector<int> cols;
	while (rows.size() != nrows) {
		int row = rand() % n;
		int cnt = count(rows.begin(), rows.end(), row);
		if (cnt == 0) rows.push_back(row);
	}
	while (cols.size() != ncols) {
		int col = rand() % n;
		int cnt = count(cols.begin(), cols.end(), col);
		if (cnt == 0) cols.push_back(col);
	}
	cout << "rows: ";
	for (int i = 0; i < rows.size(); ++i) {
		cout << rows[i] << " ";
	}
	cout << endl;
	cout << "cols: ";
	for (int i = 0; i < cols.size(); ++i) {
		cout << cols[i] << " ";
	}
	cout << endl;

	// Compute completeness
	float res = completeness_function(cells, rows, cols);
	
	cout << "Completeness = " << res << endl;
}