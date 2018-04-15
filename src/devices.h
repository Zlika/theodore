/*
 * This file is part of theodore, a Thomson emulator based on
 * Daniel Coulom's DCTO8D emulator (http://dcto8.free.fr/).
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

/* Emulation of Thomson devices */

#ifndef __DEVICES_H
#define __DEVICES_H

void Readoctetk7();
void Writeoctetk7();
void Readsector();
void Writesector();
void Formatdisk();
void Imprime();
void Readmousebutton();
void Readpenxy(int device);

void Loadk7(const char *filename);
void Loadfd(const char *filename);
void Loadmemo(const char *filename);
void Unloadk7();
void Unloadfd();
void Unloadmemo();
// Chargement d'un fichier avec auto-detection du type : k7 (*.k7), fd (*.fd) ou memo7 (*.rom)
void Load(char *filename);
void PrintK7Index(char *index);
// Callback appellee quand k7index est modifie
void SetUpdateK7IndexCallback(void (*callback)());

#endif /* __DCTO8DDEVICES_H */
