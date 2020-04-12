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

/* Virtual keyboard */

#ifndef __VKEYB_H
#define __VKEYB_H

#include <stdint.h>

// Directions to move the currently selected key
enum VkbMoveDirection { VKB_MOVE_LEFT, VKB_MOVE_RIGHT, VKB_MOVE_UP, VKB_MOVE_DOWN };

// Configure the virtual keyboard feature
extern void configure_virtual_keyboard(uint16_t *video_buffer, int width, int height);
// Set the virtual keyboard transparency (0 = transparent, 255 = opaque)
extern void set_virtual_keyboard_transparency(unsigned int alpha);
// Show the virtual keyboard during the current video frame
extern void show_virtual_keyboard(void);
// Move the selected key in the given direction
extern void move_key(enum VkbMoveDirection direction);
// Return the scancode of the currently selected key
extern int get_current_key_scancode(void);

#endif /* __VKEYB_H */
