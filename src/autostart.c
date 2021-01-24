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

/* Functions to autostart a program or implement autodetection features */

#include "autostart.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "autostartkeys.h"
#include "logger.h"
#include "keymap.h"
#include "motoemulator.h"
#include "md5.h"

#define SIZE_BUFFER_TAPE 32
#define TAPE_BASIC_PATTERN1 "BAS\0"
#define TAPE_BASIC_PATTERN1_SIZE 4
#define TAPE_BASIC_PATTERN2 "ENTETE  MO"
#define TAPE_BASIC_PATTERN2_SIZE 10
#define TAPE_BASIC_PATTERN3 "ENTETE  TO"
#define TAPE_BASIC_PATTERN3_SIZE 10

static const Key *autostart_keys = NULL;
static int current_autostart_key_pos = -1;

static Media currentMedia = NO_MEDIA;
static bool program_is_basic = true;
#define MD5_LENGTH 16
#define MD5_BUFFER_SIZE 1024
static unsigned char md5_digest[MD5_LENGTH];

/* Compare 2 strings without casing. */
static bool streq_nocase(const char *s1, const char *s2)
{
  int i;
  for (i = 0; s1[i] != '\0'; i++)
  {
    if (toupper(s1[i]) != toupper(s2[i]))
    {
      return false;
    }
  }
  return true;
}

static bool is_fd_file(const char *filename)
{
  return (strlen(filename) > 3 && streq_nocase(filename + strlen(filename) - 3, ".fd"));
}

bool is_sap_file(const char *filename)
{
  return (strlen(filename) > 4 && streq_nocase(filename + strlen(filename) - 4, ".sap"));
}

Media get_media_type(const char *filename)
{
  if (strlen(filename) > 3 && streq_nocase(filename + strlen(filename) - 3, ".k7"))
  {
      return MEDIA_TAPE;
  }
  else if (is_fd_file(filename) || is_sap_file(filename))
  {
    return MEDIA_FLOPPY;
  }
  else if (strlen(filename) > 4 && (streq_nocase(filename + strlen(filename) - 4, ".rom")
        || streq_nocase(filename + strlen(filename) - 3, ".m7")
        || streq_nocase(filename + strlen(filename) - 3, ".m5")))
  {
    return MEDIA_CARTRIDGE;
  }
  else
  {
    return NO_MEDIA;
  }
}

static bool find_tape_pattern(char *buffer, int size_buffer, char *pattern, int size_pattern)
{
  int i, k;
  bool found;
  for (i = 0; i <= size_buffer - size_pattern; i++)
  {
    found = true;
    for (k = 0; k < size_pattern; k++)
    {
      if (buffer[i+k] != pattern[k])
      {
        found = false;
        break;
      }
    }
    if (found)
    {
      return true;
    }
  }
  return false;
}

/* Returns true if the first file of the tape has a BAS extension, false otherwise. */
static bool autodetect_tape_first_file_is_basic(const char *filename)
{
  FILE *file;
  char tape_buffer[SIZE_BUFFER_TAPE];

  file = fopen(filename, "rb");
  if (file == NULL)
  {
    LOG_ERROR("Cannot open file %s.\n", filename);
    return false;
  }
  if (fread(tape_buffer, SIZE_BUFFER_TAPE, 1, file) != 1)
  {
    LOG_ERROR("Cannot read file %s.\n", filename);
    fclose(file);
    return false;
  }
  fclose(file);
  return find_tape_pattern(tape_buffer, SIZE_BUFFER_TAPE, TAPE_BASIC_PATTERN1, TAPE_BASIC_PATTERN1_SIZE)
      || find_tape_pattern(tape_buffer, SIZE_BUFFER_TAPE, TAPE_BASIC_PATTERN2, TAPE_BASIC_PATTERN2_SIZE)
      || find_tape_pattern(tape_buffer, SIZE_BUFFER_TAPE, TAPE_BASIC_PATTERN3, TAPE_BASIC_PATTERN3_SIZE);
}

char *autodetect_model(const char *filename)
{
  if ((strstr(filename, "to8d") != NULL) || (strstr(filename, "TO8D") != NULL))
  {
    return "TO8D";
  }
  if ((strstr(filename, "to8") != NULL) || (strstr(filename, "TO8") != NULL))
  {
    return "TO8";
  }
  if ((strstr(filename, "to9p") != NULL) || (strstr(filename, "TO9P") != NULL))
  {
    return "TO9+";
  }
  if ((strstr(filename, "to9") != NULL) || (strstr(filename, "TO9") != NULL))
  {
    return "TO9";
  }
  if ((strstr(filename, "to770") != NULL) || (strstr(filename, "TO770") != NULL))
  {
    return "TO7/70";
  }
  if ((strstr(filename, "to7") != NULL) || (strstr(filename, "TO7") != NULL))
  {
    return "TO7";
  }
  if ((strstr(filename, "mo6") != NULL) || (strstr(filename, "MO6") != NULL))
  {
    return "MO6";
  }
  if ((strstr(filename, "pc128") != NULL) || (strstr(filename, "PC128") != NULL))
  {
    return "PC128";
  }
  if ((strstr(filename, "mo5") != NULL) || (strstr(filename, "MO5") != NULL))
  {
    return "MO5";
  }
  if ((strstr(filename, "memo5") != NULL) || (strstr(filename, "MEMO5") != NULL)
      || (strstr(filename, ".m5") != NULL) || (strstr(filename, ".M5") != NULL))
  {
    return "MO5";
  }
  if ((strstr(filename, "memo7") != NULL) || (strstr(filename, "MEMO7") != NULL)
      || (strstr(filename, ".m7") != NULL) || (strstr(filename, ".M7") != NULL))
  {
    return "TO8";
  }
  return "";
}

static void compute_md5(const char *filename, unsigned char *hash)
{
  FILE *file;
  char buffer[MD5_BUFFER_SIZE];
  unsigned long size_read;
  MD5_CTX ctx;

  MD5_Init(&ctx);

  file = fopen(filename, "rb");
  if (file == NULL)
  {
    LOG_ERROR("Cannot open file %s.\n", filename);
    return;
  }
  while ((size_read = fread(buffer, 1, MD5_BUFFER_SIZE, file)) > 0)
  {
    MD5_Update(&ctx, buffer, size_read);
  }
  fclose(file);
  MD5_Final(hash, &ctx);
}

void autostart_init(const char *filename, bool compute_hash)
{
  currentMedia = get_media_type(filename);
  program_is_basic = autodetect_tape_first_file_is_basic(filename);
  current_autostart_key_pos = -1;
  memset(md5_digest, 0, sizeof(md5_digest));
  if (compute_hash)
  {
    char md5string[2*MD5_LENGTH+1];
    compute_md5(filename, md5_digest);
    for (int i = 0; i < MD5_LENGTH; i++)
    {
      sprintf(&md5string[i*2], "%02x", (unsigned int)md5_digest[i]);
    }
    LOG_DEBUG("MD5 hash of file %s is %s.\n", filename, md5string);
  }
}

static const Key* get_default_autostart_keys(ThomsonModel model, Media media)
{
  const Key* keys = NULL;
  switch (model)
    {
      case MO5:
        if (program_is_basic)
        {
          keys = MO5_AUTOSTART_BASIC_KEYS;
        }
        else
        {
          keys = MO5_AUTOSTART_BIN_KEYS;
        }
        break;
      case MO6:
        if (media == MEDIA_CARTRIDGE)
        {
          keys = CARTRIDGE_AUTOSTART_KEYS;
        }
        else if (program_is_basic)
        {
          keys = MO6_AUTOSTART_BASIC_KEYS;
        }
        else
        {
          keys = MO6_AUTOSTART_BIN_KEYS;
        }
        break;
      case PC128:
        if (media == MEDIA_CARTRIDGE)
        {
          keys = CARTRIDGE_AUTOSTART_KEYS;
        }
        else if (program_is_basic)
        {
          keys = PC128_AUTOSTART_BASIC_KEYS;
        }
        else
        {
          keys = PC128_AUTOSTART_BIN_KEYS;
        }
        break;
      case TO7:
        if (media == MEDIA_CARTRIDGE)
        {
          keys = TO7_CARTRIDGE_AUTOSTART_KEYS;
        }
        else if (program_is_basic)
        {
          keys = TO7_AUTOSTART_BASIC_KEYS;
        }
        else
        {
          keys = TO7_AUTOSTART_BIN_KEYS;
        }
        break;
      case TO7_70:
        if (media == MEDIA_CARTRIDGE)
        {
          keys = TO7_CARTRIDGE_AUTOSTART_KEYS;
        }
        else if (program_is_basic)
        {
          keys = TO770_AUTOSTART_BASIC_KEYS;
        }
        else
        {
          keys = TO770_AUTOSTART_BIN_KEYS;
        }
        break;
      // Most games are started with the 'B' key (Basic 512) on TO8/TO8D/TO9+
      // and the 'D' key (Basic 128) on TO9.
      // Tapes generally requires BASIC 1 ('C' key on TO8/TO8D/TO9+, 'E' key on TO9).
      // Cartridges are started by '0'.
      case TO9:
        if (media == MEDIA_CARTRIDGE)
        {
          keys = CARTRIDGE_AUTOSTART_KEYS;
        }
        else if (media == MEDIA_TAPE)
        {
          keys = BASIC1_TO9_AUTOSTART_KEYS;
        }
        else
        {
          keys = BASIC128_KEYS;
        }
        break;
      default:
        if (media == MEDIA_CARTRIDGE)
        {
          keys = CARTRIDGE_AUTOSTART_KEYS;
        }
        else if (media == MEDIA_TAPE)
        {
          keys = BASIC1_TO8_AUTOSTART_KEYS;
        }
        else
        {
          keys = BASIC512_KEYS;
        }
        break;
    }
  return keys;
}

static const Key* find_specific_autostart_keys(ThomsonModel model)
{
  const Key* keys = NULL;
  int line = 0;
  while (SPECIFIC_START_KEYS_TABLE[line].keys != NULL)
  {
    // Test if we found the game
    if (memcmp(md5_digest, SPECIFIC_START_KEYS_TABLE[line].hash, MD5_LENGTH) == 0)
    {
      // Test if it is the correct model
      if (SPECIFIC_START_KEYS_TABLE[line].model == model)
      {
        keys = SPECIFIC_START_KEYS_TABLE[line].keys;
        LOG_DEBUG("Found matching MD5 in database.\n");
        break;
      }
    }
    line++;
  }
  return keys;
}

static bool is_hash_computed()
{
  int i;
  for (i = 0; i < MD5_LENGTH; i++)
  {
    if (md5_digest[i] != 0)
    {
      return true;
    }
  }
  return false;
}

bool autostart_nextkey()
{
  // First key?
  if (current_autostart_key_pos < 0)
  {
    ThomsonModel model = GetThomsonModel();
    current_autostart_key_pos = 0;
    // First search if it is a known game with a specific start method, and
    // if not use the default start method
    autostart_keys = NULL;
    if (is_hash_computed())
    {
      autostart_keys = find_specific_autostart_keys(model);
    }
    if (autostart_keys == NULL)
    {
      autostart_keys = get_default_autostart_keys(model, currentMedia);
    }
  }

  // No more keys?
  if (autostart_keys[current_autostart_key_pos].retrokey == RETROK_UNKNOWN)
  {
    current_autostart_key_pos = -1;
    return false;
  }

  keyboard(libretroKeyCodeToThomsonScanCode[autostart_keys[current_autostart_key_pos].retrokey],
           autostart_keys[current_autostart_key_pos].down);
  current_autostart_key_pos++;
  return true;
}
