import random

import dimod

from pyakmaxsat import AKMaxSATSolver


def main():
    N = 15
    Q = {(i, j): random.randint(-5, 5)
         for i in range(N) for j in range(i, N)}

    # bqm_spin = dimod.BinaryQuadraticModel.from_ising(Q)
    # Q, offset = bqm_spin.to_qubo()
    # bqm_binary = dimod.BinaryQuadraticModel.from_qubo(Q, offset)

    solver = AKMaxSATSolver()
    exact_solver = dimod.ExactSolver()

    # sampleset = solver.sample_ising(h, J)
    # print('akmaxsat:', sampleset.record[0]['sample'])
    # print(sampleset)

    # sampleset_exact = exact_solver.sample_ising(h, J)
    # print('exact:   ', sampleset_exact.lowest().record[0]['sample'])
    # print(sampleset_exact.lowest())

    sampleset = solver.sample_qubo(Q)
    # print(sampleset.record[0]['sample'].tolist())
    print(sampleset)

    sampleset_exact = exact_solver.sample_qubo(Q)
    # print(sampleset_exact.lowest().record[0]['sample'].tolist())
    print(sampleset_exact.lowest())

#     sampleset = solver.sample(bqm_spin)
#     print('akmaxsat:', sampleset.record[0]['sample'])
#     print(sampleset)

#     sampleset_exact = exact_solver.sample(bqm_spin)
#     print('exact:   ', sampleset_exact.lowest().record[0]['sample'])
#     print(sampleset_exact.lowest())

    # sampleset = solver.sample(bqm_binary)
    # print(sampleset.record[0]['sample'].tolist())
    # print(sampleset)

    # sampleset_exact = exact_solver.sample(bqm_binary)
    # print(sampleset_exact.lowest().record[0]['sample'].tolist())
    # print(sampleset_exact.lowest())


if __name__ == '__main__':
    main()
