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

/* Tools to manage the SAP file format */

#include "sap.h"
#include <stdio.h>

#define SECTOR_SIZE     256

#define SAP_HEADER_SIZE        66
#define SAP_SECTOR_SIZE        262
#define SAP_SECTOR_DATA_OFFSET 4
#define SAP_MAGIC_NUM          0xB3

bool sap2fd(const char *sap_filename, const char *fd_filename)
{
  FILE *sap_file = NULL, *fd_file = NULL;
  unsigned char sap_sector[SAP_SECTOR_SIZE];
  int i;

  sap_file = fopen(sap_filename, "rb");
  if (sap_file == NULL)
  {
    return false;
  }
  fd_file = fopen(fd_filename, "w");
  if (fd_file == NULL)
  {
    fclose(sap_file);
    return false;
  }
  fseek(sap_file, SAP_HEADER_SIZE, SEEK_SET);

  while (fread(sap_sector, SAP_SECTOR_SIZE, 1, sap_file))
  {
    for (i = 0; i < SECTOR_SIZE; i++)
    {
      sap_sector[SAP_SECTOR_DATA_OFFSET + i] = sap_sector[SAP_SECTOR_DATA_OFFSET + i] ^ SAP_MAGIC_NUM;
    }
    if (!fwrite(sap_sector + SAP_SECTOR_DATA_OFFSET, SECTOR_SIZE, 1, fd_file))
    {
      fclose(fd_file);
      fclose(sap_file);
      return false;
    }
  }
  fclose(fd_file);
  fclose(sap_file);
  return true;
}
