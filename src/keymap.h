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

/* Mapping Thomson <-> PC keyboard and definition of the virtual keyboard */

#ifndef __KEYMAP_H
#define __KEYMAP_H

#include "libretro-common/include/libretro.h"

/* Mapping libretro -> Thomson TO scancodes */
extern const char libretroKeyCodeToThomsonToScanCode[RETROK_LAST];
/* Mapping libretro -> Thomson MO5 scancodes */
extern const char libretroKeyCodeToThomsonMo5ScanCode[RETROK_LAST];
/* Mapping libretro -> Thomson MO6 scancodes */
extern const char libretroKeyCodeToThomsonMo6ScanCode[RETROK_LAST];

/* Mapping libretro -> Thomson scancodes for the current MO/TO version */
extern const char *libretroKeyCodeToThomsonScanCode;

/* Number of keys of the virtual keyboard */
#define VIRTUALKB_NB_KEYS 38
/* Keysyms for the virtual keyboard (AZERTY mode, i.e. all models but PC128) */
extern const int virtualkb_keysyms_azerty[VIRTUALKB_NB_KEYS];
/* Keysyms for the virtual keyboard (QWERTY mode, i.e. only for Olivetti Prodest PC128) */
extern const int virtualkb_keysyms_qwerty[VIRTUALKB_NB_KEYS];
/* Virtual keyboard keysyms for the current computer version */
extern const int *virtualkb_keysyms;
/* Letters of the virtual keyboard */
extern const char* virtualkb_chars[VIRTUALKB_NB_KEYS];

#endif /* __KEYMAP_H */
