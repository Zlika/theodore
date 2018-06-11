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

#include "libretro-common/include/libretro.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "devices.h"
#include "keymap.h"
#include "sap.h"
#include "to8demulator.h"
#include "video.h"

#define PACKAGE_NAME "theodore"
#ifdef GIT_VERSION
#define PACKAGE_VERSION GIT_VERSION
#else
#define PACKAGE_VERSION "unknown"
#endif

#ifdef _3DS
extern "C" void* linearMemAlign(size_t size, size_t alignment);
extern "C" void linearFree(void* mem);
#endif

#define MAX_CONTROLLERS   2
#define VIDEO_FPS         50
#define AUDIO_SAMPLE_RATE 22050
#define AUDIO_SAMPLE_PER_FRAME AUDIO_SAMPLE_RATE / VIDEO_FPS
#define CPU_FREQUENCY     1000
#define MAX_CHEATS        10
#define CHEAT_LENGTH      9
#define CHEAT_SEP_POS     6

static retro_log_printf_t log_cb = NULL;
static retro_environment_t environ_cb = NULL;
static retro_video_refresh_t video_cb = NULL;
static retro_audio_sample_t audio_cb = NULL;
static retro_audio_sample_batch_t audio_batch_cb = NULL;
static retro_input_poll_t input_poll_cb = NULL;
static retro_input_state_t input_state_cb = NULL;

static unsigned int input_type[MAX_CONTROLLERS];
static uint32_t *video_buffer = NULL;
static int16_t audio_stereo_buffer[2*(AUDIO_SAMPLE_RATE / VIDEO_FPS)];

// nb of thousandth of cycles in excess to run the next time
static int excess;
// current index in virtualkb_* arrays
static int virtualkb_index = 0;
// true if a key of the virtual keyboard was being pressed during the last call of update_input()
static bool virtualkb_pressed = false;

typedef struct
{
  int address;
  char value;
} Cheat;
static Cheat cheats[MAX_CHEATS] = { {0} };

void retro_set_environment(retro_environment_t env)
{
  // Emulator can be started without loading a game
  bool no_rom = true;
  env(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_rom);

  // Emulator's preferences
  static const struct retro_variable vars[] = {
      { PACKAGE_NAME"_rom", "Thomson flavor; TO8|TO8D" },
      { PACKAGE_NAME"_floppy_write_protect", "Floppy write protection; enabled|disabled" },
      { PACKAGE_NAME"_tape_write_protect", "Tape write protection; enabled|disabled" },
      { PACKAGE_NAME"_printer_emulation", "Dump printer data to file; disabled|enabled" },
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

  struct retro_input_descriptor desc[] = {
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "Left" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "Up" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "Down" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "Fire" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B Key" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT,"Virtual Keyboard: Change Letter (Up)" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Virtual Keyboard: Press Letter" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,     "Virtual Keyboard: Change Letter (Up)" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,     "Virtual Keyboard: Change Letter (Down)" },

        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "Left" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "Up" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "Down" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "Fire" },

        { 2, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_X, "Light Pen X" },
        { 2, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_Y, "Light Pen Y" },
        { 2, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_PRESSED, "Light Pen Button" },

        { 0 },
  };
  environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

  Hardreset();
#ifdef _3DS
  video_buffer = (uint32_t*)linearMemAlign(XBITMAP * YBITMAP * sizeof(uint32_t), 0x80);
#else
  video_buffer = (uint32_t *)malloc(XBITMAP * YBITMAP * sizeof(uint32_t));
#endif
  int size = SetLibRetroVideoBuffer(video_buffer);
  struct retro_message msg;
  char str[255];
  sprintf(str, "sizeof struc pix = %d", size);
  msg.msg = str;
  msg.frames = VIDEO_FPS * 30;
  environ_cb(RETRO_ENVIRONMENT_SET_MESSAGE, &msg);
}

void retro_deinit(void)
{
  if (video_buffer)
  {
#ifdef _3DS
    linearFree(video_buffer);
#else
    free(video_buffer);
#endif
    video_buffer = NULL;
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
  info->valid_extensions = "fd|sap|k7|m7|rom";
  info->need_fullpath = true;
  info->block_extract = false;
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
  memset(info, 0, sizeof(*info));
  info->timing.fps = VIDEO_FPS;
  info->timing.sample_rate = AUDIO_SAMPLE_RATE;
  info->geometry.base_width = XBITMAP;
  info->geometry.base_height = YBITMAP;
  info->geometry.max_width = XBITMAP;
  info->geometry.max_height = YBITMAP;
  info->geometry.aspect_ratio = (float) XBITMAP / (float) YBITMAP;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
  if (port < MAX_CONTROLLERS)
  {
    input_type[port] = device;
  }
}

void retro_cheat_reset(void)
{
  if (log_cb) log_cb(RETRO_LOG_INFO, "Reset cheats.\n");
  memset(cheats, 0, sizeof(cheats));
}

static bool parse_cheat(const char *code, Cheat *cheat)
{
  if ((strlen(code) != CHEAT_LENGTH) || (code[CHEAT_SEP_POS] != '-'))
  {
    return false;
  }
  cheat->address = strtol(code, NULL, 16);
  cheat->value = (char) strtol(code + CHEAT_SEP_POS + 1, NULL, 16);
  if (cheat->address <= 0 || cheat->address >= RAM_SIZE)
  {
    return false;
  }
  return true;
}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
  Cheat cheat;
  if (log_cb) log_cb(RETRO_LOG_INFO, "Setting cheat %s at index %d, enabled=%s.\n",
                    code, index, enabled ? "yes" : "no");
  if ((index < MAX_CHEATS) && enabled)
  {
    if (!parse_cheat(code, &cheat))
    {
      if (log_cb) log_cb(RETRO_LOG_ERROR, "Wrong cheat format");
      return;
    }
    cheats[index] = cheat;
  }
}

void apply_cheats()
{
  int i;
  for (int i = 0; i < MAX_CHEATS; i++)
  {
    if (cheats[i].address == 0)
    {
      return;
    }
    ram[cheats[i].address] = cheats[i].value;
  }
}

void retro_reset(void)
{
  Hardreset();
}

static void pointerToScreenCoordinates(int *x, int *y)
{
  *x = (*x + 0x7FFF) * XBITMAP / 0xFFFF;
  *y = (*y + 0x7FFF) * YBITMAP / 0xFFFF;
}

static void print_current_virtualkb_key()
{
  struct retro_message msg;
  msg.msg = virutalkb_chars[virtualkb_index];
  msg.frames = VIDEO_FPS;
  environ_cb(RETRO_ENVIRONMENT_SET_MESSAGE, &msg);
}

static void update_input(void)
{
  int i;
  input_poll_cb();
  // Joysticks
  for (i = 0; i < MAX_CONTROLLERS; i++)
  {
    Joysemul(JOY0_UP + 4*i, input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP));
    Joysemul(JOY0_DOWN + 4*i, input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN));
    Joysemul(JOY0_LEFT + 4*i, input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT));
    Joysemul(JOY0_RIGHT + 4*i, input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT));
    Joysemul(JOY0_FIRE + i, input_state_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A));
  }
  // Light pen
  int xpointer = input_state_cb(MAX_CONTROLLERS, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_X);
  int ypointer = input_state_cb(MAX_CONTROLLERS, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_Y);
  pointerToScreenCoordinates(&xpointer, &ypointer);
  xpen = xpointer - 16;
  ypen = (ypointer - 16) / 2;
  penbutton = input_state_cb(MAX_CONTROLLERS, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_PRESSED);
  // Virtual keyboard
  // Emulation of the B key with the B button of the joypad
  if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B))
  {
    TO8key(libretroKeyCodeToThomsonScanCode[RETROK_b], true);
  }
  // Change letter with Select/X/Y and press it with Start
  bool select = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT);
  bool start = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START);
  bool x = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X);
  bool y = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y);
  if (!virtualkb_pressed)
  {
    if (select || x)
    {
      virtualkb_index = (virtualkb_index + 1) % VIRTUALKB_NB_KEYS;
      print_current_virtualkb_key();
      virtualkb_pressed = true;
    }
    else if (y)
    {
      virtualkb_index = ((virtualkb_index - 1) % VIRTUALKB_NB_KEYS + VIRTUALKB_NB_KEYS) % VIRTUALKB_NB_KEYS;
      print_current_virtualkb_key();
      virtualkb_pressed = true;
    }
    else if (start)
    {
      TO8key(libretroKeyCodeToThomsonScanCode[virtualkb_keysyms[virtualkb_index]], true);
      virtualkb_pressed = true;
    }
  }
  else
  {
    if (!select && !start && !x && !y)
    {
      virtualkb_pressed = false;
    }
  }
}

static void check_variables(void)
{
  struct retro_variable var = {0};

  var.key = PACKAGE_NAME"_floppy_write_protect";
  if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
  {
    SetFloppyWriteProtect(strcmp(var.value, "enabled") == 0);
  }
  var.key = PACKAGE_NAME"_tape_write_protect";
  if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
  {
    SetTapeWriteProtect(strcmp(var.value, "enabled") == 0);
  }
  var.key = PACKAGE_NAME"_printer_emulation";
  if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
  {
    SetPrinterEmulationEnabled(strcmp(var.value, "enabled") == 0);
  }
  var.key = PACKAGE_NAME"_rom";
  if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
  {
    if (strcmp(var.value, "TO8D") == 0)
    {
      SetThomsonFlavor(TO8D);
    }
    else if (strcmp(var.value, "TO8") == 0)
    {
      SetThomsonFlavor(TO8);
    }
  }
}

// Converts an 8-bit unsigned audio sample (as produced by the emulator)
// into a 16-bit signed audio sample (as expected by the libretro audio callback).
static int16_t u8toS16_audio_sample(uint8_t sample)
{
  return (sample * 65535 / 255) - (65536 / 2);
}

void retro_run(void)
{
  int i;
  int mcycles; // nb of thousandths of cycles between 2 samples
  int icycles; // integer number of cycles between 2 samples
  int16_t audio_sample;
  // 45 cycles of the 6809 at 992250 Hz = one sample at 22050 Hz
  for(i = 0; i < AUDIO_SAMPLE_PER_FRAME; i++)
  {
    // Computes the nb of cycles between 2 samples and runs the emulation for this nb of cycles
    // Nb of theoretical cycles for this period of time =
    // theoretical number + previous remaining - cycles in excess during the previous period
    mcycles = CPU_FREQUENCY * 100000 / 2205; // theoretical thousandths of cycles
    mcycles += excess;                       // corrected thousandths of cyces
    icycles = mcycles / 1000;                // integer number of cycles to run
    excess = mcycles - 1000 * icycles;       // remaining to do the next time
    excess -= 1000 * Run(icycles);           // remove thousandths in excess
    audio_sample = u8toS16_audio_sample(sound + 96);
    audio_stereo_buffer[(i << 1) + 0] = audio_stereo_buffer[(i << 1) + 1] = audio_sample;
  }

  update_input();
  apply_cheats();

  bool updated = false;
  if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
  {
    check_variables();
  }

  audio_batch_cb(audio_stereo_buffer, AUDIO_SAMPLE_PER_FRAME);
  video_cb(video_buffer, XBITMAP, YBITMAP, sizeof(uint32_t)*XBITMAP);
}

size_t retro_serialize_size(void)
{
  return to8d_serialize_size();
}

bool retro_serialize(void *data, size_t size)
{
  if (size != to8d_serialize_size()) return false;
  to8d_serialize(data);
  return true;
}

bool retro_unserialize(const void *data, size_t size)
{
  if (size != to8d_serialize_size()) return false;
  to8d_unserialize(data);
  return true;
}

static bool exists(const char *filename)
{
    FILE *file;
    if ((file = fopen(filename, "r")))
    {
        fclose(file);
        return true;
    }
    return false;
}

static bool streq_nocase(const char *s1, const char *s2)
{
  int i;
  for (i = 0; s1[i] != '\0'; i++)
  {
    if (toupper(s1[i]) != toupper(s2[i]))
    {
      return false;
    }
  }
  return true;
}

// Load file with auto-detection of type based on the file extension:
// k7 (*.k7), fd (*.fd) or memo7 (*.rom)
static bool load_file(const char *filename)
{
  char fd_filename[256];
  size_t filename_size;

  if (strlen(filename) > 3 && streq_nocase(filename + strlen(filename) - 3, ".k7"))
  {
    Loadk7(filename);
  }
  else if (strlen(filename) > 3 && streq_nocase(filename + strlen(filename) - 3, ".fd"))
  {
    Loadfd(filename);
  }
  else if (strlen(filename) > 4 && (streq_nocase(filename + strlen(filename) - 4, ".rom")
      || streq_nocase(filename + strlen(filename) - 3, ".m7")))
  {
    Loadmemo(filename);
  }
  else if (strlen(filename) > 4 && streq_nocase(filename + strlen(filename) - 4, ".sap"))
  {
    filename_size = strlen(filename);
    strcpy(fd_filename, filename);
    fd_filename[filename_size - 3] = 'f';
    fd_filename[filename_size - 2] = 'd';
    fd_filename[filename_size - 1] = '\0';
    if (!exists(fd_filename))
    {
      if (log_cb) log_cb(RETRO_LOG_INFO, "Converting SAP file to FD format.\n");
      if (!sap2fd(filename, fd_filename))
      {
        if (log_cb) log_cb(RETRO_LOG_ERROR, "Cannot convert file to SAP format.\n");
        return false;
      }
    }
    Loadfd(fd_filename);
  }
  else
  {
    if (log_cb) log_cb(RETRO_LOG_ERROR, "Unknown file type for file %s.\n", filename);
    return false;
  }
  return true;
}

static void keyboard_cb(bool down, unsigned keycode,
    uint32_t character, uint16_t key_modifiers)
{
  //printf( "Down: %s, Code: %d, Char: %u, Mod: %u.\n",
  //        down ? "yes" : "no", keycode, character, key_modifiers);

  // Thomson <-> PC keyboard mapping for special keys
  // STOP <-> TAB
  // CNT <-> CTRL
  // CAPSLOCK <-> CAPSLOCK
  // ACC <-> ALT
  // HOME <-> HOME
  // Arrows <-> arrows
  // INS <-> INSERT
  // EFF <-> DEL
  // F1-F5 <-> F1-F5
  // F6-F10 <-> SHIFT+F1-F5
  if (key_modifiers & RETROKMOD_SHIFT)
  {
    TO8key(THOMSON_LEFT_SHIFT, down);
  }
  if (key_modifiers & RETROKMOD_CTRL)
  {
    TO8key(THOMSON_CNT, down);
  }
  if (key_modifiers & RETROKMOD_ALT)
  {
    TO8key(THOMSON_ACC, down);
  }
  if (key_modifiers & RETROKMOD_CAPSLOCK)
  {
    TO8key(THOMSON_CAPSLOCK, down);
  }

  if (keycode < 320)
  {
    unsigned char scancode = libretroKeyCodeToThomsonScanCode[keycode];
    if (scancode != 0xFF)
    {
      TO8key(scancode, down);
    }
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

  struct retro_keyboard_callback cb = { keyboard_cb };
  environ_cb(RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK, &cb);

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
  return RETRO_REGION_PAL;
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
