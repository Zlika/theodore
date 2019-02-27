Next Release
===========
* Fix sound in some games (#8).
* Add emulation of illegal opcode 0x11F1. Fixes some MO5 games (#10).
* Add core option to break on illegal opcodes in the debugger.
* Fix keyboard emulation (#11).
* MO5: add heuristic to choose between RUN" and LOADM"",,R for autorun (#9).
* Add auto-detection of the Thomson model based on the content filename.
* Change mapping of virtual keyboard: now Start button starts the program and Select button simulates a keystroke.
* Fix save state/rewind during tape loading.
* Add support for so called "Classic platforms" in Makefile.

Release 1.6 (2019/02/05)
===========
* Add MO5 emulation.
* A more complex heuristic is used to start the game when the 'B' button of the gamepad is pressed, depending on the media type and of the emulated computer (cf. README file for more info).
* Add core option to autorun the game.
* Remove custom cheat code management (useless since RetroArch 1.7.4 and its built-in cheat code feature).

Release 1.5 (2018/12/29)
===========
* Add TO9 emulation.
* On TO9, the 'B' button of the gamepad simulates a keystroke on the 'D' key of the keyboard (BASIC 128), instead of the 'B' key for TO8/TO8D/TO9+ (BASIC 512).
* Fix Caps Lock emulation.
* Fix cartridge/tape no longer working after reset or change of Thomson version.
* Fix tape drive emulation.
* Fix MSVC2005/2008 build.

Release 1.4 (2018/11/24)
===========
* Add TO9+ emulation.
* Add support for Nintendo Switch platform.

Release 1.3 (2018/08/13)
===========
* Workaround for the protection used by some FIL games (Atomik, Avenger, Way of the Tiger).
* Workaround for the protection used by some Infogrames games (Dieux du stade 2, Dossier Boerhaave, Le temps d'une histoire).
* Add support for SAP sectors with format=4 (fixes Space Tunnel).
* Add experimental command-line disassembler/debugger.

Release 1.2 (2018/07/30)
===========
* Add full support for SAP files (without conversion to FD format).
* Fix build for 3DS, PS3 and MSVC2003/2005/2010.
* Release keys pressed through the virtual keyboard (fixes Avenger game).

Release 1.1 (2018/06/21)
===========
* Fix gamma correction.
* Add 'Space' to the virtual keyboard.
* Fix segfault during serialization.
* More accurate sound level.
* Fix compilation errors and wrong colors on ngc, wii and wiiu (#3).

Release 1.0 (2018/05/12)
===========
* Add support for save states.
* Add TO8 emulation.
* Add support for cheat codes.

Release 0.9 (2018/05/01)
===========
* Remove the standalone version of the emulator.
* Add an option to enable/disable printer emulation.
* Add a simple virtual keyboard feature to play without a keyboard.
* Add support for the SAP file format.
* Fix an occasional segfault.

Release 0.8 (2018/04/23)
===========
First release.
* Import of DCTO8D 2009.05 source code (http://dcto8.free.fr/).
* Replace makefile by autotools for compilation.
* Fix a buffer overflow and a memory leak.
* Upgrade to SDL 2.
* Add fullscreen mode.
* Simplify source code, minimize coupling and define clean interfaces between compilation units.
* Add compatibility with libretro.
