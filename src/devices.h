/*
 * This file is part of theodore (https://github.com/Zlika/theodore),
 * a Thomson emulator based on Daniel Coulom's DCTO8D emulator
 * (http://dcto8.free.fr/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* Emulation of Thomson devices (floppy/tape/cartridge drives, printer, light pen, mouse) */

#ifndef __DEVICES_H
#define __DEVICES_H

#include "boolean.h"

// Set the MO/TO mode: true = TO, false = MO
void SetModeTO(bool isTO);

// Set or unset the floppy's write protection
void SetFloppyWriteProtect(bool enabled);
// Set or unset the tape's write protection
void SetTapeWriteProtect(bool enabled);
// Enable or disable the printer emulation
void SetPrinterEmulationEnabled(bool enabled);

// Load a floppy disk (fd format)
void LoadFd(const char *filename);
// Load a floppy disk (sap format)
void LoadSap(const char *filename);
// Load a tape
void LoadTape(const char *filename);
// Load a memo7 cartridge
void LoadMemo(const char *filename);
// Load a memo7 cartridge from an array
void LoadMemoFromArray(const char *rom, unsigned int rom_size);
// Unload the floppy disk
void UnloadFloppy(void);
// Unload the tape
void UnloadTape(void);
// Unload the cartridge
void UnloadMemo(void);
// Rewind the tape
void RewindTape(void);

// Run an input/output related opcode.
// These "wrong" opcodes come from the patching of the ROM
// and are used to emulate I/O functions of the monitor.
void RunIoOpcode(int opcode);

// The following functions are used for libretro's save states feature.
// Returns the amount of data required to serialize the internal state of the device module.
unsigned int device_serialize_size(void);
// Serializes the internal state of the device module.
void device_serialize(void *data);
// Unserializes the internal state of the device module.
void device_unserialize(const void *data);

#endif /* __DEVICES_H */
