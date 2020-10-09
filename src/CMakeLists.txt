include_directories(../akmaxsat_1.1)

pybind11_add_module(cxxakmaxsat MODULE
    main.cpp
    akmaxsat_solver.cpp
)
