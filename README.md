# PyAKMaxSAT

Python binding of AK-MaxSAT solver.

AK-MaxSAT is a complete solver for MaxSAT, guaranteed to find the optimal assignment. Winner of [2010 MaxSAT Evaluation competition](http://www.maxsat.udl.cat/10/results/#wms-random). This can be applied to any QUBO/Ising for energy minimization.

PyAKMAXSAT has a solver that inherits the Sampler class defined in D-Wave Ocean SDK. It can also output the BinaryQuadraticModel(BQM) which is compatible with the class.

## Installation
### cmake setup

If you had not installed cmake>=3.11.2, you need install cmake>=3.11.2

* macOS
```
$ brew install cmake
```

* Linux
```
# if you installed old version by apt-get
$ apt-get purge cmake

# install cmake 
$ wget https://cmake.org/files/v3.11/cmake-3.11.2.tar.gz
$ tar xvf cmake-3.11.2.tar.gz
$ cd cmake-3.11.2
$ ./bootstrap && make && sudo make install 
```

* Windows

Please install cmake from [here](https://cmake.org/download/).

Alternatively, you can use
```
$ pip install -U cmake
```

Make sure the enviroment path for CMake is set correctly.

### install from github repository

```
$ pip install git+https://github.com/mullzhang/pyakmaxsat.git
```

or

```
$ git clone git@github.com:mullzhang/pyakmaxsat
$ cd pyakmaxsat
$ python setup.py install
```

## How to use

```Python
import random
from pyqubo import Array
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

```Python
from pyakmaxsat import AKMaxSATSolver

solver = AKMaxSATSolver()
sampleset = solver.sample_wcnf('path/to/file.wcnf')
print(sampleset)
```
