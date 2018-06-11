/*
 * This file is part of theodore (https://github.com/Zlika/theodore),
 * a Thomson emulator based on Daniel Coulom's DCTO8D emulator
 * (http://dcto8.free.fr/).
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "video.h"
#include "to8demulator.h"

#define NB_VIDEO_MODES 5

struct pix {char b, g, r, a;};        //structure pixel BGRA
typedef struct { int w, h; uint32_t* pixels;} Surface;

// global variables //////////////////////////////////////////////////////////
static Surface screen;
static struct pix pcolor[20][8];      //couleurs BGRA de la palette (pour 8 pixels)
static int currentvideomemory;        //index octet courant en memoire video thomson
static int currentlinesegment;        //numero de l'octet courant dans la ligne video
static uint32_t *pcurrentpixel;       //pointeur ecran : pixel courant
static uint32_t *pcurrentline;        //pointeur ecran : debut ligne courante
static uint32_t *pmin;                //pointeur ecran : premier pixel
static uint32_t *pmax;                //pointeur ecran : dernier pixel + 1

// Forward declarations
static void Decode320x16(void);
static void Decode320x4(void);
static void Decode320x4special(void);
static void Decode160x16(void);
static void Decode640x2(void);
// Current video memory decoding function
static void (*Decodevideo)(void) = Decode320x16;
// Array of the different video memory decoding functions (indexed by the video mode)
static void (*DecodevideoModes[NB_VIDEO_MODES])(void) =
  { Decode320x16, Decode320x4, Decode320x4special,
    Decode160x16, Decode640x2 };

//definition des intensites pour correction gamma (circuit palette EF9369 + circuit d'adaptation TEA5114)
static const int intens[16] = {0,100,127,147,163,179,191,203,215,223,231,239,243,247,251,255};

// Modification de la palette ////////////////////////////////////////////////
void Palette(int n, int r, int v, int b)
{
  int i;
  for(i = 0; i < 8; i++)
  {
    pcolor[n][i].b = intens[b];
    pcolor[n][i].g = intens[v];
    pcolor[n][i].r = intens[r];
    pcolor[n][i].a = 0xff;
  }
}

void SetVideoMode(enum VideoMode mode)
{
  Decodevideo = DecodevideoModes[mode];
}

// Decodage octet video mode 320x16 standard /////////////////////////////////
static void Decode320x16(void)
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
    while(pcurrentpixel < pcurrentline + k) memcpy(pcurrentpixel++, c, sizeof(uint32_t));
  }
}

// Decodage octet video mode bitmap4 320x200 4 couleurs //////////////////////
static void Decode320x4(void)
{
  int i, k, c0, c1;
  void *c;
  c0 = pagevideo[currentvideomemory | 0x2000]; //color1
  c1 = pagevideo[currentvideomemory++];        //color2
  k = currentlinesegment << 4;
  for(i = 7; i >= 0; i--)
  {
    k += 2; c = pcolor + (((c0 << 1) >> i & 2) | (c1 >> i & 1));
    while(pcurrentpixel < pcurrentline + k) memcpy(pcurrentpixel++, c, sizeof(uint32_t));
  }
}

// Decodage octet video mode bitmap4 special 320x200 4 couleurs //////////////
static void Decode320x4special(void)
{
  int i, k, c0;
  void *c;
  c0 = pagevideo[currentvideomemory | 0x2000] << 8;
  c0 |= pagevideo[currentvideomemory++] & 0xff;
  k = currentlinesegment << 4;
  for(i = 14; i >= 0; i -= 2)
  {
    k += 2; c = pcolor + (c0 >> i & 3);
    while(pcurrentpixel < pcurrentline + k) memcpy(pcurrentpixel++, c, sizeof(uint32_t));
  }
}

// Decodage octet video mode bitmap16 160x200 16 couleurs ////////////////////
static void Decode160x16(void)
{
  int i, k, c0;
  void *c;
  c0 = pagevideo[currentvideomemory | 0x2000] << 8;
  c0 |= pagevideo[currentvideomemory++] & 0xff;
  k = currentlinesegment << 4;
  for(i = 12; i >= 0; i -= 4)
  {
    k += 4; c = pcolor + (c0 >> i & 0x0f);
    while(pcurrentpixel < pcurrentline + k) memcpy(pcurrentpixel++, c, sizeof(uint32_t));
  }
}

// Decodage octet video mode 640x200 2 couleurs //////////////////////////////
static void Decode640x2(void)
{
  int i, k, c0;
  void *c;
  c0 = pagevideo[currentvideomemory | 0x2000] << 8;
  c0 |= pagevideo[currentvideomemory++] & 0xff;
  k = currentlinesegment << 4;
  for(i = 15; i >= 0; i--)
  {
    k += 1; c = pcolor + (((c0 >> i) & 1) ? 1 : 0);
    while(pcurrentpixel < pcurrentline + k) memcpy(pcurrentpixel++, c, sizeof(uint32_t));
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
  while(pcurrentpixel < pcurrentline + k) memcpy(pcurrentpixel++, c, sizeof(uint32_t));
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
  uint32_t *p0, *p1;
  p1 = pmin + (videolinenumber - 47) * 2 * XBITMAP;
  if(videolinenumber == 263) p1 = pmax;
  p0 = pcurrentline;
  pcurrentline += XBITMAP;
  while(pcurrentline < p1)
  {
    memcpy(pcurrentline, p0, sizeof(uint32_t) * XBITMAP);
    pcurrentline += XBITMAP;
  }
  if(pcurrentline == pmax)
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
  pmin = screen.pixels;
  pmax = pmin + screen.w * screen.h;
  for(pcurrentpixel = pmin; pcurrentpixel < pmax; pcurrentpixel++)
    memcpy(pcurrentpixel, pcolor, sizeof(uint32_t));
}

static void InitScreen(void)
{
  int i;
  pcurrentline = pmin;    //initialisation pointeur ligne courante
  pcurrentpixel = pmin;   //initialisation pointeur pixel courant
  currentlinesegment = 0; //initialisation numero d'octet dans la ligne
  currentvideomemory = 0; //initialisation index en memoire video thomson
  videolinecycle = 52;
  for(videolinenumber = 48; videolinenumber < 264; videolinenumber++)
  {Displaysegment(); Nextline();}
  videolinecycle = 0; videolinenumber = 0;
}

int SetLibRetroVideoBuffer(uint32_t *video_buffer)
{
  screen.w = XBITMAP;
  screen.h = YBITMAP;
  screen.pixels = video_buffer;

  pmin = screen.pixels;
  pmax = pmin + XBITMAP * YBITMAP;
  InitScreen();
  ClearScreen();
  return sizeof(struct pix);
}

int video_serialize_size(void)
{
  return sizeof(pcolor) + sizeof(currentvideomemory) + sizeof(currentlinesegment)
      + sizeof(int) + sizeof(int) + sizeof(int);
}

void video_serialize(void *data)
{
  int offset = 0;
  int pcurrentpixelOffset = pcurrentpixel - pmin;
  int pcurrentlineOffset = pcurrentline - pmin;
  int decodeVideoIndex = 0;
  int i;
  char *buffer = (char *) data;
  memcpy(buffer+offset, pcolor, sizeof(pcolor));
  offset += sizeof(pcolor);
  memcpy(buffer+offset, &currentvideomemory, sizeof(currentvideomemory));
  offset += sizeof(currentvideomemory);
  memcpy(buffer+offset, &currentlinesegment, sizeof(currentlinesegment));
  offset += sizeof(currentlinesegment);
  memcpy(buffer+offset, &pcurrentpixelOffset, sizeof(pcurrentpixelOffset));
  offset += sizeof(pcurrentpixelOffset);
  memcpy(buffer+offset, &pcurrentlineOffset, sizeof(pcurrentlineOffset));
  offset += sizeof(pcurrentlineOffset);
  for (i = 0; i < NB_VIDEO_MODES; i++)
  {
    if (Decodevideo == DecodevideoModes[i])
    {
      decodeVideoIndex = i;
      break;
    }
  }
  memcpy(buffer+offset, &decodeVideoIndex, sizeof(decodeVideoIndex));
}

void video_unserialize(const void *data)
{
  int offset = 0;
  int pcurrentpixelOffset;
  int pcurrentlineOffset;
  int decodeVideoIndex;
  const char *buffer = (const char *) data;
  memcpy(pcolor, buffer+offset, sizeof(pcolor));
  offset += sizeof(pcolor);
  memcpy(&currentvideomemory, buffer+offset, sizeof(currentvideomemory));
  offset += sizeof(currentvideomemory);
  memcpy(&currentlinesegment, buffer+offset, sizeof(currentlinesegment));
  offset += sizeof(currentlinesegment);
  memcpy(&pcurrentpixelOffset, buffer+offset, sizeof(pcurrentpixelOffset));
  pcurrentpixel = pmin + pcurrentpixelOffset;
  offset += sizeof(pcurrentpixelOffset);
  memcpy(&pcurrentlineOffset, buffer+offset, sizeof(pcurrentlineOffset));
  pcurrentline = pmin + pcurrentlineOffset;
  offset += sizeof(pcurrentlineOffset);
  memcpy(&decodeVideoIndex, buffer+offset, sizeof(decodeVideoIndex));
  Decodevideo = DecodevideoModes[decodeVideoIndex];
}
