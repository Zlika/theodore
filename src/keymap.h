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

#ifndef __KEYMAP_H
#define __KEYMAP_H

/* Thomson scancodes of special keys (TO computers) */
#define THOMSON_TO_ACC         0x14
#define THOMSON_TO_CAPSLOCK    0x50
#define THOMSON_TO_LEFT_SHIFT  0x51
#define THOMSON_TO_RIGHT_SHIFT 0x52
#define THOMSON_TO_CNT         0x53
/* Thomson scancodes of special keys (MO computers) */
#define THOMSON_MO_ACC         0x36
#define THOMSON_MO_LEFT_SHIFT  0x38
#define THOMSON_MO_CNT         0x35

/* Thomson scancodes of special keys for the current MO/TO version */
int thomson_acc = THOMSON_TO_ACC;
int thomson_capslock = THOMSON_TO_CAPSLOCK;
int thomson_left_shift = THOMSON_TO_LEFT_SHIFT;
int thomson_right_shift = THOMSON_TO_RIGHT_SHIFT;
int thomson_cnt = THOMSON_TO_CNT;

/* Mapping libretro -> Thomson TO scancodes */
const char libretroKeyCodeToThomsonToScanCode[512] =
{
    -1,    /* 0 */  -1,    /* 1 */  -1,    /* 2 */  -1,    /* 3 */  -1,    /* 4 */  -1,    /* 5 */  -1,    /* 6 */  -1,    /* 7 */
  0x4c,    /* SDLK_BACKSPACE=8 */
  0x30,    /* SDLK_TAB=9 */
    -1,    /* 10 */ -1,    /* 11 */
    -1,    /* SDLK_CLEAR = 12 */
  0x46,    /* SDLK_RETURN = 13 */
    -1,    /* 14 */ -1,    /* 15 */ -1,    /* 16 */ -1,    /* 17 */ -1,    /* 18 */
    -1,    /* SDLK_PAUSE = 19 */
    -1,    /* 20 */ -1,    /* 21 */ -1,    /* 22 */ -1,    /* 23 */ -1,    /* 24 */ -1,    /* 25 */ -1,    /* 26 */
    -1,    /* SDLK_ESCAPE = 27 */
    -1,    /* 28 */ -1,    /* 29 */ -1,    /* 30 */ -1,    /* 31 */
  0x34,    /* SDLK_SPACE = 32 */
    -1,    /* SDLK_EXCLAIM = 33 */
    -1,    /* SDLK_QUOTEDBL = 34 */
    -1,    /* SDLK_HASH = 35 */
  0x3c,    /* SDLK_DOLLAR = 36 */
    -1,    /* 37 */
  0x3c,    /* SDLK_AMPERSAND = 38 */
  0x19,    /* SDLK_QUOTE = 39 */
  0x09,    /* SDLK_LEFTPAREN = 40 */
  0x4c,    /* SDLK_RIGHTPAREN = 41 */
  0x29,    /* SDLK_ASTERISK = 42 */
  0x0c,    /* SDLK_PLUS = 43 */
  0x37,    /* SDLK_COMMA = 44 */
  0x44,    /* SDLK_MINUS = 45 */
  0x26,    /* SDLK_PERIOD = 46 */
  0x47,    /* SDLK_SLASH = 47 */
  0x49,    /* SDLK_0 = 48 */
  0x29,    /* SDLK_1 = 49 */
  0x21,    /* SDLK_2 = 50 */
  0x19,    /* SDLK_3 = 51 */
  0x11,    /* SDLK_4 = 52 */
  0x09,    /* SDLK_5 = 53 */
  0x01,    /* SDLK_6 = 54 */
  0x31,    /* SDLK_7 = 55 */
  0x39,    /* SDLK_8 = 56 */
  0x41,    /* SDLK_9 = 57 */
  0x47,    /* SDLK_COLON = 58 */
  0x26,    /* SDLK_SEMICOLON = 59 */
  0x4f,    /* SDLK_LESS = 60 */
  0x0c,    /* SDLK_EQUALS = 61 */
  0x4f,    /* SDLK_GREATER  = 62 */
  0x37,    /* SDLK_QUESTION = 63 */
    -1,    /* SDLK_AT = 64 */
  0x2a,    /* A = 65 */
  0x0f,    /* B = 66 */
  0x1f,    /* C = 67 */
  0x1b,    /* D = 68 */
  0x1a,    /* E = 69 */
  0x13,    /* F = 70 */
  0x0b,    /* G = 71 */
  0x03,    /* H = 72 */
  0x3a,    /* I = 73 */
  0x33,    /* J = 74 */
  0x3b,    /* K = 75 */
  0x43,    /* L = 76 */
  0x4b,    /* M = 77 */
  0x07,    /* N = 78 */
  0x42,    /* O = 79 */
  0x4a,    /* P = 80 */
  0x2b,    /* Q = 81 */
  0x12,    /* R = 82 */
  0x23,    /* S = 83 */
  0x0a,    /* T = 84 */
  0x32,    /* U = 85 */
  0x17,    /* V = 86 */
  0x2f,    /* W = 87 */
  0x27,    /* X = 88 */
  0x02,    /* Y = 89 */
  0x22,    /* Z = 90 */
    -1,    /* SDLK_LEFTBRACKET = 91 */
  0x44,    /* SDLK_BACKSLASH = 92 */     /* Might be 0x60 for UK keyboards */
    -1,    /* SDLK_RIGHTBRACKET = 93 */
  0x44,    /* SDLK_CARET = 94 */
  0x01,    /* SDLK_UNDERSCORE = 95 */
  0x28,    /* SDLK_BACKQUOTE = 96 */
  0x2a,    /* A = 97 */
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
  0x4b,    /* M = 109 */
  0x07,    /* N = 110 */
  0x42,    /* O = 111 */
  0x4a,    /* P = 112 */
  0x2b,    /* Q = 113 */
  0x12,    /* R = 114 */
  0x23,    /* S = 115 */
  0x0a,    /* T = 116 */
  0x32,    /* U = 117 */
  0x17,    /* V = 118 */
  0x2f,    /* W = 119 */
  0x27,    /* X = 120 */
  0x02,    /* Y = 121 */
  0x22,    /* Z = 122 */
    -1,    /* 123 */    -1,    /* 124 */    -1,    /* 125 */    -1,    /* 126 */
  0x16,    /* SDLK_DELETE = 127 */
    /* End of ASCII mapped keysyms */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 128-143*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 144-159*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 160-175*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 176-191*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 192-207*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 208-223*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 224-239*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 240-255*/
    /* Numeric keypad: */
  0x1e,    /* SDLK_KP0 = 256 */
  0x15,    /* SDLK_KP1 = 257 */
  0x25,    /* SDLK_KP2 = 258 */
  0x4e,    /* SDLK_KP3 = 259 */
  0x1d,    /* SDLK_KP4 = 260 */
  0x2d,    /* SDLK_KP5 = 261 */
  0x2e,    /* SDLK_KP6 = 262 */
  0x1c,    /* SDLK_KP7 = 263 */
  0x24,    /* SDLK_KP8 = 264 */
  0x35,    /* SDLK_KP9 = 265 */
  0x26,    /* SDLK_KP_PERIOD = 266 */
    -1,    /* SDLK_KP_DIVIDE = 267 */
    -1,    /* SDLK_KP_MULTIPLY = 268 */
    -1,    /* SDLK_KP_MINUS = 269 */
    -1,    /* SDLK_KP_PLUS = 270 */
  0x36,    /* SDLK_KP_ENTER = 271 */
    -1,    /* SDLK_KP_EQUALS = 272 */
    /* Arrows + Home/End pad */
  0x04,    /* SDLK_UP = 273 */
  0x3d,    /* SDLK_DOWN = 274 */
  0x05,    /* SDLK_RIGHT = 275 */
  0x0d,    /* SDLK_LEFT = 276 */
  0x0e,    /* SDLK_INSERT = 277 */
  0x06,    /* SDLK_HOME = 278 */
    -1,    /* SDLK_END = 279 */
    -1,    /* SDLK_PAGEUP = 280 */
    -1,    /* SDLK_PAGEDOWN = 281 */
    /* Function keys */
  0x20,    /* SDLK_F1 = 282 */
  0x00,    /* SDLK_F2 = 283 */
  0x08,    /* SDLK_F3 = 284 */
  0x10,    /* SDLK_F4 = 285 */
  0x18,    /* SDLK_F5 = 286 */
  0x20,    /* SDLK_F6 = 287 */
  0x00,    /* SDLK_F7 = 288 */
  0x08,    /* SDLK_F8 = 289 */
  0x10,    /* SDLK_F9 = 290 */
  0x18,    /* SDLK_F10 = 291 */
    -1,    /* SDLK_F11 = 292 */
    -1,    /* SDLK_F12 = 293 */
    -1,    /* SDLK_F13 = 294 */
    -1,    /* SDLK_F14 = 295 */
    -1,    /* SDLK_F15 = 296 */
    -1,    /* 297 */  -1,      /* 298 */  -1,      /* 299 */
    /* Key state modifier keys */
    -1,    /* SDLK_NUMLOCK = 300 */
  0x50,    /* SDLK_CAPSLOCK = 301 */
    -1,    /* SDLK_SCROLLOCK = 302 */
  0x52,    /* SDLK_RSHIFT = 303 */
  0x51,    /* SDLK_LSHIFT = 304 */
  0x53,    /* SDLK_RCTRL = 305 */
  0x53,    /* SDLK_LCTRL = 306 */
  0x14,    /* SDLK_RALT = 307 */
  0x14,    /* SDLK_LALT = 308 */
    -1,    /* SDLK_RMETA = 309 */
    -1,    /* SDLK_LMETA = 310 */
    -1,    /* SDLK_LSUPER = 311 */
    -1,    /* SDLK_RSUPER = 312 */
    -1,    /* SDLK_MODE = 313 */     /* "Alt Gr" key */
    -1,    /* SDLK_COMPOSE = 314 */
    /* Miscellaneous function keys */
    -1,    /* SDLK_HELP = 315 */
    -1,    /* SDLK_PRINT = 316 */
    -1,    /* SDLK_SYSREQ = 317 */
    -1,    /* SDLK_BREAK = 318 */
    -1,    /* SDLK_MENU = 319 */
    -1,    /* SDLK_POWER = 320 */
    -1,    /* SDLK_EURO = 321 */
    -1     /* SDLK_UNDO = 322 */
};

/* Mapping libretro -> Thomson MO scancodes */
const char libretroKeyCodeToThomsonMoScanCode[512] =
{
    -1,    /* 0 */  -1,    /* 1 */  -1,    /* 2 */  -1,    /* 3 */  -1,    /* 4 */  -1,    /* 5 */  -1,    /* 6 */  -1,    /* 7 */
    -1,    /* SDLK_BACKSPACE=8 */
  0x37,    /* SDLK_TAB=9 */
    -1,    /* 10 */ -1,    /* 11 */
    -1,    /* SDLK_CLEAR = 12 */
  0x34,    /* SDLK_RETURN = 13 */
    -1,    /* 14 */ -1,    /* 15 */ -1,    /* 16 */ -1,    /* 17 */ -1,    /* 18 */
    -1,    /* SDLK_PAUSE = 19 */
    -1,    /* 20 */ -1,    /* 21 */ -1,    /* 22 */ -1,    /* 23 */ -1,    /* 24 */ -1,    /* 25 */ -1,    /* 26 */
    -1,    /* SDLK_ESCAPE = 27 */
    -1,    /* 28 */ -1,    /* 29 */ -1,    /* 30 */ -1,    /* 31 */
  0x20,    /* SDLK_SPACE = 32 */
  0x2f,    /* SDLK_EXCLAIM = 33 */
  0x1f,    /* SDLK_QUOTEDBL = 34 */
  0x1f,    /* SDLK_HASH = 35 */
  0x17,    /* SDLK_DOLLAR = 36 */
    -1,    /* 37 */
  0x2f,    /* SDLK_AMPERSAND = 38 */
  0x17,    /* SDLK_QUOTE = 39 */
  0x0f,    /* SDLK_LEFTPAREN = 40 */
  0x26,    /* SDLK_RIGHTPAREN = 41 */
  0x2c,    /* SDLK_ASTERISK = 42 */
  0x2e,    /* SDLK_PLUS = 43 */
  0x08,    /* SDLK_COMMA = 44 */
  0x26,    /* SDLK_MINUS = 45 */
  0x10,    /* SDLK_PERIOD = 46 */
  0x24,    /* SDLK_SLASH = 47 */
  0x1e,    /* SDLK_0 = 48 */
  0x2f,    /* SDLK_1 = 49 */
  0x27,    /* SDLK_2 = 50 */
  0x1f,    /* SDLK_3 = 51 */
  0x17,    /* SDLK_4 = 52 */
  0x0f,    /* SDLK_5 = 53 */
  0x07,    /* SDLK_6 = 54 */
  0x06,    /* SDLK_7 = 55 */
  0x0e,    /* SDLK_8 = 56 */
  0x16,    /* SDLK_9 = 57 */
  0x47,    /* SDLK_COLON = 58 */
  0x10,    /* SDLK_SEMICOLON = 59 */
  0x08,    /* SDLK_LESS = 60 */
  0x2e,    /* SDLK_EQUALS = 61 */
  0x10,    /* SDLK_GREATER  = 62 */
  0x24,    /* SDLK_QUESTION = 63 */
  0x18,    /* SDLK_AT = 64 */
  0x2d,    /* A = 65 */
  0x22,    /* B = 66 */
  0x32,    /* C = 67 */
  0x1b,    /* D = 68 */
  0x1d,    /* E = 69 */
  0x13,    /* F = 70 */
  0x0b,    /* G = 71 */
  0x03,    /* H = 72 */
  0x0c,    /* I = 73 */
  0x02,    /* J = 74 */
  0x0a,    /* K = 75 */
  0x12,    /* L = 76 */
  0x1a,    /* M = 77 */
  0x00,    /* N = 78 */
  0x14,    /* O = 79 */
  0x1c,    /* P = 80 */
  0x2b,    /* Q = 81 */
  0x15,    /* R = 82 */
  0x23,    /* S = 83 */
  0x0d,    /* T = 84 */
  0x04,    /* U = 85 */
  0x2a,    /* V = 86 */
  0x30,    /* W = 87 */
  0x28,    /* X = 88 */
  0x05,    /* Y = 89 */
  0x25,    /* Z = 90 */
    -1,    /* SDLK_LEFTBRACKET = 91 */
  0x44,    /* SDLK_BACKSLASH = 92 */     /* Might be 0x60 for UK keyboards */
    -1,    /* SDLK_RIGHTBRACKET = 93 */
  0x07,    /* SDLK_CARET = 94 */
  0x0e,    /* SDLK_UNDERSCORE = 95 */
  0x1e,    /* SDLK_BACKQUOTE = 96 */
  0x2d,    /* A = 97 */
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
  0x1a,    /* M = 109 */
  0x00,    /* N = 110 */
  0x14,    /* O = 111 */
  0x1c,    /* P = 112 */
  0x2b,    /* Q = 113 */
  0x15,    /* R = 114 */
  0x23,    /* S = 115 */
  0x0d,    /* T = 116 */
  0x04,    /* U = 117 */
  0x2a,    /* V = 118 */
  0x30,    /* W = 119 */
  0x28,    /* X = 120 */
  0x05,    /* Y = 121 */
  0x25,    /* Z = 122 */
    -1,    /* 123 */    -1,    /* 124 */    -1,    /* 125 */    -1,    /* 126 */
  0x01,    /* SDLK_DELETE = 127 */
    /* End of ASCII mapped keysyms */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 128-143*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 144-159*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 160-175*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 176-191*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 192-207*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 208-223*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 224-239*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 240-255*/
    /* Numeric keypad: */
  0x1e,    /* SDLK_KP0 = 256 */
  0x2f,    /* SDLK_KP1 = 257 */
  0x27,    /* SDLK_KP2 = 258 */
  0x1f,    /* SDLK_KP3 = 259 */
  0x17,    /* SDLK_KP4 = 260 */
  0x0f,    /* SDLK_KP5 = 261 */
  0x07,    /* SDLK_KP6 = 262 */
  0x06,    /* SDLK_KP7 = 263 */
  0x0e,    /* SDLK_KP8 = 264 */
  0x16,    /* SDLK_KP9 = 265 */
  0x10,    /* SDLK_KP_PERIOD = 266 */
    -1,    /* SDLK_KP_DIVIDE = 267 */
    -1,    /* SDLK_KP_MULTIPLY = 268 */
    -1,    /* SDLK_KP_MINUS = 269 */
    -1,    /* SDLK_KP_PLUS = 270 */
    -1,    /* SDLK_KP_ENTER = 271 */
    -1,    /* SDLK_KP_EQUALS = 272 */
    /* Arrows + Home/End pad */
  0x31,    /* SDLK_UP = 273 */
  0x21,    /* SDLK_DOWN = 274 */
  0x19,    /* SDLK_RIGHT = 275 */
  0x29,    /* SDLK_LEFT = 276 */
  0x09,    /* SDLK_INSERT = 277 */
  0x11,    /* SDLK_HOME = 278 */
    -1,    /* SDLK_END = 279 */
    -1,    /* SDLK_PAGEUP = 280 */
    -1,    /* SDLK_PAGEDOWN = 281 */
    /* Function keys */
    -1,    /* SDLK_F1 = 282 */
    -1,    /* SDLK_F2 = 283 */
    -1,    /* SDLK_F3 = 284 */
    -1,    /* SDLK_F4 = 285 */
    -1,    /* SDLK_F5 = 286 */
    -1,    /* SDLK_F6 = 287 */
    -1,    /* SDLK_F7 = 288 */
    -1,    /* SDLK_F8 = 289 */
    -1,    /* SDLK_F9 = 290 */
    -1,    /* SDLK_F10 = 291 */
    -1,    /* SDLK_F11 = 292 */
    -1,    /* SDLK_F12 = 293 */
    -1,    /* SDLK_F13 = 294 */
    -1,    /* SDLK_F14 = 295 */
    -1,    /* SDLK_F15 = 296 */
    -1,    /* 297 */  -1,      /* 298 */  -1,      /* 299 */
    /* Key state modifier keys */
    -1,    /* SDLK_NUMLOCK = 300 */
    -1,    /* SDLK_CAPSLOCK = 301 */
    -1,    /* SDLK_SCROLLOCK = 302 */
    -1,    /* SDLK_RSHIFT = 303 */
  0x38,    /* SDLK_LSHIFT = 304 */
    -1,    /* SDLK_RCTRL = 305 */
    -1,    /* SDLK_LCTRL = 306 */
    -1,    /* SDLK_RALT = 307 */
    -1,    /* SDLK_LALT = 308 */
    -1,    /* SDLK_RMETA = 309 */
    -1,    /* SDLK_LMETA = 310 */
    -1,    /* SDLK_LSUPER = 311 */
    -1,    /* SDLK_RSUPER = 312 */
    -1,    /* SDLK_MODE = 313 */     /* "Alt Gr" key */
    -1,    /* SDLK_COMPOSE = 314 */
    /* Miscellaneous function keys */
    -1,    /* SDLK_HELP = 315 */
    -1,    /* SDLK_PRINT = 316 */
    -1,    /* SDLK_SYSREQ = 317 */
    -1,    /* SDLK_BREAK = 318 */
    -1,    /* SDLK_MENU = 319 */
    -1,    /* SDLK_POWER = 320 */
    -1,    /* SDLK_EURO = 321 */
    -1     /* SDLK_UNDO = 322 */
};

/* Mapping libretro -> Thomson scancodes for the current MO/TO version */
const char *libretroKeyCodeToThomsonScanCode = libretroKeyCodeToThomsonToScanCode;

#define VIRTUALKB_NB_KEYS 38

const int virtualkb_keysyms[VIRTUALKB_NB_KEYS] =
{
    RETROK_KP0, RETROK_KP1, RETROK_KP2, RETROK_KP3, RETROK_KP4,
    RETROK_KP5, RETROK_KP6, RETROK_KP7, RETROK_KP8, RETROK_KP9,
    RETROK_a, RETROK_b, RETROK_c, RETROK_d, RETROK_e, RETROK_f,
    RETROK_g, RETROK_h, RETROK_i, RETROK_j, RETROK_k, RETROK_l,
    RETROK_m, RETROK_n, RETROK_o, RETROK_p, RETROK_q, RETROK_r,
    RETROK_s, RETROK_t, RETROK_u, RETROK_v, RETROK_w, RETROK_x,
    RETROK_y, RETROK_z, RETROK_SPACE, RETROK_RETURN
};

const char* virtualkb_chars[VIRTUALKB_NB_KEYS] =
{
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
    "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
    "U", "V", "W", "X", "Y", "Z", "Space", "Enter"
};

#endif /* __KEYMAP_H */
