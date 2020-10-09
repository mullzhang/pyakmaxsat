#include "akmaxsat_solver.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "akmaxsat.hpp"

using namespace std;

vector<int> solve_ising(string filename) {
    ifstream istr(filename);
    CNF_Formula<long long> cf(istr);

    fast_backtrack(cf);
    cf.printSolution();

    vector<int> solution = cf.getSolution();
    return solution;
}