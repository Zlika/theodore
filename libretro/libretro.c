/*
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

#include <string.h>
#include "libretro.h"
#include "../config.h"
#include "../src/dc6809emul.h"
#include "../src/dcto8dglobal.h"
#include "../src/dcto8ddevices.h"
#include "../src/dcto8demulation.h"
#include "../src/dcto8dvideo.h"
#include "../src/dcto8doptions.h"

#define MAX_CONTROLLERS 2

static retro_log_printf_t log_cb = NULL;
static retro_environment_t environ_cb = NULL;
static retro_video_refresh_t video_cb = NULL;
static retro_audio_sample_t audio_cb = NULL;
static retro_audio_sample_batch_t audio_batch_cb = NULL;
static retro_input_poll_t input_poll_cb = NULL;
static retro_input_state_t input_state_cb = NULL;

static unsigned int input_type[MAX_CONTROLLERS];
static uint32_t *video_buffer = NULL;

void retro_set_environment(retro_environment_t env)
{
  // Emulator can be started without loading a game
  bool no_rom = true;
  env(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_rom);

  // Emulator's preferences
  static const struct retro_variable vars[] = {
      { PACKAGE_NAME"_fd_write", "Floppy write protection; enabled|disabled" },
      { PACKAGE_NAME"_k7_write", "Tape write protection; enabled|disabled" },
      { NULL, NULL }
  };
  env(RETRO_ENVIRONMENT_SET_VARIABLES, (void *) vars);

  environ_cb = env;
}

void retro_set_video_refresh(retro_video_refresh_t video_refresh)
{
  video_cb = video_refresh;
}

void retro_set_audio_sample(retro_audio_sample_t audio_sample)
{
  audio_cb = audio_sample;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t audio_sample_batch)
{
  audio_batch_cb = audio_sample_batch;
}

void retro_set_input_poll(retro_input_poll_t input_poll)
{
  input_poll_cb = input_poll;
}

void retro_set_input_state(retro_input_state_t input_state)
{
  input_state_cb = input_state;
}

void retro_init(void)
{
  unsigned int level = 4;
  struct retro_log_callback log;
  if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
  {
    log_cb = log.log;
  }
  else
  {
    log_cb = NULL;
  }
  environ_cb(RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL, &level);

  char dummy[256] = {0};
  Initoptions(dummy, dummy);
  Init6809();
  log_cb(RETRO_LOG_INFO, "Init6809 ok\n");
  Hardreset();
  log_cb(RETRO_LOG_INFO, "Hardreset ok\n");
  video_buffer = CreateLibRetroVideoBuffer();
  log_cb(RETRO_LOG_INFO, "Video buffer ok\n");
}

void retro_deinit(void)
{
  if (video_buffer)
  {
    free(video_buffer);
  }
}

unsigned retro_api_version(void)
{
  return RETRO_API_VERSION;
}

void retro_get_system_info(struct retro_system_info *info)
{
  memset(info, 0, sizeof(*info));
  info->library_name = PACKAGE_NAME;
  info->library_version = PACKAGE_VERSION;
  info->need_fullpath = true;
  info->valid_extensions = "fd|k7|rom";
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
  memset(info, 0, sizeof(*info));
  info->timing.fps = 50.0;
  info->timing.sample_rate = 22050;
  info->geometry.base_width = XBITMAP;
  info->geometry.base_height = YBITMAP * 2;
  info->geometry.max_width = XBITMAP;
  info->geometry.max_height = YBITMAP * 2;
  info->geometry.aspect_ratio = (float) XBITMAP / (YBITMAP * 2.0);
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
  if (port < MAX_CONTROLLERS)
  {
    input_type[port] = device;
  }
}

void retro_reset(void)
{
  Unloadk7();
  Unloadfd();
  Unloadmemo();
  Hardreset();
}

void retro_run(void)
{
  Run(19845);
  video_cb(video_buffer, XBITMAP, YBITMAP * 2, sizeof(uint32_t)*XBITMAP);
}

size_t retro_serialize_size(void)
{
  return 0;
}

bool retro_serialize(void *data, size_t size)
{
  return false;
}

bool retro_unserialize(const void *data, size_t size)
{
  return false;
}

void retro_cheat_reset(void)
{
}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
}

// Load file with auto-detection of type based on the file extension:
// k7 (*.k7), fd (*.fd) or memo7 (*.rom)
static bool load_file(const char *filename)
{
  if(strlen(filename) > 3 && !strcmp(filename + strlen(filename) - 3, ".k7"))
  {
    Loadk7(filename);
  }
  else if(strlen(filename) > 3 && !strcmp(filename + strlen(filename) - 3, ".fd"))
  {
    Loadfd(filename);
  }
  else if(strlen(filename) > 4 && !strcmp(filename + strlen(filename) - 4, ".rom"))
  {
    Loadmemo(filename);
  }
  else
  {
    if (log_cb)
    {
      log_cb(RETRO_LOG_ERROR, "Unknown file type for file %s.\n", filename);
    }
    return false;
  }
  return true;
}

static void check_variables(void)
{
  struct retro_variable var = {0};

  var.key = PACKAGE_NAME"_fd_write";
  if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
  {
    options.fdprotection = (strcmp(var.value, "enabled") == 0);
  }
  var.key = PACKAGE_NAME"_k7_write";
  if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
  {
    options.k7protection = (strcmp(var.value, "enabled") == 0);
  }
}

bool retro_load_game(const struct retro_game_info *game)
{
  enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
  if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt) && log_cb)
  {
    log_cb(RETRO_LOG_ERROR, "XRGB8888 is not supported.\n");
    return false;
  }

  check_variables();

  if (game && game->path)
  {
    if (log_cb)
    {
      log_cb(RETRO_LOG_INFO, "Loading file %s.\n", game->path);
    }

    return load_file(game->path);
  }
  return true;
}

bool retro_load_game_special(
  unsigned game_type,
  const struct retro_game_info *info, size_t num_info)
{
  return false;
}

void retro_unload_game(void)
{
  Unloadk7();
  Unloadfd();
  Unloadmemo();
}

unsigned retro_get_region(void)
{
  return 0;
}

void *retro_get_memory_data(unsigned id)
{
  switch (id)
  {
    case RETRO_MEMORY_SYSTEM_RAM:
      return ram;
  }
  return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
  switch (id)
  {
    case RETRO_MEMORY_SYSTEM_RAM:
      return RAM_SIZE;
  }
  return 0;
}
