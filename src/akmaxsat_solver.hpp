#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>
#include <vector>

using namespace std;

vector<int> solve_qubo(string filename);