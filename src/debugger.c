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
#include "motoemulator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "boolean.h"

#define DBG_ARRAY_LENGTH 100
#define BP_LENGTH 20

static DebuggerMode dbg_mode = DEBUG_DISABLED;
static char dbg_instruction[DBG_ARRAY_LENGTH] = { 0 };
static char dbg_registers[DBG_ARRAY_LENGTH] = { 0 };
static char dbg_command[DBG_ARRAY_LENGTH] = { 0 };
static bool break_on_illegal_opcode = false;

// Breakpoints on the values of the program counter
static unsigned short bp_pc[BP_LENGTH] = { 0 };
static int bp_pc_numitems = 0;
// Breakpoints on memory read addresses
static unsigned short bp_read_mem[BP_LENGTH] = { 0 };
static int bp_read_mem_numitems = 0;
// Breakpoints on memory write addresses
static unsigned short bp_write_mem[BP_LENGTH] = { 0 };
static int bp_write_mem_numitems = 0;

void debugger_setMode(DebuggerMode mode)
{
  dbg_mode = mode;
}

static void print_registers(char* string)
{
  sprintf(string, "A=%02X B=%02X X=%04X Y=%04X U=%04X S=%04X DP=%02X CC=%02X",
      *dc6809_a & 0xFF, *dc6809_b & 0xFF, dc6809_x & 0xFFFF, dc6809_y & 0xFFFF,
      dc6809_u & 0xFFFF, dc6809_s & 0xFFFF, *dc6809_dp & 0xFF, dc6809_cc & 0xFF);
}

static void list_breakpoints()
{
  int i;
  printf("Breakpoints at PC addresses:\n");
  for (i = 0; i < bp_pc_numitems; i++)
  {
    printf("%04x\n", bp_pc[i]);
  }
  printf("Breakpoints on memory read addresses:\n");
  for (i = 0; i < bp_read_mem_numitems; i++)
  {
    printf("%04x\n", bp_read_mem[i]);
  }
  printf("Breakpoints on memory write addresses:\n");
  for (i = 0; i < bp_write_mem_numitems; i++)
  {
    printf("%04x\n", bp_write_mem[i]);
  }
}

static void clear_breakpoints()
{
  memset(bp_pc, 0, BP_LENGTH);
  bp_pc_numitems = 0;
  memset(bp_read_mem, 0, BP_LENGTH);
  bp_read_mem_numitems = 0;
  memset(bp_write_mem, 0, BP_LENGTH);
  bp_write_mem_numitems = 0;
}

static void add_breakpoint(unsigned short *bp_list, int *bp_num_items, char *value)
{
  char *endptr;
  unsigned short val = (unsigned short) strtol(value, &endptr, 16);
  if (val == 0 && *endptr != '\n')
  {
    printf("Invalid address value\n");
    return;
  }
  if (*bp_num_items < BP_LENGTH)
  {
    bp_list[(*bp_num_items)++] = val;
  }
}

static void read_debugger_command()
{
  char *endptr = NULL;
  unsigned short address;
  char value;

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
      add_breakpoint(bp_pc, &bp_pc_numitems, dbg_command + 6);
    }
    // Add a breakpoint when reading at the given address (in hex)
    else if (strncmp(dbg_command, "bp read ", 8) == 0)
    {
      add_breakpoint(bp_read_mem, &bp_read_mem_numitems, dbg_command + 8);
    }
    // Add a breakpoint when writing at the givent address (in hex)
    else if (strncmp(dbg_command, "bp write ", 9) == 0)
    {
      add_breakpoint(bp_write_mem, &bp_write_mem_numitems, dbg_command + 9);
    }
    // Read a memory location
    else if (strncmp(dbg_command, "read ", 5) == 0)
    {
      address = (unsigned short) strtol(dbg_command + 5, &endptr, 16);
      if (address == 0 && *endptr != '\n')
      {
        printf("Invalid address value\n");
        return;
      }
      printf("ram[%04x] = %02x\n", address, Mgetc(address) & 0xFF);
    }
    // Write a memory location
    else if (strncmp(dbg_command, "write ", 6) == 0)
    {
      address = (unsigned short) strtol(dbg_command + 6, &endptr, 16);
      if (address == 0 && *endptr != '\n')
      {
        printf("Invalid address value\n");
        return;
      }
      value = (char) strtol(dbg_command + 10, &endptr, 16);
      if (value == 0 && *endptr != '\n')
      {
        printf("Invalid value\n");
        return;
      }
      Mputc(address, value);
    }
    // Unknown command
    else
    {
      printf("Unknown command\n");
    }
  }
}

static bool check_breakpoint(unsigned short *bp_list, int bp_list_numitems, unsigned short address)
{
  int i = 0;
  while (i < bp_list_numitems)
  {
    if (bp_list[i] == address)
    {
      return true;
    }
    ++i;
  }
  return false;
}

void debug(unsigned short address)
{
  if (dbg_mode != DEBUG_DISABLED)
  {
    if (check_breakpoint(bp_pc, bp_pc_numitems, address))
    {
      printf("Breakpoint at PC=%04x\n", address);
      debugger_setMode(DEBUG_STEP);
    }
    dasm6809(address, dbg_instruction);
    print_registers(dbg_registers);
    if (dbg_mode != DEBUG_RUN)
    {
      printf("%s %s\n", dbg_instruction, dbg_registers);
      if (dbg_mode == DEBUG_STEP)
      {
        read_debugger_command();
      }
    }
  }
}

static void debug_mem(unsigned short address, bool is_read)
{
  if (dbg_mode != DEBUG_DISABLED)
  {
    if (is_read ? check_breakpoint(bp_read_mem, bp_read_mem_numitems, address)
                : check_breakpoint(bp_write_mem, bp_write_mem_numitems, address))
    {
      printf("Breakpoint: memory %s at %04x\n", is_read ? "read" : "write", address);
      // If in RUN mode, print the current instruction
      // (in other modes, it was already printed)
      if (dbg_mode == DEBUG_RUN)
      {
        printf("%s %s\n", dbg_instruction, dbg_registers);
      }
      debugger_setMode(DEBUG_STEP);
      read_debugger_command();
    }
  }
}

void debug_mem_read(unsigned short address)
{
  debug_mem(address, true);
}

void debug_mem_write(unsigned short address)
{
  debug_mem(address, false);
}

void debugger_break(void)
{
  debugger_setMode(DEBUG_STEP);
  printf("%s %s\n", dbg_instruction, dbg_registers);
  read_debugger_command();
}

void debugger_set_break_on_illegal_opcode(bool enabled)
{
  break_on_illegal_opcode = enabled;
}

void debugger_illegal_opcode(void)
{
  if (break_on_illegal_opcode)
  {
    printf("Break on illegal opcode\n");
    debugger_break();
  }
}
