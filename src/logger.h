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

/* Logging functions */

#ifndef __LOGGER_H
#define __LOGGER_H

#include "libretro-common/include/libretro.h"

extern retro_log_printf_t log_cb;

#define LOG(level, ...) { if (log_cb) log_cb(level, __VA_ARGS__); }

/* Log functions with different severity levels */
#define LOG_DEBUG(...) LOG(RETRO_LOG_DEBUG, __VA_ARGS__)
#define LOG_INFO(...) LOG(RETRO_LOG_INFO, __VA_ARGS__)
#define LOG_WARN(...) LOG(RETRO_LOG_WARN, __VA_ARGS__)
#define LOG_ERROR(...) LOG(RETRO_LOG_ERROR, __VA_ARGS__)

#endif /* __LOGGER_H */
