//////////////////////////////////////////////////////////////////////////////
// DCTO8DGLOBAL.H - declarations globales
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
//
//2007-12-28 : v11.0 released

#ifndef __DCTO8DGLOBAL_H
#define __DCTO8DGLOBAL_H

#define XBITMAP 672
#define YBITMAP 216
#define YSTATUS 20

#define LANGUAGE_MAX 2        //nombre de langues differentes
#define TEXT_MAXLENGTH 256    //longueur maxi d'une chaine de texte

#define BOUTON_MAX 104        //nombre de boutons differents

#define JOYSTICKKEY_MAX 10    //nombre total de contacts des deux manettes

#define FLAGS 0   // flags in SDL_CreateRGBSurface are unused and should be set to 0

typedef struct
{
 int x;         //ordonnee du coin haut-gauche
 int y;         //abscisse du coin haut-gauche
 int w;         //largeur en pixels
 int h;         //hauteur en pixels
 char *text;    //texte a editer
} dialogeditbox;

typedef struct
{
 //char name[12]; //nom du bouton
 int x;         //ordonnee du coin haut-gauche
 int y;         //abscisse du coin haut-gauche
 //int w;         //largeur en pixels
 //int h;         //hauteur en pixels
 int n;         //numero de la surface SDL
} dialogbutton;

typedef struct
{
 char name[12]; //nom du bouton
 int w;         //largeur en pixels
 int h;         //hauteur en pixels
} button;

#endif
