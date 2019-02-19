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

#ifndef __AUTODETECT_H
#define __AUTODETECT_H

#include "boolean.h"

/* Returns true if the first file of the tape has a BAS extension, false otherwise. */
bool autodetect_tape_first_file_is_basic(const char *filename);
/* Returns the name of the Thomson model found in the name of the file,
   or an empty string if not found. */
char *autodetect_model(const char *filename);

#endif /* __AUTODETECT_H */
