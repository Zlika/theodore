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

#ifdef THEODORE_DASM
#include "debugger.h"
#endif
#include "autodetect.h"
#include "devices.h"
#include "keymap.h"
#include "sap.h"
#include "motoemulator.h"
#include "video.h"

#define PACKAGE_NAME "theodore"
#ifdef GIT_VERSION
#define PACKAGE_VERSION GIT_VERSION
#else
#define PACKAGE_VERSION "unknown"
#endif

#ifdef _3DS
void* linearMemAlign(size_t size, size_t alignment);
void linearFree(void* mem);
#endif

#define MAX_CONTROLLERS   2
#define VIDEO_FPS         50
#define AUDIO_SAMPLE_RATE 22050
#define AUDIO_SAMPLE_PER_FRAME AUDIO_SAMPLE_RATE / VIDEO_FPS
#define CPU_FREQUENCY     1000000
// Pitch = length in bytes between two lines in video buffer
#define PITCH             sizeof(uint32_t) * XBITMAP
// Autorun: Number of frames to wait before simulating
// the key stroke to start the program
#define AUTORUN_DELAY     70

static retro_log_printf_t log_cb = NULL;
static retro_environment_t environ_cb = NULL;
static retro_video_refresh_t video_cb = NULL;
static retro_audio_sample_t audio_cb = NULL;
static retro_audio_sample_batch_t audio_batch_cb = NULL;
static retro_input_poll_t input_poll_cb = NULL;
static retro_input_state_t input_state_cb = NULL;

static unsigned int input_type[MAX_CONTROLLERS];
static uint32_t *video_buffer = NULL;
static int16_t audio_stereo_buffer[2*AUDIO_SAMPLE_PER_FRAME];

// nb of thousandth of cycles in excess to run the next time
static int excess = 0;

// current index in virtualkb_* arrays
static int virtualkb_index = 0;
// true if a key of the virtual keyboard was being pressed during the last call of update_input()
static bool virtualkb_pressed = false;
// scancode of the last key simulated by the virtual keyboard
static int virtualkb_lastscancode = 0;
// Autorun counter
static int autorun_counter = -1;

#define MO5_AUTOSTART_BASIC_KEYS_LENGTH 5
// Key strokes to start a BASIC game on MO5: RUN"
static const int MO5_AUTOSTART_BASIC_KEYS[MO5_AUTOSTART_BASIC_KEYS_LENGTH] =
                                        { RETROK_r, RETROK_u, RETROK_n, RETROK_2, RETROK_RETURN };
#define MO5_AUTOSTART_BIN_KEYS_LENGTH 12
// Key strokes to start a BINARY game on MO5: LOADM"",,R
static const int MO5_AUTOSTART_BIN_KEYS[MO5_AUTOSTART_BIN_KEYS_LENGTH] =
                                        { RETROK_l, RETROK_o, RETROK_q, RETROK_d, RETROK_SEMICOLON,
                                          RETROK_2, RETROK_2, -1, RETROK_m, RETROK_m, RETROK_r, RETROK_RETURN };
static int mo5_autostart_keys_length = MO5_AUTOSTART_BASIC_KEYS_LENGTH;
static const int *mo5_autostart_keys = MO5_AUTOSTART_BASIC_KEYS;
static int current_mo5_autostart_key_pos = -1;

static const struct retro_variable prefs[] = {
    { PACKAGE_NAME"_rom", "Thomson model; Auto|TO8|TO8D|TO9|TO9+|MO5" },
    { PACKAGE_NAME"_autorun", "Auto run game; disabled|enabled" },
    { PACKAGE_NAME"_floppy_write_protect", "Floppy write protection; enabled|disabled" },
    { PACKAGE_NAME"_tape_write_protect", "Tape write protection; enabled|disabled" },
    { PACKAGE_NAME"_printer_emulation", "Dump printer data to file; disabled|enabled" },
#ifdef THEODORE_DASM
    { PACKAGE_NAME"_disassembler", "Interactive disassembler; disabled|enabled" },
    { PACKAGE_NAME"_break_illegal_opcode", "Break on illegal opcode; disabled|enabled" },
#endif
    { NULL, NULL }
};

typedef enum { NO_MEDIA, MEDIA_FLOPPY, MEDIA_TAPE, MEDIA_CARTRIDGE } Media;
static Media currentMedia = NO_MEDIA;

void retro_set_environment(retro_environment_t env)
{
  // Emulator can be started without loading a game
  bool no_rom = true;
  env(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_rom);

  // Emulator's preferences
  env(RETRO_ENVIRONMENT_SET_VARIABLES, (void *) prefs);

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
  struct retro_input_descriptor desc[] = {
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "Left" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "Up" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "Down" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "Fire" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT,"Virtual Keyboard: Press Letter" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start Program" },
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

        { 0, 0, 0, 0, 0 },
  };
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

  environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

  Hardreset();
#ifdef _3DS
  video_buffer = (uint32_t *)linearMemAlign(XBITMAP * YBITMAP * sizeof(uint32_t), 0x80);
#else
  video_buffer = (uint32_t *)malloc(XBITMAP * YBITMAP * sizeof(uint32_t));
#endif
  SetLibRetroVideoBuffer(video_buffer);
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
  info->valid_extensions = "fd|sap|k7|m7|m5|rom";
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
}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
  // Unused parameters
  (void) index;
  (void) enabled;
  (void) code;
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
  msg.msg = virtualkb_chars[virtualkb_index];
  msg.frames = VIDEO_FPS;
  environ_cb(RETRO_ENVIRONMENT_SET_MESSAGE, &msg);
}

static void autostart_mo5_begin(void)
{
  current_mo5_autostart_key_pos = 0;
  virtualkb_lastscancode = libretroKeyCodeToThomsonScanCode[mo5_autostart_keys[0]];
  keyboard(libretroKeyCodeToThomsonMoScanCode[RETROK_LSHIFT], true);
}

static void autostart_mo5_continue(void)
{
  if (current_mo5_autostart_key_pos >= 0)
  {
    if (mo5_autostart_keys[++current_mo5_autostart_key_pos] == -1)
    {
      // Special case: release the shift key
      keyboard(libretroKeyCodeToThomsonMoScanCode[RETROK_LSHIFT], false);
      return;
    }
    virtualkb_pressed = true;
    virtualkb_lastscancode = libretroKeyCodeToThomsonMoScanCode[
                                                     mo5_autostart_keys[current_mo5_autostart_key_pos]];
    keyboard(virtualkb_lastscancode, true);
    if (current_mo5_autostart_key_pos == mo5_autostart_keys_length - 1)
    {
      keyboard(libretroKeyCodeToThomsonMoScanCode[RETROK_LSHIFT], false);
      current_mo5_autostart_key_pos = -1;
    }
  }
}

// Try to start the currently loaded game by simulating keystrokes on the keyboard.
static void autostart_program(void)
{
  switch (currentMedia)
  {
    case MEDIA_FLOPPY:
      // Most games are started with the 'B' key (Basic 512) on TO8/TO8D/TO9+
      // and the 'D' key (Basic 128) on TO9
      if (GetThomsonModel() == TO9)
      {
        virtualkb_lastscancode = libretroKeyCodeToThomsonScanCode[RETROK_d];
      }
      else if (GetThomsonModel() == MO5)
      {
        autostart_mo5_begin();
      }
      else
      {
        virtualkb_lastscancode = libretroKeyCodeToThomsonScanCode[RETROK_b];
      }
      break;
    case MEDIA_TAPE:
      // Tapes are most often started with the BASIC 1.0
      // ('C' key on TO8/TO8D/TO9+, 'E' key on TO9)
      if (GetThomsonModel() == TO9)
      {
        virtualkb_lastscancode = libretroKeyCodeToThomsonScanCode[RETROK_e];
      }
      else if (GetThomsonModel() == MO5)
      {
        autostart_mo5_begin();
      }
      else
      {
        virtualkb_lastscancode = libretroKeyCodeToThomsonScanCode[RETROK_c];
      }
      break;
    case MEDIA_CARTRIDGE:
      // Cartridges are started by the '0' key
      // (on MO5, cartridge programs are already autostarted)
      if (GetThomsonModel() != MO5)
      {
        virtualkb_lastscancode = libretroKeyCodeToThomsonScanCode[RETROK_0];
      }
      break;
    default:
      virtualkb_lastscancode = -1;
  }

  if (virtualkb_lastscancode != -1)
  {
    keyboard(virtualkb_lastscancode, true);
    virtualkb_pressed = true;
  }
}

static void update_input(void)
{
  int i;
  int xpointer, ypointer;
  bool select, start, x, y;

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
  xpointer = input_state_cb(MAX_CONTROLLERS, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_X);
  ypointer = input_state_cb(MAX_CONTROLLERS, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_Y);
  pointerToScreenCoordinates(&xpointer, &ypointer);
  xpen = xpointer - 16;
  ypen = (ypointer - 16) / 2;
  penbutton = input_state_cb(MAX_CONTROLLERS, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_PRESSED);
  
  // Virtual keyboard:
  // - Start program with Start
  // - Change letter with X/Y and press it with Select
  start = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START);
  select = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT);
  x = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X);
  y = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y);
  if (!virtualkb_pressed)
  {
    // Try to start the currently loaded program
    if (start)
    {
      autostart_program();
    }
    else if (x)
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
    else if (select)
    {
      virtualkb_lastscancode = libretroKeyCodeToThomsonScanCode[virtualkb_keysyms[virtualkb_index]];
      keyboard(virtualkb_lastscancode, true);
      virtualkb_pressed = true;
    }
    else
    {
      autostart_mo5_continue();
    }
  }
  else
  {
    if (!select && !x && !y && !start)
    {
      virtualkb_pressed = false;
      keyboard(virtualkb_lastscancode, false);
    }
  }
}

static void change_model(const char *model)
{
  if (strcmp(model, "Auto") == 0)
  {
    return;
  }
  if (strncmp(model, "MO", 2) == 0)
  {
    libretroKeyCodeToThomsonScanCode = libretroKeyCodeToThomsonMoScanCode;
  }
  else
  {
    libretroKeyCodeToThomsonScanCode = libretroKeyCodeToThomsonToScanCode;
  }
  if (strcmp(model, "TO8") == 0)
  {
    SetThomsonModel(TO8);
  }
  else if (strcmp(model, "TO8D") == 0)
  {
    SetThomsonModel(TO8D);
  }
  else if (strcmp(model, "TO9") == 0)
  {
    SetThomsonModel(TO9);
  }
  else if (strcmp(model, "TO9+") == 0)
  {
    SetThomsonModel(TO9P);
  }
  else if (strcmp(model, "MO5") == 0)
  {
    SetThomsonModel(MO5);
  }
  // Default: TO8
  else
  {
    SetThomsonModel(TO8);
  }
}

static void check_variables(void)
{
  struct retro_variable var = {0, 0};

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
    change_model(var.value);
  }
#ifdef THEODORE_DASM
  var.key = PACKAGE_NAME"_disassembler";
  if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
  {
    if (strcmp(var.value, "enabled") == 0)
    {
      debugger_setMode(DEBUG_STEP);
    }
    else
    {
      debugger_setMode(DEBUG_DISABLED);
    }
  }
  var.key = PACKAGE_NAME"_break_illegal_opcode";
  if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
  {
    if (strcmp(var.value, "enabled") == 0)
    {
      debugger_set_break_on_illegal_opcode(true);
    }
    else
    {
      debugger_set_break_on_illegal_opcode(false);
    }
  }
#endif
}

void retro_run(void)
{
  bool updated;
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
    mcycles = 1000 * CPU_FREQUENCY / AUDIO_SAMPLE_RATE; // theoretical thousandths of cycles
    mcycles += excess;                       // corrected thousandths of cycles
    icycles = mcycles / 1000;                // integer number of cycles to run
    excess = mcycles - 1000 * icycles;       // remaining to do the next time
    excess -= 1000 * Run(icycles);           // remove thousandths in excess
    audio_sample = GetAudioSample();
    audio_stereo_buffer[(i << 1) + 0] = audio_stereo_buffer[(i << 1) + 1] = audio_sample;
  }

  update_input();

  if (autorun_counter > 0)
  {
    autorun_counter--;
    if (autorun_counter == 0)
    {
      autostart_program();
    }
  }

  updated = false;
  if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
  {
    check_variables();
  }

  audio_batch_cb(audio_stereo_buffer, AUDIO_SAMPLE_PER_FRAME);
  video_cb(video_buffer, XBITMAP, YBITMAP, PITCH);
}

size_t retro_serialize_size(void)
{
  return toemulator_serialize_size();
}

bool retro_serialize(void *data, size_t size)
{
  if (size != toemulator_serialize_size()) return false;
  toemulator_serialize(data);
  return true;
}

bool retro_unserialize(const void *data, size_t size)
{
  if (size != toemulator_serialize_size()) return false;
  toemulator_unserialize(data);
  return true;
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

static void check_automodel(const char *filename)
{
  struct retro_variable var = {0, 0};

  var.key = PACKAGE_NAME"_rom";
  if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
  {
    change_model(autodetect_model(filename));
  }
}

static void check_autorun(void)
{
  struct retro_variable var = {0, 0};

  var.key = PACKAGE_NAME"_autorun";
  if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
  {
    if (strcmp(var.value, "enabled") == 0)
    {
      autorun_counter = AUTORUN_DELAY;
    }
  }
}

// Load file with auto-detection of type based on the file extension.
static bool load_file(const char *filename)
{
  if (strlen(filename) > 3 && streq_nocase(filename + strlen(filename) - 3, ".k7"))
  {
    currentMedia = MEDIA_TAPE;
    LoadTape(filename);
    // Check if the first file is a BASIC or BIN file to know how to run it
    if (autodetect_tape_first_file_is_basic(filename))
    {
      mo5_autostart_keys = MO5_AUTOSTART_BASIC_KEYS;
      mo5_autostart_keys_length = MO5_AUTOSTART_BASIC_KEYS_LENGTH;
    }
    else
    {
      mo5_autostart_keys = MO5_AUTOSTART_BIN_KEYS;
      mo5_autostart_keys_length = MO5_AUTOSTART_BIN_KEYS_LENGTH;
    }
  }
  else if (strlen(filename) > 3 && streq_nocase(filename + strlen(filename) - 3, ".fd"))
  {
    currentMedia = MEDIA_FLOPPY;
    LoadFd(filename);
  }
  else if (strlen(filename) > 4 && (streq_nocase(filename + strlen(filename) - 4, ".rom")
      || streq_nocase(filename + strlen(filename) - 3, ".m7")
      || streq_nocase(filename + strlen(filename) - 3, ".m5")))
  {
    currentMedia = MEDIA_CARTRIDGE;
    LoadMemo(filename);
  }
  else if (strlen(filename) > 4 && streq_nocase(filename + strlen(filename) - 4, ".sap"))
  {
    currentMedia = MEDIA_FLOPPY;
    LoadSap(filename);
  }
  else
  {
    currentMedia = NO_MEDIA;
    if (log_cb) log_cb(RETRO_LOG_ERROR, "Unknown file type for file %s.\n", filename);
    return false;
  }
  check_automodel(filename);
  check_autorun();
  return true;
}

static void keyboard_cb(bool down, unsigned keycode,
    uint32_t character, uint16_t key_modifiers)
{
  (void) character, (void) key_modifiers; // Unused parameters
  //printf( "Down: %s, Code: %d, Char: %u, Mod: %u.\n",
  //        down ? "yes" : "no", keycode, character, key_modifiers);

  if (keycode <= RETROK_LAST)
  {
    unsigned char scancode = libretroKeyCodeToThomsonScanCode[keycode];
    if (scancode != 0xFF)
    {
      keyboard(scancode, down);
    }
  }
}

bool retro_load_game(const struct retro_game_info *game)
{
  struct retro_keyboard_callback keyb_cb = { keyboard_cb };
  enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
  if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt) && log_cb)
  {
    log_cb(RETRO_LOG_ERROR, "XRGB8888 is not supported.\n");
    return false;
  }

  environ_cb(RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK, &keyb_cb);

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
  (void) game_type; // Unused parameter
  (void) info;      // Unused parameter
  (void) num_info;  // Unused parameter
  return false;
}

void retro_unload_game(void)
{
  UnloadTape();
  UnloadFloppy();
  UnloadMemo();
  currentMedia = NO_MEDIA;
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
