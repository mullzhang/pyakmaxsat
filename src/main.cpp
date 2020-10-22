#include <pybind11/pybind11.h>

#include "akmaxsat_solver.hpp"

PYBIND11_MODULE(cxxakmaxsat, m) {
    m.doc() = "Python binding of AK-MaxSAT";

    m.def("solve_qubo", &solve_qubo, "Solve QUBO problem");
}
