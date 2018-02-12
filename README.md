# Collatz Conjecture Calculator
  
Checks if a number (or all numbers in an interval) obeys the Collatz conjecture making use of the **GMP** (For arbitrarily large numbers) and **pthread** (For multiple threads calculations) libraries.

### Prerequisites

Both programs require **GMP** but _collatzInterval_ also requires **pthread** library.

To install **pthread** (Ubuntu):

```sudo apt-get install libpthread-stubs0-dev ```

To install **GMP**:

1- Go to `https://gmplib.org/#DOWNLOAD` and download the .tar file

2- Go to `https://gmplib.org/manual/Installing-GMP.html` and follow the instructions


### Compiling the codes

A Makefile is available. If all prerequisites are met, simply go to the directory in which the files are located and type:

```make```

## Running the programs

#### collatzNumber

Execute the _collatzNumber_ program by typing:

```./collatzNumber <Number to be checked>```

The program will apply the collatz conjecture to the number and either output that the number obeys the conjecture or that it has exceeded the maximum amount of iterations.

Example:

```
$./collatzNumber 100000
100000 obeys the collatz conjecture
```

#### collatzInterval

Execute the _collatzInterval_ program by typing:

```./collatzInterval <Lower Bound> <Upper Bound> <Optional Parameter>```

Optional parameter:
  '-k' or '-known': If all numbers from 1 to \<Lower Bound\> are known to obey the Collatz conjecture, using this flag cuts down computational time significantly

The program will apply the collatz conjecture to every number in the interval between \<Lower Bound\> and \<Upper Bound\>

Examples:

```
$./collatzInterval 1 100000
All numbers in the interval obey the collatz conjecture
```

```
$./collatzInterval 1000 1000000 -k
All numbers in the interval obey the collatz conjecture
```
