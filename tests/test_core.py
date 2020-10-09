import random

import dimod

from pyakmaxsat import AKMaxSATSolver


def main():
    N = 15
    h = {0: -10}
    J = {(i, j): random.randint(-10, 10)
         for i in range(N) for j in range(i + 1, N)}

    solver = AKMaxSATSolver(precision=1e-6)
    sampleset = solver.sample_ising(h, J)
    # print(sampleset.record[0]['sample'].tolist())
    print(sampleset)

    exact_solver = dimod.ExactSolver()
    sampleset_exact = exact_solver.sample_ising(h, J)
    # print(sampleset_exact.lowest().record[0]['sample'].tolist())
    print(sampleset_exact.lowest())

    Q = {(i, j): random.randint(-10, 10)
         for i in range(N) for j in range(i, N)}

    sampleset = solver.sample_qubo(Q)
    # print(sampleset.record[0]['sample'].tolist())
    print(sampleset)

    sampleset_exact = exact_solver.sample_qubo(Q)
    # print(sampleset_exact.lowest().record[0]['sample'].tolist())
    print(sampleset_exact.lowest())

    bqm_spin = dimod.BinaryQuadraticModel.from_ising(h, J)
    sampleset = solver.sample(bqm_spin)
    # print(sampleset.record[0]['sample'].tolist())
    print(sampleset)

    sampleset_exact = exact_solver.sample(bqm_spin)
    # print(sampleset_exact.lowest().record[0]['sample'].tolist())
    print(sampleset_exact.lowest())

    bqm_binary = dimod.BinaryQuadraticModel.from_qubo(Q)
    sampleset = solver.sample(bqm_binary)
    # print(sampleset.record[0]['sample'].tolist())
    print(sampleset)

    sampleset_exact = exact_solver.sample(bqm_binary)
    # print(sampleset_exact.lowest().record[0]['sample'].tolist())
    print(sampleset_exact.lowest())


if __name__ == '__main__':
    main()
