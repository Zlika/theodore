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
#include "libretro-common/include/boolean.h"

// Maximum number of keys to be hold
#define VKB_MAX_HOLD_KEYS 3

// Directions to move the currently selected key
enum VkbMoveDirection { VKB_MOVE_LEFT, VKB_MOVE_RIGHT, VKB_MOVE_UP, VKB_MOVE_DOWN };
// Positions of the virtual keyboard
enum VkbPosition { VKB_POS_DOWN, VKB_POS_UP };
// Virtual keyboard models
enum VkbModel { VKB_MODEL_MO5, VKB_MODEL_MO6, VKB_MODEL_PC128,
                VKB_MODEL_TO7, VKB_MODEL_TO770, VKB_MODEL_TO8 };

// Configure the virtual keyboard feature
extern void vkb_configure_virtual_keyboard(uint16_t *video_buffer, int width, int height);
// Set the virtual keyboard model
extern void vkb_set_virtual_keyboard_model(enum VkbModel model);
// Set the virtual keyboard transparency (0 = transparent, 255 = opaque)
extern void vkb_set_virtual_keyboard_transparency(unsigned int alpha);
// Set the position of the virtual keyboard
extern void vkb_set_virtual_keyboard_position(enum VkbPosition position);
// Get the currently configured position of the virtual keyboard
extern enum VkbPosition vkb_get_virtual_keyboard_position(void);

// Show the virtual keyboard during the current video frame
extern void vkb_show_virtual_keyboard(void);

// Move the selected key in the given direction
extern void vkb_move_key(enum VkbMoveDirection direction);
// Return the scancode of the currently selected key
extern int vkb_get_current_key_scancode(void);
// Move to the key at the x/y screen coordinates.
// Returns false if no key was found at these coordinates.
extern bool vkb_move_at(int x, int y);
// Hold (or unhold) the currently selected key.
// Returns false if too much keys are already held.
extern bool vkb_hold_current_key(void);
// Return the scancodes of the keys currently hold.
// scancodes array must be of size VKB_MAX_HOLD_KEYS.
// Array element is -1 if no key is held for this element.
extern void vkb_get_current_hold_keys_scancode(int *scancodes);
// Return true if the key with the given scancode is currently held.
extern bool vkb_is_key_held(int scancode);

#endif /* __VKEYB_H */
