[:gb:](https://github.com/Zlika/theodore/blob/master/README.md)
[:fr:](https://github.com/Zlika/theodore/blob/master/README-FR.md)

Theodore - Thomson TO8/TO9 emulator
====================================

[![Build status](https://travis-ci.org/Zlika/theodore.svg?branch=master)](https://travis-ci.org/Zlika/theodore)
[![Build status](https://ci.appveyor.com/api/projects/status/7lo7cohkpmn50ogk?svg=true)](https://ci.appveyor.com/project/Zlika/theodore)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/15677/badge.svg)](https://scan.coverity.com/projects/zlika-theodore)
[![GitHub tag](https://img.shields.io/github/tag/Zlika/theodore.svg)](https://github.com/Zlika/theodore/releases)

This is a [libretro](https://github.com/libretro) core for emulation of [Thomson computers](https://en.wikipedia.org/wiki/Thomson_computers). It is based on Daniel Coulom's [DCTO8D](http://dcto8.free.fr/) emulator, and currently supports the following models: TO8, TO8D, TO9, TO9+.

### How to install

This emulator is available on the following platforms:
* RetroArch: [Install RetroArch](http://www.retroarch.com/?page=platforms), then start RetroArch and download the "theodore" core using the "Core Updater" feature.
* [Recalbox](https://www.recalbox.com/): Starting from version 2018.06.27, this emulator is built in your favorite retrogaming operating system.

It should be compatible with all the platforms supported by RetroArch (Android, Linux, MacOS, Raspberry Pi, Windows, Nintendo...).

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

B => Simulates a keystroke on the 'B' key of the keyboard (allows to start most games without the need for a keyboard)

**Virtual keyboard feature:** the Y/X buttons of the controller can be used to select a digit/letter/enter key (Y=go down, X=go up) and the start button simulates a keystroke on the selected key. On controllers without Y/X keys, select can also be used to roll the virtual keyboard up.
The order of the keys in the virtual keyboard is: digits (0->9) then letters (A->Z) then "Space" then "Enter".

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

RetroArch already uses lots of keyboard shortcuts for its own need that interfere with the core's keyboard emulation. To avoid this problem, configure RetroArch with a "Hotkey", as indicated in [Introduction to Hotkeys](https://docs.libretro.com/guides/retroarch-keyboard-controls/#introduction-to-hotkeys).

### :floppy_disk: File formats

The emulator can read the following file formats: *.fd and *.sap (floppy disks), *.k7 (tapes), *.m7 and *.rom (cartridges).

### Thomson flavors

By default, the core emulates a TO8 computer. Using the "Thomson flavor" option you can switch to other models (currently: TO8, TO8D, TO9, TO9+).

### Save states

The emulator supports libretro's "save state" feature. Under RetroArch, use the following keys: F2 (save state), F4 (load state), F6/F7 (change state slot).

### :innocent: Cheat codes

The emulator supports cheat codes with the following format: 0AAAAA:DD with AAAAA the RAM address (in hexadecimal) and DD the value (in hexadecimal) that will be written at the RAM address after each invocation of retro_run().

Cf. the "cheat" directory for some actual cheat codes, as well as a Python script that helps find cheat codes from a set of save state files.

### Disassembler / Debugger

A simple disassembler / debugger is available. It requires the use of the command-line, and then is not available on platforms where the standard input & output are not (easily) available.
The core must be compiled with the "DASM=1" option to enable this feature (binaries produced by the libretro buildbot does not enable this feature, so you have to compile the core by yourself).
```
make DASM=1
```
RetroArch must then be launched from the command-line:
```
retroarch -L theodore_libretro.so /path/to/game
```
Then open the RetroArch menu and enable the core's option "Interactive disassembler".
From the command-line, the following commands are then available:
* Press `Enter` key: step-by-step debugging. The following data is printed at each step: current address (value of the Program Counter register), executed instruction, current value of the CPU's registers.
* `trace` or `t`: switch to "trace" mode. The core will print each instruction executed without any stop (except if a breakpoint is met).
* `run` or `r`: switch to "run" mode. The core will run the instructions without printing them and without any stop (except if a breakpoint is met).
* `exit` or `quit` or `q`: exit the disassembler/debugger.
* `bp clear`: clear all the breakpoints.
* `bp list`: print the list of the currently defined breakpoints.
* `bp pc xxxx` (with xxxx an hexadecimal number): add a breakpoint at for the given Program Counter value.
* `bp read xxxx` (with xxxx an hexadecimal number): add a breakpoint when the emulator reads memory at the given address.
* `bp write xxxx` (with xxxx an hexadecimal number): add a breakpoint when the emulator writes memory at the given address.
* `read xxxx` (with xxxx an hexadecimal number): read the value at the given memory address.
* `write xxxx yy` (with xxxx and yy hexadecimal numbers): write the value yy at memory address xxxx.
