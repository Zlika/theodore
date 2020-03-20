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

#include "keymap.h"
#include "motoemulator.h"

#define SIZE_BUFFER_TAPE 32
#define TAPE_BASIC_PATTERN1 "BAS\0"
#define TAPE_BASIC_PATTERN1_SIZE 4
#define TAPE_BASIC_PATTERN2 "ENTETE  MO"
#define TAPE_BASIC_PATTERN2_SIZE 10
#define TAPE_BASIC_PATTERN3 "ENTETE  TO"
#define TAPE_BASIC_PATTERN3_SIZE 10

typedef struct { int retrokey; bool down; } Key;

#define MO5_AUTOSTART_BASIC_KEYS_LENGTH 12
// Key strokes to start a BASIC game on MO5: RUN"
static const Key MO5_AUTOSTART_BASIC_KEYS[MO5_AUTOSTART_BASIC_KEYS_LENGTH] =
{ {RETROK_r, true}, {RETROK_r, false}, {RETROK_u, true}, {RETROK_u, false},
  {RETROK_n, true}, {RETROK_n, false}, {RETROK_LSHIFT, true}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false} };
#define MO5_AUTOSTART_BIN_KEYS_LENGTH 24
// Key strokes to start a BINARY game on MO5: LOADM"",,R
static const Key MO5_AUTOSTART_BIN_KEYS[MO5_AUTOSTART_BIN_KEYS_LENGTH] =
{ {RETROK_l, true}, {RETROK_l, false}, {RETROK_o, true}, {RETROK_o, false},
  {RETROK_q, true}, {RETROK_q, false}, {RETROK_d, true}, {RETROK_d, false},
  {RETROK_SEMICOLON, true}, {RETROK_SEMICOLON, false},
  {RETROK_LSHIFT, true}, {RETROK_2, true}, {RETROK_2, false}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, false}, {RETROK_m, true}, {RETROK_m, false}, {RETROK_m, true}, {RETROK_m, false},
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false} };
#define MO6_AUTOSTART_BASIC_KEYS_LENGTH 15+14*4
// Key strokes to start a BASIC game on MO6: 2 + RUN"
static const Key MO6_AUTOSTART_BASIC_KEYS[MO6_AUTOSTART_BASIC_KEYS_LENGTH] =
{ {RETROK_2, true}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_u, true}, {RETROK_u, false},
  {RETROK_n, true}, {RETROK_n, false}, {RETROK_LSHIFT, true}, {RETROK_3, true}, {RETROK_3, false},
  {RETROK_LSHIFT, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false} };
#define MO6_AUTOSTART_BIN_KEYS_LENGTH 27+14*4
// Key strokes to start a BINARY game on MO6: 2 + LOADM"",,R
static const Key MO6_AUTOSTART_BIN_KEYS[MO6_AUTOSTART_BIN_KEYS_LENGTH] =
{ {RETROK_2, true}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_l, true}, {RETROK_l, false}, {RETROK_o, true}, {RETROK_o, false},
  {RETROK_q, true}, {RETROK_q, false}, {RETROK_d, true}, {RETROK_d, false},
  {RETROK_SEMICOLON, true}, {RETROK_SEMICOLON, false},
  {RETROK_LSHIFT, true}, {RETROK_3, true}, {RETROK_3, false}, {RETROK_3, true}, {RETROK_3, false},
  {RETROK_LSHIFT, false}, {RETROK_m, true}, {RETROK_m, false}, {RETROK_m, true}, {RETROK_m, false},
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false} };
// Key strokes to start a BASIC game on PC128: 2 + RUN"
static const Key PC128_AUTOSTART_BASIC_KEYS[MO6_AUTOSTART_BASIC_KEYS_LENGTH] =
{ {RETROK_2, true}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_u, true}, {RETROK_u, false},
  {RETROK_n, true}, {RETROK_n, false}, {RETROK_LSHIFT, true}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false} };
// Key strokes to start a BINARY game on PC128: 2 + LOADM"",,R
static const Key PC128_AUTOSTART_BIN_KEYS[MO6_AUTOSTART_BIN_KEYS_LENGTH] =
{ {RETROK_2, true}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_l, true}, {RETROK_l, false}, {RETROK_o, true}, {RETROK_o, false},
  {RETROK_a, true}, {RETROK_a, false}, {RETROK_d, true}, {RETROK_d, false},
  {RETROK_m, true}, {RETROK_m, false},
  {RETROK_LSHIFT, true}, {RETROK_2, true}, {RETROK_2, false}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, false}, {RETROK_COMMA, true}, {RETROK_COMMA, false}, {RETROK_COMMA, true}, {RETROK_COMMA, false},
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false} };
#define BASIC512_AUTOSTART_KEYS_LENGTH 2
static const Key BASIC512_KEYS[BASIC512_AUTOSTART_KEYS_LENGTH] = { {RETROK_b, true}, {RETROK_b, false} };
#define BASIC128_AUTOSTART_KEYS_LENGTH 2
static const Key BASIC128_KEYS[BASIC128_AUTOSTART_KEYS_LENGTH] = { {RETROK_d, true}, {RETROK_d, false} };
#define BASIC1_TO8_AUTOSTART_KEYS_LENGTH 2
static const Key BASIC1_TO8_AUTOSTART_KEYS[BASIC1_TO8_AUTOSTART_KEYS_LENGTH] = { {RETROK_c, true}, {RETROK_c, false} };
#define BASIC1_TO9_AUTOSTART_KEYS_LENGTH 2
static const Key BASIC1_TO9_AUTOSTART_KEYS[BASIC1_TO9_AUTOSTART_KEYS_LENGTH] = { {RETROK_e, true}, {RETROK_e, false} };
#define CARTRIDGE_AUTOSTART_KEYS_LENGTH 2
static const Key CARTRIDGE_AUTOSTART_KEYS[CARTRIDGE_AUTOSTART_KEYS_LENGTH] = { {RETROK_KP0, true}, {RETROK_KP0, false} };
#define TO7_CARTRIDGE_AUTOSTART_KEYS_LENGTH 2
static const Key TO7_CARTRIDGE_AUTOSTART_KEYS[TO7_CARTRIDGE_AUTOSTART_KEYS_LENGTH] = { {RETROK_KP1, true}, {RETROK_KP1, false} };
#define TO7_AUTOSTART_BASIC_KEYS_LENGTH 15+14*4
// Key strokes to start a BASIC game on TO7: 1 + RUN"
static const Key TO7_AUTOSTART_BASIC_KEYS[TO7_AUTOSTART_BASIC_KEYS_LENGTH] =
{ {RETROK_1, true}, {RETROK_1, true}, {RETROK_1, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_u, true}, {RETROK_u, false},
  {RETROK_n, true}, {RETROK_n, false}, {RETROK_LSHIFT, true}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false} };
#define TO7_AUTOSTART_BIN_KEYS_LENGTH 27+14*4
// Key strokes to start a BINARY game on TO7: 1 + LOADM"",,R
static const Key TO7_AUTOSTART_BIN_KEYS[TO7_AUTOSTART_BIN_KEYS_LENGTH] =
{ {RETROK_1, true}, {RETROK_1, true}, {RETROK_1, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_l, true}, {RETROK_l, false}, {RETROK_o, true}, {RETROK_o, false},
  {RETROK_q, true}, {RETROK_q, false}, {RETROK_d, true}, {RETROK_d, false},
  {RETROK_SEMICOLON, true}, {RETROK_SEMICOLON, false},
  {RETROK_LSHIFT, true}, {RETROK_2, true}, {RETROK_2, false}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, false}, {RETROK_m, true}, {RETROK_m, false}, {RETROK_m, true}, {RETROK_m, false},
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false} };
#define TO770_AUTOSTART_BASIC_KEYS_LENGTH 15+14*13
// Key strokes to start a BASIC game on TO7/70: 1 + RUN" (same as TO7 but longer wait to start BASIC 128)
static const Key TO770_AUTOSTART_BASIC_KEYS[TO770_AUTOSTART_BASIC_KEYS_LENGTH] =
{ {RETROK_1, true}, {RETROK_1, true}, {RETROK_1, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_u, true}, {RETROK_u, false},
  {RETROK_n, true}, {RETROK_n, false}, {RETROK_LSHIFT, true}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false} };
#define TO770_AUTOSTART_BIN_KEYS_LENGTH 27+14*13
// Key strokes to start a BINARY game on TO7: 1 + LOADM"",,R (same as TO7 but longer wait to start BASIC 128)
static const Key TO770_AUTOSTART_BIN_KEYS[TO770_AUTOSTART_BIN_KEYS_LENGTH] =
{ {RETROK_1, true}, {RETROK_1, true}, {RETROK_1, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_l, true}, {RETROK_l, false}, {RETROK_o, true}, {RETROK_o, false},
  {RETROK_q, true}, {RETROK_q, false}, {RETROK_d, true}, {RETROK_d, false},
  {RETROK_SEMICOLON, true}, {RETROK_SEMICOLON, false},
  {RETROK_LSHIFT, true}, {RETROK_2, true}, {RETROK_2, false}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, false}, {RETROK_m, true}, {RETROK_m, false}, {RETROK_m, true}, {RETROK_m, false},
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false} };

static int autostart_keys_length = 0;
static const Key *autostart_keys = NULL;
static int current_autostart_key_pos = -1;

static Media currentMedia = NO_MEDIA;
static bool program_is_basic = true;

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

void autostart_init(const char *filename)
{
  currentMedia = get_media_type(filename);
  program_is_basic = autodetect_tape_first_file_is_basic(filename);
  current_autostart_key_pos = -1;
}

bool autostart_nextkey()
{
  if (current_autostart_key_pos < 0)
  {
    current_autostart_key_pos = 0;
    switch (GetThomsonModel())
    {
      case MO5:
        if (program_is_basic)
        {
          autostart_keys = MO5_AUTOSTART_BASIC_KEYS;
          autostart_keys_length = MO5_AUTOSTART_BASIC_KEYS_LENGTH;
        }
        else
        {
          autostart_keys = MO5_AUTOSTART_BIN_KEYS;
          autostart_keys_length = MO5_AUTOSTART_BIN_KEYS_LENGTH;
        }
        break;
      case MO6:
        if (currentMedia == MEDIA_CARTRIDGE)
        {
          autostart_keys = CARTRIDGE_AUTOSTART_KEYS;
          autostart_keys_length = CARTRIDGE_AUTOSTART_KEYS_LENGTH;
        }
        else if (program_is_basic)
        {
          autostart_keys = MO6_AUTOSTART_BASIC_KEYS;
          autostart_keys_length = MO6_AUTOSTART_BASIC_KEYS_LENGTH;
        }
        else
        {
          autostart_keys = MO6_AUTOSTART_BIN_KEYS;
          autostart_keys_length = MO6_AUTOSTART_BIN_KEYS_LENGTH;
        }
        break;
      case PC128:
        if (currentMedia == MEDIA_CARTRIDGE)
        {
          autostart_keys = CARTRIDGE_AUTOSTART_KEYS;
          autostart_keys_length = CARTRIDGE_AUTOSTART_KEYS_LENGTH;
        }
        else if (program_is_basic)
        {
          autostart_keys = PC128_AUTOSTART_BASIC_KEYS;
          autostart_keys_length = MO6_AUTOSTART_BASIC_KEYS_LENGTH;
        }
        else
        {
          autostart_keys = PC128_AUTOSTART_BIN_KEYS;
          autostart_keys_length = MO6_AUTOSTART_BIN_KEYS_LENGTH;
        }
        break;
      case TO7:
        if (currentMedia == MEDIA_CARTRIDGE)
        {
          autostart_keys = TO7_CARTRIDGE_AUTOSTART_KEYS;
          autostart_keys_length = TO7_CARTRIDGE_AUTOSTART_KEYS_LENGTH;
        }
        else if (program_is_basic)
        {
          autostart_keys = TO7_AUTOSTART_BASIC_KEYS;
          autostart_keys_length = TO7_AUTOSTART_BASIC_KEYS_LENGTH;
        }
        else
        {
          autostart_keys = TO7_AUTOSTART_BIN_KEYS;
          autostart_keys_length = TO7_AUTOSTART_BIN_KEYS_LENGTH;
        }
        break;
      case TO7_70:
        if (currentMedia == MEDIA_CARTRIDGE)
        {
          autostart_keys = TO7_CARTRIDGE_AUTOSTART_KEYS;
          autostart_keys_length = TO7_CARTRIDGE_AUTOSTART_KEYS_LENGTH;
        }
        else if (program_is_basic)
        {
          autostart_keys = TO770_AUTOSTART_BASIC_KEYS;
          autostart_keys_length = TO770_AUTOSTART_BASIC_KEYS_LENGTH;
        }
        else
        {
          autostart_keys = TO770_AUTOSTART_BIN_KEYS;
          autostart_keys_length = TO770_AUTOSTART_BIN_KEYS_LENGTH;
        }
        break;
      // Most games are started with the 'B' key (Basic 512) on TO8/TO8D/TO9+
      // and the 'D' key (Basic 128) on TO9.
      // Tapes generally requires BASIC 1 ('C' key on TO8/TO8D/TO9+, 'E' key on TO9).
      // Cartridges are started by '0'.
      case TO9:
        if (currentMedia == MEDIA_CARTRIDGE)
        {
          autostart_keys = CARTRIDGE_AUTOSTART_KEYS;
          autostart_keys_length = CARTRIDGE_AUTOSTART_KEYS_LENGTH;
        }
        else if (currentMedia == MEDIA_TAPE)
        {
          autostart_keys = BASIC1_TO9_AUTOSTART_KEYS;
          autostart_keys_length = BASIC1_TO9_AUTOSTART_KEYS_LENGTH;
        }
        else
        {
          autostart_keys = BASIC128_KEYS;
          autostart_keys_length = BASIC128_AUTOSTART_KEYS_LENGTH;
        }
        break;
      default:
        if (currentMedia == MEDIA_CARTRIDGE)
        {
          autostart_keys = CARTRIDGE_AUTOSTART_KEYS;
          autostart_keys_length = CARTRIDGE_AUTOSTART_KEYS_LENGTH;
        }
        else if (currentMedia == MEDIA_TAPE)
        {
          autostart_keys = BASIC1_TO8_AUTOSTART_KEYS;
          autostart_keys_length = BASIC1_TO8_AUTOSTART_KEYS_LENGTH;
        }
        else
        {
          autostart_keys = BASIC512_KEYS;
          autostart_keys_length = BASIC512_AUTOSTART_KEYS_LENGTH;
        }
        break;
    }
  }

  if (current_autostart_key_pos >= autostart_keys_length)
  {
    current_autostart_key_pos = -1;
    return false;
  }

  keyboard(libretroKeyCodeToThomsonScanCode[autostart_keys[current_autostart_key_pos].retrokey],
           autostart_keys[current_autostart_key_pos].down);
  current_autostart_key_pos++;
  return true;
}
