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

#include "sap.h"
#include <stdio.h>
#include <string.h>

#define SAP_HEADER_SIZE        66
#define SAP_SECTOR_DATA_OFFSET 4
#define SAP_SECTOR_CRC_LENGTH  2
#define SAP_SECTOR_OVERHEAD    SAP_SECTOR_DATA_OFFSET + SAP_SECTOR_CRC_LENGTH
#define SAP_MAGIC_NUM          0xB3

#define SECTOR_SIZE(f)         (f == 2 ? 128 : 256)
#define SAP_SECTOR_SIZE(f)     (SECTOR_SIZE(f) + SAP_SECTOR_OVERHEAD)
#define SAP_SECTOR_MAX_SIZE    (256 + SAP_SECTOR_OVERHEAD)
#define SAP_SECTORS_PER_TRACK  16

#define SAP_HEADER             "SYSTEME D'ARCHIVAGE PUKALL S.A.P."

/* table de calcul du CRC */
static const short int puktable[] = {
   0x0000, 0x1081, 0x2102, 0x3183,
   0x4204, 0x5285, 0x6306, 0x7387,
   0x8408, 0x9489, 0xa50a, 0xb58b,
   0xc60c, 0xd68d, 0xe70e, 0xf78f
};

SapFile sap_open(const char *filename)
{
  FILE *file;
  char header[SAP_HEADER_SIZE];
  SapFile sapFile = { 0, NULL };

  file = fopen(filename, "rb+");
  if (file == NULL)
  {
    return sapFile;
  }
  if (fread(header, SAP_HEADER_SIZE, 1, file) != 1)
  {
    fclose(file);
    return sapFile;
  }
  if (((header[0] != 1) && (header[0] != 2)) || strncmp(header + 1, SAP_HEADER, sizeof(SAP_HEADER) - 1) != 0)
  {
    fclose(file);
    return sapFile;
  }
  sapFile.format = header[0];
  sapFile.handle = file;
  return sapFile;
}

// Update CRC with new data c.
static short int update_crc(char c, short int crc)
{
    short int index;
    index = (crc ^ c) & 0xf;
    crc = ((crc >> 4) & 0xfff) ^ puktable[index];
    c >>= 4;
    index = (crc ^ c) & 0xf;
    crc = ((crc >> 4) & 0xfff) ^ puktable[index];
    return crc;
}

// Computes CRC of a sector.
static short int compute_crc(char *sap_sector, int sector_size)
{
    int i;
    short int crc = 0xffff;

    for (i = 0; i < sector_size - SAP_SECTOR_CRC_LENGTH; i++)
    {
      crc = update_crc(sap_sector[i], crc);
    }
    return crc;
}

DiskErrCode sap_readSector(const SapFile *file, int track, int sector, char *data)
{
  int i;
  short int expected_crc, actual_crc;
  char sap_sector[SAP_SECTOR_MAX_SIZE];
  int sector_size = SECTOR_SIZE(file->format);
  int sap_sector_size = SAP_SECTOR_SIZE(file->format);
  int offset = SAP_HEADER_SIZE + (track * SAP_SECTORS_PER_TRACK + sector - 1) * sap_sector_size;

  if (fseek(file->handle, offset, SEEK_SET))
  {
    return DISK_IO_ERROR;
  }
  if (fread(sap_sector, sap_sector_size, 1, file->handle) != 1)
  {
    return DISK_IO_ERROR;
  }
  for (i = 0; i < sector_size; i++)
  {
    sap_sector[SAP_SECTOR_DATA_OFFSET + i] ^= SAP_MAGIC_NUM;
    data[i] = sap_sector[SAP_SECTOR_DATA_OFFSET + i];
  }
  // Check sector CRC
  expected_crc = compute_crc(sap_sector, sap_sector_size);
  actual_crc = (sap_sector[sap_sector_size-2] << 8) + (sap_sector[sap_sector_size-1] & 0xFF);
  if (actual_crc != expected_crc)
  {
    return DISK_IO_ERROR;
  }
  return DISK_NO_ERROR;
}

DiskErrCode sap_writeSector(const SapFile *file, int track, int sector, char *data)
{
  int i;
  short int crc;
  char sap_sector[SAP_SECTOR_MAX_SIZE];
  int sector_size = SECTOR_SIZE(file->format);
  int sap_sector_size = SAP_SECTOR_SIZE(file->format);
  int offset = SAP_HEADER_SIZE + (track * SAP_SECTORS_PER_TRACK + sector - 1) * sap_sector_size;

  if (fseek(file->handle, offset, SEEK_SET))
  {
    return DISK_IO_ERROR;
  }
  // Re-use the same sector's header data then the current one
  if (fread(sap_sector, SAP_SECTOR_DATA_OFFSET, 1, file->handle) != 1)
  {
    return DISK_IO_ERROR;
  }
  // Rewind to the beginning of the sector
  if (fseek(file->handle, offset, SEEK_SET))
  {
    return DISK_IO_ERROR;
  }
  // Sector protected
  if (sap_sector[1] != 0)
  {
    return DISK_SECTOR_PROTECTED_ERROR;
  }
  memcpy(sap_sector + SAP_SECTOR_DATA_OFFSET, data, sector_size);
  // Compute sector CRC
  crc = compute_crc(sap_sector, sap_sector_size);
  sap_sector[sap_sector_size-2] = crc >> 8;
  sap_sector[sap_sector_size-1] = crc & 0xFF;
  // Encrypt sector data
  for (i = 0; i < sector_size; i++)
  {
    sap_sector[SAP_SECTOR_DATA_OFFSET + i] ^= SAP_MAGIC_NUM;
  }
  if (fwrite(sap_sector, sap_sector_size, 1, file->handle) != 1)
  {
    return DISK_IO_ERROR;
  }
  return DISK_NO_ERROR;
}

bool sap_close(SapFile *file)
{
  bool result = (fclose(file->handle) == 0);
  file->format = 0;
  file->handle = NULL;
  return result;
}
