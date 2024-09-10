#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

vector<vector<int> > generate_random_matrix(int n, int m, float perCertNA) {
     srand(time(NULL));
     vector<vector<int> > cells(n, vector<int>(m));
     int noNA = n * m - n * m * perCertNA / 100;
     int i = 0;
     while (i < noNA) {
          int row = rand() % n;
          int col = rand() % m;
          if (cells[row][col] == 0) {
               cells[row][col] = 1;
               ++i;
          }
     }
     return cells;
}

void show_matrix(vector<vector<int> > cells) {
     int n = cells.size();
     int m = cells[0].size();
     for (int i = 0; i < n; ++i) {
          for (int j = 0; j < m; ++j) {
               cout << cells[i][j] << " ";
          }
          cout << endl;
     }
     cout << endl;
}

int main(int argc, char *argv[]) {
     int n, m, perCertNA;
     n = atoi(argv[1]);
     m = atoi(argv[2]);
     perCertNA = atoi(argv[3]);;
     vector<vector<int> > cells = generate_random_matrix(n, m, perCertNA);
     string nrows = to_string(n);
     string mcols = to_string(m);
     string NA = to_string(perCertNA);
     string file_name = "dataset_" + nrows + "x" + mcols + "_" + NA + ".txt";
     ofstream file;
     file.open(file_name);
     for (int i = 0; i < n; ++i) {
          for (int j = 0; j < m; ++j) {
               file << cells[i][j] << " ";
          }
          file << endl;
     }
     file.close();
}