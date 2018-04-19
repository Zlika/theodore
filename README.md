Theodore - Thomson TO8D emulator
================================

[![Build Status](https://travis-ci.org/Zlika/theodore.svg?branch=master)](https://travis-ci.org/Zlika/theodore)
[![Build status](https://ci.appveyor.com/api/projects/status/7lo7cohkpmn50ogk?svg=true)](https://ci.appveyor.com/project/Zlika/theodore)

This is a fork of Daniel Coulom's [DCTO8D](http://dcto8.free.fr/), a [Thomson TO8D emulator](https://en.wikipedia.org/wiki/Thomson_TO8).
It adds several improvements, in particular compatibility with the [libretro](https://github.com/libretro) API.

### Requirements

The following packages are required to compile the standalone version of the emulator:
* a C compiler (GCC or clang)
* the autotools (autoconf, automake, libtool)
* the SDL 2 library

On Debian and derivatives:
```
sudo apt-get install build-essential autoconf automake libtool libsdl2-dev
```

To compile the libretro version of the emulator, only a C compiler is required.

### How to compile

To compile the standalone version of the emulator:
```
autoreconf -i
./configure
make
```
To compile the libretro version of the emulator:
```
cd libretro
make
```

### Compatibility

The source code is portable. However, this software is only tested on Linux.
The Travis job checks that the code also builds on MacOS, and the AppVeyor job checks that the code also builds on Windows.
Pull requests are welcomed to fix compilation and execution problems on other platforms.
