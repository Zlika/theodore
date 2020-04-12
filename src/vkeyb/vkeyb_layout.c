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

/* Virtual keyboard layout */

#include "vkeyb_layout.h"
#include "../keymap.h"

const struct VKey mo5_kb[] =
{
  // 1st row (keys 0 to 14)
  { 0x37,   4,  34, 31, 31, &mo5_kb[14], &mo5_kb[1],  &mo5_kb[44], &mo5_kb[15] }, /* 0: STOP */
  { 0x2f,  36,  34, 31, 31, &mo5_kb[0],  &mo5_kb[2],  &mo5_kb[44], &mo5_kb[16] }, /* 1: 1 */
  { 0x27,  68,  34, 31, 31, &mo5_kb[1],  &mo5_kb[3],  &mo5_kb[45], &mo5_kb[17] }, /* 2: 2 */
  { 0x1f, 100,  34, 31, 31, &mo5_kb[2],  &mo5_kb[4],  &mo5_kb[46], &mo5_kb[18] }, /* 3: 3 */
  { 0x17, 132,  34, 31, 31, &mo5_kb[3],  &mo5_kb[5],  &mo5_kb[57], &mo5_kb[19] }, /* 4: 4 */
  { 0x0f, 164,  34, 31, 31, &mo5_kb[4],  &mo5_kb[6],  &mo5_kb[57], &mo5_kb[20] }, /* 5: 5 */
  { 0x07, 196,  34, 31, 31, &mo5_kb[5],  &mo5_kb[7],  &mo5_kb[57], &mo5_kb[21] }, /* 6: 6 */
  { 0x06, 228,  34, 31, 31, &mo5_kb[6],  &mo5_kb[8],  &mo5_kb[57], &mo5_kb[22] }, /* 7: 7 */
  { 0x0e, 260,  34, 31, 31, &mo5_kb[7],  &mo5_kb[9],  &mo5_kb[57], &mo5_kb[23] }, /* 8: 8 */
  { 0x16, 292,  34, 31, 31, &mo5_kb[8],  &mo5_kb[10], &mo5_kb[52], &mo5_kb[24] }, /* 9: 9 */
  { 0x1e, 324,  34, 31, 31, &mo5_kb[9],  &mo5_kb[11], &mo5_kb[53], &mo5_kb[25] }, /* 10: 0 */
  { 0x26, 356,  34, 31, 31, &mo5_kb[10], &mo5_kb[12], &mo5_kb[54], &mo5_kb[26] }, /* 11: - */
  { 0x2e, 388,  34, 31, 31, &mo5_kb[11], &mo5_kb[13], &mo5_kb[54], &mo5_kb[27] }, /* 12: + */
  { 0x36, 420,  34, 31, 31, &mo5_kb[12], &mo5_kb[14], &mo5_kb[55], &mo5_kb[28] }, /* 13: ACC */
  { 0x31, 452,  34, 31, 31, &mo5_kb[13], &mo5_kb[15], &mo5_kb[56], &mo5_kb[29] }, /* 14: Up */
  // 2nd row (keys 15 to 29)
  { 0x35,   4,  66, 31, 31, &mo5_kb[29], &mo5_kb[16], &mo5_kb[0],  &mo5_kb[30] }, /* 15: CNT */
  { 0x2d,  52,  66, 31, 31, &mo5_kb[15], &mo5_kb[17], &mo5_kb[1],  &mo5_kb[31] }, /* 16: A */
  { 0x25,  84,  66, 31, 31, &mo5_kb[16], &mo5_kb[18], &mo5_kb[2],  &mo5_kb[32] }, /* 17: Z */
  { 0x1d, 116,  66, 31, 31, &mo5_kb[17], &mo5_kb[19], &mo5_kb[3],  &mo5_kb[33] }, /* 18: E */
  { 0x15, 148,  66, 31, 31, &mo5_kb[18], &mo5_kb[20], &mo5_kb[4],  &mo5_kb[34] }, /* 19: R */
  { 0x0d, 180,  66, 31, 31, &mo5_kb[19], &mo5_kb[21], &mo5_kb[5],  &mo5_kb[35] }, /* 20: T */
  { 0x05, 212,  66, 31, 31, &mo5_kb[20], &mo5_kb[22], &mo5_kb[6],  &mo5_kb[36] }, /* 21: Y */
  { 0x04, 244,  66, 31, 31, &mo5_kb[21], &mo5_kb[23], &mo5_kb[7],  &mo5_kb[37] }, /* 22: U */
  { 0x0c, 276,  66, 31, 31, &mo5_kb[22], &mo5_kb[24], &mo5_kb[8],  &mo5_kb[38] }, /* 23: I */
  { 0x14, 308,  66, 31, 31, &mo5_kb[23], &mo5_kb[25], &mo5_kb[9],  &mo5_kb[39] }, /* 24: O */
  { 0x1c, 340,  66, 31, 31, &mo5_kb[24], &mo5_kb[26], &mo5_kb[10], &mo5_kb[40] }, /* 25: P */
  { 0x24, 372,  66, 31, 31, &mo5_kb[25], &mo5_kb[27], &mo5_kb[11], &mo5_kb[41] }, /* 26: / */
  { 0x2c, 404,  66, 31, 31, &mo5_kb[26], &mo5_kb[28], &mo5_kb[12], &mo5_kb[42] }, /* 27: * */
  { 0x29, 436,  66, 31, 31, &mo5_kb[27], &mo5_kb[29], &mo5_kb[13], &mo5_kb[43] }, /* 28: Left */
  { 0x19, 468,  66, 31, 31, &mo5_kb[28], &mo5_kb[15], &mo5_kb[14], &mo5_kb[43] }, /* 29: Right */
  // 3rd row (keys 30 to 43)
  { 0x33,   4,  98, 31, 31, &mo5_kb[43], &mo5_kb[31], &mo5_kb[15], &mo5_kb[44] }, /* 30: RAZ */
  { 0x11,  36,  98, 31, 31, &mo5_kb[30], &mo5_kb[32], &mo5_kb[16], &mo5_kb[44] }, /* 31: Back */
  { 0x2b,  68,  98, 31, 31, &mo5_kb[31], &mo5_kb[33], &mo5_kb[17], &mo5_kb[45] }, /* 32: Q */
  { 0x23, 100,  98, 31, 31, &mo5_kb[32], &mo5_kb[34], &mo5_kb[18], &mo5_kb[46] }, /* 33: S */
  { 0x1b, 132,  98, 31, 31, &mo5_kb[33], &mo5_kb[35], &mo5_kb[19], &mo5_kb[47] }, /* 34: D */
  { 0x13, 164,  98, 31, 31, &mo5_kb[34], &mo5_kb[36], &mo5_kb[20], &mo5_kb[48] }, /* 35: F */
  { 0x0b, 196,  98, 31, 31, &mo5_kb[35], &mo5_kb[37], &mo5_kb[21], &mo5_kb[49] }, /* 36: G */
  { 0x03, 228,  98, 31, 31, &mo5_kb[36], &mo5_kb[38], &mo5_kb[22], &mo5_kb[50] }, /* 37: H */
  { 0x02, 260,  98, 31, 31, &mo5_kb[37], &mo5_kb[39], &mo5_kb[23], &mo5_kb[51] }, /* 38: J */
  { 0x0a, 292,  98, 31, 31, &mo5_kb[38], &mo5_kb[40], &mo5_kb[24], &mo5_kb[52] }, /* 39: K */
  { 0x12, 324,  98, 31, 31, &mo5_kb[39], &mo5_kb[41], &mo5_kb[25], &mo5_kb[53] }, /* 40: L */
  { 0x1a, 356,  98, 31, 31, &mo5_kb[40], &mo5_kb[42], &mo5_kb[26], &mo5_kb[54] }, /* 41: M */
  { 0x34, 388,  98, 48, 31, &mo5_kb[41], &mo5_kb[43], &mo5_kb[27], &mo5_kb[54] }, /* 42: Entree */
  { 0x21, 452,  98, 31, 31, &mo5_kb[42], &mo5_kb[30], &mo5_kb[28], &mo5_kb[55] }, /* 43: Down */
  // 4rd row (keys 44 to 56)
  { 0x38,   4, 130, 62, 31, &mo5_kb[56], &mo5_kb[45], &mo5_kb[30], &mo5_kb[0]  }, /* 44: Shift */
  { 0x30,  84, 130, 31, 31, &mo5_kb[44], &mo5_kb[46], &mo5_kb[32], &mo5_kb[2]  }, /* 45: W */
  { 0x28, 116, 130, 31, 31, &mo5_kb[45], &mo5_kb[47], &mo5_kb[33], &mo5_kb[3]  }, /* 46: X */
  { 0x32, 148, 130, 31, 31, &mo5_kb[46], &mo5_kb[48], &mo5_kb[34], &mo5_kb[57]  }, /* 47: C */
  { 0x2a, 180, 130, 31, 31, &mo5_kb[47], &mo5_kb[49], &mo5_kb[35], &mo5_kb[57] }, /* 48: V */
  { 0x22, 212, 130, 31, 31, &mo5_kb[48], &mo5_kb[50], &mo5_kb[36], &mo5_kb[57] }, /* 49: B */
  { 0x00, 244, 130, 31, 31, &mo5_kb[49], &mo5_kb[51], &mo5_kb[37], &mo5_kb[57] }, /* 50: N */
  { 0x08, 276, 130, 31, 31, &mo5_kb[50], &mo5_kb[52], &mo5_kb[38], &mo5_kb[57]  }, /* 51: Comma */
  { 0x10, 308, 130, 31, 31, &mo5_kb[51], &mo5_kb[53], &mo5_kb[39], &mo5_kb[9]  }, /* 52: Dot */
  { 0x18, 340, 130, 31, 31, &mo5_kb[52], &mo5_kb[54], &mo5_kb[40], &mo5_kb[10] }, /* 53: At */
  { 0x39, 372, 130, 62, 31, &mo5_kb[53], &mo5_kb[55], &mo5_kb[42], &mo5_kb[12] }, /* 54: BASIC */
  { 0x09, 436, 130, 31, 31, &mo5_kb[54], &mo5_kb[56], &mo5_kb[43], &mo5_kb[13] }, /* 55: INS */
  { 0x01, 468, 130, 31, 31, &mo5_kb[55], &mo5_kb[44], &mo5_kb[43], &mo5_kb[14] }, /* 56: EFF */
  // Last row
  { 0x20, 164, 161,128, 32, &mo5_kb[57], &mo5_kb[57], &mo5_kb[49], &mo5_kb[6] },  /* 57: Space */
};
