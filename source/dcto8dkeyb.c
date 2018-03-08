//////////////////////////////////////////////////////////////////////////////
// DCTO8DKEYB.C - keyboard input and customization
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

#include <SDL.h>
#include <stdio.h>
#include <string.h>
#ifdef WIN32
 #include <windows.h>
#endif
#include "dcto8dglobal.h"
#include "dcto8dkeyb.h"
#include "dcto8dinterface.h"

//variables globales
char to8dkeycode[256]; //scancode to8d en fonction du scancode pc
char to8djoycode[256]; //numero bouton joystick en fonction du scancode pc
int keybpriority;      //0=manettes prioritaires 1=clavier prioritaire
int lastkeycode;       //keycode derniere touche enfoncee
int lastkeysym;        //keysym derniere touche enfoncee

#define KEYBOARDBUTTON_MAX 86 //nombre de boutons boite de dialogue clavier
const dialogbutton keyboardbutton[KEYBOARDBUTTON_MAX] = {
{ 48,  30, 20}, //0x00 "F7  F2",
{172,  60, 21}, //0x01 " _ 6",
{184,  86, 22}, //0x02 "  Y",
{192, 112, 23}, //0x03 "  H",
{420,  60, 24}, //0x04 "[haut]",
{432,  86, 25}, //0x05 "[droite]",
{372, 138, 26}, //0x06 "RAZ",
{204, 138, 27}, //0x07 "  N",
{ 86,  30, 28}, //0x08 "F8  F3",
{146,  60, 29}, //0x09 " ( 5",
{158,  86, 30}, //0x0a "  T",
{166, 112, 31}, //0x0b "  G",
{328,  60, 32}, //0x0c "= +",
{406,  86, 33}, //0x0d "[gauche]",
{406, 138, 34}, //0x0e "INS",
{178, 138, 35}, //0x0f "  B",
{124,  30, 36}, //0x10 "F9  F4",
{120,  60, 37}, //0x11 " ' 4",
{132,  86, 38}, //0x12 "  R",
{140, 112, 39}, //0x13 "  F",
{380,  60, 40}, //0x14 "ACC",
{458, 112, 41}, //0x15 "  1   pad",
{432, 138, 42}, //0x16 "EFF",
{152, 138, 43}, //0x17 "  V",
{162,  30, 44}, //0x18 "F10 F5",
{ 94,  60, 45}, //0x19 " \" 3",
{106,  86, 46}, //0x1a "  E",
{114, 112, 47}, //0x1b "  D",
{458,  60, 48}, //0x1c "  7   pad",
{458,  86, 49}, //0x1d "  4   pad",
{458, 138, 50}, //0x1e "  0   pad",
{126, 138, 51}, //0x1f "  C",
{ 10,  30, 52}, //0x20 "F6  F1",
{ 68,  60, 53}, //0x21 " é 2",
{ 80,  86, 54}, //0x22 "  Z",
{ 88, 112, 55}, //0x23 "  S",
{484,  60, 56}, //0x24 "  8   pad",
{484, 112, 57}, //0x25 "  2   pad",
{484, 138, 58}, //0x26 "  .   pad",
{100, 138, 59}, //0x27 "  X",
{ 10,  60, 60}, //0x28 " # @",
{ 42,  60, 61}, //0x29 " * 1",
{ 54,  86, 62}, //0x2a "  A",
{ 62, 112, 63}, //0x2b "  Q",
{ 36, 112, 64}, //0x2c " [ {",
{484,  86, 65}, //0x2d "  5   pad",
{510,  86, 66}, //0x2e "  6   pad",
{ 74, 138, 67}, //0x2f "  W",
{ 10,  86, 68}, //0x30 "  STOP",
{198,  60, 69}, //0x31 " è 7",
{210,  86, 70}, //0x32 "  U",
{218, 112, 71}, //0x33 "  J",
{100, 164, 72}, //0x34 "ESPACE",
{510,  60, 73}, //0x35 "  9   pad",
{510, 138, 74}, //0x36 "ENT   pad",
{230, 138, 75}, //0x37 " , ?",
{999,  30, 76}, //0x38 "?????",
{224,  60, 77}, //0x39 " ! 8",
{236,  86, 78}, //0x3a "  I",
{244, 112, 79}, //0x3b "  K",
{340,  86, 80}, //0x3c "  $  &",
{420, 112, 81}, //0x3d "[bas]",
{348, 112, 82}, //0x3e "  ] }",
{256, 138, 83}, //0x3f " ; .",
{999,  30, 84}, //0x40 "?????",
{250,  60, 85}, //0x41 " ç 9",
{262,  86, 86}, //0x42 "  O",
{270, 112, 87}, //0x43 "  L",
{354,  60, 88}, //0x44 "  - \\",
{322, 112, 89}, //0x45 "ù %",
{380,  86, 90}, //0x46 "ENT",
{282, 138, 91}, //0x47 " : /",
{999,  30, 92}, //0x48 "?????",
{276,  60, 93}, //0x49 " à 0",
{288,  86, 94}, //0x4a "  P",
{296, 112, 95}, //0x4b "  M",
{302,  60, 96}, //0x4c " ) °",
{314,  86, 97}, //0x4d " ^ ¨",
{510, 112, 98}, //0x4e "  3   pad",
{308, 138, 99}, //0x4f "> <",
{ 10, 138, 100}, //0x50 "LCK",
{ 36, 138, 101}, //0x51 "SHIFT",
{334, 138, 102}, //0x52 "SHIFT",
{ 10, 112, 103}, //0x53 "CNT",
{ 90, 256, 16}, //0x00 "[defaut]",
{284, 256, 17}  //0x01 "[sauver]",
};

#define JOYSTICKBUTTON_MAX 12 //nombre de boutons boite de dialogue manettes
const dialogbutton joystickbutton[JOYSTICKBUTTON_MAX] = {
{135,  30, 24}, //0x00 "[0] haut",
{135,  80, 81}, //0x01 "[0] bas",
{110,  55, 33}, //0x02 "[0] gauche",
{160,  55, 25}, //0x03 "[0] droite",
{293,  30, 24}, //0x04 "[1] haut",
{293,  80, 81}, //0x05 "[1] bas",
{268,  55, 33}, //0x06 "[1] gauche",
{318,  55, 25}, //0x07 "[1] droite",
{125, 112, 19}, //0x08 "[0] action",
{283, 112, 19}, //0x09 "[1] action",
{ 43, 216, 16}, //0x00 "[defaut]",
{238, 216, 17}  //0x01 "[sauver]",
};

//variables externes
extern int language;
extern int touche[];
extern int rmask, gmask, bmask, amask;
extern int dialog;             //0 ou n°boite de dialogue affichee
extern SDL_Surface *screen;    //surface d'affichage de l'ecran
extern SDL_Surface *textbox;   //surface d'affichage de texte
extern SDL_Surface *dialogbox; //surface d'affichage dialogbox
extern char *msg[MSG_MAX][LANGUAGE_MAX]; //messages en plusieurs langues
extern button bouton[];

//Emulation clavier : affichage du nom de la derniere touche pressee //////////
void Displaykey()
{
 SDL_Rect rect;
 int i;
 char string[80];
 extern int blanc;
 //code et nom de la touche PC
 rect.x = 56; rect.y = 194; rect.w = 140; rect.h = 15;
 sprintf(string, "0x%02x = %s", lastkeycode, SDL_GetKeyName(lastkeysym));
 Drawtextbox(dialogbox, string, rect, 0, blanc, -1);
 //code et nom de la touche MO5
 rect.x = 321; rect.y = 194; rect.w = 185;
 rect.h = 15;
 i = to8dkeycode[lastkeycode & 0xff];
 sprintf(string, "%s : %s", msg[15][language],
  (i < KEYBOARDKEY_MAX) ? bouton[keyboardbutton[i].n].name : msg[18][language]);
 Drawtextbox(dialogbox, string, rect, 0, blanc, -1);
}

//Emulation manettes : affichage du nom de la derniere touche pressee /////////
void Displayjoy()
{
 SDL_Rect rect;
 int i;
 char string[80];
 extern int blanc;
 //code et nom de la touche PC
 rect.x = 80; rect.y = 154; rect.w = 140; rect.h = 15;
 sprintf(string, "0x%02x = %s", lastkeycode, SDL_GetKeyName(lastkeysym));
 Drawtextbox(dialogbox, string, rect, 0, blanc, -1);
 //code et nom de la fonction manette
 rect.x = 230; rect.y = 154; rect.w = 180; rect.h = 15;
 i = to8djoycode[lastkeycode & 0xff];
 sprintf(string, "%s : %s", msg[35][language],
  (i < JOYSTICKKEY_MAX) ? bouton[joystickbutton[i].n].name : msg[18][language]);
 Drawtextbox(dialogbox, string, rect, 0, blanc, -1);
}

//Création de la boite de dialogue du clavier ////////////////////////////////
void Drawkeyboardbox()
{
 SDL_Rect rect;
 int i;
 char string[256];
 extern int pause6809;
 extern int bleu, gris0;
 pause6809 = 1;
 lastkeycode = 0;
 lastkeysym = 0;
 if(dialog != 3)
 {
  Createdialogbox(546, 292);
  rect.x = 10; rect.w = dialogbox->w - 32;
  rect.y = 5; rect.h = 15;
  Drawtextbox(dialogbox, msg[14][language], rect, 1, bleu, 0); //titre
  dialog = 3;
 }
 //message d'aide
 rect.x = 126; rect.y = 215; rect.w = 370;
 sprintf(string, "%s %s.", msg[16][language], msg[15][language]);
 Drawtextbox(dialogbox, string, rect, 0, gris0, 0);
 rect.y += 15;
 Drawtextbox(dialogbox, msg[17][language], rect, 0, gris0, 0);
 //dessin des boutons
 for(i = 0; i < KEYBOARDBUTTON_MAX; i++) Drawbutton(&keyboardbutton[i], 0);
 Displaykey(0);  //nom de la touche lastkey
 pause6809 = 0;
}

//Création de la boite de dialogue des manettes //////////////////////////////
void Drawjoystickbox()
{
 SDL_Rect rect;
 int i;
 char string[256];
 extern int pause6809;
 extern int blanc, bleu, gris0;
 pause6809 = 1;
 lastkeycode = 0;
 lastkeysym = 0;
 if(dialog != 4)
 {
  Createdialogbox(454, 252);
  rect.x = 10; rect.w = dialogbox->w - 32;
  rect.y = 5; rect.h = 15;
  Drawtextbox(dialogbox, msg[34][language], rect, 1, bleu, 0); //titre
  dialog = 4;
 }
 //message d'aide
 rect.x = 80; rect.y = 175; rect.w = 370;
 sprintf(string, "%s %s.", msg[16][language], msg[35][language]);
 Drawtextbox(dialogbox, string, rect, 0, gris0, 0);
 rect.y += 15;
 Drawtextbox(dialogbox, msg[17][language], rect, 0, gris0, 0);
 //dessin des boutons
 for(i = 0; i < JOYSTICKBUTTON_MAX; i++) Drawbutton(&joystickbutton[i], 0);
 //numero des manettes
 rect.x = 136; rect.y = 132; rect.w = 22;
 Drawtextbox(dialogbox, " [0]", rect, 0, blanc, 1);
 rect.x += 158;
 Drawtextbox(dialogbox, " [1]", rect, 0, blanc, 1);
 Displayjoy(0); //nom de la fonction lastjoy
 pause6809 = 0;
}

// Restauration de la configuration par defaut des touches ///////////////////
void Restorekeydefault()
{
 int i, j;
 for(i = 0; i < 256; i++)
 {
  for(j = 0; j < KEYBOARDKEY_MAX; j++) if(pckeycode[j] == i) break;
  to8dkeycode[i] = j;
 }
}

// Restauration de la configuration par defaut des manettes //////////////////
void Restorejoydefault()
{
 int i, j;
 for(i = 0; i < 256; i++)
 {
  for(j = 0; j < JOYSTICKKEY_MAX; j++) if(pcjoycode[j] == i) break;
  to8djoycode[i] = j;
 }
}

// Sauvegarde de la configuration du clavier /////////////////////////////////
void Savekeyfile()
{
 extern FILE *fpi;
 fseek(fpi, 0x40, SEEK_SET);
 fwrite(to8dkeycode, 256, 1, fpi);
}

// Sauvegarde de la configuration des manettes ///////////////////////////////
void Savejoyfile()
{
 extern FILE *fpi;
 fseek(fpi, 0x140, SEEK_SET);
 fwrite(to8djoycode, 256, 1, fpi);
}

//Traitement des clics boite de dialogue clavier /////////////////////////////
void Keyclick()
{
 int i, j, n, x, y;
 extern SDL_Rect dialogrect;    //position dialogbox
 extern int xmouse, ymouse;
 //recherche d'un clic bouton
 for(i = 0; i < KEYBOARDBUTTON_MAX; i++)
 {
  n = keyboardbutton[i].n;
  x = dialogrect.x + keyboardbutton[i].x;
  y = dialogrect.y + keyboardbutton[i].y;
  if(xmouse > x) if(xmouse < (x + bouton[keyboardbutton[i].n].w))
  if(ymouse > y) if(ymouse < (y + bouton[keyboardbutton[i].n].h))
  break;
 }
 if(i >= KEYBOARDBUTTON_MAX) return;
 //dessin du bouton enfonce puis relache
 Drawbutton(&keyboardbutton[i], 1);
 Displayscreen(); SDL_Delay(200);
 Drawbutton(&keyboardbutton[i], 0);
 //boutons restauration et sauvegarde
 if(n == 16) {Restorekeydefault(); return;}
 if(n == 17) {Savekeyfile(); return;}
 //suppression de l'ancienne affectation de la touche
 for(j = 0; j < 256; j++) if(to8dkeycode[j] == i) to8dkeycode[j] = KEYBOARDKEY_MAX;
 //ajout de la nouvelle affectation
 if(lastkeycode != 0) to8dkeycode[lastkeycode & 0xff] = i;
 Displaykey();
}

//Traitement des clics boite de dialogue manettes ////////////////////////////
void Joyclick()
{
 int i, j, n, x, y;
 extern SDL_Rect dialogrect;    //position dialogbox
 extern int xmouse, ymouse;
 //recherche d'un clic bouton
 for(i = 0; i < JOYSTICKBUTTON_MAX; i++)
 {
  n = joystickbutton[i].n;
  x = dialogrect.x + joystickbutton[i].x;
  y = dialogrect.y + joystickbutton[i].y;
  if(xmouse > x) if(xmouse < (x + bouton[joystickbutton[i].n].w))
  if(ymouse > y) if(ymouse < (y + bouton[joystickbutton[i].n].h))
  break;
 }
 if(i >= JOYSTICKBUTTON_MAX) return;
 //dessin du bouton enfonce puis relache
 Drawbutton(&joystickbutton[i], 1);
 Displayscreen(); SDL_Delay(200);
 Drawbutton(&joystickbutton[i], 0);
 //touches restauration et sauvegarde
 if(n == 16) {Restorejoydefault(); return;}
 if(n == 17) {Savejoyfile(); return;}
 //suppression de l'ancienne affectation de la touche
 for(j = 0; j < 256; j++) if(to8djoycode[j] == i) to8djoycode[j] = JOYSTICKKEY_MAX;
 //ajout de la nouvelle affectation
 if(lastkeycode != 0) to8djoycode[lastkeycode & 0xff] = i;
 Displayjoy();
}

// Initialisation du clavier /////////////////////////////////////////////////
void Keyboardinit()
{
 int i, j;
 extern FILE *fpi;
 //initialisation des tables par defaut
 for(i = 0; i < 256; i++)
 {
  for(j = 0; j < KEYBOARDKEY_MAX; j++) if(pckeycode[j] == i) break;
  to8dkeycode[i] = j;
  for(j = 0; j < JOYSTICKKEY_MAX; j++) if(pcjoycode[j] == i) break;
  to8djoycode[i] = j;
 }
 //recuperation des valeurs de dcto8d.ini (0x40=touches 0x140=joysticks)
 fseek(fpi, 0, SEEK_END);
 i = (int)ftell(fpi); //Info(i);
 if(i < 0x140) return;
 fseek(fpi, 0x40, SEEK_SET); j = fgetc(fpi); fseek(fpi, 0x40, SEEK_SET);
 if(j == KEYBOARDKEY_MAX) fread(to8dkeycode, 256, 1, fpi);
 if(i < 0x240) return;
 fseek(fpi, 0x140, SEEK_SET); j = fgetc(fpi); fseek(fpi, 0x140, SEEK_SET);
 if(j == JOYSTICKKEY_MAX) fread(to8djoycode, 256, 1, fpi);
}

// Key up ////////////////////////////////////////////////////////////////////
void Keyup(int keysym, int scancode)
{
 int ijoy, ikey, keycode;
 extern void Joysemul(int i, int state);
 extern void TO8key(int n);
 //le scancode seul ne permet pas de distinguer le pave numerique
 //keycode = scancode + 0x40 pour le pave numerique
 //keycode = scancode pour toutes les autres touches
 keycode = scancode;
 if(keysym == 0x12c) keycode += 0x40;           //numlock
 if((keysym & 0xff0) == 0x100) keycode += 0x40; //autres touches numpad

 //emulation joystick
 ijoy = to8djoycode[keycode & 0xff];
 ikey = to8dkeycode[keycode & 0xff];

 if(ijoy < JOYSTICKKEY_MAX) if((ikey == KEYBOARDKEY_MAX) || (keybpriority == 0))
 {
  Joysemul(ijoy, 0x80);
  if(dialog == 4) Drawbutton(&joystickbutton[ijoy], 0);
 }

 //emulation clavier
 if(ikey < KEYBOARDKEY_MAX) if((ijoy == JOYSTICKKEY_MAX) || (keybpriority == 1))
 {
  touche[ikey] = 0x80;
  TO8key(ikey);
  //dessin de la touche relachee
  if(dialog == 3) Drawbutton(&keyboardbutton[ikey], 0);
 }
}

// Key down //////////////////////////////////////////////////////////////////
void Keydown(int sym, int scancode, int unicode)
{
 int ijoy, ikey, keycode;
 extern int pause6809;
 extern dialogeditbox *focus;
 extern void Initprog();
 extern void exit(int n);
 extern void Joysemul(int i, int state);
 extern void TO8key(int n);
 //le scancode seul ne permet pas de distinguer le pave numerique
 //keycode = scancode + 0x40 pour le pave numerique
 //keycode = scancode pour toutes les autres touches
 keycode = scancode;
 if(sym == 0x12c) keycode += 0x40;           //numlock
 if((sym & 0xff0) == 0x100) keycode += 0x40; //autres touches numpad
 lastkeycode = keycode;
 lastkeysym = sym;

 //essai (infructueux) de detection des deux touches shift simultanees
 //quand une touche shift est enfoncée, les mouvements de l'autre
 //ne sont pas detectes, et elle est toujours consideree comme relachee
 //idem pour les touches CTRL droit et gauche
 //difference pour les touches ALT : si la gauche est enfoncee, la droite
 //est detectee, mais pas l'inverse.
 //Remarque : ce comportement est observe dans Windows, mais pas dans Linux

 //static int flag = 0;
 //Uint8 *keystate;
 //SDL_PumpEvents();
 //keystate = SDL_GetKeyState(NULL);
 //flag = 256 - flag;
 //Keyboard(flag + 2 * keystate[SDLK_LSHIFT] + keystate[SDLK_RSHIFT]); return;

 //SDL_Event event;
 //event.key.type = SDL_KEYUP;
 //event.key.state = SDL_RELEASED;
 //Keyboard(SDL_GetModState());
 //SDL_PushEvent(&event);
 //SDL_SetModState(0);

 //touches de raccourcis dcto8d
 if(sym == SDLK_ESCAPE) {Initprog(); pause6809 = 0; return;}
 if(sym == SDLK_PAUSE) {pause6809 = 1; return;}
 //if(keycode == 0x01) {Initprog(); pause6809 = 0; return;}//touche ESC enfoncee
 //if(keycode == 0x45) {pause6809 = 1; return;}            //touche PAUSE enfoncee

 //la touche AltGr envoie 2 evenements keydown
 //le premier keysym = 0x132 (LCTRL)
 //le deuxieme keysym = 0x133 (RALT)
 //il faut appeler Keyup pour annuler le faux Keydown de LCTRL
 //if(keysym == 0x133) Keyup(0x132);
 if(keycode == 0x38) Keyup(0, 0x1d);

 //editbox active
 if(focus != NULL)
 {
  int xmax;
  char *text;
  char string[TEXT_MAXLENGTH];
  extern int xcursor;
  text = focus->text;
  xmax = strlen(text);
  xmax = 4;
  switch(sym)
  {
   case SDLK_RIGHT : if(xcursor < strlen(text)) xcursor++; break;
   case SDLK_LEFT : if(xcursor > 0) xcursor--; break;
   case SDLK_BACKSPACE : if(xcursor < 1) break;
        strcpy(string, text + xcursor);
        text[--xcursor] = 0;
        strcat(text, string);
        break;
   case SDLK_DELETE : if(xcursor >= strlen(text)) break;
        strcpy(string, text + xcursor + 1);
        text[xcursor] = 0;
        strcat(text, string);
        break;
   default:
        if(xcursor > xmax - 1) break;
        if((unicode & 0xff) < 0x20) break;
        if((unicode & 0xff) > 0x126) break;
        strcpy(string, text + xcursor);
        text[xcursor] = unicode & 0xff;
        text[++xcursor] = 0;
        strcat(text, string);
  }
  Draweditbox(focus);
  return;
 }

 pause6809 = 0;  //l'appui sur une touche arrete la pause

 //emulation joystick
 ijoy = to8djoycode[keycode & 0xff]; //Info(i);
 ikey = to8dkeycode[keycode & 0xff];
 if(dialog == 3) Displaykey();
 if(dialog == 4) Displayjoy();


 if(ijoy < JOYSTICKKEY_MAX) if((ikey == KEYBOARDKEY_MAX) || (keybpriority == 0))
 {
  Joysemul(ijoy, 0x00);
  if(dialog == 4) Drawbutton(&joystickbutton[ijoy], 1);
 }

 //emulation clavier
 if(ikey < KEYBOARDKEY_MAX) if((ijoy == JOYSTICKKEY_MAX) || (keybpriority == 1))
 {
  touche[ikey] = 0x00; TO8key(ikey);
  if(dialog == 3) Drawbutton(&keyboardbutton[ikey], 1);
 }
}

// Test touches SHIFT, ALT et CTRL gauche et droite //////////////////////////
void Testshiftkey()
{
 //Dans Windows, si une touche majuscule droite ou gauche est enfoncee
 //l'evenement SDL_KEYDOWN n'est pas envoye par l'autre.
 //Il faut donc tester periodiquement leur etat (pour Flipper par exemple).
 //Malheureusement les fonctions SDL_GetModState et SDL_GetKeyState ne
 //marchent pas en cas d'appui simultane sur SHIFT, CTRL et ALT droit et gauche
 //Cette routine utilise la fonction GetAsyncKeyState de windows pour
 //detecter la veritable position des touches.
 //Elle est appellee environ 20 fois par seconde par la fonction Playsound

 //SDL_GetModState ne marche pas si les 2 touches sont enfoncees
 //int mode = SDL_GetModState();
 //extern int touche[]; //position touches MO5
 //touche[to8dkey[SDLK_LSHIFT]] = ((mode & 1)) ? 0x00 : 0x80;
 //touche[to8dkey[SDLK_RSHIFT]] = ((mode & 2)) ? 0x00 : 0x80;
 //sprintf(infos, "ModState=%04x", mode);
 //Printstring(infos, 0, 238, 7);
 //SDL_GetKeyState ne marche pas mieux
 //Uint8 *keystate = SDL_GetKeyState(NULL);
 //sprintf(infos, "Left=%i    Right=%i",
 //        keystate[SDLK_LSHIFT], keystate[SDLK_RSHIFT]);
 //Printstring(infos, 100, 238, 7);

#ifdef WIN32
 static int lshift, rshift; //position des touches shift
 //Remarque : VK_LSHIFT et VK_RSHIFT ne marchent pas avec Windows 98se
 //D'après Microsoft : Windows 95/98/Me: Windows 95 does not support
 //the left- and right-distinguishing constants. If you call
 //GetAsyncKeyState with these constants, the return value is zero.
 if(GetAsyncKeyState(VK_LSHIFT) < 0)
   {if(!lshift) {lshift = 1; Keydown(SDLK_LSHIFT, 0x2a, 0);}}
   else {if(lshift) {lshift = 0; Keyup(SDLK_LSHIFT, 0x2a);}}
 if(GetAsyncKeyState(VK_RSHIFT) < 0)
   {if(!rshift) {rshift = 1; Keydown(SDLK_RSHIFT, 0x36, 0);}}
   else {if(rshift) {rshift = 0; Keyup(SDLK_RSHIFT, 0x36);}}
#endif
}
