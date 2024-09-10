#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
using namespace std;

// To execute the command and store the output
string execute_command(const string& command) {
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() fails!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int main() {
    ifstream input_file("inputExperiments_25%NA_gurobi.txt");
    ofstream output_file("outputExperiments_25%NA_gurobi.txt");

    if (!input_file.is_open()) {
        cerr << "Input file can't be opened" << endl;
        return 1;
    }
    if (!output_file.is_open()) {
        cerr << "Output file can't be opened" << endl;
        return 1;
    }

    string line;
    while (getline(input_file, line)) {
        // Construct command to execute (3 times each instance --> only greedy and CMSA)
        //for (int i = 0; i < 3; ++i) {
            string result = execute_command(line);
            output_file << "INSTANCE " << line << endl << endl << result;
        //}
    }

    input_file.close();
    output_file.close();
    return 0;
}
