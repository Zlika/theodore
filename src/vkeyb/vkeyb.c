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

#include "vkeyb.h"
#include "vkeyb_config.h"
#include "vkeyb_layout.h"
#include "ui.h"

#include "bmp_keyboard_mo5.inc"

static const struct VKey *current_key = &mo5_kb[0];

static uint16_t color_select = 0xFFC0;

void configure_virtual_keyboard(uint16_t *video_buffer, int width, int height)
{
  vkb_video_buffer = video_buffer;
  vkb_screen_width = width;
  vkb_screen_height = height;
}

void set_virtual_keyboard_transparency(unsigned int alpha)
{
  if (alpha > 255)
  {
    vkb_alpha = 255;
  }
  else
  {
    vkb_alpha = alpha;
  }
}

void show_virtual_keyboard(void)
{
  int keyb_x = (vkb_screen_width - KEYB_MO5_IMG_WIDTH) / 2;
  int keyb_y = vkb_screen_height - KEYB_MO5_IMG_HEIGHT;
  draw_bmp(keyb_x, keyb_y, KEYB_MO5_IMG_DATA, KEYB_MO5_IMG_WIDTH, KEYB_MO5_IMG_HEIGHT);
  draw_box(keyb_x+current_key->x, keyb_y+current_key->y, current_key->width, current_key->height, color_select);
}

void move_key(enum VkbMoveDirection direction)
{
  switch (direction)
  {
    case VKB_MOVE_LEFT:
      current_key = current_key->left;
      break;
    case VKB_MOVE_RIGHT:
      current_key = current_key->right;
      break;
    case VKB_MOVE_UP:
      current_key = current_key->up;
      break;
    case VKB_MOVE_DOWN:
      current_key = current_key->down;
      break;
  }
}

int get_current_key_scancode(void)
{
  return current_key->scancode;
}
