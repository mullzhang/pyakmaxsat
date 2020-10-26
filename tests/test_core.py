import unittest

import dimod
from pyqubo import Array

from pyakmaxsat import AKMaxSATSolver


class TestCore(unittest.TestCase):
    def create_prob_instance(self):
        N = 15
        K = 3
        numbers = [4.8097315016016315, 4.325157567810298, 2.9877429101815127,
                   3.199880179616316, 0.5787939511978596, 1.2520928214246918,
                   2.262867466401502, 1.2300003067401255, 2.1601079352817925,
                   3.63753899583021, 4.598232793833491, 2.6215815162575646,
                   3.4227134835783364, 0.28254151584552023, 4.2548151473817075]

        q = Array.create('q', N, 'BINARY')
        H = sum(numbers[i] * q[i] for i in range(N)) + 5.0 * (sum(q) - K)**2
        model = H.compile()
        Q, offset = model.to_qubo(index_label=True)
        return dimod.BinaryQuadraticModel.from_qubo(Q, offset)

    def test_sample_qubo(self):
        bqm = self.create_prob_instance()
        Q, _ = bqm.to_qubo()

        solver = AKMaxSATSolver()
        exact_solver = dimod.ExactSolver()

        sampleset = solver.sample_qubo(Q)
        sampleset_exact = exact_solver.sample_qubo(Q)
        self.assertListEqual(sampleset.record[0].sample.tolist(),
                             sampleset_exact.lowest().record[0].sample.tolist())
        self.assertEqual(round(sampleset.record[0].energy, 8),
                         round(sampleset_exact.lowest().record[0].energy, 8))

    def test_sample_ising(self):
        bqm = self.create_prob_instance()
        h, J, _ = bqm.to_ising()

        solver = AKMaxSATSolver()
        exact_solver = dimod.ExactSolver()

        sampleset = solver.sample_ising(h, J)
        sampleset_exact = exact_solver.sample_ising(h, J)
        self.assertListEqual(sampleset.record[0].sample.tolist(),
                             sampleset_exact.lowest().record[0].sample.tolist())
        self.assertEqual(round(sampleset.record[0].energy, 8),
                         round(sampleset_exact.lowest().record[0].energy, 8))

    def test_sample(self):
        bqm = self.create_prob_instance()

        solver = AKMaxSATSolver()
        exact_solver = dimod.ExactSolver()

        sampleset = solver.sample(bqm)
        sampleset_exact = exact_solver.sample(bqm)
        self.assertListEqual(sampleset.record[0].sample.tolist(),
                             sampleset_exact.lowest().record[0].sample.tolist())
        self.assertEqual(round(sampleset.record[0].energy, 8),
                         round(sampleset_exact.lowest().record[0].energy, 8))


if __name__ == '__main__':
    unittest.main()
