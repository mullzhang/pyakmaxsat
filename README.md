# PyAKMaxSAT

Python binding of AK-MaxSAT solver.

AK-MaxSAT is a complete solver for MaxSAT, guaranteed to find the optimal assignment. Winner of [2010 MaxSAT Evaluation competition](http://www.maxsat.udl.cat/10/results/#wms-random). This can be applied to any QUBO/Ising for energy minimization.

PyAKMAXSAT has a solver that inherits the Sampler class defined in D-Wave Ocean SDK. It can also output the BinaryQuadraticModel(BQM) which is compatible with the class.

## install
### install from github repository

```
$ git clone git@github.com:mullzhang/pyakmaxsat
$ cd pyakmaxsat
$ python setup.py install
```

## How to use
### Python example

```Python
import random
from pyakmaxsat import AKMaxSATSolver

N = 15
K = 3
numbers = [random.uniform(0, 5) for _ in range(N)]
q = Array.create('q', N, 'BINARY')
H = sum(numbers[i] * q[i] for i in range(N)) + 5.0 * (sum(q) - K)**2
model = H.compile()
bqm = model.to_bqm()

solver = AKMaxSATSolver()
sampleset = solver.sample(bqm)
print(sampleset)
```
