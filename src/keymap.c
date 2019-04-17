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

#include "keymap.h"

// Thomson <-> PC keyboard mapping for special keys
// STOP <-> TAB
// CNT <-> CTRL
// CAPSLOCK <-> CAPSLOCK
// ACC <-> BACKSPACE
// HOME <-> HOME
// Arrows <-> arrows
// INS <-> INSERT
// EFF <-> DEL
// RAZ <-> ALT
// F1-F5 <-> F1-F5
// F6-F10 <-> SHIFT+F1-F5
// Yellow key (MO5) <-> Left SHIFT
// BASIC (MO5) <-> Right SHIFT

/* Mapping libretro -> Thomson TO scancodes */
const char libretroKeyCodeToThomsonToScanCode[RETROK_LAST] =
{
    -1,    /* 0 */  -1,    /* 1 */  -1,    /* 2 */  -1,    /* 3 */  -1,    /* 4 */  -1,    /* 5 */  -1,    /* 6 */  -1,    /* 7 */
  0x14,    /* RETROK_BACKSPACE = 8 (ACC on Thomson keyboard) */
  0x30,    /* RETROK_TAB = 9 (STOP on Thomson keyboard) */
    -1,    /* 10 */ -1,    /* 11 */
    -1,    /* RETROK_CLEAR = 12 */
  0x46,    /* RETROK_RETURN = 13 */
    -1,    /* 14 */ -1,    /* 15 */ -1,    /* 16 */ -1,    /* 17 */ -1,    /* 18 */
    -1,    /* RETROK_PAUSE = 19 */
    -1,    /* 20 */ -1,    /* 21 */ -1,    /* 22 */ -1,    /* 23 */ -1,    /* 24 */ -1,    /* 25 */ -1,    /* 26 */
    -1,    /* RETROK_ESCAPE = 27 */
    -1,    /* 28 */ -1,    /* 29 */ -1,    /* 30 */ -1,    /* 31 */
  0x34,    /* RETROK_SPACE = 32 */
    -1,    /* RETROK_EXCLAIM = 33 */
    -1,    /* RETROK_QUOTEDBL = 34 */
    -1,    /* RETROK_HASH = 35 */
  0x3c,    /* RETROK_DOLLAR = 36 */
    -1,    /* 37 */
  0x3c,    /* RETROK_AMPERSAND = 38 */
  0x45,    /* RETROK_QUOTE = 39 (ù on Thomson keyboard) */
  0x09,    /* RETROK_LEFTPAREN = 40 */
  0x4c,    /* RETROK_RIGHTPAREN = 41 */
  0x29,    /* RETROK_ASTERISK = 42 */
  0x0c,    /* RETROK_PLUS = 43 */
  0x3f,    /* RETROK_COMMA = 44 */
  0x4c,    /* RETROK_MINUS = 45 */
  0x47,    /* RETROK_PERIOD = 46 (: on Thomson keyboard) */
  0x4f,    /* RETROK_SLASH = 47 (> on Thomson keyboard) */
  0x49,    /* RETROK_0 = 48 */
  0x29,    /* RETROK_1 = 49 */
  0x21,    /* RETROK_2 = 50 */
  0x19,    /* RETROK_3 = 51 */
  0x11,    /* RETROK_4 = 52 */
  0x09,    /* RETROK_5 = 53 */
  0x01,    /* RETROK_6 = 54 */
  0x31,    /* RETROK_7 = 55 */
  0x39,    /* RETROK_8 = 56 */
  0x41,    /* RETROK_9 = 57 */
  0x47,    /* RETROK_COLON = 58 */
  0x4b,    /* RETROK_SEMICOLON = 59 (M on Thomson keyboard) */
  0x4f,    /* RETROK_LESS = 60 */
  0x0c,    /* RETROK_EQUALS = 61 */
  0x4f,    /* RETROK_GREATER  = 62 */
  0x37,    /* RETROK_QUESTION = 63 */
    -1,    /* RETROK_AT = 64 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 65 - 77 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 78 - 90 */
  0x4d,    /* RETROK_LEFTBRACKET = 91 (^ on Thomson keyboard) */
  0x3e,    /* RETROK_BACKSLASH = 92 (] on Thomson keyboard) */
  0x3c,    /* RETROK_RIGHTBRACKET = 93 ($ on Thomson keyboard) */
  0x44,    /* RETROK_CARET = 94 */
  0x01,    /* RETROK_UNDERSCORE = 95 */
  0x28,    /* RETROK_BACKQUOTE = 96 */
  0x2b,    /* A = 97 */
  0x0f,    /* B = 98 */
  0x1f,    /* C = 99 */
  0x1b,    /* D = 100 */
  0x1a,    /* E = 101 */
  0x13,    /* F = 102 */
  0x0b,    /* G = 103 */
  0x03,    /* H = 104 */
  0x3a,    /* I = 105 */
  0x33,    /* J = 106 */
  0x3b,    /* K = 107 */
  0x43,    /* L = 108 */
  0x37,    /* M = 109 (, on Thomson keyboard) */
  0x07,    /* N = 110 */
  0x42,    /* O = 111 */
  0x4a,    /* P = 112 */
  0x2a,    /* Q = 113 */
  0x12,    /* R = 114 */
  0x23,    /* S = 115 */
  0x0a,    /* T = 116 */
  0x32,    /* U = 117 */
  0x17,    /* V = 118 */
  0x22,    /* W = 119 */
  0x27,    /* X = 120 */
  0x02,    /* Y = 121 */
  0x2f,    /* Z = 122 */
    -1,    /* RETROK_LEFTBRACE = 123 */
    -1,    /* RETROK_BAR = 124 */
    -1,    /* RETROK_RIGHTBRACE = 125 */
    -1,    /* RETROK_TILDE = 126 */
  0x16,    /* RETROK_DELETE = 127 */
    /* End of ASCII mapped keycodes */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 128-143 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 144-159 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 160-175 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 176-191 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 192-207 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 208-223 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 224-239 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 240-255 */
    /* Numeric keypad: */
  0x1e,    /* RETROK_KP0 = 256 */
  0x15,    /* RETROK_KP1 = 257 */
  0x25,    /* RETROK_KP2 = 258 */
  0x4e,    /* RETROK_KP3 = 259 */
  0x1d,    /* RETROK_KP4 = 260 */
  0x2d,    /* RETROK_KP5 = 261 */
  0x2e,    /* RETROK_KP6 = 262 */
  0x1c,    /* RETROK_KP7 = 263 */
  0x24,    /* RETROK_KP8 = 264 */
  0x35,    /* RETROK_KP9 = 265 */
  0x26,    /* RETROK_KP_PERIOD = 266 */
    -1,    /* RETROK_KP_DIVIDE = 267 */
    -1,    /* RETROK_KP_MULTIPLY = 268 */
    -1,    /* RETROK_KP_MINUS = 269 */
    -1,    /* RETROK_KP_PLUS = 270 */
  0x36,    /* RETROK_KP_ENTER = 271 */
    -1,    /* RETROK_KP_EQUALS = 272 */
    /* Arrows + Home/End pad */
  0x04,    /* RETROK_UP = 273 */
  0x3d,    /* RETROK_DOWN = 274 */
  0x05,    /* RETROK_RIGHT = 275 */
  0x0d,    /* RETROK_LEFT = 276 */
  0x0e,    /* RETROK_INSERT = 277 */
  0x06,    /* RETROK_HOME = 278 */
    -1,    /* RETROK_END = 279 */
    -1,    /* RETROK_PAGEUP = 280 */
    -1,    /* RETROK_PAGEDOWN = 281 */
    /* Function keys */
  0x20,    /* RETROK_F1 = 282 */
  0x00,    /* RETROK_F2 = 283 */
  0x08,    /* RETROK_F3 = 284 */
  0x10,    /* RETROK_F4 = 285 */
  0x18,    /* RETROK_F5 = 286 */
  0x20,    /* RETROK_F6 = 287 */
  0x00,    /* RETROK_F7 = 288 */
  0x08,    /* RETROK_F8 = 289 */
  0x10,    /* RETROK_F9 = 290 */
  0x18,    /* RETROK_F10 = 291 */
    -1,    /* RETROK_F11 = 292 */
    -1,    /* RETROK_F12 = 293 */
    -1,    /* RETROK_F13 = 294 */
    -1,    /* RETROK_F14 = 295 */
    -1,    /* RETROK_F15 = 296 */
    -1,    /* 297 */  -1,      /* 298 */  -1,      /* 299 */
    /* Key state modifier keys */
    -1,    /* RETROK_NUMLOCK = 300 */
  0x50,    /* RETROK_CAPSLOCK = 301 */
    -1,    /* RETROK_SCROLLOCK = 302 */
  0x52,    /* RETROK_RSHIFT = 303 */
  0x51,    /* RETROK_LSHIFT = 304 */
  0x53,    /* RETROK_RCTRL = 305 */
  0x53,    /* RETROK_LCTRL = 306 */
  0x06,    /* RETROK_RALT = 307 */
  0x06,    /* RETROK_LALT = 308 (RAZ on Thomson keyboard) */
    -1,    /* RETROK_RMETA = 309 */
    -1,    /* RETROK_LMETA = 310 */
    -1,    /* RETROK_LSUPER = 311 */
    -1,    /* RETROK_RSUPER = 312 */
    -1,    /* RETROK_MODE = 313 */     /* "Alt Gr" key */
    -1,    /* RETROK_COMPOSE = 314 */
    /* Miscellaneous function keys */
    -1,    /* RETROK_HELP = 315 */
    -1,    /* RETROK_PRINT = 316 */
    -1,    /* RETROK_SYSREQ = 317 */
    -1,    /* RETROK_BREAK = 318 */
    -1,    /* RETROK_MENU = 319 */
    -1,    /* RETROK_POWER = 320 */
    -1,    /* RETROK_EURO = 321 */
    -1,    /* RETROK_UNDO = 322 */
  0x2c     /* 323 ([ on Thomson keyboard) */
};

/* Mapping libretro -> Thomson MO5 scancodes */
const char libretroKeyCodeToThomsonMo5ScanCode[RETROK_LAST] =
{
    -1,    /* 0 */  -1,    /* 1 */  -1,    /* 2 */  -1,    /* 3 */  -1,    /* 4 */  -1,    /* 5 */  -1,    /* 6 */  -1,    /* 7 */
  0x36,    /* RETROK_BACKSPACE = 8 (ACC on Thomson keyboard) */
  0x37,    /* RETROK_TAB = 9 (STOP on Thomson keyboard) */
    -1,    /* 10 */ -1,    /* 11 */
    -1,    /* RETROK_CLEAR = 12 */
  0x34,    /* RETROK_RETURN = 13 */
    -1,    /* 14 */ -1,    /* 15 */ -1,    /* 16 */ -1,    /* 17 */ -1,    /* 18 */
    -1,    /* RETROK_PAUSE = 19 */
    -1,    /* 20 */ -1,    /* 21 */ -1,    /* 22 */ -1,    /* 23 */ -1,    /* 24 */ -1,    /* 25 */ -1,    /* 26 */
    -1,    /* RETROK_ESCAPE = 27 */
    -1,    /* 28 */ -1,    /* 29 */ -1,    /* 30 */ -1,    /* 31 */
  0x20,    /* RETROK_SPACE = 32 */
    -1,    /* RETROK_EXCLAIM = 33 */
    -1,    /* RETROK_QUOTEDBL = 34 */
    -1,    /* RETROK_HASH = 35 */
    -1,    /* RETROK_DOLLAR = 36 */
    -1,    /* 37 */
    -1,    /* RETROK_AMPERSAND = 38 */
    -1,    /* RETROK_QUOTE = 39 */
    -1,    /* RETROK_LEFTPAREN = 40 */
    -1,    /* RETROK_RIGHTPAREN = 41 */
    -1,    /* RETROK_ASTERISK = 42 */
    -1,    /* RETROK_PLUS = 43 */
  0x10,    /* RETROK_COMMA = 44 (. on Thomson keyboard) */
  0x26,    /* RETROK_MINUS = 45 */
  0x18,    /* RETROK_PERIOD = 46 (@ on Thomson keyboard) */
    -1,    /* RETROK_SLASH = 47 */
  0x1e,    /* RETROK_0 = 48 */
  0x2f,    /* RETROK_1 = 49 */
  0x27,    /* RETROK_2 = 50 */
  0x1f,    /* RETROK_3 = 51 */
  0x17,    /* RETROK_4 = 52 */
  0x0f,    /* RETROK_5 = 53 */
  0x07,    /* RETROK_6 = 54 */
  0x06,    /* RETROK_7 = 55 */
  0x0e,    /* RETROK_8 = 56 */
  0x16,    /* RETROK_9 = 57 */
    -1,    /* RETROK_COLON = 58 */
  0x1a,    /* RETROK_SEMICOLON = 59 (M on Thomson keyboard) */
    -1,    /* RETROK_LESS = 60 */
  0x2e,    /* RETROK_EQUALS = 61 (+ on Thomson keyboard) */
    -1,    /* RETROK_GREATER  = 62 */
    -1,    /* RETROK_QUESTION = 63 */
    -1,    /* RETROK_AT = 64 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 65 - 77 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 78 - 90 */
    -1,    /* RETROK_LEFTBRACKET = 91 */
    -1,    /* RETROK_BACKSLASH = 92 */
    -1,    /* RETROK_RIGHTBRACKET = 93 */
    -1,    /* RETROK_CARET = 94 */
    -1,    /* RETROK_UNDERSCORE = 95 */
    -1,    /* RETROK_BACKQUOTE = 96 */
  0x2b,    /* A = 97 */
  0x22,    /* B = 98 */
  0x32,    /* C = 99 */
  0x1b,    /* D = 100 */
  0x1d,    /* E = 101 */
  0x13,    /* F = 102 */
  0x0b,    /* G = 103 */
  0x03,    /* H = 104 */
  0x0c,    /* I = 105 */
  0x02,    /* J = 106 */
  0x0a,    /* K = 107 */
  0x12,    /* L = 108 */
  0x08,    /* M = 109 (, on Thomson keyboard) */
  0x00,    /* N = 110 */
  0x14,    /* O = 111 */
  0x1c,    /* P = 112 */
  0x2d,    /* Q = 113 */
  0x15,    /* R = 114 */
  0x23,    /* S = 115 */
  0x0d,    /* T = 116 */
  0x04,    /* U = 117 */
  0x2a,    /* V = 118 */
  0x25,    /* W = 119 */
  0x28,    /* X = 120 */
  0x05,    /* Y = 121 */
  0x30,    /* Z = 122 */
    -1,    /* 123 */    -1,    /* 124 */    -1,    /* 125 */    -1,    /* 126 */
  0x01,    /* RETROK_DELETE = 127 */
    /* End of ASCII mapped keycodes */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 128-143*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 144-159*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 160-175*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 176-191*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 192-207*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 208-223*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 224-239*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 240-255*/
    /* Numeric keypad: */
  0x1e,    /* RETROK_KP0 = 256 */
  0x2f,    /* RETROK_KP1 = 257 */
  0x27,    /* RETROK_KP2 = 258 */
  0x1f,    /* RETROK_KP3 = 259 */
  0x17,    /* RETROK_KP4 = 260 */
  0x0f,    /* RETROK_KP5 = 261 */
  0x07,    /* RETROK_KP6 = 262 */
  0x06,    /* RETROK_KP7 = 263 */
  0x0e,    /* RETROK_KP8 = 264 */
  0x16,    /* RETROK_KP9 = 265 */
  0x10,    /* RETROK_KP_PERIOD = 266 */
    -1,    /* RETROK_KP_DIVIDE = 267 */
    -1,    /* RETROK_KP_MULTIPLY = 268 */
    -1,    /* RETROK_KP_MINUS = 269 */
    -1,    /* RETROK_KP_PLUS = 270 */
    -1,    /* RETROK_KP_ENTER = 271 */
    -1,    /* RETROK_KP_EQUALS = 272 */
    /* Arrows + Home/End pad */
  0x31,    /* RETROK_UP = 273 */
  0x21,    /* RETROK_DOWN = 274 */
  0x19,    /* RETROK_RIGHT = 275 */
  0x29,    /* RETROK_LEFT = 276 */
  0x09,    /* RETROK_INSERT = 277 */
  0x11,    /* RETROK_HOME = 278 */
    -1,    /* RETROK_END = 279 */
    -1,    /* RETROK_PAGEUP = 280 */
    -1,    /* RETROK_PAGEDOWN = 281 */
    /* Function keys */
    -1,    /* RETROK_F1 = 282 */
    -1,    /* RETROK_F2 = 283 */
    -1,    /* RETROK_F3 = 284 */
    -1,    /* RETROK_F4 = 285 */
    -1,    /* RETROK_F5 = 286 */
    -1,    /* RETROK_F6 = 287 */
    -1,    /* RETROK_F7 = 288 */
    -1,    /* RETROK_F8 = 289 */
    -1,    /* RETROK_F9 = 290 */
    -1,    /* RETROK_F10 = 291 */
    -1,    /* RETROK_F11 = 292 */
    -1,    /* RETROK_F12 = 293 */
    -1,    /* RETROK_F13 = 294 */
    -1,    /* RETROK_F14 = 295 */
    -1,    /* RETROK_F15 = 296 */
    -1,    /* 297 */  -1,      /* 298 */  -1,      /* 299 */
    /* Key state modifier keys */
    -1,    /* RETROK_NUMLOCK = 300 */
    -1,    /* RETROK_CAPSLOCK = 301 */
    -1,    /* RETROK_SCROLLOCK = 302 */
  0x39,    /* RETROK_RSHIFT = 303 (BASIC key on Thomson keyboard) */
  0x38,    /* RETROK_LSHIFT = 304 (Yellow key on Thomson keyboard) */
  0x35,    /* RETROK_RCTRL = 305 (CNT key on Thomson keyboard) */
  0x35,    /* RETROK_LCTRL = 306 (CNT key on Thomson keyboard) */
  0x33,    /* RETROK_RALT = 307 (RAZ key on Thomson keyboard) */
  0x33,    /* RETROK_LALT = 308 (RAZ key on Thomson keyboard) */
    -1,    /* RETROK_RMETA = 309 */
    -1,    /* RETROK_LMETA = 310 */
    -1,    /* RETROK_LSUPER = 311 */
    -1,    /* RETROK_RSUPER = 312 */
    -1,    /* RETROK_MODE = 313 */     /* "Alt Gr" key */
    -1,    /* RETROK_COMPOSE = 314 */
    /* Miscellaneous function keys */
    -1,    /* RETROK_HELP = 315 */
    -1,    /* RETROK_PRINT = 316 */
    -1,    /* RETROK_SYSREQ = 317 */
    -1,    /* RETROK_BREAK = 318 */
    -1,    /* RETROK_MENU = 319 */
    -1,    /* RETROK_POWER = 320 */
    -1,    /* RETROK_EURO = 321 */
    -1,    /* RETROK_UNDO = 322 */
    -1     /* 323 */
};

/* Mapping libretro -> Thomson MO6 scancodes */
const char libretroKeyCodeToThomsonMo6ScanCode[RETROK_LAST] =
{
    -1,    /* 0 */  -1,    /* 1 */  -1,    /* 2 */  -1,    /* 3 */  -1,    /* 4 */  -1,    /* 5 */  -1,    /* 6 */  -1,    /* 7 */
  0x36,    /* RETROK_BACKSPACE = 8 (ACC on Thomson keyboard) */
  0x3e,    /* RETROK_TAB = 9 (STOP on Thomson keyboard) */
    -1,    /* 10 */ -1,    /* 11 */
    -1,    /* RETROK_CLEAR = 12 */
  0x26,    /* RETROK_RETURN = 13 */
    -1,    /* 14 */ -1,    /* 15 */ -1,    /* 16 */ -1,    /* 17 */ -1,    /* 18 */
    -1,    /* RETROK_PAUSE = 19 */
    -1,    /* 20 */ -1,    /* 21 */ -1,    /* 22 */ -1,    /* 23 */ -1,    /* 24 */ -1,    /* 25 */ -1,    /* 26 */
    -1,    /* RETROK_ESCAPE = 27 */
    -1,    /* 28 */ -1,    /* 29 */ -1,    /* 30 */ -1,    /* 31 */
  0x04,    /* RETROK_SPACE = 32 */
    -1,    /* RETROK_EXCLAIM = 33 */
    -1,    /* RETROK_QUOTEDBL = 34 */
    -1,    /* RETROK_HASH = 35 */
    -1,    /* RETROK_DOLLAR = 36 */
    -1,    /* 37 */
    -1,    /* RETROK_AMPERSAND = 38 */
  0x44,    /* RETROK_QUOTE = 39 (ù on Thomson MO6 keyboard) */
    -1,    /* RETROK_LEFTPAREN = 40 */
    -1,    /* RETROK_RIGHTPAREN = 41 */
    -1,    /* RETROK_ASTERISK = 42 */
    -1,    /* RETROK_PLUS = 43 */
  0x02,    /* RETROK_COMMA = 44 (; on Thomson keyboard) */
  0x42,    /* RETROK_MINUS = 45 (')' on Thomson MO6 keyboard) */
  0x24,    /* RETROK_PERIOD = 46 (: on Thomson MO6 keyboard) */
  0x0a,    /* RETROK_SLASH = 47 (> on Thomson MO6 keyboard) */
  0x33,    /* RETROK_0 = 48 */
  0x3d,    /* RETROK_1 = 49 */
  0x3c,    /* RETROK_2 = 50 */
  0x3b,    /* RETROK_3 = 51 */
  0x3a,    /* RETROK_4 = 52 */
  0x39,    /* RETROK_5 = 53 */
  0x38,    /* RETROK_6 = 54 */
  0x30,    /* RETROK_7 = 55 */
  0x31,    /* RETROK_8 = 56 */
  0x32,    /* RETROK_9 = 57 */
    -1,    /* RETROK_COLON = 58 */
  0x13,    /* RETROK_SEMICOLON = 59 (M on Thomson keyboard) */
    -1,    /* RETROK_LESS = 60 */
  0x35,    /* RETROK_EQUALS = 61 (= on Thomson MO6 keyboard) */
    -1,    /* RETROK_GREATER  = 62 */
    -1,    /* RETROK_QUESTION = 63 */
    -1,    /* RETROK_AT = 64 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 65 - 77 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 78 - 90 */
  0x43,    /* RETROK_LEFTBRACKET = 91 (^ on Thomson MO6 keyboard) */
  0x41,    /* RETROK_BACKSLASH = 92 (] on Thomson MO6 keyboard) */
  0x25,    /* RETROK_RIGHTBRACKET = 93 ($ on Thomson MO6 keyboard) */
    -1,    /* RETROK_CARET = 94 */
    -1,    /* RETROK_UNDERSCORE = 95 */
  0x03,    /* RETROK_BACKQUOTE = 96 (# on Thomson MO6 keyboard) */
  0x1d,    /* A = 97 */
  0x14,    /* B = 98 */
  0x16,    /* C = 99 */
  0x1b,    /* D = 100 */
  0x2b,    /* E = 101 */
  0x1a,    /* F = 102 */
  0x19,    /* G = 103 */
  0x18,    /* H = 104 */
  0x21,    /* I = 105 */
  0x10,    /* J = 106 */
  0x11,    /* K = 107 */
  0x12,    /* L = 108 */
  0x01,    /* M = 109 (, on Thomson keyboard) */
  0x00,    /* N = 110 */
  0x22,    /* O = 111 */
  0x23,    /* P = 112 */
  0x2d,    /* Q = 113 */
  0x2a,    /* R = 114 */
  0x1c,    /* S = 115 */
  0x29,    /* T = 116 */
  0x20,    /* U = 117 */
  0x15,    /* V = 118 */
  0x2c,    /* W = 119 */
  0x05,    /* X = 120 */
  0x28,    /* Y = 121 */
  0x06,    /* Z = 122 */
    -1,    /* 123 */    -1,    /* 124 */    -1,    /* 125 */    -1,    /* 126 */
  0x08,    /* RETROK_DELETE = 127 */
    /* End of ASCII mapped keycodes */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 128-143*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 144-159*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 160-175*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 176-191*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 192-207*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 208-223*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 224-239*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 240-255*/
    /* Numeric keypad: */
  0x33,    /* RETROK_KP0 = 256 */
  0x3d,    /* RETROK_KP1 = 257 */
  0x3c,    /* RETROK_KP2 = 258 */
  0x3b,    /* RETROK_KP3 = 259 */
  0x3a,    /* RETROK_KP4 = 260 */
  0x39,    /* RETROK_KP5 = 261 */
  0x38,    /* RETROK_KP6 = 262 */
  0x30,    /* RETROK_KP7 = 263 */
  0x31,    /* RETROK_KP8 = 264 */
  0x32,    /* RETROK_KP9 = 265 */
    -1,    /* RETROK_KP_PERIOD = 266 */
    -1,    /* RETROK_KP_DIVIDE = 267 */
    -1,    /* RETROK_KP_MULTIPLY = 268 */
    -1,    /* RETROK_KP_MINUS = 269 */
    -1,    /* RETROK_KP_PLUS = 270 */
    -1,    /* RETROK_KP_ENTER = 271 */
    -1,    /* RETROK_KP_EQUALS = 272 */
    /* Arrows + Home/End pad */
  0x0e,    /* RETROK_UP = 273 */
  0x0c,    /* RETROK_DOWN = 274 */
  0x0b,    /* RETROK_RIGHT = 275 */
  0x0d,    /* RETROK_LEFT = 276 */
  0x09,    /* RETROK_INSERT = 277 */
  0x1e,    /* RETROK_HOME = 278 */
    -1,    /* RETROK_END = 279 */
    -1,    /* RETROK_PAGEUP = 280 */
    -1,    /* RETROK_PAGEDOWN = 281 */
    /* Function keys */
  0x1f,    /* RETROK_F1 = 282 */
  0x27,    /* RETROK_F2 = 283 */
  0x2f,    /* RETROK_F3 = 284 */
  0x37,    /* RETROK_F4 = 285 */
  0x3f,    /* RETROK_F5 = 286 */
    -1,    /* RETROK_F6 = 287 */
    -1,    /* RETROK_F7 = 288 */
    -1,    /* RETROK_F8 = 289 */
    -1,    /* RETROK_F9 = 290 */
    -1,    /* RETROK_F10 = 291 */
    -1,    /* RETROK_F11 = 292 */
    -1,    /* RETROK_F12 = 293 */
    -1,    /* RETROK_F13 = 294 */
    -1,    /* RETROK_F14 = 295 */
    -1,    /* RETROK_F15 = 296 */
    -1,    /* 297 */  -1,      /* 298 */  -1,      /* 299 */
    /* Key state modifier keys */
    -1,    /* RETROK_NUMLOCK = 300 */
  0x17,    /* RETROK_CAPSLOCK = 301 */
    -1,    /* RETROK_SCROLLOCK = 302 */
  0x0f,    /* RETROK_RSHIFT = 303 (BASIC key on Thomson keyboard) */
  0x07,    /* RETROK_LSHIFT = 304 (Shift key on Thomson keyboard) */
  0x2e,    /* RETROK_RCTRL = 305 (CNT key on Thomson keyboard) */
  0x2e,    /* RETROK_LCTRL = 306 (CNT key on Thomson keyboard) */
  0x1e,    /* RETROK_RALT = 307 (RAZ key on Thomson keyboard) */
  0x1e,    /* RETROK_LALT = 308 (RAZ key on Thomson keyboard) */
    -1,    /* RETROK_RMETA = 309 */
    -1,    /* RETROK_LMETA = 310 */
    -1,    /* RETROK_LSUPER = 311 */
    -1,    /* RETROK_RSUPER = 312 */
    -1,    /* RETROK_MODE = 313 */     /* "Alt Gr" key */
    -1,    /* RETROK_COMPOSE = 314 */
    /* Miscellaneous function keys */
    -1,    /* RETROK_HELP = 315 */
    -1,    /* RETROK_PRINT = 316 */
    -1,    /* RETROK_SYSREQ = 317 */
    -1,    /* RETROK_BREAK = 318 */
    -1,    /* RETROK_MENU = 319 */
    -1,    /* RETROK_POWER = 320 */
    -1,    /* RETROK_EURO = 321 */
    -1,    /* RETROK_UNDO = 322 */
  0x40     /* 323 ([ on Thomson keyboard) */
};

/* Mapping libretro -> Thomson scancodes for the current MO/TO version */
const char *libretroKeyCodeToThomsonScanCode = libretroKeyCodeToThomsonToScanCode;

/* Keysyms for the virtual keyboard (AZERTY mode, i.e. all models but PC128) */
const int virtualkb_keysyms_azerty[VIRTUALKB_NB_KEYS] =
{
    RETROK_KP0, RETROK_KP1, RETROK_KP2, RETROK_KP3, RETROK_KP4,
    RETROK_KP5, RETROK_KP6, RETROK_KP7, RETROK_KP8, RETROK_KP9,
    RETROK_q, RETROK_b, RETROK_c, RETROK_d, RETROK_e, RETROK_f,
    RETROK_g, RETROK_h, RETROK_i, RETROK_j, RETROK_k, RETROK_l,
    RETROK_SEMICOLON, RETROK_n, RETROK_o, RETROK_p, RETROK_a, RETROK_r,
    RETROK_s, RETROK_t, RETROK_u, RETROK_v, RETROK_z, RETROK_x,
    RETROK_y, RETROK_w, RETROK_SPACE, RETROK_RETURN
};

/* Keysyms for the virtual keyboard (QWERTY mode, i.e. only for Olivetti Prodest PC128) */
const int virtualkb_keysyms_qwerty[VIRTUALKB_NB_KEYS] =
{
    RETROK_KP0, RETROK_KP1, RETROK_KP2, RETROK_KP3, RETROK_KP4,
    RETROK_KP5, RETROK_KP6, RETROK_KP7, RETROK_KP8, RETROK_KP9,
    RETROK_a, RETROK_b, RETROK_c, RETROK_d, RETROK_e, RETROK_f,
    RETROK_g, RETROK_h, RETROK_i, RETROK_j, RETROK_k, RETROK_l,
    RETROK_m, RETROK_n, RETROK_o, RETROK_p, RETROK_q, RETROK_r,
    RETROK_s, RETROK_t, RETROK_u, RETROK_v, RETROK_w, RETROK_x,
    RETROK_y, RETROK_z, RETROK_SPACE, RETROK_RETURN
};

/* Virtual keyboard keysyms for the current computer version */
const int *virtualkb_keysyms = virtualkb_keysyms_azerty;

const char* virtualkb_chars[VIRTUALKB_NB_KEYS] =
{
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
    "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
    "U", "V", "W", "X", "Y", "Z", "Space", "Enter"
};

