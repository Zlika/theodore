///////////////////////////////////////////////////////////////////////////////
// DCTO8DINTERFACE.C - Fonctions d'interface utilisateur
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
///////////////////////////////////////////////////////////////////////////////

#include <SDL.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "dcto8dinterface.h"
#include "dcto8dfont.h"
#include "dcto8dmsg.h"
#include "dcto8ddevices.h"
#include "dcto8dvideo.h"
#include "dcto8doptions.h"
#include "dcto8ddesass.h"
#include "dcto8dkeyb.h"

#define DIRLIST_LENGTH 32
#define DIRLIST_NMAX 500
#define POPUPTABLE_NMAX 10
#define BOUTON_MAX 104        //nombre de boutons differents
#define TEXT_MAXLENGTH 256
#define PATH_MAX 256
#define FLAGS 0   // flags in SDL_CreateRGBSurface are unused and should be set to 0

//masques et couleurs
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
const int rmask = 0xff000000, gmask = 0x00ff0000;
const int bmask = 0x0000ff00, amask = 0x000000ff;
const int blanc = 0xffffffff, noir  = 0x000000ff;
const int bleu  = 0x306090ff, grist = 0xe0e0e0e4;
const int gris0 = 0xe0e0e0ff;
const int gris1 = 0xd0d0d0ff;
const int gris2 = 0x808080ff;
const int gris3 = 0x606060ff;
#else
const int rmask = 0x000000ff, gmask = 0x0000ff00;
const int bmask = 0x00ff0000, amask = 0xff000000;
const int blanc = 0xffffffff, noir  = 0xff000000;
const int bleu  = 0xff906030, grist = 0xe4e0e0e0;
const int gris0 = 0xffe0e0e0;
const int gris1 = 0xffd0d0d0;
const int gris2 = 0xff808080;
const int gris3 = 0xff606060;
#endif

//definition des boutons
button bouton[BOUTON_MAX] = {
{"\xa4""1",        16,  16},   //00 croix
{" ""\xa4""6",       16,   8}, //01 previous
{" ""\xa4""7",       16,   8}, //02 next
{"\xa4""8",        16,  16},   //03 fleche pour popup
{"",           0,   0}, //04
{"",           0,   0}, //05
{"",           0,   0}, //06
{"",           0,   0}, //07
{"Options",   42,  16}, //08
{" ?",        18,  16}, //09
{" + 10",     35,  18}, //10 +10
{"k7",        18,  16}, //11
{"fd",        18,  16}, //12
{"memo",      36,  16}, //13
{"{0}",      75,  18}, //14 desassembler
{" + 1",      30,  18}, //15 +1
{"{1}",     172,  18}, //16
{"{2}",     172,  18}, //17
{"{3}",     150,  18}, //18 parametres par defaut
{" Action",   44,  18}, //19
{"F7  F2",    38,  18}, //0x00
{" _ 6",      26,  24}, //0x01
{"  Y",       26,  24}, //0x02
{"  H",       26,  24}, //0x03
{"  ""\xa4""2",      26,  24}, //0x04 fleche haut
{"  ""\xa4""5",      26,  24}, //0x05 fleche droite
{"RAZ",       34,  24}, //0x06
{"  N",       26,  24}, //0x07
{"F8  F3",    38,  18}, //0x08
{" (  5",     26,  24}, //0x09
{"  T",       26,  24}, //0x0a
{"  G",       26,  24}, //0x0b
{" = +",      26,  24}, //0x0c
{" ""\xa4""4",       26,  24}, //0x0d fleche gauche
{"INS",       26,  24}, //0x0e
{"  B",       26,  24}, //0x0f
{"F9  F4",    38,  18}, //0x10
{" '  4",     26,  24}, //0x11
{"  R",       26,  24}, //0x12
{"  F",       26,  24}, //0x13
{"ACC",       26,  24}, //0x14
{"  1",       26,  24}, //0x15 pad
{"EFF",       26,  24}, //0x16
{"  V",       26,  24}, //0x17
{"F10 F5",    38,  18}, //0x18
{" \" 3",     26,  24}, //0x19
{"  E",       26,  24}, //0x1a
{"  D",       26,  24}, //0x1b
{"  7",       26,  24}, //0x1c pad
{"  4",       26,  24}, //0x1d pad
{"  0",       26,  24}, //0x1e pad
{"  C",       26,  24}, //0x1f
{"F6  F1",    38,  18}, //0x20
{" ""\xa4"" 2",      26,  24}, //0x21
{"  Z",       26,  24}, //0x22
{"  S",       26,  24}, //0x23
{"  8",       26,  24}, //0x24 pad
{"  2",       26,  24}, //0x25 pad
{"   .",      26,  24}, //0x26 pad
{"  X",       26,  24}, //0x27
{" # @",      32,  24}, //0x28
{" * 1",      26,  24}, //0x29
{"  A",       26,  24}, //0x2a
{"  Q",       26,  24}, //0x2b
{" [  {",     26,  24}, //0x2c
{"  5",       26,  24}, //0x2d pad
{"  6",       26,  24}, //0x2e pad
{"  W",       26,  24}, //0x2f
{"  STOP",    44,  24}, //0x30
{" ""\xa4"" 7",      26,  24}, //0x31
{"  U",       26,  24}, //0x32
{"  J",       26,  24}, //0x33
{"ESPACE",   208,  24}, //0x34
{"  9",       26,  24}, //0x35 pad
{"ENT",       26,  24}, //0x36 pad
{" , ?",      26,  24}, //0x37
{"?????",     26,  24}, //0x38
{" !  8",     26,  24}, //0x39
{"  I",       26,  24}, //0x3a
{"  K",       26,  24}, //0x3b
{"  $  &",    40,  24}, //0x3c
{"  ""\xa4""3",      26,  24}, //0x3d fleche bas
{"  ]  }",    32,  24}, //0x3e
{" ;  .",     26,  24}, //0x3f
{"?????",     26,  24}, //0x40
{" ""\xa4"" 9",      26,  24}, //0x41
{"  O",       26,  24}, //0x42
{"  L",       26,  24}, //0x43
{" - \\",     26,  24}, //0x44
{"\xa4"" %",       26,  24}, //0x45
{"ENT",       26,  50}, //0x46
{" : /",      26,  24}, //0x47
{"?????",     26,  24}, //0x48
{" ""\xa4"" 0",      26,  24}, //0x49
{"  P",       26,  24}, //0x4a
{"  M",       26,  24}, //0x4b
{"  ) ""\xa4""",     26,  24}, //0x4c
{" ^ ""\xa4""",      26,  24}, //0x4d
{"  3",       26,  24}, //0x4e pad
{" > <",      26,  24}, //0x4f
{"LCK",       26,  24}, //0x50
{"SHIFT",     38,  24}, //0x51
{"SHIFT",     38,  24}, //0x52
{"CNT",       26,  24}  //0x53
};

int fontw[] = {                                    //largeur des caracteres
 0,10, 7, 7,12,12, 4, 4,10,13,13, 0, 0, 0, 0, 0,   //croix et fleches
 6, 6, 6, 6, 6, 6, 4, 4, 6, 6, 6, 6, 4, 0, 0, 0,   //lettres accentuees & degre
 3, 3, 5, 7, 6, 8, 6, 2, 3, 3, 6, 6, 3, 4, 3, 5,
 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 6, 6, 6, 6,
11, 7, 7, 7, 8, 7, 6, 8, 8, 3, 5, 7, 6, 9, 8, 8,
 7, 8, 8, 7, 7, 8, 7,11, 7, 7, 7, 3, 5, 3, 6, 6,
 3, 6, 6, 6, 6, 6, 3, 6, 6, 2, 2, 6, 2, 8, 6, 6,
 6, 6, 3, 5, 3, 6, 6, 8, 5, 5, 5, 4, 2, 4, 7, 2};

const dialogbutton closebutton = {-21, 5, 0};

#define STATUSBUTTON_MAX 5    //nombre de boutons de la barre de statut
const dialogbutton statusbutton[STATUSBUTTON_MAX] = {
{ 56,  2, 11}, //0x00 "k7",
{186,  2, 12}, //0x01 "fd",
{316,  2, 13}, //0x02 "memo",
{-64,  2,  8}, //0x03 "Options",
{-20,  2,  9}  //0x04 "?",
};

#define OPTIONBUTTON_MAX 8   //nombre de boutons boite de dialogue options
const dialogbutton optionbutton[OPTIONBUTTON_MAX] = {
{108,  32,  3}, //langue
{108,  52,  3}, //zoom
{108,  72,  3}, //processeur
{108,  92,  3}, //affichage
{108, 112,  3}, //protection k7
{108, 132,  3}, //protection fd
{108, 152,  3}, //emulation manettes
{ 10, 180, 18}  //valeurs par defaut
};

SDL_Surface *buttonup[BOUTON_MAX][LANGUAGE_MAX];
SDL_Surface *buttondown[BOUTON_MAX][LANGUAGE_MAX];
SDL_Surface *fontsurface[2];   //0=noir 1=blanc
SDL_Surface *textbox = NULL;   //surface d'affichage de texte
SDL_Surface *dialogbox = NULL; //surface d'affichage dialogbox
SDL_Surface *statusbar = NULL; //surface de la barre de statut
SDL_Rect dialogrect;           //position dialogbox
SDL_Rect popuptablerect;       //position popup table
currentdialog dialog = DIALOG_NOTHING;
int popuptable = 0;            //pouptable inactive par defaut
const dialogeditbox *focus = NULL;   //editbox ayant le focus
int xmove, ymove;              //position souris dans dialogbox deplacee
int dircount;                  //nombre de fichiers dans le repertoire
int dirmin, dirmax;            //plage de numeros de fichiers affiches
int blink = -1;                //flag de clignotement du curseur
int xcursor = 0;               //position du curseur dans la chaine de caracteres
char dirlist[DIRLIST_NMAX][DIRLIST_LENGTH]; //liste des fichiers du repertoire
const char *popuptabletext[POPUPTABLE_NMAX];  //pointeur vers lignes de la popup table
char path[3][PATH_MAX];   //repertoires des fichiers k7, fd, memo
char k7name[100];               // nom du fichier cassette
char fdname[100];               // nom du fichier disquette
char memoname[100];             // nom du fichier cartouche
char* devnames[] = { k7name, fdname, memoname };
// pointeurs fonctions de chargement de fichier
void (*LoadFunc[3])(char *filename) = { Loadk7, Loadfd, Loadmemo };
// pointeurs fonctions de déchargement
void (*UnloadFunc[3])() = { Unloadk7, Unloadfd, Unloadmemo };
int pause6809;                  //processor pause state

static void Optionclick();
static void Setoption(int i);

// Message d'erreur SDL //////////////////////////////////////////////////////
void SDL_error(const char* function, const char* message)
{
 char string[256];
 sprintf(string, "%s : %s\n%s", function, message, SDL_GetError());
 SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                          "SDL Error",
                          string,
                          NULL);
 SDL_Delay(1000);
}

// About message box /////////////////////////////////////////////////////////
static void About()
{
 SDL_version linked;
 char message[1024];

 message[0] = '\0';
 sprintf(message + strlen(message), "\n%s", _(MSG_ABOUT));
 SDL_GetVersion(&linked);
 sprintf(message + strlen(message), "\n\nSDL %d.%d.%d", linked.major, linked.minor, linked.patch);

 SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                          _(MSG_ABOUT_TITLE),
                          message,
                          NULL);
}

//Creation d'une surface contenant un texte////////////////////////////////////
static SDL_Surface *Rendertext(const char *string, int color, int background)
{
 //color
 //0 = noir largeur variable
 //1 = blanc largeur variable
 //2 = noir largeur fixe
 //3 = blanc largeur fixe
 SDL_Surface *surface;
 SDL_Rect rfont, rtext;
 int i, j, c, n;
 char text[TEXT_MAXLENGTH];
 n = strlen(string);
 if(n > TEXT_MAXLENGTH - 1) n = TEXT_MAXLENGTH - 1;
 text[n] = 0;

 //conversion des caracteres accentues et degre
 for(i = 0; i < n; i++)
 {
  c = string[i] & 0xff;
  switch(c)
  {
   case 224: c = 16; break; //à
   case 226: c = 17; break; //â
   case 232: c = 18; break; //è
   case 233: c = 19; break; //é
   case 234: c = 20; break; //ê
   case 235: c = 21; break; //ë
   case 238: c = 22; break; //î
   case 239: c = 23; break; //ï
   case 244: c = 24; break; //ô
   case 249: c = 25; break; //ù
   case 251: c = 26; break; //û
   case 231: c = 27; break; //ç
   case 176: c = 28; break; //°
  }
  text[i] = c;
 }

 //conversion des carateres speciaux
 for(i = 0; i < n; i++)
 {
  if(text[i] != '\xa4') continue;
  text[i] = text[i + 1] - 48; n--;
  for(j = i + 1; j < n; j++) text[j] = text[j + 1];
 }

 //calcul de la longueur du texte
 j = 0;
 if(color & 2) j += 8 * n;
 else for(i = 0; i < n; i++) j += fontw[text[i] & 0x7f];
 //creation de la surface du texte
 surface = SDL_CreateRGBSurface(FLAGS, j, 12, 32, rmask, gmask, bmask, amask);
 SDL_FillRect(surface, NULL, background);
 //composition du texte
 rtext.x = 0; rtext.y = 0;
 rtext.h = 12; rfont.h = 12;
 for(i = 0; i < n; i++)
 {
  c = text[i] & 0x7f;
  rfont.x = 0; for(j = 0; j < (c & 0x0f); j++) rfont.x += fontw[(c & 0xf0) + j];
  rfont.y = 13 * (c / 16) + 1;
  rfont.w = fontw[c];
  if(color & 2) if(rfont.w < 8) rtext.x += (8 - rfont.w) / 2;
  SDL_BlitSurface(fontsurface[color & 1], &rfont, surface, &rtext);
  if(color & 2) if(rfont.w < 8) rtext.x -= (8 - rfont.w) / 2;
  if(color & 2) rtext.x += 8;
  else rtext.x += fontw[c];
 }
 return surface;
}

//Initialisation image de fond d'un bouton relache ////////////////////////////
static void Initbuttonup(SDL_Surface *s, int w, int h)
{
 SDL_Rect r;
 r.x = 0; r.y = 0; r.w = w;     r.h = h;     SDL_FillRect(s, &r, gris1);
 r.x = 1; r.y = 1; r.w = w - 1; r.h = h - 1; SDL_FillRect(s, &r, noir);
 r.x = 0; r.y = 0; r.w = w - 1; r.h = h - 1; SDL_FillRect(s, &r, blanc);
 r.x = 1; r.y = 1; r.w = w - 2; r.h = h - 2; SDL_FillRect(s, &r, gris2);
 r.x = 1; r.y = 1; r.w = w - 3; r.h = h - 3; SDL_FillRect(s, &r, gris1);
 r.x = 0; r.y = 0; r.w = 1;     r.h = 1;     SDL_FillRect(s, &r, gris0);
 r.x = w - 2; r.y = 0;     SDL_FillRect(s, &r, gris0);
 r.x = w - 1; r.y = 0;     SDL_FillRect(s, &r, gris0);
 r.x = 1;     r.y = 1;     SDL_FillRect(s, &r, blanc);
 r.x = w - 3; r.y = 1;     SDL_FillRect(s, &r, blanc);
 r.x = w - 1; r.y = 1;     SDL_FillRect(s, &r, gris0);
 r.x = 1;     r.y = h - 3; SDL_FillRect(s, &r, blanc);
 r.x = w - 3; r.y = h - 3; SDL_FillRect(s, &r, gris2);
 r.x = 0;     r.y = h - 2; SDL_FillRect(s, &r, gris0);
 r.x = w - 2; r.y = h - 2; SDL_FillRect(s, &r, noir);
 r.x = 0;     r.y = h - 1; SDL_FillRect(s, &r, gris0);
 r.x = 1;     r.y = h - 1; SDL_FillRect(s, &r, gris0);
 r.x = w - 1; r.y = h - 1; SDL_FillRect(s, &r, gris0);
}

//Initialisation image de fond d'un bouton enfonce ////////////////////////////
static void Initbuttondown(SDL_Surface *s, int w, int h)
{
 SDL_Rect r;
 r.x = 0; r.y = 0; r.w = w;     r.h = h;     SDL_FillRect(s, &r, noir);
 r.x = 1; r.y = 1; r.w = w - 1; r.h = h - 1; SDL_FillRect(s, &r, gris2);
 r.x = 2; r.y = 2; r.w = w - 2; r.h = h - 2; SDL_FillRect(s, &r, blanc);
 r.x = 2; r.y = 2; r.w = w - 3; r.h = h - 3; SDL_FillRect(s, &r, gris1);
 r.x = 0; r.y = 0; r.w = 1;     r.h = 1;     SDL_FillRect(s, &r, gris0);
 r.x = w - 1; r.y = 0;     SDL_FillRect(s, &r, gris0);
 r.x = 1;     r.y = 1;     SDL_FillRect(s, &r, noir);
 r.x = 2;     r.y = 2;     SDL_FillRect(s, &r, gris2);
 r.x = w - 2; r.y = h - 2; SDL_FillRect(s, &r, blanc);
 r.x = 0;     r.y = h - 1; SDL_FillRect(s, &r, gris0);
 r.x = w - 1; r.y = h - 1; SDL_FillRect(s, &r, gris0);
}

//Initialisation des surfaces des polices ////////////////////////////////////
void Initfontsurfaces()
{
 SDL_Rect r;
 int i, j, k;

 fontsurface[0] = SDL_CreateRGBSurface //texte noir
                  (FLAGS, 128, 104, 32, rmask, gmask, bmask, amask);
 fontsurface[1] = SDL_CreateRGBSurface //texte blanc
                  (FLAGS, 128, 104, 32, rmask, gmask, bmask, amask);
 r.h = 1; r.w = 1;
 for(i = 0; i < 104; i++) for(j = 0; j < 16; j++)
 {
  int c = dcto8dfont[16 * i + j];
  r.x = 8 * j; r.y = i;
  for(k = 128; k > 0; k >>= 1)
  {
   if((c & k) == 0)
   {
    SDL_FillRect(fontsurface[0], &r, noir);
    SDL_FillRect(fontsurface[1], &r, blanc);
   }
   r.x += 1;
  }
 }
}

//Initialisation des surfaces des boutons ////////////////////////////////////
void Initbuttonsurfaces()
{
 SDL_Rect r;
 int i, j, w, h;
 const char *string;

 for(i = 0; i < BOUTON_MAX; i++)
 {
  if(bouton[i].name[0] == 0) continue; //bouton non cree
  w = bouton[i].w;
  h = bouton[i].h;
  string = bouton[i].name;
  //creation du bouton dans chaque langue
  for(j = 0; j < LANGUAGE_MAX; j++)
  {
   //boutons non traduits
   if(j > 0) if(string[0] != '{')
   {
    buttonup[i][j] = buttonup[i][0];
    buttondown[i][j] = buttondown[i][0];
    continue;
   }
   //creation de la surface et du fond
   buttonup[i][j] = SDL_CreateRGBSurface
    (FLAGS, w, h, 32, rmask, gmask, bmask, amask);
   buttondown[i][j] = SDL_CreateRGBSurface
    (FLAGS, w, h, 32, rmask, gmask, bmask, amask);
   Initbuttonup(buttonup[i][j], w, h);
   Initbuttondown(buttondown[i][j], w, h);
   //recherche du texte traduit en plusieurs langues
   if(string[0] == '{')
   {
    int imsg;
    imsg = strtol(string + 1, NULL, 10);
    string = msg_btn[imsg][j];
   }
   //creation de l'image du texte
   SDL_FreeSurface(textbox);
   r.x = 2; r.y = 2;
   if(i > 19) r.y = 4; //4 pour les touches
   textbox = Rendertext(string, 0, gris1);
   if(textbox == NULL) {SDL_error(__func__, "textbox == NULL"); return;}

   //application du texte sur la surface du bouton
   if(SDL_BlitSurface(textbox, NULL, buttonup[i][j], &r) < 0) SDL_error(__func__, "SDL_BlitSurface");
   if(SDL_BlitSurface(textbox, NULL, buttondown[i][j], &r) < 0) SDL_error(__func__, "SDL_BlitSurface");
  }
 }
}

//Tri du repertoire path /////////////////////////////////////////////////////
static void Sortdirectory(char *path)
{
 DIR *dir;
 DIR *dummy;
 struct dirent *entry;
 char name[PATH_MAX];
 dircount = 0;
 if((dir = opendir(path)) == NULL) return;
 while((entry = readdir(dir)))
 {
  if(!strcasecmp(entry->d_name, ".")) continue;
  strcpy(name, path);
  strcat(name, entry->d_name);
  dirlist[dircount][0] = '\0';
  dummy = opendir(name);
  if(dummy) {closedir(dummy); strcpy(dirlist[dircount], " ""\xa4"": ");}
  if(!strcmp(entry->d_name, "..")) strcpy(dirlist[dircount], " ""\xa4""9 ");
  if(strlen(dirlist[dircount]) > 0) dirlist[dircount][0] = 15;
  strncat(dirlist[dircount], entry->d_name, DIRLIST_LENGTH);
  dirlist[dircount][DIRLIST_LENGTH - 1] = '\0';
  if(++dircount >= DIRLIST_NMAX) break;
 }
 closedir(dir);
 //tri de la liste
 qsort(dirlist, dircount, DIRLIST_LENGTH,
      (int(*)(const void*, const void*))strcmp);
}

//Draw textbox ///////////////////////////////////////////////////////////////
void Drawtextbox(SDL_Surface *surf, const char *string, SDL_Rect rect,
                 int textcolor, int boxcolor, int relief)
{
 int r;
 SDL_Rect textrect;
 SDL_Surface *Rendertext(const char *string, int color, int background);
 SDL_FillRect(surf, &rect, boxcolor);
 if(relief > 0) //en relief
 {
  SDL_Rect bord;
  bord.x = rect.x + rect.w; bord.y = rect.y + 1;
  bord.w = 1; bord.h = rect.h;
  SDL_FillRect(surf, &bord, gris2);
  bord.x = rect.x + 1; bord.y = rect.y + rect.h;
  bord.w = rect.w; bord.h = 1;
  SDL_FillRect(surf, &bord, gris2);
 }
 if(relief < 0) //en creux
 {
  SDL_Rect bord;
  boxcolor = blanc;
  bord.x = rect.x; bord.y = rect.y;
  bord.w = rect.w; bord.h = 1;
  SDL_FillRect(surf, &bord, gris2);
  bord.w = 1; bord.h = rect.h;
  SDL_FillRect(surf, &bord, gris2);
 }
 if(string[0] == 0) return;
 SDL_FreeSurface(textbox);
 textbox = Rendertext(string, textcolor, boxcolor);
 if(textbox == NULL) {SDL_error(__func__, "textbox == NULL"); return;}
 textrect.x = -2; textrect.w = rect.w - 4;
 textrect.y = -2; textrect.h = rect.h;
 r = SDL_BlitSurface(textbox, &textrect, surf, &rect);
 if(r < 0) SDL_error(__func__, "r < 0");
}

//Dessin d'un bouton///////////////////////////////////////////////////////////
void Drawbutton(const dialogbutton *bouton, int push)
{
 SDL_Rect rect;
 int n;
 n = bouton->n;
 rect.x = bouton->x;
 rect.y = bouton->y;
 if(push) SDL_BlitSurface(buttondown[n][options.language], NULL, dialogbox, &rect);
 else SDL_BlitSurface(buttonup[n][options.language], NULL, dialogbox, &rect);
}

//Dessin d'une editbox/////////////////////////////////////////////////////////
void Draweditbox(const dialogeditbox *box)
{
 SDL_Rect rect;
 char text[TEXT_MAXLENGTH];
 rect.x = box->x; rect.w = box->w;
 rect.y = box->y; rect.h = box->h;
 strcpy(text, box->text);
 //positionnement du curseur
 if(focus == box) if(blink)
 {
  text[xcursor] = '|';
  text[xcursor + 1] = 0;
  strcat(text, box->text + xcursor);
 }
 Drawtextbox(dialogbox, text, rect, 0, blanc, -1);
}

//Index cassette /////////////////////////////////////////////////////////////
static void Drawk7index()
{
 SDL_Rect rect;
 char index[10];
 index[0] = 0;
 PrintK7Index(index);
 rect.x = 2; rect.w = 52; rect.y = 2; rect.h = 15;
 Drawtextbox(statusbar, index, rect, 0, blanc, -1);
}

//Draw status bar ////////////////////////////////////////////////////////////
void Drawstatusbar()
{
 SDL_Rect rect;
 SDL_Surface *surf;
 int i, r;

 if(statusbar == NULL) return;

 //fond
 rect.x = 0; rect.w = options.xclient;
 rect.y = 0; rect.h = YSTATUS - 1;
 SDL_FillRect(statusbar, &rect, gris0);
 rect.y = YSTATUS - 1; rect.h = 1;
 SDL_FillRect(statusbar, &rect, blanc);

 //index cassette
 Drawk7index();

 //nom cassette, disquette, memo7
 rect.y = 2; rect.h = 15; rect.w = 104;
 rect.x = 76;  Drawtextbox(statusbar, k7name, rect, 0, blanc, -1);
 rect.x = 206; Drawtextbox(statusbar, fdname, rect, 0, blanc, -1);
 rect.x = 354; Drawtextbox(statusbar, memoname, rect, 0, blanc, -1);

 //boutons
 for(i = 0; i < STATUSBUTTON_MAX; i++)
 {
  rect.x = statusbutton[i].x; if(rect.x < 0) rect.x += options.xclient;
  rect.y = statusbutton[i].y; if(rect.y < 0) rect.y += options.yclient;
  surf = buttonup[statusbutton[i].n][options.language];
  r = SDL_BlitSurface(surf, NULL, statusbar, &rect);
  if(r < 0) {SDL_error(__func__, "r < 0"); return;}
 }
}

static void InitPaths()
{
 char currentdir[256];
 memset(currentdir, 0, 256);
 if (getcwd(currentdir, 256) == NULL) currentdir[0] = '\0';
 strcat(currentdir, "/");
 strcpy(path[0], currentdir);
 strcpy(path[1], currentdir);
 strcpy(path[2], currentdir);
}

//Init status bar ////////////////////////////////////////////////////////////
void Initstatusbar()
{
 InitPaths();
 SetUpdateK7IndexCallback(&Drawk7index);
 statusbar = SDL_CreateRGBSurface(FLAGS, 2048, YSTATUS, 32, rmask, gmask, bmask, amask);
 if(statusbar == NULL) SDL_error(__func__, "statusbar == NULL");
 Drawstatusbar();
}

//Create box /////////////////////////////////////////////////////////////////
static void Createbox(int color)
{
 SDL_Rect rect;
 SDL_Delay(100);
 if(dialogbox != NULL) SDL_FreeSurface(dialogbox);
 dialogbox = SDL_CreateRGBSurface(FLAGS, dialogrect.w, dialogrect.h,
                                  32, rmask, gmask, bmask, amask);
 //fond
 rect.x = 0; rect.w = dialogbox->w;
 rect.y = 0; rect.h = dialogbox->h;
 SDL_FillRect(dialogbox, &rect, color);
 //bordure haut blanche
 rect.h = 1;
 SDL_FillRect(dialogbox, &rect, blanc);
 //bordure gauche blanche
 rect.x = 0; rect.w = 1;
 rect.y = 0; rect.h = dialogbox->h - 1;
 SDL_FillRect(dialogbox, &rect, blanc);
 //bordure bas gris fonce
 rect.x = 1; rect.w = dialogbox->w - 2;
 rect.y = dialogbox->h - 2; rect.h = 1;
 SDL_FillRect(dialogbox, &rect, gris2);
 //bordure bas gris noire
 rect.x += 1; rect.y += 1;
 SDL_FillRect(dialogbox, &rect, noir);
 //bordure droite gris fonce
 rect.x = dialogbox->w - 2; rect.w = 1;
 rect.y = 1; rect.h = dialogbox->h - 3;
 SDL_FillRect(dialogbox, &rect, gris2);
 //bordure droite gris noire
 rect.x += 1; rect.y += 1;
 SDL_FillRect(dialogbox, &rect, noir);
}

//Create dialog box //////////////////////////////////////////////////////////
void Createdialogbox(int w, int h)
{
 SDL_Rect rect;
 int r;
 dialogrect.x = 10; dialogrect.w = w;
 dialogrect.y = YSTATUS + 10; dialogrect.h = h;
 Createbox(gris0); //boite opaque
 //barre de titre bleue
 rect.x = 4; rect.w = dialogbox->w - 8;
 rect.y = 3; rect.h = 20;
 SDL_FillRect(dialogbox, &rect, bleu);
 //bouton de fermeture
 rect.x = closebutton.x + dialogbox->w;
 rect.y = closebutton.y;
 r = SDL_BlitSurface(buttonup[closebutton.n][options.language], NULL, dialogbox, &rect);
 if(r < 0) SDL_error(__func__, "r < 0");
}

//Draw menu box ////////////////////////////////////////////////////////////
static void Drawmenubox()
{
 SDL_Rect rect;
 char string[50];
 dialogrect.x = options.xclient - 122; dialogrect.w = 120;
 dialogrect.y = YSTATUS; dialogrect.h = 4 * 16 + 10;
 Createbox(gris0);
 rect.x = 10; rect.w = 105; rect.y = 4; rect.h = 14;

 sprintf(string, "%s...", _(MSG_MENU_SETTINGS));
 Drawtextbox(dialogbox, string, rect, 0, gris0, 0);
 rect.y += 16;
 sprintf(string, "%s...", _(MSG_MENU_KEYBOARD));
 Drawtextbox(dialogbox, string, rect, 0, gris0, 0);
 rect.y += 16;
 sprintf(string, "%s...", _(MSG_MENU_JOYSTICKS));
 Drawtextbox(dialogbox, string, rect, 0, gris0, 0);
 rect.y += 16;
 sprintf(string, "%s...", _(MSG_MENU_DISASSEMBLY));
 Drawtextbox(dialogbox, string, rect, 0, gris0, 0);
 rect.y += 16;

 dialog = 1003;
}

//Draw popup directory ////////////////////////////////////////////////////////
static void Drawpopupdirectory(int n)
{
 SDL_Rect rect;
 int i;
 const char *string;
 Sortdirectory(path[n]);
 if(dircount <= 0) return;
 dirmax = dirmin + 20;
 if(dirmax > dircount) dirmax = dircount;
 dialogrect.x = statusbutton[n].x; dialogrect.w = 200;
 dialogrect.y = YSTATUS; dialogrect.h = 16 * (dirmax - dirmin) + 26;
 if((dirmax < dircount) || (dirmin > 0)) dialogrect.h += 16; //[suite]/[debut]
 Createbox(gris0);
 rect.x = 10; rect.w = dialogrect.w - 12;
 rect.y = 4; rect.h = 14;
 Drawtextbox(dialogbox, _(MSG_UNLOAD), rect, 0, gris0, 0);
 for(i = dirmin; i < dirmax; i++)
 {
  rect.y += 16;
  Drawtextbox(dialogbox, dirlist[i], rect, 0, gris0, 0);
 }
 if((dirmax < dircount) || (dirmin > 0))
 {
  rect.y += 16;
  string = (dirmax == dircount) ? _(MSG_BACK_TO_FIRST) : _(MSG_NEXT);
  Drawtextbox(dialogbox, string, rect, 0, gris0, 0);
 }
 dialog = 1000 + n;
}

//Traitement des clics souris dans la barre de statut ////////////////////////
static void Statusclick()
{
 SDL_Rect rect;
 int i, r, x, y;

 //recherche du bouton concerne par le clic
 for(i = 0; i < STATUSBUTTON_MAX; i++)
 {
  x = statusbutton[i].x; if(x < 0) x += options.xclient;
  y = statusbutton[i].y; if(y < 0) y += options.yclient;
  if(xmouse > x) if(xmouse < (x + bouton[statusbutton[i].n].w))
  if(ymouse > y) if(ymouse < (y + bouton[statusbutton[i].n].h))
  break;
 }
 if(i >= STATUSBUTTON_MAX) {dialog = DIALOG_NOTHING; return;}

 //dessin du bouton enfonce
 rect.x = statusbutton[i].x; if(rect.x < 0) rect.x += options.xclient;
 rect.y = statusbutton[i].y; if(rect.y < 0) rect.y += options.yclient;
 r = SDL_BlitSurface(buttondown[statusbutton[i].n][options.language], NULL, screen, &rect);
 if(r < 0) {SDL_error(__func__, "r < 0"); return;}

 //traitement
 switch(i)
 {
  case 0: dirmin = 0; Drawpopupdirectory(0); break;
  case 1: dirmin = 0; Drawpopupdirectory(1); break;
  case 2: dirmin = 0; Drawpopupdirectory(2); break;
  case 3: Drawmenubox(); break;
  case 4: About(); break;
 }
 Drawstatusbar();
}

//Deplacement de la boite de dialogue ////////////////////////////////////////
void Dialogmove()
{
 int ytotal;
 ytotal = options.yclient + YSTATUS;
 dialogrect.x = xmouse - xmove;
 dialogrect.y = ymouse - ymove;
 if(dialogrect.x > (options.xclient - dialogrect.w))
   dialogrect.x = options.xclient - dialogrect.w;
 if(dialogrect.y > (ytotal - dialogrect.h))
    dialogrect.y = ytotal - dialogrect.h;
 if(dialogrect.x < 0)
    dialogrect.x = 0;
 if(dialogrect.y < YSTATUS)
    dialogrect.y = YSTATUS;
}

//Traitement des clics souris dans la boite de dialogue //////////////////////
static void Dialogclick()
{
 int x, y;

 //suppression du focus actuel
 if(focus != NULL) {blink = 0; Draweditbox(focus); focus = NULL;}

 //clic bouton de fermeture
 x = dialogrect.x + dialogrect.w + closebutton.x;
 y = dialogrect.y + closebutton.y;
 if(xmouse > x) if(xmouse < (x + bouton[closebutton.n].w))
 if(ymouse > y) if(ymouse < (y + bouton[closebutton.n].h))
 {dialog = DIALOG_NOTHING; xmove = ymove = 0; return;}

 //clic bandeau superieur
 if(ymouse < (dialogrect.y + 22))
 {
  xmove = xmouse - dialogrect.x;
  ymove = ymouse - dialogrect.y;
  return;
 }

 //traitements particuliers
 if(dialog == DIALOG_OPTIONS) Optionclick();  //boite de dialogue Parametres
 if(dialog == DIALOG_KEYBOARD) Keyclick();     //boite de dialogue Clavier
 if(dialog == DIALOG_JOYSTICK) Joyclick();     //boite de dialogue Manettes
 if(dialog == DIALOG_DISASM) Desassclick();  //boite de dialogue Desassemblage
}

//Traitement des clics dans un menu deroulant /////////////////////////////////
static void Menuclick()
{
 int i, j, n;
 char filename[PATH_MAX];
 filename[0] = '\0';

 n = dialog - 1000;

 if(n == 3) //clic dans le menu options
 {
  if(ymouse < (YSTATUS + 20)) {Drawoptionbox(); return;}
  if(ymouse < (YSTATUS + 36)) {Drawkeyboardbox(); return;}
  if(ymouse < (YSTATUS + 52)) {Drawjoystickbox(); return;}
  if(ymouse < (YSTATUS + 68)) {pause6809 = 1; Drawdesassbox(); pause6809 = 0; return;}
  return;
 }
 if((n < 0) || (n > 2)) return;

 //clic dans une popupdirectory
 i = dirmin - 1 + (ymouse - YSTATUS - 4) / 16;
 if(i < dirmin) //[decharger]
 {
  devnames[n][0] = '\0';
  UnloadFunc[n]();
  dialog = DIALOG_NOTHING;
  Drawstatusbar();
  return;
 }
 if(i == dirmax) //[suite] ou [debut]
 {
  dirmin += 20; if(dirmin >= dircount) dirmin = 0;
  Drawpopupdirectory(n);
  return;
 }
 if(dirlist[i][2] == '9') //ouverture repertoire niveau superieur
 {
  for(j = strlen(path[n]) - 2; j > 0; j--)
  {
   if(path[n][j] == '/') {path[n][j + 1] = 0; break;}
   if(path[n][j] == '\\') {path[n][j + 1] = 0; break;}
  }
  dirmin = 0;
  Drawpopupdirectory(n);
  return;
 }
 if(dirlist[i][2] == ':') //ouverture sous-repertoire
 {
  strcat(path[n], dirlist[i] + 4);
  strcat(path[n], "/");
  dirmin = 0;
  Drawpopupdirectory(n);
  return;
 }
 if(dirlist[i][0] > 15) //ouverture fichier
 {
  strcpy(devnames[n], dirlist[i]);
  strcpy(filename, path[n]);
  strcat(filename, dirlist[i]);
  LoadFunc[n](filename);
  Drawstatusbar();
  dialog = DIALOG_NOTHING;
  return;
 }
}

//Traitement des clics souris ////////////////////////////////////////////////
void Mouseclick()
{
 int i;
 pause6809 = 1;

 //clic dans une popuptable
 if(popuptable > 0)
 {
  i = -1;
  if(xmouse > (dialogrect.x + popuptablerect.x))
  if(ymouse > (dialogrect.y + popuptablerect.y))
  if(xmouse < (dialogrect.x + popuptablerect.x + popuptablerect.w))
  if(ymouse < (dialogrect.y + popuptablerect.y + popuptablerect.h))
  i = (ymouse - dialogrect.y - popuptablerect.y) / 15;
  if(dialog == DIALOG_OPTIONS) Setoption(i);
  pause6809 = 0;
  return;
 }

 //clic dans la barre de statut
 if(ymouse < YSTATUS)
 {Statusclick(); pause6809 = 0; return;}

 //clic dans la boite de dialogue
 if(dialog > 0)
 if(xmouse > dialogrect.x) if(xmouse < (dialogrect.x + dialogrect.w))
 if(ymouse > dialogrect.y) if(ymouse < (dialogrect.y + dialogrect.h))
 {if(dialog > 999) Menuclick(); else Dialogclick();}

 //redemarrage de l'emulation, meme si elle etait arretee
 //sinon il n'y a pas d'affichage
 pause6809 = 0;
}

//Draw popup table ////////////////////////////////////////////////////////////
void Drawpopuptable(int n, int x, int y)
{
 SDL_Rect rect;
 int i;
 popuptable = n;
 popuptablerect.x = rect.x = x;
 popuptablerect.w = rect.w = 54;
 popuptablerect.y = rect.y = y + 18;
 popuptablerect.h = 0;
 rect.h = 15;
 for(i = 0; i < POPUPTABLE_NMAX; i++)
 {
  if(popuptabletext[i][0] == 0) break;
  Drawtextbox(dialogbox, popuptabletext[i], rect, 0, blanc, 1);
  popuptablerect.h += 15;
  rect.y += 15;
 }
}

//Draw option box ////////////////////////////////////////////////////////////
void Drawoptionbox()
{
 SDL_Rect rect;
 int i;
 char string[50];
 if(dialog != DIALOG_OPTIONS)
 {
  Createdialogbox(172, 216);
  rect.x = 10; rect.w = dialogbox->w - 32;
  rect.y = 5; rect.h = 15;
  Drawtextbox(dialogbox, _(MSG_MENU_SETTINGS), rect, 1, bleu, 0); //titre
  dialog = DIALOG_OPTIONS;
 }
 //options
 rect.x = 10; rect.w = 94;
 rect.y = 32; rect.h = 15;
 Drawtextbox(dialogbox, options.language ? _(MSG_SETTINGS_LANG_EN) : _(MSG_SETTINGS_LANG_FR), rect, 0, blanc, -1);
 sprintf(string, "Zoom : %i%s", 100 * options.xclient / XBITMAP, "%");
 rect.y += 20;
 Drawtextbox(dialogbox, string, rect, 0, blanc, -1);
 sprintf(string, "%s : %i%s", _(MSG_SETTINGS_PROC), options.frequency / 10, "%");
 rect.y += 20;
 Drawtextbox(dialogbox, string, rect, 0, blanc, -1);
 sprintf(string, "%s : %i%s", _(MSG_SETTINGS_FPS), 100 / options.vblnumbermax, "%");
 rect.y += 20;
 Drawtextbox(dialogbox, string, rect, 0, blanc, -1);
 sprintf(string, "%s k7 : %s", _(MSG_SETTINGS_WRITE),
         options.k7protection ? _(MSG_NO) : _(MSG_YES));
 rect.y += 20;
 Drawtextbox(dialogbox, string, rect, 0, blanc, -1);
 sprintf(string, "%s fd : %s", _(MSG_SETTINGS_WRITE),
         options.fdprotection ? _(MSG_NO) : _(MSG_YES));
 rect.y += 20;
 Drawtextbox(dialogbox, string, rect, 0, blanc, -1);
 sprintf(string, "%s", options.keybpriority ? _(MSG_SETTINGS_NUMPAD) : _(MSG_MENU_JOYSTICKS));
 rect.y += 20;
 Drawtextbox(dialogbox, string, rect, 0, blanc, -1);
 //dessin des boutons
 for(i = 0; i < OPTIONBUTTON_MAX; i++) Drawbutton(&optionbutton[i], 0);
}

//Traitement des clics des boutons d'options /////////////////////////////////
static void Optionclick()
{
 int i, x, y;

 //recherche du bouton du clic
 for(i = 0; i < OPTIONBUTTON_MAX; i++)
 {
  x = dialogrect.x + optionbutton[i].x;
  y = dialogrect.y + optionbutton[i].y;
  if(xmouse > x) if(xmouse < (x + bouton[optionbutton[i].n].w))
  if(ymouse > y) if(ymouse < (y + bouton[optionbutton[i].n].h))
  break;
 }
 if(i >= OPTIONBUTTON_MAX) return;
 //dessin du bouton enfonce
 Drawbutton(&optionbutton[i], 1);
 Displayscreen(); SDL_Delay(200);
 //traitement en fonction du bouton
 x = optionbutton[i].x;
 y = optionbutton[i].y;
 switch(i)
 {
  case 0: popuptabletext[0] = _(MSG_SETTINGS_LANG_FR); //langue
          popuptabletext[1] = _(MSG_SETTINGS_LANG_EN);
          popuptabletext[2] = "";
          Drawpopuptable(1, x, y);
          break;
  case 1: popuptabletext[0] = "  50%"; //zoom
          popuptabletext[1] = "100%";
          popuptabletext[2] = "150%";
          popuptabletext[3] = "200%";
          popuptabletext[4] = "";
          Drawpopuptable(2, x, y);
          break;
  case 2: popuptabletext[0] = "  10%"; //processeur
          popuptabletext[1] = "  50%";
          popuptabletext[2] = "100%";
          popuptabletext[3] = "200%";
          popuptabletext[4] = "500%";
          popuptabletext[5] = "";
          Drawpopuptable(3, x, y);
          break;
  case 3: popuptabletext[0] = "  10%"; //affichage
          popuptabletext[1] = "  25%";
          popuptabletext[2] = "  50%";
          popuptabletext[3] = "100%";
          popuptabletext[4] = "";
          Drawpopuptable(4, x, y);
          break;
  case 4: popuptabletext[0] = _(MSG_YES); //ecriture k7
          popuptabletext[1] = _(MSG_NO);
          popuptabletext[2] = "";
          Drawpopuptable(5, x, y);
          break;
  case 5: popuptabletext[0] = _(MSG_YES); //ecriture fd
          popuptabletext[1] = _(MSG_NO);
          popuptabletext[2] = "";
          Drawpopuptable(6, x, y);
          break;
  case 6: popuptabletext[0] = _(MSG_MENU_JOYSTICKS); //manettes ou clavier numerique
          popuptabletext[1] = _(MSG_SETTINGS_NUMPAD);
          popuptabletext[2] = "";
          Drawpopuptable(7, x, y);
          break;
  case 7: Resetoptions();    //restore default values
          Resizescreen(options.xclient, YSTATUS + options.yclient);
          dialog = DIALOG_NOTHING;
          Drawoptionbox();
          break;
 }
 if(dialog == DIALOG_OPTIONS) if(popuptable == 0) Drawoptionbox();
}

//Mise a jour d'un parametre avec la valeur i de la popuptable////////////////
static void Setoption(int i)
{
 int f[5] = {100, 500, 1000, 2000, 5000}; //frequences processeur
 int v[4] = {10, 4, 2, 1}; //nombre de vbl entre deux trames affichees
 if(i >= 0) switch(popuptable)
 {
  case 1: options.language = i; break;
  case 2: Resizescreen(XBITMAP * (i + 1) / 2, YSTATUS + YBITMAP * (i + 1)); break;
  case 3: options.frequency = f[i]; break;
  case 4: options.vblnumbermax = v[i]; break;
  case 5: options.k7protection = i; break;
  case 6: options.fdprotection = i; break;
  case 7: options.keybpriority = i; break;
 }
 dialog = DIALOG_NOTHING;
 popuptable = 0;
 Drawoptionbox();
}
