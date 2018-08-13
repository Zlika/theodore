/*
 * This file is part of theodore, a Thomson emulator
 * (https://github.com/Zlika/theodore).
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

/* Rudimentary command-line debugger. */

#ifndef __DEBUGGER_H
#define __DEBUGGER_H

typedef enum
{
  /* Debugger disabled: no disassembly. */
  DEBUG_DISABLED,
  /* Debugger enabled but no instruction is printed until a breakpoint is reached
     (the debugger will then switch to DEBUG_STEP mode). */
  DEBUG_RUN,
  /* Debugger enabled: disassembles and prints all the instructions. */
  DEBUG_TRACE,
  /* Step-by-step mode: disassembles and prints an instruction and wait for a keystroke. */
  DEBUG_STEP
} DebuggerMode;

/* Sets the debugger mode. */
void debugger_setMode(DebuggerMode mode);
/* Debug the instruction at the given address. */
void debug(unsigned short address);
/* Check if a breakpoint is set for a memory read at the given address. */
void debug_mem_read(unsigned short address);
/* Check if a breakpoint is set for a memory write at the given address. */
void debug_mem_write(unsigned short address);

#endif
