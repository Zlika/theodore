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
#include "libretro-common/include/boolean.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef THEODORE_DASM
#include "debugger.h"
#endif
#include "autostart.h"
#include "devices.h"
#include "keymap.h"
#include "logger.h"
#include "sap.h"
#include "motoemulator.h"
#include "video.h"
#include "vkeyb/vkeyb.h"

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
#define AUDIO_SAMPLE_PER_FRAME (AUDIO_SAMPLE_RATE / VIDEO_FPS)
#define CPU_FREQUENCY     1000000
// Pitch = length in bytes between two lines in video buffer
#define PITCH             sizeof(pixel_fmt_t) * XBITMAP
// Autorun: Number of frames to wait before simulating
// the key stroke to start the program
#define AUTORUN_DELAY     70
// Virtual keyboard: Number of frames to wait when B button is pushed
// to make the key sticky
#define VKB_STICKY_KEY_DELAY 25

retro_log_printf_t log_cb = NULL;
static retro_environment_t environ_cb = NULL;
static retro_video_refresh_t video_cb = NULL;
static retro_audio_sample_t audio_cb = NULL;
static retro_audio_sample_batch_t audio_batch_cb = NULL;
static retro_input_poll_t input_poll_cb = NULL;
static retro_input_state_t input_state_cb = NULL;

static unsigned int input_type[MAX_CONTROLLERS];
static pixel_fmt_t *video_buffer = NULL;
static int16_t audio_stereo_buffer[2*AUDIO_SAMPLE_PER_FRAME];

// nb of thousandth of cycles in excess to run the next time
static int excess = 0;

// Autorun counter
static int autorun_counter = -1;
// True when autostart is in progress
static bool autostart_pending = false;

// True if the virtual keyboard must be showed
static bool vkb_show = false;

struct ButtonsState
{
  bool up, down, right, left;
  bool select, start;
  bool a, b, x, y;
  int frames_since_b_pressed;
};
// Last state of the buttons
struct ButtonsState last_btn_state = { false, false, false, false,
                                       false, false,
                                       false, false, false, false, 0 };

static const struct retro_variable prefs[] = {
    { PACKAGE_NAME"_rom", "Thomson model; Auto|TO8|TO8D|TO9|TO9+|MO5|MO6|PC128|TO7|TO7/70" },
    { PACKAGE_NAME"_autorun", "Auto run game; disabled|enabled" },
    { PACKAGE_NAME"_vkb_transparency", "Virtual keyboard transparency; 0%|10%|20%|30%|40%|50%|60%|70%|80%|90%" },
    { PACKAGE_NAME"_floppy_write_protect", "Floppy write protection; enabled|disabled" },
    { PACKAGE_NAME"_tape_write_protect", "Tape write protection; enabled|disabled" },
    { PACKAGE_NAME"_printer_emulation", "Dump printer data to file; disabled|enabled" },
#ifdef THEODORE_DASM
    { PACKAGE_NAME"_disassembler", "Interactive disassembler; disabled|enabled" },
    { PACKAGE_NAME"_break_illegal_opcode", "Break on illegal opcode; disabled|enabled" },
#endif
    { NULL, NULL }
};

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
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "Fire" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT,"Show/Hide Virtual Keyboard" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Start Program" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,     "Move Virtual Keyboard" },

        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "Left" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "Up" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "Down" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "Fire" },

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
  video_buffer = (pixel_fmt_t *)linearMemAlign(XBITMAP * YBITMAP * sizeof(pixel_fmt_t), 0x80);
#else
  video_buffer = (pixel_fmt_t *)malloc(XBITMAP * YBITMAP * sizeof(pixel_fmt_t));
#endif
  SetLibRetroVideoBuffer(video_buffer);

  vkb_configure_virtual_keyboard(video_buffer, XBITMAP, YBITMAP);
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

static void update_input_virtual_keyboard()
{
  bool select, start;
  bool b, y;
  bool left, right, up, down;
  bool click;
  
  // Virtual keyboard:
  // - Start: starts the program
  // - Select: show the virtual keyboard
  start = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START);
  select = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT);
  y = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y);
  up = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP);
  down = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN);
  left = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT);
  right = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT);
  b = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B);
  click = input_state_cb(MAX_CONTROLLERS, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_PRESSED);

  // Try to start the currently loaded program
  if (!vkb_show && start && !last_btn_state.start)
  {
    autostart_pending = true;
  }
  else if (autostart_pending)
  {
    autostart_pending = autostart_nextkey();
  }

  // Show the virtual keyboard?
  if (select && !last_btn_state.select)
  {
    vkb_show = !vkb_show;
    // Release current key and sticky keys when virtual keyboard hidden
    if (!vkb_show)
    {
      int sticky_scancodes[VKB_MAX_STICKY_KEYS];
      int i;
      vkb_get_current_sticky_keys_scancode(sticky_scancodes);
      for (i = 0; i < VKB_MAX_STICKY_KEYS; i++)
      {
        if (sticky_scancodes[i] != -1)
        {
          keyboard(sticky_scancodes[i], false);
        }
      }
      vkb_release_all_sticky_keys();
      keyboard(vkb_get_current_key_scancode(), false);
      // If start is pressed than release also the Enter key
      if (start)
      {
        keyboard(libretroKeyCodeToThomsonScanCode[RETROK_RETURN], false);
      }
    }
  }

  if (vkb_show)
  {
    // Move keyboard
    if (y && !last_btn_state.y)
    {
      vkb_set_virtual_keyboard_position((vkb_get_virtual_keyboard_position() + 1) % 2);
    }
    // Direct click on the keyboard (touch screen)
    if (click)
    {
      int xpointer, ypointer;
      xpointer = input_state_cb(MAX_CONTROLLERS, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_X);
      ypointer = input_state_cb(MAX_CONTROLLERS, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_Y);
      pointerToScreenCoordinates(&xpointer, &ypointer);
      if (vkb_move_at(xpointer, ypointer))
      {
        // A key was touched: act as if the B button was pushed
        b = true;
      }
    }
    // Press key
    if ((b && !last_btn_state.b) || (!b && last_btn_state.b))
    {
      last_btn_state.frames_since_b_pressed = 0;
      // Do not release key if held
      if (b || !vkb_is_key_sticky(vkb_get_current_key_scancode()))
      {
        keyboard(vkb_get_current_key_scancode(), b);
      }
    }
    // Press and hold key
    if (b && last_btn_state.b)
    {
      last_btn_state.frames_since_b_pressed += 1;
      if (last_btn_state.frames_since_b_pressed == VKB_STICKY_KEY_DELAY)
      {
        int scancodes_prev[VKB_MAX_STICKY_KEYS];
        vkb_get_current_sticky_keys_scancode(scancodes_prev);
        if (vkb_make_current_key_sticky())
        {
          int scancodes[VKB_MAX_STICKY_KEYS];
          int i;
          vkb_get_current_sticky_keys_scancode(scancodes);
          for (i = 0; i < VKB_MAX_STICKY_KEYS; i++)
          {
            if (scancodes_prev[i] != scancodes[i])
            {
              // Key held
              if (scancodes[i] != -1)
              {
                keyboard(scancodes[i], true);
              }
              // Key released
              else
              {
                keyboard(scancodes_prev[i], false);
              }
            }
          }
        }
      }
    }
    if (!b)
    {
      // Move current key
      if (right && !last_btn_state.right)
      {
        vkb_move_key(VKB_MOVE_RIGHT);
      }
      else if (left && !last_btn_state.left)
      {
        vkb_move_key(VKB_MOVE_LEFT);
      }
      else if (down && !last_btn_state.down)
      {
        vkb_move_key(VKB_MOVE_DOWN);
      }
      else if (up && !last_btn_state.up)
      {
        vkb_move_key(VKB_MOVE_UP);
      }
      // If start is pressed than press the Enter key
      if ((start && !last_btn_state.start) || (!start && last_btn_state.start))
      {
        keyboard(libretroKeyCodeToThomsonScanCode[RETROK_RETURN], start);
      }
    }
  }

  last_btn_state.select = select;
  last_btn_state.start = start;
  last_btn_state.y = y;
  last_btn_state.b = b;
  last_btn_state.left = left;
  last_btn_state.right = right;
  last_btn_state.up = up;
  last_btn_state.down = down;
}

static void update_input(void)
{
  int xpointer, ypointer;

  input_poll_cb();

  if (!vkb_show)
  {
    // Joystick 1
    Joysemul(JOY0_UP, input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP));
    Joysemul(JOY0_DOWN, input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN));
    Joysemul(JOY0_LEFT, input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT));
    Joysemul(JOY0_RIGHT, input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT));
    Joysemul(JOY0_FIRE, input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B));
    // Joystick 2
    Joysemul(JOY1_UP, input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP));
    Joysemul(JOY1_DOWN, input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN));
    Joysemul(JOY1_LEFT, input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT));
    Joysemul(JOY1_RIGHT, input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT));
    Joysemul(JOY1_FIRE, input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B));

    // Light pen
    xpointer = input_state_cb(MAX_CONTROLLERS, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_X);
    ypointer = input_state_cb(MAX_CONTROLLERS, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_Y);
    pointerToScreenCoordinates(&xpointer, &ypointer);
    xpen = xpointer - 16;
    ypen = (ypointer - 16) / 2;
    penbutton = input_state_cb(MAX_CONTROLLERS, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_PRESSED);
  }

  // Virtual keyboard management
  update_input_virtual_keyboard();
}

static void change_model(const char *model)
{
  if (strcmp(model, "Auto") == 0)
  {
    // Auto-detection of the model is only done when a game is loaded
    return;
  }
  if ((strcmp(model, "MO5") == 0) || (strcmp(model, "TO7") == 0) || (strcmp(model, "TO7/70") == 0))
  {
    libretroKeyCodeToThomsonScanCode = libretroKeyCodeToThomsonMo5ScanCode;
  }
  else if ((strcmp(model, "MO6") == 0) || (strcmp(model, "PC128") == 0))
  {
    libretroKeyCodeToThomsonScanCode = libretroKeyCodeToThomsonMo6ScanCode;
  }
  else
  {
    libretroKeyCodeToThomsonScanCode = libretroKeyCodeToThomsonToScanCode;
  }
  if (strcmp(model, "TO8") == 0)
  {
    SetThomsonModel(TO8);
    vkb_set_virtual_keyboard_model(VKB_MODEL_TO8);
  }
  else if (strcmp(model, "TO8D") == 0)
  {
    SetThomsonModel(TO8D);
    vkb_set_virtual_keyboard_model(VKB_MODEL_TO8);
  }
  else if (strcmp(model, "TO9") == 0)
  {
    SetThomsonModel(TO9);
    vkb_set_virtual_keyboard_model(VKB_MODEL_TO8);
  }
  else if (strcmp(model, "TO9+") == 0)
  {
    SetThomsonModel(TO9P);
    vkb_set_virtual_keyboard_model(VKB_MODEL_TO8);
  }
  else if (strcmp(model, "MO5") == 0)
  {
    SetThomsonModel(MO5);
    vkb_set_virtual_keyboard_model(VKB_MODEL_MO5);
  }
  else if (strcmp(model, "MO6") == 0)
  {
    SetThomsonModel(MO6);
    vkb_set_virtual_keyboard_model(VKB_MODEL_MO6);
  }
  else if (strcmp(model, "PC128") == 0)
  {
    SetThomsonModel(PC128);
    vkb_set_virtual_keyboard_model(VKB_MODEL_PC128);
  }
  else if (strcmp(model, "TO7") == 0)
  {
    SetThomsonModel(TO7);
    vkb_set_virtual_keyboard_model(VKB_MODEL_TO7);
  }
  else if (strcmp(model, "TO7/70") == 0)
  {
    SetThomsonModel(TO7_70);
    vkb_set_virtual_keyboard_model(VKB_MODEL_TO770);
  }
  // Default: TO8
  else
  {
    SetThomsonModel(TO8);
    vkb_set_virtual_keyboard_model(VKB_MODEL_TO8);
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
  var.key = PACKAGE_NAME"_vkb_transparency";
  if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
  {
    int alpha = 255 - (255 * atoi(var.value) / 100);
    vkb_set_virtual_keyboard_transparency(alpha);
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
  if (vkb_show)
  {
    vkb_show_virtual_keyboard();
  }

  if (autorun_counter > 0)
  {
    autorun_counter--;
    if (autorun_counter == 0)
    {
      autostart_pending = true;
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

static void check_automodel(const char *filename)
{
  struct retro_variable var = {0, 0};

  var.key = PACKAGE_NAME"_rom";
  if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && (strcmp(var.value, "Auto") == 0))
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
  // On some systems (e.g. Nintendo Switch) it seems that it is not possible to open a file
  // that is already opened. As autostart_init() opens the file to read its header before
  // closing it, it must be called BEFORE the LoadXxx() functions that open the file
  // without (immediately) closing it.
  autostart_init(filename);

  Media currentMedia = get_media_type(filename);
  switch (currentMedia)
  {
    case MEDIA_TAPE:
      LoadTape(filename);
      break;
    case MEDIA_FLOPPY:
      if (is_sap_file(filename))
      {
        LoadSap(filename);
      }
      else
      {
        LoadFd(filename);
      }
      break;
    case MEDIA_CARTRIDGE:
      LoadMemo(filename);
      break;
    default:
      LOG_ERROR("Unknown file type for file %s.\n", filename);
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
  // Use of RGB565 pixel format instead of XRGB8888
  // for better compatibility with low-end devices
  enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_RGB565;
  if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
  {
    LOG_ERROR("RGB5656 is not supported.\n");
    return false;
  }

  environ_cb(RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK, &keyb_cb);

  check_variables();

  if (game && game->path)
  {
    LOG_INFO("Loading file %s.\n", game->path);
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
