[:gb:](https://github.com/Zlika/theodore/blob/master/README.md)
[:fr:](https://github.com/Zlika/theodore/blob/master/README-FR.md)

Theodore - Thomson MO/TO emulator
====================================

[![Build status](https://travis-ci.org/Zlika/theodore.svg?branch=master)](https://travis-ci.org/Zlika/theodore)
[![Build status](https://ci.appveyor.com/api/projects/status/7lo7cohkpmn50ogk?svg=true)](https://ci.appveyor.com/project/Zlika/theodore)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/15677/badge.svg)](https://scan.coverity.com/projects/zlika-theodore)
[![GitHub tag](https://img.shields.io/github/tag/Zlika/theodore.svg)](https://github.com/Zlika/theodore/releases)

This is a [libretro](https://github.com/libretro) core for emulation of [Thomson computers](https://en.wikipedia.org/wiki/Thomson_computers). It is based on Daniel Coulom's [DCTO8D](http://dcto8.free.fr/), [DCTO9P](http://dcto9p.free.fr/) and [DCMO5](http://dcmo5.free.fr/) emulators, and supports the following models: TO7, TO7/70, TO8, TO8D, TO9, TO9+, MO5, MO6 and also the Olivetti Prodest PC128 (a rebranded MO6 for the Italian market).

### How to install

This emulator is available on the following platforms:
* RetroArch: [Install RetroArch](http://www.retroarch.com/?page=platforms), then start RetroArch and download the "theodore" core using the "Core Updater" feature.
* [Recalbox](https://www.recalbox.com/): Starting from version 2018.06.27, Theodore is built into your favorite retrogaming operating system.
* [Lakka](http://www.lakka.tv/): Starting from version 2.2.2, Theodore is built into this lightweight Linux distribution.
* [Kodi](https://kodi.tv/): Starting from version 18, Kodi allows to download and run libretro-based emulators, including Theodore.

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

B => "Fire" button

Start => "Start Program". Simulates one or several keystrokes on the keyboard to start a game. This feature allows to start most games without the need for a keyboard. The key depends on the loaded media and of the current computer emulated. On MO5/MO6/TO7/TO7-70, the command used depends on the format detected for the first file of the tape (BAS => RUN", BIN => LOADM"",,R).

| Media loaded | Thomson computer | Key                 |
| ------------ | ---------------- | ------------------- |
| Floppy disk  | TO8/TO8D/TO9+    | 'B' key (BASIC 512) |
|              | TO9              | 'D' key (BASIC 128) |
|              | MO5              | RUN" + Enter        |
| Tape         | TO8/TO8D/TO9+    | 'C' key (BASIC 1.0) |
|              | TO9              | 'E' key (BASIC 1.0) |
|              | MO5/MO6/TO7      | RUN" or LOADM"",,R + Enter |
| Cartridge    | All but MO5/TO7  | '0' key             |
|              | TO7/TO7-70       | '1' key             |
|              | MO5              | Nothing (cartridges are already autostarted on MO5) |

If the game does not start, then maybe another BASIC must be used.

TO7 and TO7/70: When a cartridge is not loaded, the BASIC cartridge is automatically loaded (BASIC 1 for TO7, BASIC 128 for TO7/70).

**Virtual keyboard feature:** the Y/X buttons of the controller can be used to select a digit/letter/enter key (Y=go down, X=go up) and the Select button simulates a keystroke on the selected key.
The order of the keys in the virtual keyboard is: digits (0->9) then letters (A->Z) then "Space" then "Enter".

### Keyboard: mapping of special keys

| Thomson keyboard | PC keyboard |
| ------------- | ------------- |
| STOP  | TAB  |
| CNT  | CTRL  |
| CAPSLOCK  | CAPSLOCK  |
| ACC  | BACKSPACE  |
| HOME  | HOME  |
| Arrows  | Arrows  |
| INS  | INSERT  |
| EFF  | DEL  |
| RAZ  | ALT  |
| F1-F5  | F1-F5  |
| F6-F10  | SHIFT+F1-F5  |
| Yellow key (MO5) | Left SHIFT |
| BASIC (MO5/MO6) | Right SHIFT |

RetroArch already uses lots of keyboard shortcuts for its own need that interfere with the core's keyboard emulation. To avoid this problem, configure RetroArch with a "Hotkey", as indicated in [Introduction to Hotkeys](https://docs.libretro.com/guides/retroarch-keyboard-controls/#introduction-to-hotkeys), and/or use the "Game Focus" mode (toggle with "Scroll Lock" key).

### :floppy_disk: File formats

The emulator can read the following file formats: *.fd and *.sap (floppy disks), *.k7 (tapes), *.m7/*.m5 and *.rom (cartridges).

### :computer: Thomson models

By default, the core tries to guess the required Thomson model based on the name of the file loaded (e.g. saphir_to8.fd will switch to TO8, pulsar_mo5.k7 will switch to MO5 and so on). The fallback is to emulate a TO8 computer. Using the "Thomson model" option you can force the emulation of a particular model, or use "Auto" for the default "best guess" behavior.

### :rewind: Save states & Rewind

The emulator supports libretro's "save state" feature. Under RetroArch, use the following keys: F2 (save state), F4 (load state), F6/F7 (change state slot). Under Recalbox, use the following buttons: Hotkey + Y (save state), Hotkey + X (load state), Hotkey + "Up/Down Arrow" (change state slot).
The emulator also supports libretro's "rewind" feature. Under RetroArch, press and hold the "R" key. Under Recalbox, press and hold the HotKey button and the "Left Arrow".

### :innocent: Cheat codes

The emulator is compatible with RetroArch's cheat codes feature (cf. [Cheat code searching creation interface](https://www.libretro.com/index.php/upcoming-retroarch-1-7-4-cheat-code-searchingcreation-interface-with-rumble-features/)).

The [libretro-database-thomson](https://github.com/Zlika/libretro-database-thomson) repository includes cheat codes for some Thomson games.

### :beetle: Disassembler / Debugger

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

Another core's option, "Break on illegal opcode", allows to break and enter step-by-step debugging when an illegal opcode is met.
