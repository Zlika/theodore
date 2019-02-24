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

/* Auto-detection functions */

#include <stdio.h>
#include <string.h>

#include "autodetect.h"

#define SIZE_BUFFER_TAPE 32
#define TAPE_BASIC_PATTERN1 "BAS\0"
#define TAPE_BASIC_PATTERN1_SIZE 4
#define TAPE_BASIC_PATTERN2 "ENTETE  MO"
#define TAPE_BASIC_PATTERN2_SIZE 10
#define TAPE_BASIC_PATTERN3 "ENTETE  TO"
#define TAPE_BASIC_PATTERN3_SIZE 10

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

bool autodetect_tape_first_file_is_basic(const char *filename)
{
  FILE *file;
  char tape_buffer[SIZE_BUFFER_TAPE];

  file = fopen(filename, "rb");
  if (fread(tape_buffer, SIZE_BUFFER_TAPE, 1, file) != 1)
  {
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
  if ((strstr(filename, "mo5") != NULL) || (strstr(filename, "MO5") != NULL))
  {
    return "MO5";
  }
  return "";
}
