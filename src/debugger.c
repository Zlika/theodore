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

#include "debugger.h"
#include "6809disasm.h"
#include "6809cpu.h"

#include <stdio.h>
#include <string.h>

#define DBG_ARRAY_LENGTH 100

static DebuggerMode dbg_mode = DEBUG_DISABLED;
static char dbg_instruction[DBG_ARRAY_LENGTH] = { 0 };
static char dbg_registers[DBG_ARRAY_LENGTH] = { 0 };
static char dbg_command[DBG_ARRAY_LENGTH] = { 0 };

void debugger_setMode(DebuggerMode mode)
{
  dbg_mode = mode;
}

static void print_registers(char* string)
{
  sprintf(string, "A=%02X B=%02X X=%04X Y=%04X CC=%02X",
      *dc6809_a & 0xFF, *dc6809_b & 0xFF, dc6809_x & 0xFFFF, dc6809_y & 0xFFFF, dc6809_cc & 0xFF);
}

static void read_debugger_command()
{
  if (fgets(dbg_command, DBG_ARRAY_LENGTH, stdin) != NULL)
  {
    if (strncmp(dbg_command, "run", 3) == 0)
    {
      debugger_setMode(DEBUG_RUN);
    }
    else if (strncmp(dbg_command, "exit", 4) == 0)
    {
      debugger_setMode(DEBUG_DISABLED);
    }
  }
}

void debug(int address)
{
  if (dbg_mode != DEBUG_DISABLED)
  {
    dasm6809(address & 0xFFFF, dbg_instruction);
    print_registers(dbg_registers);
    printf("%s %s\n", dbg_instruction, dbg_registers);
    if (dbg_mode == DEBUG_STEP)
    {
      read_debugger_command();
    }
  }
}
