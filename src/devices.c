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

#include <stdio.h>
#include <string.h>

#include "6809cpu.h"
#include "to8demulator.h"

// Global variables
static bool fdprotection = true;
static bool k7protection = true;
static bool printerEnabled = false;
static FILE *ffd = NULL;   // floppy file
static FILE *fk7 = NULL;   // tape file
static FILE *fprn = NULL;  // printer file

// 6809 registers
#define CC dc6809_cc
#define A *dc6809_a
#define B *dc6809_b
#define X dc6809_x
#define Y dc6809_y

void SetFloppyWriteProtect(bool enabled)
{
  fdprotection = enabled;
}

void SetTapeWriteProtect(bool enabled)
{
  k7protection = enabled;
}

void SetPrinterEmulationEnabled(bool enabled)
{
  printerEnabled = enabled;
}

// Printer emulation
static void Print(void)
{
  if (printerEnabled)
  {
    if(fprn == NULL) fprn = fopen("thomson-printer.txt", "ab");
    if(fprn != NULL) {fputc(B, fprn); CC &= 0xfe;};
  }
}

// Read/write error
static void Diskerror(int n)
{
  Mputc(0x604e, n - 1); // error 53 = I/O error
  CC |= 0x01;           // error indicator
  return;
}

// Floppy drive: Read a sector
static void Readsector(void)
{
  char buffer[256];
  int i, j, u, p, s;
  if(ffd == NULL) {Diskerror(71); return;}
  u = Mgetc(0x6049) & 0xff; if(u > 03) {Diskerror(53); return;}
  p = Mgetc(0x604a) & 0xff; if(p != 0) {Diskerror(53); return;}
  p = Mgetc(0x604b) & 0xff; if(p > 79) {Diskerror(53); return;}
  s = Mgetc(0x604c) & 0xff; if((s == 0) || (s > 16)) {Diskerror(53); return;}
  s += 16 * p + 1280 * u;
  fseek(ffd, 0, SEEK_END);
  if((s << 8) > ftell(ffd)) {Diskerror(53); return;}
  for(j = 0; j < 256; j++) buffer[j] = 0xe5;
  fseek(ffd, (s - 1) << 8, SEEK_SET);
  i = ((Mgetc(0x604f) & 0xff) << 8) + (Mgetc(0x6050) & 0xff);
  if(fread(buffer, 256, 1, ffd) == 0) {Diskerror(53); return;}
  for(j = 0; j < 256; j++) Mputc(i++, buffer[j]);
}

// Floppy drive: Write a sector
static void Writesector(void)
{
  char buffer[256];
  int i, j, u, p, s;
  if(ffd == NULL) {Diskerror(71); return;}
  // error 72 = write protection
  if(fdprotection) {Diskerror(72); return;}
  u = Mgetc(0x6049) & 0xff; if(u > 03) {Diskerror(53); return;}
  p = Mgetc(0x604a) & 0xff; if(p != 0) {Diskerror(53); return;}
  p = Mgetc(0x604b) & 0xff; if(p > 79) {Diskerror(53); return;}
  s = Mgetc(0x604c) & 0xff; if((s == 0) || (s > 16)) {Diskerror(53); return;}
  s += 16 * p + 1280 * u;
  fseek(ffd, (s - 1) << 8, SEEK_SET);
  i = 256 * (Mgetc(0x604f) & 0xff) + (Mgetc(0x6050) & 0xff);
  for(j = 0; j < 256; j++) buffer[j] = Mgetc(i++);
  if(fwrite(buffer, 256, 1, ffd) == 0) Diskerror(53);
}

// Format a floppy disk
static void Formatdisk(void)
{
  char buffer[256];
  int i, u, fatlength;
  if(ffd == NULL) {Diskerror(71); return;}
  // error 72 = write protection
  if(fdprotection) {Diskerror(72); return;}
  u = Mgetc(0x6049) & 0xff; if(u > 03) return; // Unit
  u = (1280 * u) << 8; // Start of the unit in the .fd file
  fatlength = 160;     // 80=160Ko, 160=320Ko
  //rem: fatlength provisoire !!!!! (tester la variable adequate)
  // Initialisation of the whole disk with 0xE5
  for(i = 0; i < 256; i++) buffer[i] = 0xe5;
  fseek(ffd, u, SEEK_SET);
  for(i = 0; i < (fatlength * 8); i++)
    if(fwrite(buffer, 256, 1, ffd) == 0) {Diskerror(53); return;}
  // Initialisation of track 20 at 0xFF
  for(i = 0; i < 256; i++) buffer[i] = 0xff;
  fseek(ffd, u + 0x14000, SEEK_SET);
  for(i = 0; i < 16; i++)
    if(fwrite(buffer, 256, 1, ffd) == 0) {Diskerror(53); return;}
  // Write the FAT
  buffer[0x00] = 0;
  buffer[0x29] = 0xfe; buffer[0x2a] = 0xfe;
  for(i = fatlength + 1; i < 256; i++) buffer[i] = 0xfe;
  fseek(ffd, u + 0x14100, SEEK_SET);
  if(fwrite(buffer, 256, 1, ffd) == 0) {Diskerror(53); return;}
}

void Unloadfd(void)
{
  if(ffd) {fclose(ffd); ffd = NULL;}
}

void Loadfd(const char *filename)
{
  //fermeture disquette eventuellement ouverte
  Unloadfd();
  if(filename[0] == '\0') return;
  //ouverture de la nouvelle disquette
  ffd = fopen(filename, "rb+");
  if(ffd == NULL) return;
}

// Tape drive: read a byte
static void Readbytek7(void)
{
  int byte = 0;
  if(fk7 == NULL) {Initprog(); return;}
  byte = fgetc(fk7);
  if(byte == EOF)
  {
    Initprog();
    fseek(fk7, 0, SEEK_SET);
    return;
  }
  A = byte; Mputc(0x2045, byte);
}

// Tape drive: write a byte
static void Writebytek7(void)
{
  if(fk7 == NULL) {Initprog(); return;}
  if(k7protection) {Initprog(); return;}
  if(fputc(A, fk7) == EOF) {Initprog(); return;}
  Mputc(0x2045, 0);
}

void Unloadk7(void)
{
  if(fk7) {fclose(fk7); fk7 = NULL;}
}

void Loadk7(const char *filename)
{
  Unloadk7();
  if(filename[0] == '\0') return;
  fk7 = fopen(filename, "rb+");
  if(fk7 == NULL) return;
  fseek(fk7, 0, SEEK_END);
  fseek(fk7, 0, SEEK_SET);
}

void Unloadmemo(void)
{
  carflags = 0;
  Hardreset();
}

void Loadmemo(const char *filename)
{
  FILE *fp = NULL;
  int i, c, carsize;
  // Open the memo7 file
  fp = fopen(filename, "rb");
  if(fp == NULL) {Unloadmemo(); return;}
  // Loading
  carsize = 0;
  while(((c = fgetc(fp)) != EOF) && (carsize < 0x10000)) car[carsize++] = c;
  fclose(fp);
  for(i = 0; i < 0xc000; i++) ram[i] = -((i & 0x80) >> 7);
  cartype = 0; // cartridge <= 16 Ko
  if(carsize > 0x4000) cartype = 1;   // bank switch system
  carflags = 4; // cartridge enabled, write disabled, bank 0
  Initprog();   // init to launch the cartridge
}

// Read the buttons of the mouse
static void Readmousebutton(void)
{
  A = 3; if(penbutton) {A = 0; CC |= 0x05;}
}

// Read the position of the light pen (device=0) or the mouse (device=1)
static void Readpenxy(int device)
{
  int k;
  if((xpen < 0) || (xpen >= 640)) {CC |= 1; return;} // x out of bounds
  if((ypen < 0) || (ypen >= 200)) {CC |= 1; return;} // y out of bounds
  k = (port[0x1c] == 0x2a) ? 0 : 1; // 40 columns mode: x divided by 2
  if(device > 0) //mouse
  {
    Mputw(0x60d8, xpen >> k);
    Mputw(0x60d6, ypen);
  }
  X = xpen >> k;
  Y = ypen;
  CC &= 0xfe;
}

void RunIoOpcode(int opcode)
{
  switch(opcode)
  {
    case 0x14: Readsector(); break;      // read floppy sector
    case 0x15: Writesector(); break;     // write floppy sector
    case 0x18: Formatdisk(); break;      // format floppy
    case 0x42: Readbytek7(); break;      // read tape byte
    case 0x45: Writebytek7(); break;     // write tape byte
    case 0x4b: Readpenxy(0); break;      // read light pen position
    case 0x4e: Readpenxy(1); break;      // read mouse position
    case 0x51: Print(); break;           // print a character
    case 0x52: Readmousebutton(); break; // test mouse click
    default: break;                      // invalid opcode
  }
}
