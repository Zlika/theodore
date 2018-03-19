//////////////////////////////////////////////////////////////////////////////
// DCTO8DDEVICES.H   Emulation des peripheriques TO8
// Author   : Daniel Coulom - danielcoulom@gmail.com
// Web site : http://dcto8.free.fr
//
// This file is part of DCTO8D.
//
// DCTO8D is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DCTO8D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DCTO8D. If not, see <http://www.gnu.org/licenses/>.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __DCTO8DDEVICES_H
#define __DCTO8DDEVICES_H

void Readoctetk7();
void Writeoctetk7();
void Readsector();
void Writesector();
void Formatdisk();
void Imprime();
void Readmousebutton();
void Readpenxy(int device);

void Loadk7(char *filename);
void Loadfd(char *filename);
void Loadmemo(char *filename);
void Unloadk7();
void Unloadfd();
void Unloadmemo();
// Chargement d'un fichier avec auto-detection du type : k7 (*.k7), fd (*.fd) ou memo7 (*.rom)
void Load(char *filename);
void PrintK7Index(char *index);
// Callback appellee quand k7index est modifie
void SetUpdateK7IndexCallback(void (*callback)());

// indicateur lecture seule pour la cassette
extern int k7protection;
// indicateur lecture seule pour la disquette
extern int fdprotection;

#endif /* __DCTO8DDEVICES_H */
