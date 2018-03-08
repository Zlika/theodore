//////////////////////////////////////////////////////////////////////////////
// DCTO8DDESASS.C - boîte de dialogue pour le desassembleur
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
// along with DCTO8D.  If not, see <http://www.gnu.org/licenses/>.
//
//////////////////////////////////////////////////////////////////////////////

#include <SDL.h>
#include <stdio.h>
#include <string.h>
#include "dcto8dglobal.h"
#include "dcto8dinterface.h"

int startaddress;         //adresse de debut du desassemblage
int nextaddress;          //adresse de la deuxieme instruction desassemblee
char startaddresshexa[5]; //startaddress en chaine hexadecimale
char string[256];

#define DESASSBUTTON_MAX 3   //nombre de boutons boite de dialogue desass
const dialogbutton desassbutton[DESASSBUTTON_MAX] = {
{55, 28, 14},  //0x08 desassembler
{135, 28, 15}, //0x09 + 1
{170, 28, 10}  //0x09 + 10
};

#define DESASSEDITBOX_MAX 1   //nombre d'editboxes boite de dialogue desass
const dialogeditbox desasseditbox[DESASSEDITBOX_MAX] = {
{10, 30, 38, 15, startaddresshexa}, //0x00 debut
};

extern SDL_Surface *dialogbox; //surface d'affichage dialogbox
extern int language;
extern int dialog;             //0 ou n°boite de dialogue affichee
extern SDL_Surface *dialogbox; //surface d'affichage dialogbox
extern char *msg[MSG_MAX][LANGUAGE_MAX]; //messages en plusieurs langues
extern const int blanc;
extern const SDL_Color textnoir;
extern SDL_Surface *textbox;
extern button bouton[];

int Dasm6809(char *string, int a);

//Affichage du desassemblage a l'adresse startaddress /////////////////////////
void Displaydesass(int x)
{
 SDL_Rect rect;
 int i;
 SDL_Surface *Rendertext(char *string, int color, int background);
 //desassemblage
 rect.x = 10; rect.y = 35; rect.w = 368; rect.h = 15;
 for(i = 0; i < 20; i++)
 {
  rect.y += 15;
  x = Dasm6809(string, x) & 0xffff;
  if(i == 0) nextaddress = x;
  string[4] = 0; rect.w = 90;
  Drawtextbox(dialogbox, string, rect, 0, blanc, 0);      //adresse hexa
  string[15] = 0; rect.x += 32;
  Drawtextbox(dialogbox, string + 5, rect, 0, blanc, 0);  //contenu hexa
  string[22] = 0; rect.x += 75;
  Drawtextbox(dialogbox, string + 16, rect, 0, blanc, 0); //mnemonique
  string[40] = 0; rect.x += 40;
  Drawtextbox(dialogbox, string + 23, rect, 0, blanc, 0); //operandes
  rect.x += 90; rect.w = 30;
  Drawtextbox(dialogbox, string + 41, rect, 0, blanc, 0); //cycles
  rect.x = 10;
 }
}

//Création de la boite de dialogue du desassembleur ///////////////////////////
void Drawdesassbox()
{
 SDL_Rect rect;
 int i;
 extern const dialogeditbox *focus;
 extern int bleu;
 extern int xcursor;
 extern int pause6809;
 pause6809 = 1;
 if(dialog != 5)
 {
  Createdialogbox(290, 365);
  rect.x = 10; rect.w = dialogbox->w - 32;
  rect.y = 5; rect.h = 15;
  Drawtextbox(dialogbox, msg[32][language], rect, 1, bleu, 0); //titre
  dialog = 5;
  startaddress = 0xf000;
  sprintf(startaddresshexa, "%04X", startaddress);
  focus = &desasseditbox[0]; //toujours le focus
  xcursor = 4;
 }
 //dessin des boutons
 for(i = 0; i < DESASSBUTTON_MAX; i++) Drawbutton(&desassbutton[i], 0);
 //dessin des editboxes
 for(i = 0; i < DESASSEDITBOX_MAX; i++) Draweditbox(&desasseditbox[i]);
 //affichage du desassemblage
 Displaydesass(startaddress);
 dialog = 5;
 pause6809 = 0;
}

//Traitement des clics dans une editbox de la boite de dialogue////////////////
void Desasseditboxclick(int editbox)
{
 extern int xcursor;
 extern const dialogeditbox *focus;
 focus = &desasseditbox[editbox];
 xcursor = 0;
 Draweditbox(&desasseditbox[editbox]);
}

//Traitement des clics dans un bouton de la boite de dialogue//////////////////
void Desassbuttonclick(int button)
{
 int i, x;
 //dessin du bouton enfonce
 Drawbutton(&desassbutton[button], 1);
 Displayscreen();
 //traitement en fonction du bouton
 x = 0;
 switch(button)
 {
  case  0: sscanf(startaddresshexa, "%x", &startaddress);
           startaddress &= 0xffff;
           x = startaddress;
           break;
  case  1: x = nextaddress;
           break;
  case  2: x = nextaddress;
           for(i = 0; i < 9; i++) x = Dasm6809(string, x) & 0xffff;
           break;
 }
 sprintf(startaddresshexa, "%04X", startaddress);
 Draweditbox(&desasseditbox[0]);
 //dessin du bouton relache
 Displaydesass(x); SDL_Delay(200);
 Drawbutton(&desassbutton[button], 0);
}

//Traitement des clics dans la boite de dialogue de desassemblage /////////////
void Desassclick()
{
 int i, x, y;
 extern int dialog;
 extern int xmouse, ymouse;
 extern SDL_Rect dialogrect;
 extern const dialogeditbox *focus;

 //focus dans l'adresse de debut du desassemblage
 focus = &desasseditbox[0];

 //recherche clic sur un bouton
 for(i = 0; i < DESASSBUTTON_MAX; i++)
 {
  x = dialogrect.x + desassbutton[i].x;
  y = dialogrect.y + desassbutton[i].y;
  if(xmouse > x) if(xmouse < (x + bouton[desassbutton[i].n].w))
  if(ymouse > y) if(ymouse < (y + bouton[desassbutton[i].n].h))
  {Desassbuttonclick(i); return;}
 }

 //recherche clic dans une editbox
 for(i = 0; i < DESASSEDITBOX_MAX; i++)
 {
  x = dialogrect.x + desasseditbox[i].x;
  y = dialogrect.y + desasseditbox[i].y;
  if(xmouse > x) if(xmouse < (x + desasseditbox[i].w))
  if(ymouse > y) if(ymouse < (y + desasseditbox[i].h))
  {Desasseditboxclick(i); return;}
 }
}
