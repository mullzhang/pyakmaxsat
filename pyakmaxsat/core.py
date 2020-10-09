import os
import tempfile

import numpy as np
import dimod

from cxxakmaxsat import solve_ising


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
        h, J, offset = bqm.to_ising()
        sampleset = self.sample_ising(h, J)
        return sampleset.change_vartype(bqm.vartype, energy_offset=offset, inplace=False)

    def sample_qubo(self, Q):
        bqm = dimod.BinaryQuadraticModel.from_qubo(Q)
        return self.sample(bqm)

    def sample_ising(self, h, J):
        bqm = dimod.BinaryQuadraticModel.from_ising(h, J)
        linear = np.zeros(bqm.num_variables)
        for i, v in sorted(h.items(), key=lambda x: x[0]):
            linear[i] = v
        quadratic = np.array([[i, j, v] for (i, j), v in J.items()])

        file_ID, filename = tempfile.mkstemp()
        try:
            with os.fdopen(file_ID, 'w') as f:
                self.convert_to_cnf(linear, quadratic, f)
            solution = solve_ising(filename)
        finally:
            os.remove(filename)

        return dimod.SampleSet.from_samples_bqm(np.array(solution), bqm)

    def convert_to_cnf(self, h, J, file):
        num_variables = len(h)

        h = np.round(np.array(h) / self.precision)
        J_ij = np.round(np.array(J[:, 2]) / self.precision)
        num_clauses = np.count_nonzero(h) + np.count_nonzero(J_ij > 0) \
            + 3 * np.count_nonzero(J_ij < 0)

        file.write("p wcnf %d %d\n" % (num_variables, num_clauses))

        for i in range(len(h)):
            if h[i] != 0:
                # add 1 to node index to go from 0-based to 1-based index
                file.write("%d %d 0\n" %
                           (np.abs(h[i]), - np.sign(h[i]) * (i + 1)))

        for edge in range(J.shape[0]):
            # add 1 to node index to go from 0-based to 1-based index
            i = J[edge, 0] + 1
            j = J[edge, 1] + 1

            if J[edge, 2] > 0:
                file.write("%d %d %d 0\n" % (J_ij[edge], -i, -j))
            elif J[edge, 2] < 0:
                file.write("%d %d %d 0\n" % (-J_ij[edge], i, j))
                file.write("%d %d %d 0\n" % (-J_ij[edge], -i, j))
                file.write("%d %d %d 0\n" % (-J_ij[edge], i, -j))


AKMaxSATSampler = AKMaxSATSolver
