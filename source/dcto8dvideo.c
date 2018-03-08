///////////////////////////////////////////////////////////////////////////////
// DCTO8DVIDEO.C - Fonctions d'affichage pour dcto8d
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
#include "dcto8dglobal.h"

#define VIDEO_MODE SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE

// global variables //////////////////////////////////////////////////////////
SDL_Surface *screen = NULL;    //surface d'affichage de l'ecran
int xclient;                   //largeur fenetre utilisateur
int yclient;                   //hauteur ecran MO5 dans fenetre utilisateur
int xmouse;                    //abscisse souris dans fenetre utilisateur
int ymouse;                    //ordonnee souris dans fenetre utilisateur
struct pix {char b, g, r, a;}; //structure pixel BGRA
struct pix pcolor[20][8];      //couleurs BGRA de la palette (pour 8 pixels)
int currentvideomemory;        //index octet courant en memoire video thomson
int currentlinesegment;        //numero de l'octet courant dans la ligne video
int *pcurrentpixel;            //pointeur ecran : pixel courant
int *pcurrentline;             //pointeur ecran : debut ligne courante
int *pmin;                     //pointeur ecran : premier pixel
int *pmax;                     //pointeur ecran : dernier pixel + 1
int screencount;               //nbre ecrans affiches entre 2 affichages status
int xpixel[XBITMAP + 1];       //abscisse des pixels dans la ligne
void (*Decodevideo)();         //pointeur fonction decodage memoire video

//definition des intensites pour correction gamma anciennes valeurs dcmoto
//const int g[16]={0,60,90,110,130,148,165,180,193,205,215,225,230,235,240,255};
//definition des intensites pour correction gamma de la datasheet EF9369
int intens[16]={80,118,128,136,142,147,152,156,160,163,166,169,172,175,178,180};

extern int videolinecycle;
extern int videolinenumber;
extern char *pagevideo;

extern void Drawstatusbar();
extern void SDL_error(int n);

// Modification de la palette ////////////////////////////////////////////////
void Palette(int n, int r, int v, int b)
{
 int i;
 for(i = 0; i < 8; i++)
 {
  pcolor[n][i].b = 2 * (intens[b] - 64) + 16;
  pcolor[n][i].g = 2 * (intens[v] - 64) + 16;
  pcolor[n][i].r = 2 * (intens[r] - 64) + 16;
  pcolor[n][i].a = 0xff;
 }
}

//Display screen /////////////////////////////////////////////////////////////
void Displayscreen()
{
 extern SDL_Surface *dialogbox;
 extern SDL_Surface *statusbar;
 extern SDL_Rect dialogrect;
 extern int dialog;
 if(dialog > 0)
   if(SDL_BlitSurface(dialogbox, NULL, screen, &dialogrect) < 0) SDL_error(31);
 if(--screencount < 0)  //1 fois sur 10 pour diminuer le temps d'affichage
 {
  if(statusbar != NULL)
  if(SDL_BlitSurface(statusbar, NULL, screen, NULL) < 0) SDL_error(32);
  screencount = 9;
 }
 if(SDL_Flip(screen) < 0) SDL_error(33);
}

// Decodage octet video mode 320x16 standard /////////////////////////////////
void Decode320x16()
{
 int i, k, c0, c1, color, shape;
 void *c;
 shape = pagevideo[currentvideomemory | 0x2000];
 color = pagevideo[currentvideomemory++];
 c0 = (color & 0x07) | ((~color & 0x80) >> 4);        //background
 c1 = ((color >> 3) & 0x07) | ((~color & 0x40) >> 3); //foreground
 k = currentlinesegment << 4;
 for(i = 7; i >= 0; i--)
 {
  k += 2; c = pcolor + (((shape >> i) & 1) ? c1 : c0);
  while(pcurrentpixel < pcurrentline + xpixel[k]) memcpy(pcurrentpixel++, c, 4);
 }
}

// Decodage octet video mode bitmap4 320x200 4 couleurs //////////////////////
void Decode320x4()
{
 int i, k, c0, c1;
 void *c;
 c0 = pagevideo[currentvideomemory | 0x2000]; //color1
 c1 = pagevideo[currentvideomemory++];        //color2
 k = currentlinesegment << 4;
 for(i = 7; i >= 0; i--)
 {
  k += 2; c = pcolor + (((c0 << 1) >> i & 2) | (c1 >> i & 1));
  while(pcurrentpixel < pcurrentline + xpixel[k]) memcpy(pcurrentpixel++, c, 4);
 }
}

// Decodage octet video mode bitmap4 special 320x200 4 couleurs //////////////
void Decode320x4special()
{
 int i, k, c0;
 void *c;
 c0 = pagevideo[currentvideomemory | 0x2000] << 8;
 c0 |= pagevideo[currentvideomemory++] & 0xff;
 k = currentlinesegment << 4;
 for(i = 14; i >= 0; i -= 2)
 {
  k += 2; c = pcolor + (c0 >> i & 3);
  while(pcurrentpixel < pcurrentline + xpixel[k]) memcpy(pcurrentpixel++, c, 4);
 }
}

// Decodage octet video mode bitmap16 160x200 16 couleurs ////////////////////
void Decode160x16()
{
 int i, k, c0;
 void *c;
 c0 = pagevideo[currentvideomemory | 0x2000] << 8;
 c0 |= pagevideo[currentvideomemory++] & 0xff;
 k = currentlinesegment << 4;
 for(i = 12; i >= 0; i -= 4)
 {
  k += 4; c = pcolor + (c0 >> i & 0x0f);
  while(pcurrentpixel < pcurrentline + xpixel[k]) memcpy(pcurrentpixel++, c, 4);
 }
}

// Decodage octet video mode 640x200 2 couleurs //////////////////////////////
void Decode640x2()
{
 int i, k, c0;
 void *c;
 c0 = pagevideo[currentvideomemory | 0x2000] << 8;
 c0 |= pagevideo[currentvideomemory++] & 0xff;
 k = currentlinesegment << 4;
 for(i = 15; i >= 0; i--)
 {
  k += 1; c = pcolor + (((c0 >> i) & 1) ? 1 : 0);
  while(pcurrentpixel < pcurrentline + xpixel[k]) memcpy(pcurrentpixel++, c, 4);
 }
}

// Creation d'un segment de bordure ///////////////////////////////////////////
void Displayborder()
{
 int k;
 void *c;
 extern int bordercolor;
 currentlinesegment++;
 c = pcolor + bordercolor;
 k = currentlinesegment << 4;
 while(pcurrentpixel < pcurrentline + xpixel[k]) memcpy(pcurrentpixel++, c, 4);
}

// Creation d'un segment de ligne d'ecran /////////////////////////////////////
void Displaysegment()
{
 int segmentmax;
 segmentmax = videolinecycle - 10;
 if(segmentmax > 42) segmentmax = 42;
 if(SDL_MUSTLOCK(screen))
   if(SDL_LockSurface(screen) < 0) {SDL_error(34); return;}
 while(currentlinesegment < segmentmax)
 {
  if(videolinenumber < 56) {Displayborder(); continue;}
  if(videolinenumber > 255) {Displayborder(); continue;}
  if(currentlinesegment == 0) {Displayborder(); continue;}
  if(currentlinesegment == 41) {Displayborder(); continue;}
  Decodevideo(); currentlinesegment++;
 }
 SDL_UnlockSurface(screen);
}

// Changement de ligne ecran //////////////////////////////////////////////////
void Nextline()
{
 int *p0, *p1;
 if(SDL_MUSTLOCK(screen))
   if(SDL_LockSurface(screen) < 0) {SDL_error(35); return;}
 p1 = pmin + (videolinenumber - 47) * yclient / YBITMAP * xclient;
 if(videolinenumber == 263) p1 = pmax;
 p0 = pcurrentline;
 pcurrentline += xclient;
 while(pcurrentline < p1)
 {
  memcpy(pcurrentline, p0, 4 * xclient);
  pcurrentline += xclient;
 }
 if(p1 == pmax)
 {
  pcurrentline = pmin;    //initialisation pointeur ligne courante
  currentvideomemory = 0; //initialisation index en memoire video thomson
 }
 pcurrentpixel = pcurrentline;
 currentlinesegment = 0;
 SDL_UnlockSurface(screen);
}

// Resize screen //////////////////////////////////////////////////////////////
void Resizescreen(int x, int y)
{
 int i, savepause6809;
 extern int pause6809;
 extern int dialog;
 extern void Drawoptionbox();
 savepause6809 = pause6809;
 pause6809 = 1; SDL_Delay(200);
 //effacement surface de l'ecran
 if(screen != NULL)
 {
  pmin = (int*)(screen->pixels);
  pmax = pmin + screen->w * screen->h;
  for(pcurrentpixel = pmin; pcurrentpixel < pmax; pcurrentpixel++)
      memcpy(pcurrentpixel, pcolor, 4);
  if(SDL_Flip(screen) < 0) SDL_error(36);
 }
 //creation nouvelle surface
 SDL_FreeSurface(screen);
 y -= YSTATUS;
 xclient = (x < 336) ? 336 : x;
 yclient = (y < 216) ? 216 : y;
 screen = SDL_SetVideoMode(xclient, yclient + YSTATUS, 32, VIDEO_MODE);
 if(screen == NULL)
 {
  screen = SDL_SetVideoMode(336, 216, 8, 0);
  SDL_WM_SetCaption(" Erreur fatale : Mode video non compatible", NULL);
  SDL_error(37);
 }
 pmin = (int*)(screen->pixels) + YSTATUS * xclient;
 pmax = pmin + yclient * xclient;
 //rafraichissement de l'écran
 pcurrentline = pmin;    //initialisation pointeur ligne courante
 pcurrentpixel = pmin;   //initialisation pointeur pixel courant
 currentlinesegment = 0; //initialisation numero d'octet dans la ligne
 currentvideomemory = 0; //initialisation index en memoire video thomson
 for(i = 0; i <= XBITMAP; i++) xpixel[i] = i * xclient / XBITMAP;
 videolinecycle = 52;
 for(videolinenumber = 48; videolinenumber < 264; videolinenumber++)
 {Displaysegment(); Nextline();}
 videolinecycle = 0; videolinenumber = 0;
 Drawstatusbar();
 if(dialog == 2) Drawoptionbox();
 screencount = 0;
 Displayscreen();
 pause6809 = savepause6809;
}
