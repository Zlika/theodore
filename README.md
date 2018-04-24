Theodore - Thomson TO8D emulator
================================

[![Build Status](https://travis-ci.org/Zlika/theodore.svg?branch=master)](https://travis-ci.org/Zlika/theodore)
[![Build status](https://ci.appveyor.com/api/projects/status/7lo7cohkpmn50ogk?svg=true)](https://ci.appveyor.com/project/Zlika/theodore)

This is a fork of Daniel Coulom's [DCTO8D](http://dcto8.free.fr/), a [Thomson TO8D emulator](https://en.wikipedia.org/wiki/Thomson_TO8).
It adds several improvements, in particular compatibility with the [libretro](https://github.com/libretro) API.

### How to compile

Linux version:
```
make
```
Android version:
```
cd jni
ndk-build
```

### Gamepad: mapping of the buttons

A => "Fire" button

B => Simulates a keystroke on the 'B' key of the keyboard (allows to start most games without the need of a keyboard)

### Keyboard: mapping of special keys

| Thomson keyboard | PC keyboard |
| ------------- | ------------- |
| STOP  | TAB  |
| CNT  | CTRL  |
| CAPSLOCK  | CAPSLOCK  |
| ACC  | ALT  |
| HOME  | HOME  |
| Arrows  | Arrows  |
| INS  | INSERT  |
| EFF  | DEL  |
| F1-F5  | F1-F5  |
| F6-F10  | SHIFT+F1-F5  |

### Compatibility

The source code is portable and should compile on most platforms. However, this software is mostly tested on Linux.
The Travis job checks that the code also builds on MacOS, and the AppVeyor job checks that the code also builds on Windows.
Pull requests are welcomed to fix compilation and execution problems on other platforms.
