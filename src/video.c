/*
 * This file is part of theodore, a Thomson emulator based on
 * Daniel Coulom's DCTO8D emulator (http://dcto8.free.fr/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* Graphical display functions of the emulator */

#include <stdlib.h>
#include <string.h>
#include "video.h"
#include "to8demulator.h"

struct pix {char b, g, r, a;};        //structure pixel BGRA

// global variables //////////////////////////////////////////////////////////
typedef struct { int w, h; void* pixels;} Surface;
Surface surface;
Surface *screen = &surface;
int xmouse;                           //abscisse souris dans fenetre utilisateur
int ymouse;                           //ordonnee souris dans fenetre utilisateur
static struct pix pcolor[20][8];      //couleurs BGRA de la palette (pour 8 pixels)
static int currentvideomemory;        //index octet courant en memoire video thomson
static int currentlinesegment;        //numero de l'octet courant dans la ligne video
static int *pcurrentpixel;            //pointeur ecran : pixel courant
static int *pcurrentline;             //pointeur ecran : debut ligne courante
static int *pmin;                     //pointeur ecran : premier pixel
static int *pmax;                     //pointeur ecran : dernier pixel + 1
static int screencount = 0;           //nbre ecrans affiches entre 2 affichages status
static int xpixel[XBITMAP + 1];       //abscisse des pixels dans la ligne
void (*Decodevideo)(void);            //pointeur fonction decodage memoire video

//definition des intensites pour correction gamma de la datasheet EF9369
static const int intens[16] = {80,118,128,136,142,147,152,156,160,163,166,169,172,175,178,180};

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

// Decodage octet video mode 320x16 standard /////////////////////////////////
void Decode320x16(void)
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
void Decode320x4(void)
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
void Decode320x4special(void)
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
void Decode160x16(void)
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
void Decode640x2(void)
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
static void Displayborder(void)
{
  int k;
  void *c;
  currentlinesegment++;
  c = pcolor + bordercolor;
  k = currentlinesegment << 4;
  while(pcurrentpixel < pcurrentline + xpixel[k]) memcpy(pcurrentpixel++, c, 4);
}

// Creation d'un segment de ligne d'ecran /////////////////////////////////////
void Displaysegment(void)
{
  int segmentmax;
  segmentmax = videolinecycle - 10;
  if(segmentmax > 42) segmentmax = 42;
  while(currentlinesegment < segmentmax)
  {
    if(videolinenumber < 56) {Displayborder(); continue;}
    if(videolinenumber > 255) {Displayborder(); continue;}
    if(currentlinesegment == 0) {Displayborder(); continue;}
    if(currentlinesegment == 41) {Displayborder(); continue;}
    Decodevideo(); currentlinesegment++;
  }
}

// Changement de ligne ecran //////////////////////////////////////////////////
void Nextline(void)
{
  int *p0, *p1;
  p1 = pmin + (videolinenumber - 47) / 2 * XBITMAP;
  if(videolinenumber == 263) p1 = pmax;
  p0 = pcurrentline;
  pcurrentline += XBITMAP;
  while(pcurrentline < p1)
  {
    memcpy(pcurrentline, p0, 4 * XBITMAP);
    pcurrentline += XBITMAP;
  }
  if(p1 == pmax)
  {
    pcurrentline = pmin;    //initialisation pointeur ligne courante
    currentvideomemory = 0; //initialisation index en memoire video thomson
  }
  pcurrentpixel = pcurrentline;
  currentlinesegment = 0;
}

// Effacement de l'ecran.
static void ClearScreen(void)
{
  if(screen != NULL)
  {
    pmin = (int*)(screen->pixels);
    pmax = pmin + screen->w * screen->h;
    for(pcurrentpixel = pmin; pcurrentpixel < pmax; pcurrentpixel++)
      memcpy(pcurrentpixel, pcolor, 4);
  }
}

static void InitScreen(void)
{
  int i;
  pcurrentline = pmin;    //initialisation pointeur ligne courante
  pcurrentpixel = pmin;   //initialisation pointeur pixel courant
  currentlinesegment = 0; //initialisation numero d'octet dans la ligne
  currentvideomemory = 0; //initialisation index en memoire video thomson
  for(i = 0; i <= XBITMAP; i++) xpixel[i] = i;
  videolinecycle = 52;
  for(videolinenumber = 48; videolinenumber < 264; videolinenumber++)
  {Displaysegment(); Nextline();}
  videolinecycle = 0; videolinenumber = 0;
  screencount = 0;
}

void SetLibRetroVideoBuffer(void *video_buffer)
{
  screen->w = XBITMAP;
  screen->h = YBITMAP;
  screen->pixels = video_buffer;

  pmin = (int*)(screen->pixels);
  pmax = pmin + XBITMAP * YBITMAP;
  InitScreen();
  ClearScreen();
}
