# Recompression
This library contains various implementations of the recompression technique invented by Artur Jez, including sequential and parallel variants. The recompression technique is quiet similar to the grammar based compression algorithm called Recursive Pairing (RePair), but it differs in two points. First, the recompression technique uses two compression schemes: the block compression *bcomp* which compresses maximal blocks of same symbols and the pair compression *pcomp* which compresses non overlapping pairs of symbols based on a partitioning of the symbols in the text. The second difference is that *pcomp* compresses multiple different pairs in one iteration instead of only the most frequent one like RePair does. This two compressions are executed alternately, starting with *bcomp*. The generated context-free grammar is a run-length straight-line program where productions of the form *X->Y^d* are allowed where *Y^d* is the *d*-th repetition of *Y*. Given an uncompressed text *T* of length *N* the data structure uses only *O(z lg(N/z))* words where *z* is the number of LZ77 factors without self reference.

## Documentation
We provide a doxygen generated API reference which lists all types, classes and functions of this library.

## Requirements
This library requires:

* A C++14 compiler such as g++ 7.3 or higher.
* A 64-bit operating system (I've only tested on Linux).
* The cmake build system

### Dependencies
The build system checks if all dependencies are installed on the target system. If some dependencies are not found these dependencies will be automatically downloaded and installed.

Dependencies are:

* [In-place Parallel Super Scalar Samplesort (IPS⁴o)](https://github.com/SaschaWitt/ips4o.git)
* for testing purposes:
  * [Google test](https://github.com/google/googletest.git)
* for benchmark puposes:
  * [Succinct Data Structure Library 2.0](https://github.com/simongog/sdsl-lite.git)
  * [Pezza-LCE](https://github.com/al-xyz/prezza-lce)
  
For the parallel algorithms, you need to enable OpenMP (``-fopenmp``). To support the in-place super scalar samplesort algorithm please read the [IPS⁴o](https://github.com/SaschaWitt/ips4o.git) 's documentation.

## Installation
To download the library use to following command.

```
git clone https://github.com/ChristopherOsthues/recompression.git
mkdir build
cd build
cmake ..
make
```

To enable the benchmark output, set ``CMAKE_BUILD_TYPE=Bench`` at build time.

```
cmake .. -DCMAKE_BUILD_TYPE=Bench
make
```

To build the documentation, tests and/or benchmark executables you can enable them by setting the corresponding option to ``ON`` in the [CMakeList.txt](CMakeLists.txt) file.

## Getting started

This library is a header only library. The recompression.hpp header in the include directory provides all functionality of the library.

```cpp
#inlude <recompression.hpp>

using namespace recomp;

int main() {
    std::vector<var_t> text = {0, 0, 3, 0, 4, 2, 5, 12};
    size_t cores = 4;
    size_t alphabet_size = 13;
    parallel::parallel_recompression<var_t, term_t> recompression{"dataset"};
    rlslp<var_t, term_t> slp;
    recompression.recomp(text, slp, alphabet_size, cores);
}
```

There are some different versions available. This version are:

* **fast**: A fast sequential version of the recompression.
* **hash**: A sequential version using hash tables to store the blocks/pairs to replace them with a single text scan.
* **parallel**: A parallel version. The undirected cut is not parallelized due to reasons of data dependencies.
* **parallel_lp**: A parallel recompression version which counts the number of possibly new production introduced by the combinations of the partition sets choosing the combination that generates less productions if the values of the directed cut are equal. The undirected cut is not parallelized due to reasons of data dependencies.
* **parallel_rnd**: A full parallel version using a random generated partitioning of the symbols for *pcomp*
* **parallel_lock**: A version that executes the undirected cut in parallel solving the data dependencies with locks.
* **full_parallel**: A fully parallelized version using a parallel local search to find local optimas for the partition.


The library also provides coders to encode and output the rlslp to files. The code below shows an example.

```cpp
#inlude <recompression.hpp>

using namespace recomp;

int main() {
    std::vector<var_t> text = {0, 0, 3, 0, 4, 2, 5, 12};
    size_t cores = 4;
    size_t alphabet_size = 13;
    parallel::parallel_recompression<var_t, term_t> recompression{"dataset"};
    rlslp<var_t, term_t> slp;
    recompression.recomp(text, slp, alphabet_size, cores);
    
    // Stores the rlslp to testfile.<enc>
    encode("sorted", "testfile", slp);
    
    // Loads the rlslp from testfile.<enc>
    rlslp<var_t, term_t> in_slp = decode("sorted", "testfile");
}
```

The file extension depends on the used coder. The `sorted` coder for example uses the file extension `.rlslp`.

There are three coders available:

* **plain**: Writes plain integers using the full bit width of the underlying integer type.
* **fixed**: Uses a fixed bit width depending on the highest variables name of the rlslp
* **sorted**: Renames all variables depending on a permutation so the first elements of the right sides of the productions are sorted increasingly for all productions deriving pairs. The productions for the blocks are **not** sorted. Encodes the sorted values using delta coding and writes the unary codes of this values. All other components are encoded using fixed bit widths.

## License
The library is published under the [Apache License Version 2.0](LICENSE) license.
