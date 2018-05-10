Theodore - Thomson TO8/TO8D emulator
====================================

[![Build status](https://travis-ci.org/Zlika/theodore.svg?branch=master)](https://travis-ci.org/Zlika/theodore)
[![Build status](https://ci.appveyor.com/api/projects/status/7lo7cohkpmn50ogk?svg=true)](https://ci.appveyor.com/project/Zlika/theodore)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/15677/badge.svg)](https://scan.coverity.com/projects/zlika-theodore)
[![GitHub tag](https://img.shields.io/github/tag/Zlika/theodore.svg)](https://github.com/Zlika/theodore/releases)

This is a [libretro](https://github.com/libretro) core for emulation of the [Thomson TO8/TO8D computers](https://en.wikipedia.org/wiki/Thomson_TO8). It is based on Daniel Coulom's [DCTO8D](http://dcto8.free.fr/) emulator.

### How to compile

On Linux (and most other platforms):
```
make
```
You can then run the core with a libretro frontend like RetroArch:
```
retroarch -L theodore_libretro.so
```

To compile the Android version, you need the [Android NDK](https://developer.android.com/ndk/downloads/):
```
cd jni
ndk-build
```

### :video_game: Gamepad: mapping of the buttons

A => "Fire" button

B => Simulates a keystroke on the 'B' key of the keyboard (allows to start most games without the need of a keyboard)

**Virtual keyboard feature:** the Y/X buttons of the controller can be used to select a digit/letter/enter key (Y=go down, X=go up) and the start button simulates a keystroke on the selected key. On controllers without Y/X keys, select can also be used to roll the virtual keyboard up.
The order of the keys in the virtual keyboard is: digits (0->9) then letters (A->Z) then "Enter".

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

### :floppy_disk: File formats

The emulator can read the following file formats: *.fd and *.sap (floppy disks), *.k7 (tapes), *.m7 and *.rom (cartridges).

**SAP files:** unless a matching *.fd file already exists, *.sap files are converted on the fly to the *.fd format. The *.fd file is then loaded by the emulator.

### Thomson flavors

By default, the core emulates a TO8D computer. Using the "Thomson flavor" option you can switch to a T08 computer.

### Compatibility

The source code is portable and should compile and run on most platforms.
The Travis job checks that the code builds on Linux and MacOS, and the AppVeyor job checks that the code builds on Windows.

| Platform | Compiles | Runs |
| --- | :---: | :---: |
| Linux / amd64 | :heavy_check_mark: | :heavy_check_mark: |
| Raspbian / Raspberry Pi | :heavy_check_mark: | :heavy_check_mark: |
| Android | :heavy_check_mark: | :heavy_check_mark: |
| MacOS | :heavy_check_mark: | :question: |
| Windows | :heavy_check_mark: | :heavy_check_mark: |
| Others | :question: | :question: |

Pull requests are welcomed to fix compilation and execution problems on other platforms.
