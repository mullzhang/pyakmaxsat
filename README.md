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
h = {}
J = {(i, j): random.randint(-5, 5)
     for i in range(N) for j in range(i + 1, N)}

solver = AKMaxSATSolver(precision=1e-6)
sampleset = solver.sample_ising(h, J)
print(sampleset)
```
