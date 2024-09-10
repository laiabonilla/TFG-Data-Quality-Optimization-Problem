#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h> 
#include <sstream>
#include <string>
#include <fstream>
#include <numeric>
#define CSV_FILE_NAME "/Users/laiabonilla/Desktop/TFG/CODI/inputs/permepsy.csv"
#define TXT_FILE_NAME "/Users/laiabonilla/Desktop/TFG/CODI/inputs/mat20x20.txt"
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

int main(int argc, char *argv[]) {
     try {
          
          // Generate matrix with random values
          //vector<vector<int> > cells = generate_random_matrix();
          
          // Read matrix from CSV/TXT file
          //vector<vector<int> > cells = read_csv_file();
          vector<vector<int> > cells = read_txt_file();
          //vector<vector<int> > cells = read_csv_file_PERMEPSY();
          
          int n = cells.size();
          int m = cells[0].size();

          int min_rows = 10;
          int min_cols = 5;

          // Show matrix
          show_matrix(cells);

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
                    if(cells[i][j] == 0) sum -= s[i][j];
                    else sum += s[i][j];
               }
          }
          
          model.setObjective(sum, GRB_MAXIMIZE);

          // Optimize model
          model.optimize();

          // Print information
          cout << "Objective value: " << model.get(GRB_DoubleAttr_ObjVal) << endl;
          for (int i = 0; i < n; ++i) {
               for (int j = 0; j < m; ++j) {
                    cout << abs(s[i][j].get(GRB_DoubleAttr_X)) << " ";
               }
               cout << endl;
          }
          vector<int> bestSolRows;
          cout << "Best Solution Rows: ";
          for (int i = 0; i < n; ++i) {
               if (abs(x[i].get(GRB_DoubleAttr_X)) == 1) {
                    bestSolRows.push_back(i);
                    cout << i << " ";
               }
          }
          cout << endl;
          vector<int> bestSolCols;
          cout << "Best Solution Columns: ";
          for (int i = 0; i < m; ++i) {
               if (abs(y[i].get(GRB_DoubleAttr_X)) == 1) {
                    bestSolCols.push_back(i);
                    cout << i << " ";
               }
          }
          cout << endl;

          vector<int> allRows(n);
          vector<int> allCols(m);
          iota(begin(allRows), end(allRows), 0);
          iota(begin(allCols), end(allCols), 0);
          cout << "Completeness original dataset: " << completeness_function(cells, allRows, allCols) << endl;
          cout << "Completeness of solution dataset: " << completeness_function(cells, bestSolRows, bestSolCols) << endl;
     }
     catch(GRBException e) {
          cout << "Error code = " << e.getErrorCode() << endl;
          cout << e.getMessage() << endl;
     } 
     catch(...) {
          cout << "Exception during optimization" << endl;
     }
}