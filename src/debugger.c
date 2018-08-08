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
#include <stdlib.h>
#include <string.h>
#include "boolean.h"

#define DBG_ARRAY_LENGTH 100
#define BP_LENGTH 10

static DebuggerMode dbg_mode = DEBUG_DISABLED;
static char dbg_instruction[DBG_ARRAY_LENGTH] = { 0 };
static char dbg_registers[DBG_ARRAY_LENGTH] = { 0 };
static char dbg_command[DBG_ARRAY_LENGTH] = { 0 };

// Breakpoints on the values of the program counter
static int bp_pc[BP_LENGTH] = { 0 };
// Breakpoints on memory read addresses
static int bp_read_mem[BP_LENGTH] = { 0 };
// Breakpoints on memory write addresses
static int bp_write_mem[BP_LENGTH] = { 0 };

void debugger_setMode(DebuggerMode mode)
{
  dbg_mode = mode;
}

static void print_registers(char* string)
{
  sprintf(string, "A=%02X B=%02X X=%04X Y=%04X CC=%02X",
      *dc6809_a & 0xFF, *dc6809_b & 0xFF, dc6809_x & 0xFFFF, dc6809_y & 0xFFFF, dc6809_cc & 0xFF);
}

static void list_breakpoints()
{
  int i;
  printf("Breakpoints at PC addresses:\n");
  for (i = 0; i < BP_LENGTH; i++)
  {
    if (bp_pc[i] == 0) break;
    printf("%04x\n", bp_pc[i]);
  }
  printf("Breakpoints on memory read addresses:\n");
  for (i = 0; i < BP_LENGTH; i++)
  {
    if (bp_read_mem[i] == 0) break;
    printf("%04x\n", bp_pc[i]);
  }
  printf("Breakpoints on memory write addresses:\n");
  for (i = 0; i < BP_LENGTH; i++)
  {
    if (bp_write_mem[i] == 0) break;
    printf("%04x\n", bp_pc[i]);
  }
}
static void clear_breakpoints()
{
  memset(bp_pc, 0, BP_LENGTH);
  memset(bp_read_mem, 0, BP_LENGTH);
  memset(bp_write_mem, 0, BP_LENGTH);
}

static void add_breakpoint(int *bp_list, char *value)
{
  int i;
  int val = (int) strtol(value, NULL, 16);
  if (val == 0)
  {
    printf("Invalid address value\n");
    return;
  }
  for (i  = 0; i < BP_LENGTH; i++)
  {
    if (bp_list[i] == 0)
    {
      bp_list[i] = val;
      break;
    }
  }
}

static void read_debugger_command()
{
  while (fgets(dbg_command, DBG_ARRAY_LENGTH, stdin) != NULL)
  {
    // Continue to next opcode
    if (strcmp(dbg_command, "\n") == 0)
    {
      debugger_setMode(DEBUG_STEP);
      break;
    }
    // Switch to RUN mode
    else if ((strcmp(dbg_command, "run\n") == 0) || (strcmp(dbg_command, "r\n") == 0))
    {
      debugger_setMode(DEBUG_RUN);
      break;
    }
    // Switch to TRACE mode
    else if ((strcmp(dbg_command, "trace\n") == 0) || (strcmp(dbg_command, "t\n") == 0))
    {
      debugger_setMode(DEBUG_TRACE);
      break;
    }
    // Disable debugger
    else if ((strcmp(dbg_command, "exit\n") == 0)
        || (strcmp(dbg_command, "quit\n") == 0) || (strcmp(dbg_command, "q\n") == 0))
    {
      debugger_setMode(DEBUG_DISABLED);
      break;
    }
    // Print the current list of breakpoints
    else if (strcmp(dbg_command, "bp list\n") == 0)
    {
      list_breakpoints();
    }
    // Clear the current list of breakpoints
    else if (strcmp(dbg_command, "bp clear\n") == 0)
    {
      clear_breakpoints();
    }
    // Add a breakpoint at the given program address (in hex)
    else if (strncmp(dbg_command, "bp pc ", 6) == 0)
    {
      add_breakpoint(bp_pc, dbg_command + 6);
    }
    // Add a breakpoint when reading at the given address (in hex)
    else if (strncmp(dbg_command, "bp read ", 8) == 0)
    {
      add_breakpoint(bp_pc, dbg_command + 8);
    }
    // Add a breakpoint when writing at the givent address (in hex)
    else if (strncmp(dbg_command, "bp write ", 9) == 0)
    {
      add_breakpoint(bp_pc, dbg_command + 9);
    }
    // Unknown command
    else
    {
      printf("???\n");
    }
  }
}

bool check_breakpoint(int *bp_list, int address)
{
  int i = 0;
  while (i < BP_LENGTH && bp_list[i] != 0)
  {
    if (bp_list[i] == address)
    {
      return true;
    }
    ++i;
  }
  return false;
}

void debug(int address)
{
  if (dbg_mode != DEBUG_DISABLED)
  {
    if (check_breakpoint(bp_pc, address))
    {
      printf("Breakpoint at PC=%04x\n", address);
      debugger_setMode(DEBUG_STEP);
    }
    if (dbg_mode != DEBUG_RUN)
    {
      dasm6809(address, dbg_instruction);
      print_registers(dbg_registers);
      printf("%s %s\n", dbg_instruction, dbg_registers);
      if (dbg_mode == DEBUG_STEP)
      {
        read_debugger_command();
      }
    }
  }
}

void debug_mem_read(int address)
{
  if (dbg_mode != DEBUG_DISABLED)
  {
    if (check_breakpoint(bp_read_mem, address))
    {
      printf("Breakpoint: memory read at %04x\n", address);
      debugger_setMode(DEBUG_STEP);
    }
  }
}

void debug_mem_write(int address)
{
  if (dbg_mode != DEBUG_DISABLED)
  {
    if (check_breakpoint(bp_write_mem, address))
    {
      printf("Breakpoint: memory write at %04x\n", address);
      debugger_setMode(DEBUG_STEP);
    }
  }
}
