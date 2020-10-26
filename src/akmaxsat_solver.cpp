#include "akmaxsat_solver.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "akmaxsat.hpp"

using namespace std;

vector<int> solve_qubo(string filename) {
    srand(time(0));

    ifstream istr(filename);
    CNF_Formula<long long> cf(istr);

#ifdef RBFS
    rbfs(cf);
#else
    fast_backtrack(cf);
#endif

    cf.printSolution();

    vector<int> solution = cf.getSolution();
    return solution;
}
