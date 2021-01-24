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

/* Commands used to start games */

#include "libretro-common/include/libretro.h"
// For ThomsonModel
#include "motoemulator.h"

typedef struct { int retrokey; bool down; } Key;

// Key strokes to start a BASIC game on MO5: RUN"
static const Key MO5_AUTOSTART_BASIC_KEYS[] =
{ {RETROK_r, true}, {RETROK_r, false}, {RETROK_u, true}, {RETROK_u, false},
  {RETROK_n, true}, {RETROK_n, false}, {RETROK_LSHIFT, true}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false}, {RETROK_UNKNOWN, false} };

// Key strokes to start a BINARY game on MO5: LOADM"",,R
static const Key MO5_AUTOSTART_BIN_KEYS[] =
{ {RETROK_l, true}, {RETROK_l, false}, {RETROK_o, true}, {RETROK_o, false},
  {RETROK_q, true}, {RETROK_q, false}, {RETROK_d, true}, {RETROK_d, false},
  {RETROK_SEMICOLON, true}, {RETROK_SEMICOLON, false},
  {RETROK_LSHIFT, true}, {RETROK_2, true}, {RETROK_2, false}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, false}, {RETROK_m, true}, {RETROK_m, false}, {RETROK_m, true}, {RETROK_m, false},
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false}, {RETROK_UNKNOWN, false} };

// Key strokes to start a BASIC game on MO6: 2 + RUN"
static const Key MO6_AUTOSTART_BASIC_KEYS[] =
{ {RETROK_2, true}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_u, true}, {RETROK_u, false},
  {RETROK_n, true}, {RETROK_n, false}, {RETROK_LSHIFT, true}, {RETROK_3, true}, {RETROK_3, false},
  {RETROK_LSHIFT, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false}, {RETROK_UNKNOWN, false} };

// Key strokes to start a BINARY game on MO6: 2 + LOADM"",,R
static const Key MO6_AUTOSTART_BIN_KEYS[] =
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
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false}, {RETROK_UNKNOWN, false} };

// Key strokes to start a BASIC game on PC128: 2 + RUN"
static const Key PC128_AUTOSTART_BASIC_KEYS[] =
{ {RETROK_2, true}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_u, true}, {RETROK_u, false},
  {RETROK_n, true}, {RETROK_n, false}, {RETROK_LSHIFT, true}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false}, {RETROK_UNKNOWN, false} };

// Key strokes to start a BINARY game on PC128: 2 + LOADM"",,R
static const Key PC128_AUTOSTART_BIN_KEYS[] =
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
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false}, {RETROK_UNKNOWN, false} };

static const Key BASIC512_KEYS[] = { {RETROK_b, true}, {RETROK_b, false}, {RETROK_UNKNOWN, false} };
static const Key BASIC128_KEYS[] = { {RETROK_d, true}, {RETROK_d, false}, {RETROK_UNKNOWN, false} };
static const Key BASIC1_TO8_AUTOSTART_KEYS[] = { {RETROK_c, true}, {RETROK_c, false}, {RETROK_UNKNOWN, false} };
static const Key BASIC1_TO9_AUTOSTART_KEYS[] = { {RETROK_e, true}, {RETROK_e, false}, {RETROK_UNKNOWN, false} };
static const Key CARTRIDGE_AUTOSTART_KEYS[] = { {RETROK_KP0, true}, {RETROK_KP0, false}, {RETROK_UNKNOWN, false} };
static const Key TO7_CARTRIDGE_AUTOSTART_KEYS[] = { {RETROK_KP1, true}, {RETROK_KP1, false}, {RETROK_UNKNOWN, false} };

// Key strokes to start a BASIC game on TO7: 1 + RUN"
static const Key TO7_AUTOSTART_BASIC_KEYS[] =
{ {RETROK_1, true}, {RETROK_1, true}, {RETROK_1, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_u, true}, {RETROK_u, false},
  {RETROK_n, true}, {RETROK_n, false}, {RETROK_LSHIFT, true}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false}, {RETROK_UNKNOWN, false} };

// Key strokes to start a BINARY game on TO7: 1 + LOADM"",,R
static const Key TO7_AUTOSTART_BIN_KEYS[] =
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
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false}, {RETROK_UNKNOWN, false} };

// Key strokes to start a BASIC game on TO7/70: 1 + RUN" (same as TO7 but longer wait to start BASIC 128)
static const Key TO770_AUTOSTART_BASIC_KEYS[] =
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
  {RETROK_LSHIFT, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false}, {RETROK_UNKNOWN, false} };

// Key strokes to start a BINARY game on TO7: 1 + LOADM"",,R (same as TO7 but longer wait to start BASIC 128)
static const Key TO770_AUTOSTART_BIN_KEYS[] =
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
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false}, {RETROK_UNKNOWN, false} };

/* Start keys for specific games */
typedef struct { const unsigned char hash[16]; const ThomsonModel model; const Key *keys;} SpecificStartKeys;

/* Key strokes to start a specific game (TO8/TO9+) with: 1 + RUN"JEU + 2*/
static const Key SPECIFIC_AUTOSTART_TO8_RUNJEU_KEYS[] =
{ {RETROK_LSHIFT, true}, {RETROK_1, true}, {RETROK_1, true}, {RETROK_1, false}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_u, true}, {RETROK_u, false},
  {RETROK_n, true}, {RETROK_n, false}, {RETROK_3, true}, {RETROK_3, false},
  {RETROK_j, true}, {RETROK_j, false}, {RETROK_e, true}, {RETROK_e, false},
  {RETROK_u, true}, {RETROK_u, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_2, true}, {RETROK_2, true}, {RETROK_2, false}, {RETROK_LSHIFT, false},
  {RETROK_UNKNOWN, false} };
/* Key strokes to start a specific game (MO6) with: 1 + RUN"JEU + 1 */
static const Key SPECIFIC_AUTOSTART_MO6_RUNJEU_KEYS[] =
{ {RETROK_1, true}, {RETROK_1, true}, {RETROK_1, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_u, true}, {RETROK_u, false},
  {RETROK_n, true}, {RETROK_n, false},
  {RETROK_LSHIFT, true}, {RETROK_3, true}, {RETROK_3, false}, {RETROK_LSHIFT, false},
  {RETROK_j, true}, {RETROK_j, false}, {RETROK_e, true}, {RETROK_e, false},
  {RETROK_u, true}, {RETROK_u, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_1, true}, {RETROK_1, false},
  {RETROK_UNKNOWN, false} };
/* Key strokes to start a specific game (PC128) with: 1 + RUN"JEU + 1 */
static const Key SPECIFIC_AUTOSTART_PC128_RUNJEU_KEYS[] =
{ {RETROK_1, true}, {RETROK_1, true}, {RETROK_1, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_r, true}, {RETROK_r, false}, {RETROK_u, true}, {RETROK_u, false},
  {RETROK_n, true}, {RETROK_n, false},
  {RETROK_LSHIFT, true}, {RETROK_2, true}, {RETROK_2, false}, {RETROK_LSHIFT, false},
  {RETROK_j, true}, {RETROK_j, false}, {RETROK_e, true}, {RETROK_e, false},
  {RETROK_u, true}, {RETROK_u, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_1, true}, {RETROK_1, false},
  {RETROK_UNKNOWN, false} };
/* Key strokes to start a specific game (TO9) with: 3 + RUN"JEU + 2 */
static const Key SPECIFIC_AUTOSTART_TO9_RUNJEU_KEYS[] =
{ {RETROK_LSHIFT, true}, {RETROK_3, true}, {RETROK_3, true}, {RETROK_3, false}, {RETROK_LSHIFT, false},
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
  {RETROK_n, true}, {RETROK_n, false}, {RETROK_3, true}, {RETROK_3, false},
  {RETROK_j, true}, {RETROK_j, false}, {RETROK_e, true}, {RETROK_e, false},
  {RETROK_u, true}, {RETROK_u, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false},
  {RETROK_LSHIFT, true}, {RETROK_2, true}, {RETROK_2, true}, {RETROK_2, false}, {RETROK_LSHIFT, false},
  {RETROK_UNKNOWN, false} };
/* Key strokes to start a MO5 game with LOADM */
static const Key SPECIFIC_AUTOSTART_MO5_LOADM[] =
{ {RETROK_l, true}, {RETROK_l, false}, {RETROK_o, true}, {RETROK_o, false},
  {RETROK_q, true}, {RETROK_q, false}, {RETROK_d, true}, {RETROK_d, false},
  {RETROK_SEMICOLON, true}, {RETROK_SEMICOLON, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false},
  {RETROK_UNKNOWN, false} };
/* Key strokes to start a MO6 game with 2 + LOADM */
static const Key SPECIFIC_AUTOSTART_MO6_LOADM[] =
{ {RETROK_2, true}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_l, true}, {RETROK_l, false}, {RETROK_o, true}, {RETROK_o, false},
  {RETROK_q, true}, {RETROK_q, false}, {RETROK_d, true}, {RETROK_d, false},
  {RETROK_SEMICOLON, true}, {RETROK_SEMICOLON, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false},
  {RETROK_UNKNOWN, false} };
/* Key strokes to start a PC128 game with 2 + LOADM */
static const Key SPECIFIC_AUTOSTART_PC128_LOADM[] =
{ {RETROK_2, true}, {RETROK_2, true}, {RETROK_2, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_l, true}, {RETROK_l, false}, {RETROK_o, true}, {RETROK_o, false},
  {RETROK_a, true}, {RETROK_a, false}, {RETROK_d, true}, {RETROK_d, false},
  {RETROK_m, true}, {RETROK_m, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false},
  {RETROK_UNKNOWN, false} };
/* Key strokes to start a TO7 game with LOADM */
static const Key SPECIFIC_AUTOSTART_TO7_LOADM[] =
{ {RETROK_1, true}, {RETROK_1, true}, {RETROK_1, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false}, {RETROK_LSHIFT, true}, {RETROK_LSHIFT, false},
  {RETROK_l, true}, {RETROK_l, false}, {RETROK_o, true}, {RETROK_o, false},
  {RETROK_q, true}, {RETROK_q, false}, {RETROK_d, true}, {RETROK_d, false},
  {RETROK_SEMICOLON, true}, {RETROK_SEMICOLON, false}, {RETROK_RETURN, true}, {RETROK_RETURN, false},
  {RETROK_UNKNOWN, false} };

static const SpecificStartKeys SPECIFIC_START_KEYS_TABLE[] =
{
    // 20000-avant-jc_mo6to8.fd
    { { 0xac, 0x52, 0x09, 0x23, 0x9b, 0x4c, 0xe0, 0xf1, 0x96, 0xbb, 0x89, 0x28, 0xbc, 0x46, 0xa3, 0xd5 },
      MO6, SPECIFIC_AUTOSTART_MO6_RUNJEU_KEYS },
    { { 0xac, 0x52, 0x09, 0x23, 0x9b, 0x4c, 0xe0, 0xf1, 0x96, 0xbb, 0x89, 0x28, 0xbc, 0x46, 0xa3, 0xd5 },
      PC128, SPECIFIC_AUTOSTART_PC128_RUNJEU_KEYS },
    { { 0xac, 0x52, 0x09, 0x23, 0x9b, 0x4c, 0xe0, 0xf1, 0x96, 0xbb, 0x89, 0x28, 0xbc, 0x46, 0xa3, 0xd5 },
      TO8, SPECIFIC_AUTOSTART_TO8_RUNJEU_KEYS },
    { { 0xac, 0x52, 0x09, 0x23, 0x9b, 0x4c, 0xe0, 0xf1, 0x96, 0xbb, 0x89, 0x28, 0xbc, 0x46, 0xa3, 0xd5 },
      TO8D, SPECIFIC_AUTOSTART_TO8_RUNJEU_KEYS },
    { { 0xac, 0x52, 0x09, 0x23, 0x9b, 0x4c, 0xe0, 0xf1, 0x96, 0xbb, 0x89, 0x28, 0xbc, 0x46, 0xa3, 0xd5 },
      TO9, SPECIFIC_AUTOSTART_TO9_RUNJEU_KEYS },
    { { 0xac, 0x52, 0x09, 0x23, 0x9b, 0x4c, 0xe0, 0xf1, 0x96, 0xbb, 0x89, 0x28, 0xbc, 0x46, 0xa3, 0xd5 },
      TO9P, SPECIFIC_AUTOSTART_TO8_RUNJEU_KEYS },
    // le-5eme-axe_mo5.k7
    { { 0x73, 0xee, 0xe3, 0x4f, 0x36, 0xc2, 0x13, 0xa3, 0xef, 0x50, 0x09, 0x49, 0xab, 0xed, 0x75, 0x7a },
      MO5, SPECIFIC_AUTOSTART_MO5_LOADM },
    // le-5eme-axe_mo6.k7
    { { 0xd2, 0x94, 0x8a, 0x9f, 0x4b, 0x7d, 0xf2, 0x8d, 0x60, 0x34, 0x87, 0xdb, 0x63, 0xf3, 0xf1, 0x4d },
      MO6, SPECIFIC_AUTOSTART_MO6_LOADM },
    { { 0xd2, 0x94, 0x8a, 0x9f, 0x4b, 0x7d, 0xf2, 0x8d, 0x60, 0x34, 0x87, 0xdb, 0x63, 0xf3, 0xf1, 0x4d },
      PC128, SPECIFIC_AUTOSTART_PC128_LOADM },
    // le-5eme-axe_to7.k7
    { { 0xc7, 0xa0, 0x85, 0x11, 0x0b, 0xed, 0x38, 0x87, 0x33, 0x65, 0xd2, 0x2c, 0xc7, 0x75, 0xfa, 0x9f },
      TO7, SPECIFIC_AUTOSTART_TO7_LOADM },
    { { 0xc7, 0xa0, 0x85, 0x11, 0x0b, 0xed, 0x38, 0x87, 0x33, 0x65, 0xd2, 0x2c, 0xc7, 0x75, 0xfa, 0x9f },
      TO7_70, SPECIFIC_AUTOSTART_TO7_LOADM },
    // End of table
    { { 0 }, TO8, NULL }
};
