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
#include "bmp_keyboard_mo6.inc"
#include "bmp_keyboard_pc128.inc"
#include "bmp_keyboard_to7.inc"
#include "bmp_keyboard_to770.inc"
#include "bmp_keyboard_to8.inc"

static const uint16_t *current_kb_image_data = 0;
static int current_kb_width = 0;
static int current_kb_height = 0;
static const struct VKey *current_key = 0;
static const struct VKey* sticky_keys[VKB_MAX_STICKY_KEYS] = { 0 };
static enum VkbPosition vkb_position = VKB_POS_DOWN;
static const struct VKey *current_keyboard_layout = 0;
static int current_keyboard_keys = 0;

static uint16_t color_select = 0xFFC0;
static uint16_t color_hold = 0x06DF;
static int box_thickness = 2;

void vkb_configure_virtual_keyboard(uint16_t *video_buffer, int width, int height)
{
  vkb_video_buffer = video_buffer;
  vkb_screen_width = width;
  vkb_screen_height = height;
  vkb_set_virtual_keyboard_model(VKB_MODEL_TO8);
}

void vkb_set_virtual_keyboard_model(enum VkbModel model)
{
  vkb_release_all_sticky_keys();

  switch (model)
  {
    case VKB_MODEL_MO5:
      current_kb_image_data = KEYB_MO5_IMG_DATA;
      current_kb_width = KEYB_MO5_IMG_WIDTH;
      current_kb_height = KEYB_MO5_IMG_HEIGHT;
      current_key = MO5_DEFAULT_KEY;
      current_keyboard_layout = mo5_kb;
      current_keyboard_keys = MO5_KB_KEYS;
      break;
    case VKB_MODEL_MO6:
      current_kb_image_data = KEYB_MO6_IMG_DATA;
      current_kb_width = KEYB_MO6_IMG_WIDTH;
      current_kb_height = KEYB_MO6_IMG_HEIGHT;
      current_key = MO6_DEFAULT_KEY;
      current_keyboard_layout = mo6_kb;
      current_keyboard_keys = MO6_KB_KEYS;
      break;
    case VKB_MODEL_PC128:
      current_kb_image_data = KEYB_PC128_IMG_DATA;
      current_kb_width = KEYB_PC128_IMG_WIDTH;
      current_kb_height = KEYB_PC128_IMG_HEIGHT;
      current_key = MO6_DEFAULT_KEY;
      current_keyboard_layout = mo6_kb;
      current_keyboard_keys = MO6_KB_KEYS;
      break;
    case VKB_MODEL_TO7:
      current_kb_image_data = KEYB_TO7_IMG_DATA;
      current_kb_width = KEYB_TO7_IMG_WIDTH;
      current_kb_height = KEYB_TO7_IMG_HEIGHT;
      current_key = TO7_DEFAULT_KEY;
      current_keyboard_layout = to7_kb;
      current_keyboard_keys = TO7_KB_KEYS;
      break;
    case VKB_MODEL_TO770:
      current_kb_image_data = KEYB_TO770_IMG_DATA;
      current_kb_width = KEYB_TO770_IMG_WIDTH;
      current_kb_height = KEYB_TO770_IMG_HEIGHT;
      current_key = TO7_DEFAULT_KEY;
      current_keyboard_layout = to7_kb;
      current_keyboard_keys = TO7_KB_KEYS;
      break;
    default:
      current_kb_image_data = KEYB_TO8_IMG_DATA;
      current_kb_width = KEYB_TO8_IMG_WIDTH;
      current_kb_height = KEYB_TO8_IMG_HEIGHT;
      current_key = TO8_DEFAULT_KEY;
      current_keyboard_layout = to8_kb;
      current_keyboard_keys = TO8_KB_KEYS;
  }
}

void vkb_set_virtual_keyboard_transparency(unsigned int alpha)
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

void vkb_set_virtual_keyboard_position(enum VkbPosition position)
{
  vkb_position = position;
}

enum VkbPosition vkb_get_virtual_keyboard_position(void)
{
  return vkb_position;
}

void vkb_show_virtual_keyboard(void)
{
  int keyb_x, keyb_y;
  int i;
  // Draw keyboard
  keyb_x = (vkb_screen_width - current_kb_width) / 2;
  keyb_y = (vkb_position == VKB_POS_DOWN) ? vkb_screen_height - current_kb_height : 0;
  draw_bmp(keyb_x, keyb_y, current_kb_image_data, current_kb_width, current_kb_height);

  // Draw held keys
  for (i = 0; i < VKB_MAX_STICKY_KEYS; i++)
  {
    if (sticky_keys[i] != 0)
    {
      draw_box(keyb_x+sticky_keys[i]->x+box_thickness, keyb_y+sticky_keys[i]->y+box_thickness,
               sticky_keys[i]->width-2*box_thickness, sticky_keys[i]->height-2*box_thickness,
               box_thickness, color_hold);
    }
  }

  // Drawn current position
  draw_box(keyb_x+current_key->x, keyb_y+current_key->y,
           current_key->width, current_key->height, box_thickness, color_select);
}

void vkb_move_key(enum VkbMoveDirection direction)
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

int vkb_get_current_key_scancode(void)
{
  return current_key->scancode;
}

bool vkb_move_at(int x, int y)
{
  int i;
  int y_offset = y - ((vkb_position == VKB_POS_DOWN) ? vkb_screen_height - current_kb_height : 0);
  for (i = 0; i < current_keyboard_keys; i++)
  {
    const struct VKey *key = &current_keyboard_layout[i];
    if ((key->x <= x) && (x <= key->x + key->width)
        && (key->y <= y_offset) && (y_offset <= key->y + key->height))
    {
      current_key = key;
      return true;
    }
  }
  return false;
}

bool vkb_make_current_key_sticky(void)
{
  int i;
  // If key already held
  for (i = 0; i < VKB_MAX_STICKY_KEYS; i++)
  {
    if ((sticky_keys[i] != 0) && (sticky_keys[i]->scancode == current_key->scancode))
    {
      sticky_keys[i] = 0;
      return true;
    }
  }
  // If key not already held
  for (i = 0; i < VKB_MAX_STICKY_KEYS; i++)
  {
    if (sticky_keys[i] == 0)
    {
      sticky_keys[i] = current_key;
      return true;
    }
  }
  return false;
}

void vkb_get_current_sticky_keys_scancode(int *scancodes)
{
  int i;
  for (i = 0; i < VKB_MAX_STICKY_KEYS; i++)
  {
    if (sticky_keys[i] != 0)
    {
      scancodes[i] = sticky_keys[i]->scancode;
    }
    else
    {
      scancodes[i] = -1;
    }
  }
}

bool vkb_is_key_sticky(int scancode)
{
  int i;
  for (i = 0; i < VKB_MAX_STICKY_KEYS; i++)
  {
    if (sticky_keys[i] && (sticky_keys[i]->scancode == scancode))
    {
      return true;
    }
  }
  return false;
}

void vkb_release_all_sticky_keys(void)
{
  int i;
  for (i = 0; i < VKB_MAX_STICKY_KEYS; i++)
  {
    sticky_keys[i] = 0;
  }
}
