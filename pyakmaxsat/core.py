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

    def sample_ising(self, h, J):
        bqm = dimod.BinaryQuadraticModel.from_ising(h, J)
        return self.sample(bqm)

    def sample_qubo(self, Q):
        bqm = dimod.BinaryQuadraticModel.from_qubo(Q)
        return self.sample(bqm)

    def sample(self, bqm):
        bqm_relabeld, label_mappings = bqm.relabel_variables_as_integers(inplace=False)
        _bqm = bqm_relabeld.change_vartype(dimod.BINARY, inplace=False)

        linear = [v for i, v in sorted(_bqm.linear.items(), key=lambda x: x[0])]
        quadratic = [[i, j, v] for (i, j), v in _bqm.quadratic.items()]

        file_ID, filename = tempfile.mkstemp()
        try:
            with os.fdopen(file_ID, 'w') as f:
                AKMaxSATSolver.convert_to_wcnf(linear, quadratic, f, self.precision)
            raw_solution = solve_qubo(filename)
        finally:
            os.remove(filename)

        if bqm.vartype == dimod.BINARY:
            solution = np.where(np.array(raw_solution) == -1, 1, 0)
        elif bqm.vartype == dimod.SPIN:
            solution = np.where(np.array(raw_solution) == -1, 1, -1)

        sampleset = dimod.SampleSet.from_samples_bqm(np.array(solution), bqm_relabeld)
        return sampleset.relabel_variables(label_mappings, inplace=False)

    @staticmethod
    def convert_to_wcnf(linear, quadratic, file, precision=1e-6):
        linear_corr = np.round(np.array(linear) / precision)
        quadratic_corr = np.round(np.array(quadratic)[:, 2] / precision)

        _linear = {}
        for i, _ in enumerate(linear):
            if linear_corr[i] == 0:
                continue
            _linear[-np.sign(linear_corr[i]) * (i + 1)] = np.abs(linear_corr[i])

        _quadratic = {}
        for edge, _ in enumerate(quadratic):
            i = quadratic[edge][0] + 1
            j = quadratic[edge][1] + 1

            if quadratic[edge][2] > 0:
                _quadratic[(-i, -j)] = _quadratic.get((i, j), 0) + quadratic_corr[edge]
            elif quadratic[edge][2] < 0:
                _linear[i] = _linear.get(-i, 0) - quadratic_corr[edge]
                _quadratic[(-i, j)] = _quadratic.get((i, -j), 0) - quadratic_corr[edge]

        _linear_reduced = {}
        for i, _ in enumerate(linear):
            weight = _linear.get(i, 0) - _linear.get(-i, 0)
            if weight > 0:
                _linear_reduced[i] = weight
            elif weight < 0:
                _linear_reduced[-i] = -weight

        num_variables = len(linear)
        num_clauses = len(_linear_reduced) + len(_quadratic)
        file.write("p wcnf %d %d\n" % (num_variables, num_clauses))

        for i, v in _linear_reduced.items():
            if v == 0:
                continue
            file.write("%d %d 0\n" % (v, -i))
        for (i, j), v in _quadratic.items():
            if v == 0:
                continue
            file.write("%d %d %d 0\n" % (v, -i, -j))

    def sample_wcnf(self, filename):
        if os.path.isfile(filename):
            return solve_qubo(filename)
        else:
            raise ValueError('not found: %s' % filename)


# Alias
AKMaxSATSampler = AKMaxSATSolver


def save_wcnf(Q, filename, precision=1e-6):
    bqm = dimod.BinaryQuadraticModel.from_qubo(Q)
    linear = [v for i, v in sorted(bqm.linear.items(), key=lambda x: x[0])]
    quadratic = [[i, j, v] for (i, j), v in bqm.quadratic.items()]

    with open(filename, 'w') as f:
        AKMaxSATSolver.convert_to_wcnf(linear, quadratic, f, precision)
