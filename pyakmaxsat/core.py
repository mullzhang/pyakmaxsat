import os
import tempfile

import numpy as np
import dimod

from cxxakmaxsat import solve_qubo


class AKMaxSATSolver(dimod.Sampler):
    """ AK-MaxSAT solver """

    def __init__(self, precision=1e-6):
        self.precision = precision
        self._properties = {}
        self._parameters = {'precision': []}

    @property
    def properties(self):
        return self._properties

    @property
    def parameters(self):
        return self._parameters

    def sample(self, bqm):
        Q, offset = bqm.to_qubo()
        sampleset = self.sample_qubo(Q)
        return sampleset.change_vartype(bqm.vartype, energy_offset=offset, inplace=False)

    def sample_ising(self, h, J):
        bqm = dimod.BinaryQuadraticModel.from_ising(h, J)
        return self.sample(bqm)

    def sample_qubo(self, Q):
        bqm = dimod.BinaryQuadraticModel.from_qubo(Q)
        linear = [v for v in bqm.linear.values()]
        quadratic = [[i, j, v] for (i, j), v in bqm.quadratic.items()]

        file_ID, filename = tempfile.mkstemp()
        try:
            with os.fdopen(file_ID, 'w') as f:
                self.convert_to_cnf(linear, quadratic, f)
            raw_solution = solve_qubo(filename)
        finally:
            os.remove(filename)

        solution = np.where(np.array(raw_solution) == -1, 1, 0)
        return dimod.SampleSet.from_samples_bqm(np.array(solution), bqm)

    def convert_to_cnf(self, linear, quadratic, file):
        num_variables = len(linear)

        linear_corr = np.round(np.array(linear) / self.precision)
        quadratic_corr = np.round(np.array(quadratic)[:, 2] / self.precision)
        num_clauses = np.count_nonzero(linear_corr) \
            + np.count_nonzero(quadratic_corr > 0) \
            + 2 * np.count_nonzero(quadratic_corr < 0)

        file.write("p wcnf %d %d\n" % (num_variables, num_clauses))

        for i in range(num_variables):
            if linear_corr[i] == 0:
                continue
            file.write("%d %d 0\n" %
                       (np.abs(linear_corr[i]), np.sign(linear_corr[i]) * (i + 1)))

        for edge in range(len(quadratic)):
            i = quadratic[edge][0] + 1
            j = quadratic[edge][1] + 1

            if quadratic[edge][2] > 0:
                file.write("%d %d %d 0\n" % (quadratic_corr[edge], i, j))
            elif quadratic[edge][2] < 0:
                file.write("%d %d 0\n" % (-quadratic_corr[edge], -i))
                file.write("%d %d %d 0\n" % (-quadratic_corr[edge], i, -j))

    def sample_dimacs(self, filename):
        return solve_qubo(filename)


AKMaxSATSampler = AKMaxSATSolver
