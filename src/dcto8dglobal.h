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

#define XBITMAP 672
#define YBITMAP 216
#define YSTATUS 20

#define LANGUAGE_MAX 2        //nombre de langues differentes
#define TEXT_MAXLENGTH 256    //longueur maxi d'une chaine de texte
#define MSG_MAX 50            //nombre de textes dans chaque langue

#define BOUTON_MAX 104        //nombre de boutons differents

#define KEYBOARDKEY_MAX 84    //nombre de touches du clavier TO8D
#define JOYSTICKKEY_MAX 10    //nombre total de contacts des deux manettes

#define FLAGS SDL_SWSURFACE | SDL_DOUBLEBUF

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

//fonctions
void Createdialogbox(int w, int h);
void Displayscreen();
void Drawdesassbox();
void Drawk7index();
void Drawjoystickbox();
void Drawkeyboardbox();
void Drawpopuptable(int n, int x, int y);
void Drawoptionbox();
void Erreur(int n);
void Hardreset();
void Info(int i);
void Initprog();
void Resizescreen(int x, int y);
void SDL_error(int n);
void Setoption(int i);
