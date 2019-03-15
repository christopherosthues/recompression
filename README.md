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

## License
The library is published under the [Apache License Version 2.0](LICENSE) license.
