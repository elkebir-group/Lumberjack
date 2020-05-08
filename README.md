# Lumberjack: Cutting planes for the 1-Dollo Phylogeny problem using CryptoMiniSat.


## Contents

  1. [Getting started](#start)
     * [Dependencies](#dep)
     * [Compiling code](#build)
  2. [Usage instructions](#usage)
     * [I/O formats](#io)
     * [Lumberjack Executable](#run)
       * [Running Lumberjack](#ljrun)
       * [Lumberjack output](#ljout)

<a name="start"></a>

## Getting started

Lumberjack is written in C++11. The repository is organized as follows:

| Folder    | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `src`     | source code for Lumberjack                                       |
| `data`    | input data for Lumberjack                                       |


<a name="dep"></a>

### Dependencies   

Lumberjack can be compiled with [CMake](https://cmake.org/) (>= 2.8) and has the following dependencies:

* [Boost](https://www.boost.org/) (>= 1.69.0)
* [CryptoMiniSat](https://github.com/msoos/cryptominisat) (>= 5.6.8)

<a name="build"></a>

### Compiling code

Here we walk through how to build the executables using CMake. First, navigate to a directory in which you would like to download Lumberjack. Then perform the following steps: 

```bash
# Download repository
git clone https://github.com/elkebir-group/Lumberjack.git

# Enter downloaded RECAP folder
cd Lumberjack/

# Make new build directory and enter it
mkdir build
cd ./build/

# Use cmake to compile executables. 
# OPTIONAL: set SAT_ROOT in CMakeLists.txt if CryptoMiniSat not detected.
cmake ..
make
```

<a name="usage"></a>

## Usage instructions

Here we describe how to run the Lumberjack executable. 

<a name="io"></a>

### I/O format

Lumberjack takes as input a file containing a binary matrix, where rows correspond to clones (or taxa) and columns correspond to mutations (or characters). An example of such a file can be found in the data folder [here](./data/m25_n25_s1_k1_loss0.1.B).    

The first line of the file should give the number of rows in the matrix.

```tex
25 #taxa
```

On the second line, we indicate the number of columns in the matrix. 

```tex
25 #characters
```

The following lines each contain one row of the matrix. Matrix enteries should be separated by a space. One indicates that the mutation is present in the clone, and zero indicates it is absent. The current code does not allow for false negatives or false positives.  

```tex
0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 1 0 0 0 0 0 0 0 0
0 0 0 0 0 1 0 0 0 0 0 0 0 0 1 0 1 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 1 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 1 1 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 1 0 0 0 0 0 0 0 0
0 0 0 0 1 0 0 0 1 0 0 0 0 0 1 1 1 0 0 0 0 1 1 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 1 0 0 0 0 0 0 0 0
0 0 1 0 0 0 0 1 0 0 0 0 0 0 1 0 1 0 0 0 0 0 0 1 0
0 0 0 1 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 1 1 0 0 0 0
0 0 0 0 0 0 0 0 0 1 1 0 1 0 0 0 0 0 0 1 1 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 1 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 1 1 0 0 0 1
0 0 0 0 0 0 1 1 0 0 0 0 0 1 1 0 1 0 1 0 0 0 0 1 0
1 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 1 0 0 0 0 0 0 0 0
0 1 0 0 0 0 0 0 0 0 0 1 0 0 1 0 1 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 1 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 1 1 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 1 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 1 0 0 0 0 0 0 0 0
0 0 1 0 0 0 0 1 0 0 0 0 0 0 1 0 1 1 0 0 0 0 0 1 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 1 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 1 1 0 1 0 0 0 0 0 0 1 1 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 1 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 1 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 1 0 0 0 0 0 1 1 0 1 0 1 0 0 0 0 1 0
```

<a name="run"></a>

### Lumberjack Executable

<a name="ljrun"></a>

#### Running Lumberjack

This executable requires as input a filepath to the matrix file described above as well as a path to an outputfile. These are specified using the flags `-i`  and  `-o`, respectively.   

```bash
# An example of how to run Lumberjack
./build/lumberjack -i ./data/m25_n25_s1_k1_loss0.1.B -o ./results/m25_n25_s1_k1_loss0.1.out
```

There are additional input options that can be specified.

`-t` specifies the number of threads to use (default: 1). 

Note that the function responsible for adding cuttingplanes is `CuttingPlane::separate()`, which is based on constraints 11-14 from [SPhyr](https://doi.org/10.1093/bioinformatics/bty589) used to ensure that the solution does not contain one of the forbidden submatrices given in Definition 7 of the same paper. 

<a name="ljout"></a>

#### Lumberjack output

The executable prints the number of SAT iteractions along with the number of constraints introduced after each iteration to std::cerr. If a solution is found, the solution matrix is printed to the output filepath in the same format as the input matrix file. Note that the solution matrix will likely not be binary since enteries that were gained and then lossed will be indicated with a 2. 
