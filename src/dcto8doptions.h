//////////////////////////////////////////////////////////////////////////////
// DCTO8DOPTIONS.H - Option setting, save & restore
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

#ifndef __DCTO8DOPTIONS_H
#define __DCTO8DOPTIONS_H

//Draw option box ////////////////////////////////////////////////////////////
void Drawoptionbox();
//Traitement des clics des boutons d'options /////////////////////////////////
void Optionclick();

//Option initialization //////////////////////////////////////////////////////
void Initoptions();
//Option save ////////////////////////////////////////////////////////////////
void Saveoptions();
//Mise a jour d'un parametre avec la valeur i de la popuptable////////////////
void Setoption(int i);

//fichier dcto8d.ini
extern FILE *fpi;

//frequence 6809 en kHz
extern int frequency;

#endif /* __DCTO8DOPTIONS_H */
