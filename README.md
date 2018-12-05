# Recompression
This is a parallel implementation of the recompression technique founded by Artur Jez. Given an uncompressed text *T* of length *N* the data structure uses *O(z lg(N/z))* words where *z* is the number of LZ77 factors without self reference.

## Dependencies
The build system checks if all dependencies are installed on the target system. If some dependencies are not found these dependencies will be automatically downloaded and installed.

Dependencies are:
* [Glog](https://github.com/google/glog.git)
* [Google test](https://github.com/google/googletest.git)

## License
The library is published under the [Apache License Version 2.0](LICENSE) license.
