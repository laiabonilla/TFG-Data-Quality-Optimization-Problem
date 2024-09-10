#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <string>
#include <fstream>
#include <numeric>
#include <chrono>
#define INPUTS_PATH "/Users/laiabonilla/Desktop/TFG/CODI/inputs/"
#include "gurobi_c++.h"
using namespace std;


int min_rows, min_cols;
vector<vector<int> > cells;


vector<vector<int> > read_csv_file(string fileName) {
	ifstream file(INPUTS_PATH + fileName);
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
	string fileName = "permepsy.csv";
	ifstream file(INPUTS_PATH + fileName);
    	string line;
    	vector<vector<int> > v;
    	getline(file, line, '\n'); // we don't want variable names
    	while(getline(file, line, '\n')) {
     	string word;
        	double value;
        	vector<int> row;
        	stringstream ss(line);
        	while(getline(ss, word, ',')) {
            	if (word != "NA") row.push_back(1);
            	else row.push_back(0);
        	}
        	v.push_back(row);
    	}
    	return v;
}

vector<vector<int> > read_txt_file(string fileName) {
     ifstream file(INPUTS_PATH + fileName);
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

void show_matrix(vector<int> rows, vector<int> cols) {
     int n = rows.size();
     int m = cols.size();
     for (int i = 0; i < n; ++i) {
          for (int j = 0; j < m; ++j) {
               cout << cells[rows[i]][cols[j]] << " ";
          }
          cout << endl;
     }
     cout << endl;
}

float completeness_function(vector<int> rows, vector<int> cols) {
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

void convert_subInstance(vector<int> subInstanceRows, vector<int> subInstanceCols, vector<vector<int> >& subcells) {
	for (int i = 0; i < subInstanceRows.size(); ++i) {
		for (int j = 0; j < subInstanceCols.size(); ++j) {
			subcells[i][j] = cells[subInstanceRows[i]][subInstanceCols[j]];
		}
	}
}

bool meetsRowAdditionCriteria(vector<int> cols, int row, int criteria) {
	bool b = false;
	int cnt = 0;
	for (int i = 0; i < cols.size(); ++i) {
		if (cells[row][cols[i]] == 1) cnt += 1;
	}
	float res = float(cnt) / float(cols.size());
	if (res >= float(criteria) / float(100)) b = true;
	return b;
}

bool meetsColAdditionCriteria(vector<int> rows, int col, int criteria) {
	bool b = false;
	int cnt = 0;
	for (int i = 0; i < rows.size(); ++i) {
		if (cells[rows[i]][col] == 1) cnt += 1;
	}
	float res = float(cnt) / float(rows.size());
	if (res >= float(criteria) / float(100)) b = true;
	return b;
}

void greedy(int N, int Pn, vector<int>& bestSolRows, vector<int>& bestSolCols) {

	int n = cells.size();
	int m = cells[0].size();

	//loop N times
	for (int i = 0; i < N; ++i) {
		vector<int> rows; 
		vector<int> cols;
		while (rows.empty() || cols.empty()) {
			vector<int> randomRows; 
			//Seeding phase
			while (randomRows.size() != Pn) {
				int row = rand() % n;
				int cnt = count(randomRows.begin(), randomRows.end(), row);
				if (cnt == 0) randomRows.push_back(row);
			}
			//Column addition phase
			for (int j = 0; j < m; ++j) {
				bool b = true;
				int i = 0;
				while (b == true && i < randomRows.size()) {
					if (cells[randomRows[i]][j] == 0) b = false;
					++i;
				}
				if (b == true) cols.push_back(j);
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
		
		if (rows.size() + cols.size() > bestSolRows.size() + bestSolCols.size()) {
			bestSolRows = rows;
			bestSolCols = cols;
			sort(bestSolRows.begin(), bestSolRows.end());
			sort(bestSolCols.begin(), bestSolCols.end());
		}
	}
}

void greedyImproved(int N, int Pn, vector<int>& bestSolRows, vector<int>& bestSolCols) {

	int n = cells.size();
	int m = cells[0].size();
	
	for (int i = 0; i < n; ++i) {
		bestSolRows.push_back(i);
	}
	for (int i = 0; i < m; ++i) {
		bestSolCols.push_back(i);
	}

	//loop N times
	for (int i = 0; i < N; ++i) {
		vector<int> rows; 
		vector<int> cols;
		while (rows.empty() || cols.empty()) {
			vector<int> randomRows; 
			//Seeding phase
			while (randomRows.size() != Pn) {
				int row = rand() % n;
				int cnt = count(randomRows.begin(), randomRows.end(), row);
				if (cnt == 0) randomRows.push_back(row);
			}
			//Column addition phase
			for (int j = 0; j < m; ++j) {
				bool b = true;
				int i = 0;
				while (b == true && i < randomRows.size()) {
					if (cells[randomRows[i]][j] == 0) b = false;
					++i;
				}
				if (b == true) cols.push_back(j);
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
		
		// Improvement phase --> add row that meets criteria to the solution
		int criteria = 90;
		while ((rows.size() < min_rows) && (criteria != -10)) {
			for(int i = 0; i < n; ++i) {
				int cnt = count(rows.begin(), rows.end(), i);
				if (cnt == 0) { // row "i" not in solution rows
					//cout << "Trying to add row " << i << endl;
					bool b = meetsRowAdditionCriteria(cols, i, criteria);
					//cout << b << endl;
					if (b) rows.push_back(i);
				}
			}
			criteria -= 10;
		}
			
		// Improvement phase --> add column that meets criteria to the solution
		criteria = 90;
		while ((cols.size() < min_cols) && (criteria != -10)) {
			for(int i = 0; i < m; ++i) {
				int cnt = count(cols.begin(), cols.end(), i);
				if (cnt == 0) { // col "i" not in solution cols
					//cout << "Trying to add col " << i << endl;
					bool b = meetsColAdditionCriteria(rows, i, criteria);
					//cout << b << endl;
					if (b) cols.push_back(i);
				}
			}
			criteria -= 10;
		}
			
		//Update Best Solution (Completeness criteria)
		if (completeness_function(rows, cols) > completeness_function(bestSolRows, bestSolCols)) {
			bestSolRows = rows;
			bestSolCols = cols;
			sort(bestSolRows.begin(), bestSolRows.end());
			sort(bestSolCols.begin(), bestSolCols.end());
		}
	}
}

void addRandomRowsCols(vector<int>& subInstanceRows, vector<int>& subInstanceCols) {
	int n = cells.size();
	int m = cells[0].size();
	while (subInstanceRows.size() < min_rows) {
		int row = rand() % n;
		int cnt = count(subInstanceRows.begin(), subInstanceRows.end(), row);
		if (cnt == 0) subInstanceRows.push_back(row);
	}
	while (subInstanceCols.size() < min_cols) {
		int col = rand() % m;
		int cnt = count(subInstanceCols.begin(), subInstanceCols.end(), col);
		if (cnt == 0) subInstanceCols.push_back(col);
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

void gurobi(vector<vector<int> > subcells, vector<int> cellsRows, vector<int> cellsCols, vector<int>& bestSolRows, vector<int>& bestSolCols) {

	try {
		int n = subcells.size();
        	int m = subcells[0].size();

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
		GRBLinExpr sum = 0;
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < m; ++j) {
				if(subcells[i][j] == 0) sum -= s[i][j];
				else sum += s[i][j];
			}
		}
		
		model.setObjective(sum, GRB_MAXIMIZE);

		// Avoid printing optimization
		model.set(GRB_IntParam_LogToConsole, 0);

		// SET TIME LIMIT TO 12 HOURS (43200 seconds)
		model.getEnv().set(GRB_DoubleParam_TimeLimit, 3600); 
		
		// Optimize model
		model.optimize();


		// Print information
		//cout << "Objective value: " << model.get(GRB_DoubleAttr_ObjVal) << endl;
		/*
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < m; ++j) {
				cout << abs(s[i][j].get(GRB_DoubleAttr_X)) << " ";
			}
			cout << endl;
		}
		*/

		for (int i = 0; i < n; ++i) {
			if (abs(x[i].get(GRB_DoubleAttr_X)) == 1) {
				bestSolRows.push_back(cellsRows[i]);
			}
		}

		for (int i = 0; i < m; ++i) {
			if (abs(y[i].get(GRB_DoubleAttr_X)) == 1) {
				bestSolCols.push_back(cellsCols[i]);
			}
		}
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
	if (argc < 5) {
		cout << "INSUFFICIENT PARAMETERS" << endl;
		return 0;
	}
	else {
		string algorithm = argv[1];
		if (algorithm == "greedy" || algorithm == "gurobi" || algorithm == "CMSA") {
			srand(time(NULL));

			string file = argv[2];

			// Generate matrix with random values
			//cells = generate_random_matrix();
			          
			// Read matrix from CSV/TXT file
			
			string format = file.substr(file.size() - 3, 3);  
			if (file == "permepsy.csv") read_csv_file_PERMEPSY();
			else if (format == "csv") cells = read_csv_file(file);
			else if (format == "txt") cells = read_txt_file(file);
			else {
				cout << "INCORRECT DATASET FILE NAME" << endl;
				return 0;
			}

			int n = cells.size();
			int m = cells[0].size();

			min_rows = atoi(argv[3]);
			min_cols = atoi(argv[4]);

			if (min_rows > n || min_cols > m) {
				cout << "INCORRECT NUMBER OF MINIMUM DIMENSIONS" << endl;
				return 0;
			}
			cout << "Rows: " << cells.size() << endl;
			cout << "Cols: " << cells[0].size() << endl;
			
			vector<int> bestSolRows;
		     vector<int> bestSolCols;
		     vector<int> allRows(n);
			vector<int> allCols(m);
			iota(begin(allRows), end(allRows), 0);
			iota(begin(allCols), end(allCols), 0);

			// Start execution time
			auto start = std::chrono::steady_clock::now();

			if (algorithm == "greedy") {
				if (argc == 5) {
					// Show matrix
					//show_matrix(allRows, allCols);
					
					// Read parameters
					int N, Pn;
					N = 1000;
					Pn = 3;

					// Greedy function
					greedyImproved(N, Pn, bestSolRows, bestSolCols);
				}
				else {
					cout << "GREEDY ALGORITHM NEEDS TWO PARAMETERS" << endl;
					return 0;
				}
			}
			else if (algorithm == "gurobi") {
				// Show matrix
				//show_matrix(allRows, allCols);
				
				// Gurobi function
				gurobi(cells, allRows, allCols, bestSolRows, bestSolCols);
			}
			else { // CMSA
				if (argc == 5) {
					// Show matrix
					//show_matrix(allRows, allCols);

					// Read parameters for greedy
					int N, Pn;
					N = 1000;
					Pn = 3;
					
					// Read parameters for CMSA
					int na, maxIterations, maxAge;
					//na = atoi(argv[7]);
					//maxIterations = atoi(argv[8]);
					//maxAge = atoi(argv[9]);
					na = 10;
					maxIterations = 1000;
					maxAge = 25;

					//  Metaheuristic parameters
					int currentIteration = 0;
					int bestCompleteness = 0;
					vector<int> subInstanceRows;
					vector<int> subInstanceCols;
					vector<int> rowsAges = vector<int>(n, 0);
					vector<int> colsAges = vector<int>(m, 0);

					// Set limit time to 1 hour aprox
					bool stopExecution = false;
					float currentExecutionTime = -9999.9;

					while (currentIteration < maxIterations && !stopExecution) { // Or CPU time limit not reached
						for (int i = 0; i < na; ++i) {	
							// Construct phase
							vector<int> candidateSolRows;
							vector<int> candidateSolCols;
							greedy(N, Pn, candidateSolRows, candidateSolCols);

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
						if ((subInstanceRows.size() < min_rows) || (subInstanceCols.size() < min_cols)) addRandomRowsCols(subInstanceRows, subInstanceCols);
						sort(subInstanceRows.begin(), subInstanceRows.end());
						sort(subInstanceCols.begin(), subInstanceCols.end());
					
						// Solve phase 
						vector<int> improvedSolRows;
						vector<int> improvedSolCols; 
						vector<vector<int> > subcells(subInstanceRows.size(), vector<int>(subInstanceCols.size()));
						convert_subInstance(subInstanceRows, subInstanceCols, subcells);
						gurobi(subcells, subInstanceRows, subInstanceCols, improvedSolRows, improvedSolCols);
						
						// Adapt phase
						//adapt(cells, improvedSolRows, improvedSolCols);
						for (int i = 0; i < n; ++i) {
							int cnt1 = count(subInstanceRows.begin(), subInstanceRows.end(), i);
							if (cnt1 == 1 && rowsAges[i] != maxAge) rowsAges[i] += 1;
							int cnt2 = count(improvedSolRows.begin(), improvedSolRows.end(), i);
							if (cnt2 == 1) rowsAges[i] = 0;
						}
						for (int i = 0; i < m; ++i) {
							int cnt1 = count(subInstanceCols.begin(), subInstanceCols.end(), i);
							if (cnt1 == 1 && colsAges[i] != maxAge) colsAges[i] += 1;
							int cnt2 = count(improvedSolCols.begin(), improvedSolCols.end(), i);
							if (cnt2 == 1) colsAges[i] = 0;
						}
						vector<int> rowsToRemove;
						vector<int> colsToRemove;
						for (int i = 0; i < subInstanceRows.size(); ++i) {
							int cnt = count(improvedSolRows.begin(), improvedSolRows.end(), subInstanceRows[i]);
							if (cnt == 0 && rowsAges[subInstanceRows[i]] == maxAge) {
				    				rowsToRemove.push_back(subInstanceRows[i]);
							}
						}
						for (int i = 0; i < subInstanceCols.size(); ++i) {
							int cnt = count(improvedSolCols.begin(), improvedSolCols.end(), subInstanceCols[i]);
							if (cnt == 0 && colsAges[subInstanceCols[i]] == maxAge) {
				    				colsToRemove.push_back(subInstanceCols[i]);
							}
						}
						for(int i = 0; i < rowsToRemove.size(); ++i) {
							vector<int>::iterator position = find(subInstanceRows.begin(), subInstanceRows.end(), rowsToRemove[i]);
							if (position != subInstanceRows.end()) subInstanceRows.erase(position);
						}
						for(int i = 0; i < colsToRemove.size(); ++i) {
							vector<int>::iterator position = find(subInstanceCols.begin(), subInstanceCols.end(), colsToRemove[i]);
							if (position != subInstanceCols.end()) subInstanceCols.erase(position);
						}

						// Update best sol
						if (completeness_function(improvedSolRows, improvedSolCols) > bestCompleteness) { 
							bestSolRows = improvedSolRows;
							bestSolCols = improvedSolCols; 
							bestCompleteness = completeness_function(improvedSolRows, improvedSolCols);
						}
						
						currentIteration += 1;

						auto currentTime = std::chrono::steady_clock::now();
						auto seconds = std::chrono::duration_cast<std::chrono::duration<float> >(currentTime - start); 
						currentExecutionTime = float(seconds.count());
						if (currentExecutionTime > 3600) stopExecution = true;
					}
				}
				else {
					cout << "CMSA ALGORITHM NEEDS 5 PARAMETERS" << endl;
					return 0;
				}
			}
			// End execution time
			auto end = std::chrono::steady_clock::now();

			// PRINT RESULTS
			cout << endl;
			//cout << algorithm << " results:" << endl << endl;
			cout << "RESULTS:" << endl;
			cout << "Rows Size of solution dataset: " << bestSolRows.size() << endl;
			cout << "Cols Size of solution dataset: " << bestSolCols.size() << endl;
			/*
			cout << "Solution dataset:" << endl;
			show_matrix(bestSolRows, bestSolCols);
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
			*/

			cout << "Completeness original dataset: " << completeness_function(allRows, allCols) << endl;
			cout << "Completeness of solution dataset: " << completeness_function(bestSolRows, bestSolCols) << endl;
				
			typedef std::chrono::duration<float> float_seconds;
  			auto seconds = std::chrono::duration_cast<std::chrono::duration<float> >(end - start);
			cout << "Execution time: " << float(seconds.count()) << " seconds" << endl;
			cout << endl;
		}
		else {
			cout << "INCORRECT ALGORITHM NAME" << endl;
			return 0;
		}
	}
}

