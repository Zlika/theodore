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

// Initialize default values for to8dkeycode and to8djoycode
void Initoptions(char defaultto8dkeycode[256], char defaultto8djoycode[256]);
// Load options
void Loadoptions(char *filename);
// Save options
void Saveoptions(char *filename);
// Reset to default values
void Resetoptions();

typedef struct
{
  int language;     //0=francais 1=anglais
  int frequency;    //frequence 6809 en kHz
  int xclient;      //largeur fenetre utilisateur
  int yclient;      //hauteur ecran dans fenetre utilisateur
  int vblnumbermax; //nombre de vbl entre deux affichages
  int k7protection; // indicateur lecture seule pour la cassette
  int fdprotection; // indicateur lecture seule pour la disquette
  int keybpriority; // indicateur lecture seule pour la disquette
  int reserved1;
  int reserved2;
  int reserved3;
  int reserved4;
  int reserved5;
  char to8dkeycode[256]; //scancode to8d en fonction du scancode pc
  char to8djoycode[256]; //numero bouton joystick en fonction du scancode pc
} Options;

extern Options options;

#endif /* __DCTO8DOPTIONS_H */
