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
#include "gurobi_c++.h"
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
     int n = rows.size();
     int m = cols.size();
     for (int i = 0; i < n; ++i) {
          for (int j = 0; j < m; ++j) {
               if (cells[rows[i]][cols[j]] == 1) res += 1;
          }
     }
     return res / (n * m);
}

void convert_subInstance(vector<vector<int> > cells, vector<vector<int> >& subcells, vector<int> subInstanceRows, vector<int> subInstanceCols) {
	for (int i = 0; i < subInstanceRows.size(); ++i) {
		for (int j = 0; j < subInstanceCols.size(); ++j) {
			subcells[i][j] = cells[subInstanceRows[i]][subInstanceCols[j]];
		}
	}
}

void constructSolution(vector<vector<int> > cells, vector<int>& solRows, vector<int>& solCols, int N, int Pn) {
	// Generate initial solution using a contructive method --> GREEDY

	int n = cells.size();
	int m = cells[0].size();

	int min_rows = 2;
	int min_cols = 2;
	/*
	// Inicialize N and |P| (tunning parameters)
	int N = rand() % n; // N --> times the algorithm will found a biclique
	//cout << N << " ";
	int Pn = rand() % (n/3) + 3; // Pn --> number of random rows to start
	//cout << Pn << endl;
	*/
	//loop N times
	for (int i = 0; i < N; ++i) {
		vector<int> P; 
		//Seeding phase
		while (P.size() != Pn) {
			int row = rand() % n;
			int cnt = count(P.begin(), P.end(), row);
			if (cnt == 0) P.push_back(row);
		}
		vector<int> I; 
		vector<int> J;
		//Column addition phase
		for (int j = 0; j < m; ++j) {
			bool b = true;
			int i = 0;
			while (b == true && i < P.size()) {
				if (cells[P[i]][j] == 0) b = false;
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
					if (cells[i][J[j]] == 0) b = false;
					++j;
				}
				if (b == true) I.push_back(i);
			}
		}
		//Upload MAX
		if (I.size() >= min_rows && J.size() >= min_cols) {
			if (I.size() + J.size() > solRows.size() + solCols.size()) {
				solRows = I;
				solCols = J;
			}
		}
	}
}

void mergeSolution(vector<int> candidateSolRows, vector<int> candidateSolCols, vector<int>& subInstanceRows, vector<int>& subInstanceCols, vector<int>& rowsSetAgeToZero, vector<int>& colsSetAgeToZero) {
	// Merge candidate solutions with best solution and select which rows/cols age has to be set to 0
	for (int i = 0; i < candidateSolRows.size(); ++i) {
		int cnt = count(subInstanceRows.begin(), subInstanceRows.end(), candidateSolRows[i]);
		if (cnt == 0) {
			subInstanceRows.push_back(candidateSolRows[i]);
			rowsSetAgeToZero.push_back(candidateSolRows[i]);
		}
	}
	for (int i = 0; i < candidateSolCols.size(); ++i) {
		int cnt = count(subInstanceCols.begin(), subInstanceCols.end(), candidateSolCols[i]);
		if (cnt == 0) {
			subInstanceCols.push_back(candidateSolCols[i]);
			colsSetAgeToZero.push_back(candidateSolCols[i]);
		}
	}
}

void gurobi(vector<vector<int> > cells, vector<vector<int> > subcells, vector<int> subInstanceRows, vector<int> subInstanceCols, vector<int>& improvedSolRows, vector<int>& improvedSolCols) {
	// Apply solver to improve solution
	try {
		int n = subcells.size();
        	int m = subcells[0].size();

        	int min_rows = 2;
        	int min_cols = 2;

		// Gurobi model
	    	GRBEnv env = GRBEnv();
	    	GRBModel model = GRBModel(env);

		// Define variables
		GRBVar s[n][m];
		for (int i = 0; i < n; ++i) {
	    	for (int j = 0; j < m; ++j) {
	        	s[i][j] = model.addVar(0, 1, 0, GRB_BINARY);
	        }
	    	}
		GRBVar x[n];
	    	for (int i = 0; i < n; ++i) {
			x[i] = model.addVar(0, 1, 0, GRB_BINARY);
		}
		GRBVar y[m];
		for (int i = 0; i < m; ++i) {
			y[i] = model.addVar(0, 1, 0, GRB_BINARY);
		}

		// Constraints
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < m; ++j) {
				model.addConstr(s[i][j] <= x[i], "c1");
				model.addConstr(s[i][j] <= y[j], "c2");
				model.addConstr(s[i][j] >= x[i] + y[j] - 1, "c3");
			}
		}
		GRBLinExpr xsum = 0;
		GRBLinExpr ysum = 0;
		for (int i = 0; i < n; ++i) {
			xsum += x[i];
		}
		for (int i = 0; i < m; ++i) {
			ysum += y[i];
		}
		model.addConstr(xsum >= min_rows, "c4");
		model.addConstr(ysum >= min_cols, "c5");

		// Set objective
		GRBLinExpr sum;
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < m; ++j) {
				if(subcells[i][j] == 0) sum -= s[i][j];
				else sum += s[i][j];
			}
		}
		model.setObjective(sum, GRB_MAXIMIZE);

		// Avoid printing optimization
		model.set(GRB_IntParam_LogToConsole, 0);

		// Optimize model
		model.optimize();


		// Print information
		/*
		cout << "Objective value: " << model.get(GRB_DoubleAttr_ObjVal) << endl;
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < m; ++j) {
				cout << abs(s[i][j].get(GRB_DoubleAttr_X)) << " ";
			}
			cout << endl;
		}
		*/
		vector<int> rows;
		//cout << "Improved Rows: ";
		for (int i = 0; i < n; ++i) {
			if (abs(x[i].get(GRB_DoubleAttr_X)) == 1) {
				improvedSolRows.push_back(subInstanceRows[i]);
				rows.push_back(i);
				//cout << subInstanceRows[i] << " ";
			}
		}
		//cout << endl;
		vector<int> cols;
		//cout << "Improved Columns: ";
		for (int i = 0; i < m; ++i) {
			if (abs(y[i].get(GRB_DoubleAttr_X)) == 1) {
				improvedSolCols.push_back(subInstanceCols[i]);
				cols.push_back(i);
				//cout << subInstanceCols[i] << " ";
			}
		}
		//cout << endl;
		//cout << "Improved Completeness: " << completeness_function(cells, improvedSolRows, improvedSolCols) << endl;
	}
	catch(GRBException e) {
		cout << "Error code = " << e.getErrorCode() << endl;
		cout << e.getMessage() << endl;
	} 
	catch(...) {
		cout << "Exception during optimization" << endl;
	}

}

int main(int argc, char *argv[]) {

	int N, Pn, na, maxIterations, maxAge;
	N = atoi(argv[1]);
	Pn = atoi(argv[2]);
	na = atoi(argv[3]);
	maxIterations = atoi(argv[4]);
	maxAge = atoi(argv[5]);

	srand(time(NULL));
	// Generate matrix with random values
	//vector<vector<int> > cells = generate_random_matrix();
          
	// Read matrix from CSV/TXT file
	//vector<vector<int> > cells = read_csv_file();
	vector<vector<int> > cells = read_txt_file();
	//vector<vector<int> > cells = read_csv_file_PERMEPSY();

 	int n = cells.size();
   	int m = cells[0].size();

   	int min_rows = 2;
	int min_cols = 2;

	// Show matrix
	show_matrix(cells);
	
	// Metaheuristic parameters
	//int maxIterations = 50;
	int currentIteration = 0;
	vector<int> bestSolRows;
	vector<int> bestSolCols;
	int bestCompleteness = 0;
	vector<int> subInstanceRows;
	vector<int> subInstanceCols;
	vector<int> rowsAges = vector<int>(n, 0);
	vector<int> colsAges = vector<int>(m, 0);
	//int maxAge = 5;


	while (currentIteration < maxIterations) { // Or CPU time limit not reached
		//int na = 5; //Tunning parameter
		//while ((i < na) || subInstanceRows.size() == 0) --> PER A ARREGLAR QUE DE VEGADES NO TROBE SOL DE GREEDY
		for (int i = 0; i < na; ++i) {	
			// Construct phase
			vector<int> candidateSolRows;
			vector<int> candidateSolCols;
			constructSolution(cells, candidateSolRows, candidateSolCols, N, Pn);
			//cout << candidateSolRows.size() << " " << candidateSolCols.size() << endl;
			//cout << "Iteration " << i << ":" << endl << "Rows:";
			/*
			for (int i = 0; i < candidateSolRows.size(); ++i) {
				cout << " " << candidateSolRows[i];
			}
			cout << endl << "Cols:";
			for (int i = 0; i < candidateSolCols.size(); ++i) {
				cout << " " << candidateSolCols[i];
			}
			cout << endl;
			*/

			// Merge phase
			vector<int> rowsSetAgeToZero;
			vector<int> colsSetAgeToZero;
			mergeSolution(candidateSolRows, candidateSolCols, subInstanceRows, subInstanceCols, rowsSetAgeToZero, colsSetAgeToZero);
			for (int i = 0; i < rowsSetAgeToZero.size(); ++i) {
				rowsAges[rowsSetAgeToZero[i]] = 0;
			}
			for (int i = 0; i < colsSetAgeToZero.size(); ++i) {
				colsAges[colsSetAgeToZero[i]] = 0;
			}
		}
		sort(subInstanceRows.begin(), subInstanceRows.end());
		sort(subInstanceCols.begin(), subInstanceCols.end());
		/*
		cout << "Merged Rows:";
		for (int i = 0; i < subInstanceRows.size(); ++i) {
			cout << " " << subInstanceRows[i];
		}
		cout << endl << "Merged Cols:";
		for (int i = 0; i < subInstanceCols.size(); ++i) {
			cout << " " << subInstanceCols[i];
		}
		cout << endl;
		*/
	
		// Solve phase 
		vector<int> improvedSolRows;
		vector<int> improvedSolCols; 
		vector<vector<int> > subcells(subInstanceRows.size(), vector<int>(subInstanceCols.size()));
		convert_subInstance(cells, subcells, subInstanceRows, subInstanceCols);
		gurobi(cells, subcells, subInstanceRows, subInstanceCols, improvedSolRows, improvedSolCols);
		
		// Adapt phase
		//adapt(cells, improvedSolRows, improvedSolCols);
		for (int i = 0; i < n; ++i) {
			int cnt1 = count(subInstanceRows.begin(), subInstanceRows.end(), i);
			if (cnt1 == 1) rowsAges[i] += 1;
			int cnt2 = count(improvedSolRows.begin(), improvedSolRows.end(), i);
			if (cnt2 == 1) rowsAges[i] = 0;
		}
		for (int i = 0; i < m; ++i) {
			int cnt1 = count(subInstanceCols.begin(), subInstanceCols.end(), i);
			if (cnt1 == 1) colsAges[i] += 1;
			int cnt2 = count(improvedSolCols.begin(), improvedSolCols.end(), i);
			if (cnt2 == 1) colsAges[i] = 0;
		}
		vector<int> rowsToRemove;
		vector<int> colsToRemove;
		for (int i = 0; i < subInstanceRows.size(); ++i) {
			int cnt = count(improvedSolRows.begin(), improvedSolRows.end(), subInstanceRows[i]);
			if (cnt == 0 && rowsAges[subInstanceRows[i]] >= maxAge) {
    				rowsToRemove.push_back(subInstanceRows[i]);
			}
		}
		for (int i = 0; i < subInstanceCols.size(); ++i) {
			int cnt = count(improvedSolCols.begin(), improvedSolCols.end(), subInstanceCols[i]);
			if (cnt == 0 && colsAges[subInstanceCols[i]] >= maxAge) {
    				colsToRemove.push_back(subInstanceCols[i]);
			}
		}
		/*
		cout << "Rows ages:";
		for (int i = 0; i < rowsAges.size(); ++i) {
			cout << " " << rowsAges[i];
		} cout << endl;
		cout << "Rows to remove: ";
		for (int i = 0; i < rowsToRemove.size(); ++i) {
			cout << " " << rowsToRemove[i];
		} cout << endl;

		cout << "Subinstance Rows before removing:";
		for (int i = 0; i < subInstanceRows.size(); ++i) {
			cout << " " << subInstanceRows[i];
		}
		cout << endl;
		*/
		for(int i = 0; i < rowsToRemove.size(); ++i) {
			vector<int>::iterator position = find(subInstanceRows.begin(), subInstanceRows.end(), rowsToRemove[i]);
			if (position != subInstanceRows.end()) subInstanceRows.erase(position);
			//cout << "Removing " << rowsToRemove[i] << " from subIntanceRows" << endl;
		}
		/*
		cout << "Subinstance Rows after removing:";
		for (int i = 0; i < subInstanceRows.size(); ++i) {
			cout << " " << subInstanceRows[i];
		}
		cout << endl;
		*/
		for(int i = 0; i < colsToRemove.size(); ++i) {
			vector<int>::iterator position = find(subInstanceCols.begin(), subInstanceCols.end(), colsToRemove[i]);
			if (position != subInstanceCols.end()) subInstanceCols.erase(position);
			//cout << "Removing " << colsToRemove[i] << " from subInstanceCols" << endl;
		}
		/*
		cout << "Improved Rows:";
		for (int i = 0; i < improvedSolRows.size(); ++i) {
			cout << " " << improvedSolRows[i];
		}
		cout << endl << "Improved Cols:";
		for (int i = 0; i < improvedSolCols.size(); ++i) {
			cout << " " << improvedSolCols[i];
		}
		cout << endl;

		cout << "Subinstance Rows:";
		for (int i = 0; i < subInstanceRows.size(); ++i) {
			cout << " " << subInstanceRows[i];
		}
		cout << endl << "Subinstance Cols:";
		for (int i = 0; i < subInstanceCols.size(); ++i) {
			cout << " " << subInstanceCols[i];
		}
		cout << endl;
		*/

		if (improvedSolRows.size() >= min_rows && improvedSolCols.size() >= min_cols) {
			// Update best sol
			if (completeness_function(cells, improvedSolRows, improvedSolCols) > bestCompleteness) { 
				bestSolRows = improvedSolRows;
				bestSolCols = improvedSolCols; 
				bestCompleteness = completeness_function(cells, improvedSolRows, improvedSolCols);
			}
		}
		currentIteration += 1;
	}
	cout << "Best Solution Rows:";
	for (int i = 0; i < bestSolRows.size(); ++i) {
		cout << " " << bestSolRows[i];
	}
	cout << endl;
	cout << "Best Solution Columns:";
	for (int i = 0; i < bestSolCols.size(); ++i) {
		cout << " " << bestSolCols[i];
	}
	cout << endl;

	vector<int> allRows(n);
	vector<int> allCols(m);
	iota(begin(allRows), end(allRows), 0);
	iota(begin(allCols), end(allCols), 0);
	cout << "Completeness original dataset: " << completeness_function(cells, allRows, allCols) << endl;
	cout << "Completeness of solution dataset: " << completeness_function(cells, bestSolRows, bestSolCols) << endl;
	cout << "Number of Solution dataset rows: " << bestSolRows.size() << endl;
	cout << "Number of Solution dataset columns: " << bestSolCols.size() << endl;
}

