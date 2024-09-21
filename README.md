E3DBlipKit
=======

E3DBlipKit is a slightly modified fork of [BlipKit](https://github.com/detomon/BlipKit/) for use in [ERA-3D](https://github.com/AuzFox/ERA-3D).

Changes include:
- Removed all files except `src/` and `CMakeLists.txt`
- Removed usage of POSIX functions to allow for building on Windows
- Removed `BKWaveFileReader` and `BKWaveFileWriter`

Building the Library
--------------------

This library requires [CMake](https://cmake.org/download/) to build.

For building on Windows, it is recommended to use [w64DevKit](https://github.com/skeeto/w64devkit).

From the base directory of this project, run the following commands:

On Linux:
```sh
mkdir build && cd build
cmake ..
make
```

On Windows:
```sh
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
make
```

If everything built correctly, you will find `libblipkit.a` in `build/src`.

License
-------

This library is distributed under the MIT license. See `LICENSE`.
