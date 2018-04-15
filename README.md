Theodore - Thomson TO8D emulator
================================

[![Build Status](https://travis-ci.org/Zlika/theodore.svg?branch=master)](https://travis-ci.org/Zlika/theodore)

This is a fork of Daniel Coulom's [DCTO8D](http://dcto8.free.fr/) emulator.
It adds several improvements, in particular compatibility with [libretro](https://github.com/libretro).

### Requirements

The following packages are required to compile the project:
* A C compiler (GCC or clang)
* The autotools
* The SDL 2 library

On Debian and derivatives:
```
sudo apt-get install build-essential autoconf automake libtool libsdl2-dev
```

### How to compile

First, create the makefiles:
```
autoreconf -i
./configure
```
Then, compile the standalone version of the emulator:
```
make
```
And/Or the libretro version:
```
cd libretro
make
```
### Compatibility

The source code is portable. However, this software is only tested on Linux.
The Travis job checks that the code can also be compiled on MacOS.
Pull requests are welcomed to fix compilation and execution problems on other platforms.
