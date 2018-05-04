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

#include <stdbool.h>

// Set or unset the floppy's write protection
void SetFloppyWriteProtect(bool enabled);
// Set or unset the tape's write protection
void SetTapeWriteProtect(bool enabled);
// Enable or disable the printer emulation
void SetPrinterEmulationEnabled(bool enabled);

// Load a floppy disk (fd format)
void Loadfd(const char *filename);
// Load a tape
void Loadk7(const char *filename);
// Load a memo7 cartridge
void Loadmemo(const char *filename);
// Unload the floppy disk
void Unloadfd(void);
// Unload the tape
void Unloadk7(void);
// Unload the cartridge
void Unloadmemo(void);

// Run an input/output related opcode
void RunIoOpcode(int opcode);

#endif /* __DEVICES_H */
