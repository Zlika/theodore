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
#include "sap.h"

#define SECTOR_SIZE      256  // Size in bytes of a double density sector
#define NB_TRACKS         80  // Number of tracks in a floppy
#define SECTORS_PER_TRACK 16  // Number of sectors in a track
#define SECTORS_PER_SIDE  SECTORS_PER_TRACK * NB_TRACKS

// Global variables
static bool fdprotection = true;
static bool k7protection = true;
static bool printerEnabled = false;
static FILE *ffd = NULL;   // floppy file (fd format)
static FILE *fk7 = NULL;   // tape file
static FILE *fprn = NULL;  // printer file
static SapFile sap = { 0, NULL }; // floppy file (sap format)

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
  Mputc(0x604e, n);     // error code in DK.STA
  CC |= 0x01;           // error indicator
  return;
}

// Floppy drive: Read a sector.
// This function emulates the DKCO function of the monitor with DK.OPC=2.
static void Readsector(void)
{
  char buffer[SECTOR_SIZE];
  int i, j, u, p, s;
  int errcode;

  if (ffd == NULL && sap.handle == NULL) {Diskerror(DISK_NO_DISK_ERROR); return;}
  // Drive number (0/1: 2 sides of the internal drive,
  //               2/3: 2 sides of the external drive,
  //               4  : RAM disk)
  u = Mgetc(0x6049) & 0xff; if(u > 3) {Diskerror(DISK_IO_ERROR); return;}
  // Track number (0->79)
  p = Mgetc(0x604a) & 0xff; if(p != 0) {Diskerror(DISK_IO_ERROR); return;}
  p = Mgetc(0x604b) & 0xff; if(p >= NB_TRACKS) {Diskerror(DISK_IO_ERROR); return;}
  // Sector number
  s = Mgetc(0x604c) & 0xff; if((s == 0) || (s > SECTORS_PER_TRACK)) {Diskerror(DISK_IO_ERROR); return;}
  for (j = 0; j < SECTOR_SIZE; j++) buffer[j] = 0xe5;
  if (ffd != NULL)
  {
    // FD file
    s += SECTORS_PER_TRACK * p + SECTORS_PER_SIDE * u;
    if (fseek(ffd, 0, SEEK_END)) {Diskerror(DISK_IO_ERROR); return;}
    if ((s << 8) > ftell(ffd)) {Diskerror(DISK_IO_ERROR); return;}
    if (fseek(ffd, (s - 1) << 8, SEEK_SET)) {Diskerror(DISK_IO_ERROR); return;}
    if (fread(buffer, SECTOR_SIZE, 1, ffd) == 0) {Diskerror(DISK_IO_ERROR); return;}
  }
  else
  {
    // SAP file
    errcode = sap_readSector(&sap, p, s, buffer);
    if (errcode != DISK_NO_ERROR) {Diskerror(errcode); return;}
  }
  i = ((Mgetc(0x604f) & 0xff) << 8) + (Mgetc(0x6050) & 0xff);
  for (j = 0; j < SECTOR_SIZE; j++) Mputc(i++, buffer[j]);
}

// Floppy drive: Write a sector.
// This function emulates the DKCO function of the monitor with DK.OPC=8.
static void Writesector(void)
{
  char buffer[SECTOR_SIZE];
  int i, j, u, p, s;
  int errcode;

  if (ffd == NULL && sap.handle == NULL) {Diskerror(DISK_NO_DISK_ERROR); return;}
  if (fdprotection) {Diskerror(DISK_WRITE_PROTECTION_ERROR); return;}
  // Drive number (0/1: 2 sides of the internal drive,
  //               2/3: 2 sides of the external drive,
  //               4  : RAM disk)
  u = Mgetc(0x6049) & 0xff; if(u > 3) {Diskerror(DISK_IO_ERROR); return;}
  // Track number (0->79)
  p = Mgetc(0x604a) & 0xff; if(p != 0) {Diskerror(DISK_IO_ERROR); return;}
  p = Mgetc(0x604b) & 0xff; if(p >= NB_TRACKS) {Diskerror(DISK_IO_ERROR); return;}
  // Sector number
  s = Mgetc(0x604c) & 0xff; if((s == 0) || (s > SECTORS_PER_TRACK)) {Diskerror(DISK_IO_ERROR); return;}
  i = SECTOR_SIZE * (Mgetc(0x604f) & 0xff) + (Mgetc(0x6050) & 0xff);
  for (j = 0; j < SECTOR_SIZE; j++) buffer[j] = Mgetc(i++);
  if (ffd != NULL)
  {
    // FD file
    s += SECTORS_PER_TRACK * p + SECTORS_PER_SIDE * u;
    if (fseek(ffd, (s - 1) << 8, SEEK_SET)) {Diskerror(DISK_IO_ERROR); return;}
    if (fwrite(buffer, SECTOR_SIZE, 1, ffd) == 0) {Diskerror(DISK_IO_ERROR); return;}
  }
  else
  {
    // SAP file
    errcode = sap_writeSector(&sap, p, s, buffer);
    if (errcode != DISK_NO_ERROR) {Diskerror(errcode); return;}
  }
}

// Format a floppy disk.
// This function emulates the DKFORM function of the monitor.
static void Formatdisk(void)
{
  char buffer[SECTOR_SIZE];
  int i, u, fatlength;
  if (ffd == NULL) {Diskerror(DISK_NO_DISK_ERROR); return;}
  if (fdprotection) {Diskerror(DISK_WRITE_PROTECTION_ERROR); return;}
  u = Mgetc(0x6049) & 0xff; if(u > 03) return; // Unit
  u = (SECTORS_PER_SIDE * u) << 8; // Start of the unit in the .fd file
  fatlength = 160;     // 80=160Ko, 160=320Ko
  // rem: fatlength provisoire !!!!! (tester la variable adequate)
  // Initialisation of the whole disk with 0xE5
  for (i = 0; i < SECTOR_SIZE; i++) buffer[i] = 0xe5;
  if (fseek(ffd, u, SEEK_SET)) {Diskerror(DISK_IO_ERROR); return;}
  for (i = 0; i < (fatlength * 8); i++)
    if (fwrite(buffer, SECTOR_SIZE, 1, ffd) == 0) {Diskerror(DISK_IO_ERROR); return;}
  // Initialisation of track 20 at 0xFF
  for (i = 0; i < SECTOR_SIZE; i++) buffer[i] = 0xff;
  if (fseek(ffd, u + 0x14000, SEEK_SET)) {Diskerror(DISK_IO_ERROR); return;}
  for (i = 0; i < SECTORS_PER_TRACK; i++)
    if (fwrite(buffer, SECTOR_SIZE, 1, ffd) == 0) {Diskerror(DISK_IO_ERROR); return;}
  // Write the FAT
  buffer[0x00] = 0;
  buffer[0x29] = 0xfe; buffer[0x2a] = 0xfe;
  for (i = fatlength + 1; i < SECTOR_SIZE; i++) buffer[i] = 0xfe;
  if (fseek(ffd, u + 0x14100, SEEK_SET)) {Diskerror(DISK_IO_ERROR); return;}
  if (fwrite(buffer, SECTOR_SIZE, 1, ffd) == 0) {Diskerror(DISK_IO_ERROR); return;}
}

void UnloadFloppy(void)
{
  if (ffd) {fclose(ffd); ffd = NULL;}
  if (sap.handle) {sap_close(&sap);}
}

void LoadFd(const char *filename)
{
  //fermeture disquette eventuellement ouverte
  UnloadFloppy();
  if(filename[0] == '\0') return;
  //ouverture de la nouvelle disquette
  ffd = fopen(filename, "rb+");
}

void LoadSap(const char *filename)
{
  UnloadFloppy();
  sap = sap_open(filename);
}

void UnloadK7(void)
{
  if(fk7) {fclose(fk7); fk7 = NULL;}
}

void LoadK7(const char *filename)
{
  UnloadK7();
  if(filename[0] == '\0') return;
  fk7 = fopen(filename, "rb+");
  if(fk7 == NULL) return;
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
    if (fseek(fk7, 0, SEEK_SET)) UnloadK7();
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

void UnloadMemo(void)
{
  carflags = 0;
  Hardreset();
}

void LoadMemo(const char *filename)
{
  FILE *fp = NULL;
  int i, c, carsize;
  // Open the memo7 file
  fp = fopen(filename, "rb");
  if(fp == NULL) {UnloadMemo(); return;}
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
