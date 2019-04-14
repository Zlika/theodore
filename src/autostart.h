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

#ifndef __AUTOSTART_H
#define __AUTOSTART_H

#include "boolean.h"

/* Kind of media inserted. */
typedef enum { NO_MEDIA, MEDIA_FLOPPY, MEDIA_TAPE, MEDIA_CARTRIDGE } Media;

/* Returns the kind of media represented by the given filename. */
Media get_media_type(const char *filename);
/** Returns true if the given filename is an SAP file. */
bool is_sap_file(const char *filename);
/* Returns the name of the Thomson model found in the name of the file,
   or an empty string if not found. */
char *autodetect_model(const char *filename);

/* Initialise the autostart feature.
 * This function must be called when a file/game is loaded. */
void autostart_init(const char *filename);
/* This function must be called once per call of input_poll_cb() until it returns false
 * to simulate the keystrokes needed to start the currently loaded media. */
bool autostart_nextkey();

#endif /* __AUTOSTART_H */
