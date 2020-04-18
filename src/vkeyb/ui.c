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

/* Utility functions to write UI elements on the screen */

#include "ui.h"
#include "vkeyb_config.h"

static uint16_t blend(uint16_t fg, uint16_t bg, unsigned int alpha)
{
  if (alpha == 255)
  {
    return fg;
  }
  // Split foreground into components
  unsigned fg_r = fg >> 11;
  unsigned fg_g = (fg >> 5) & ((1u << 6) - 1);
  unsigned fg_b = fg & ((1u << 5) - 1);

  // Split background into components
  unsigned bg_r = bg >> 11;
  unsigned bg_g = (bg >> 5) & ((1u << 6) - 1);
  unsigned bg_b = bg & ((1u << 5) - 1);

  // Alpha blend components
  unsigned out_r = (fg_r * alpha + bg_r * (255 - alpha)) / 255;
  unsigned out_g = (fg_g * alpha + bg_g * (255 - alpha)) / 255;
  unsigned out_b = (fg_b * alpha + bg_b * (255 - alpha)) / 255;

  // Pack result
  return (unsigned short) ((out_r << 11) | (out_g << 5) | out_b);
}

void draw_bmp(int x, int y, const uint16_t *img, int img_width, int img_height)
{
  for (int j = 0; j < img_height; j++)
  {
    uint16_t *screen_line = vkb_video_buffer + ((y + j) * vkb_screen_width) + x;
    const uint16_t *img_line = img + j * img_width;
    for (int i = 0; i < img_width; i++)
    {
      screen_line[i] = blend(img_line[i], screen_line[i], vkb_alpha);
    }
  }
}

void draw_box(int x, int y, int width, int height, uint16_t color)
{
  uint16_t *screen_line_up = vkb_video_buffer + (y * vkb_screen_width);
  uint16_t *screen_line_down = vkb_video_buffer + ((y + height) * vkb_screen_width);
  for (int i = x; i <= x + width; i++)
  {
    screen_line_up[i] = blend(color, screen_line_up[i], vkb_alpha);
    screen_line_down[i] = blend(color, screen_line_down[i], vkb_alpha);
  }

  for (int j = y; j <= y + height; j++)
  {
    int offset = (j * vkb_screen_width) + x;
    vkb_video_buffer[offset] = blend(color, vkb_video_buffer[offset], vkb_alpha);
    vkb_video_buffer[offset + width] = blend(color, vkb_video_buffer[offset + width], vkb_alpha);
  }
}
