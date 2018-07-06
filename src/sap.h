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

/* Management of the SAP file format */

#ifndef __SAP_H
#define __SAP_H

#include <stdio.h>
#include "boolean.h"

typedef struct
{
  unsigned char format;
  FILE *handle;
} SapFile;

// Opens a SAP file.
// SapFile.handle is NULL in case of error.
SapFile sap_open(const char *filename);
// Reads a given sector from the SAP file and stores its content in the 'data' buffer.
// Returns true for success, false for failure.
bool sap_readSector(const SapFile *file, int track, int sector, char *data);
// Writes a given sector into the SAP file from the content of the 'data' buffer.
// Returns true for success, false for failure.
bool sap_writeSector(const SapFile *file, int track, int sector, char *data);
// Closes the SAP file.
// Returns true for success, false for failure.
bool sap_close(SapFile *file);

#endif /* __SAP_H */
