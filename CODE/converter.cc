#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#define FILE "/Users/laiabonilla/Desktop/TFG/CODI/inputs/permepsy.csv"
using namespace std;

int main () {
    ifstream file(FILE);
    string line;
    vector<vector<int> > v;
    while(getline(file, line, '\n')) {
        string word;
        //double value;
        vector<int> row;
        stringstream ss(line);
        while(getline(ss, word, ',')) {
            //value = stod(word);
            if (word != "NA") row.push_back(1);
            else row.push_back(0);
        }
        v.push_back(row);
    }
    ofstream output_file;
    output_file.open("mat_permepsy.txt");
    for (int i = 0; i < v.size(); ++i) {
        for (int j = 0; j < v[i].size(); ++j) {
            output_file << v[i][j] << " ";
        }
        output_file << endl;
    }
    output_file.close();
}