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

#include <stdio.h>

void Readoctetk7();
void Writeoctetk7();
void Readsector();
void Writesector();
void Formatdisk();
void Imprime();
void Readmousebutton();
void Readpenxy(int device);

// Initialisation noms de fichiers et pointeur de fonction de chargement //////
void Initfilenames();

// pointeur fichier k7
extern FILE *fk7;
// compteur du lecteur de cassette
extern int k7index;
// compteur du lecteur de cassette en fin de bande
extern int k7indexmax;
// indicateur lecture seule pour la cassette
extern int k7protection;
// indicateur lecture seule pour la disquette
extern int fdprotection;
// nom du fichier cassette
extern char k7name[100];
// nom du fichier disquette
extern char fdname[100];
// nom du fichier cartouche
extern char memoname[100];

#endif /* __DCTO8DDEVICES_H */
