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

/* Tools to manage the SAP file format */

#ifndef __SAP_H
#define __SAP_H

#include <stdbool.h>

// Converts a SAP file to the FD format.
// Returns true for success, false for failure.
bool sap2fd(const char *sap_filename, const char *fd_filename);

#endif /* __SAP_H */
