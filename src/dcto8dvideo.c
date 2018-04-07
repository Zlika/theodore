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
#include "dcto8dvideo.h"
#include "dcto8demulation.h"
#include "dcto8doptions.h"
#ifndef __LIBRETRO__
#include "dcto8dinterface.h"
#include "dcto8dicon.h"
#include "dcto8dmsg.h"
#endif

struct pix {char b, g, r, a;};        //structure pixel BGRA

// global variables //////////////////////////////////////////////////////////
#ifndef __LIBRETRO__
static SDL_Window *window = NULL;     //fenetre d'affichage de l'ecran
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;   //texture mise à jour a partir de screen
#endif
SDL_Surface *screen = NULL;           //surface d'affichage de l'ecran
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
#ifndef __LIBRETRO__
static bool is_fullscreen = false;    //true when emulator runs in fullscreen
#endif

//definition des intensites pour correction gamma de la datasheet EF9369
static const int intens[16]={80,118,128,136,142,147,152,156,160,163,166,169,172,175,178,180};

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

// Mise à jour de la fenetre graphique ///////////////////////////////////////
static void Render(void)
{
#ifndef __LIBRETRO__
 SDL_UpdateTexture(texture, NULL, screen->pixels, screen->pitch);
 SDL_RenderClear(renderer);
 SDL_RenderCopy(renderer, texture, NULL, NULL);
 SDL_RenderPresent(renderer);
#endif
}

//Display screen /////////////////////////////////////////////////////////////
void Displayscreen(void)
{
#ifndef __LIBRETRO__
 if(dialog > 0)
   if(SDL_BlitSurface(dialogbox, NULL, screen, &dialogrect) < 0) SDL_error(__func__, "SDL_BlitSurface");
 if(--screencount < 0)  //1 fois sur 10 pour diminuer le temps d'affichage
 {
  if(statusbar != NULL && !is_fullscreen)
  if(SDL_BlitSurface(statusbar, NULL, screen, NULL) < 0) SDL_error(__func__, "SDL_BlitSurface");
  screencount = 9;
 }
 Render();
#endif
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
 if(SDL_MUSTLOCK(screen) && SDL_LockSurface(screen) < 0)
 {
#ifndef __LIBRETRO__
  SDL_error(__func__, "SDL_LockSurface");
#endif
  return;
 }
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
void Nextline(void)
{
 int *p0, *p1;
 if(SDL_MUSTLOCK(screen) && SDL_LockSurface(screen) < 0)
 {
#ifndef __LIBRETRO__
  SDL_error(__func__, "SDL_LockSurface");
#endif
  return;
 }
 p1 = pmin + (videolinenumber - 47) * options.yclient / YBITMAP * options.xclient;
 if(videolinenumber == 263) p1 = pmax;
 p0 = pcurrentline;
 pcurrentline += options.xclient;
 while(pcurrentline < p1)
 {
  memcpy(pcurrentline, p0, 4 * options.xclient);
  pcurrentline += options.xclient;
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

// Effacement de l'ecran.
static void ClearScreen(void)
{
  if(screen != NULL)
   {
    pmin = (int*)(screen->pixels);
    pmax = pmin + screen->w * screen->h;
    for(pcurrentpixel = pmin; pcurrentpixel < pmax; pcurrentpixel++)
        memcpy(pcurrentpixel, pcolor, 4);
    Render();
   }
}

static void InitScreen(void)
{
  int i;
  pcurrentline = pmin;    //initialisation pointeur ligne courante
  pcurrentpixel = pmin;   //initialisation pointeur pixel courant
  currentlinesegment = 0; //initialisation numero d'octet dans la ligne
  currentvideomemory = 0; //initialisation index en memoire video thomson
  for(i = 0; i <= XBITMAP; i++) xpixel[i] = i * options.xclient / XBITMAP;
  videolinecycle = 52;
  for(videolinenumber = 48; videolinenumber < 264; videolinenumber++)
  {Displaysegment(); Nextline();}
  videolinecycle = 0; videolinenumber = 0;
#ifndef __LIBRETRO__
  Drawstatusbar();
  if(dialog == DIALOG_OPTIONS) Drawoptionbox();
#endif
  screencount = 0;
  Displayscreen();
}

#ifndef __LIBRETRO__
// Conserve le rapport largeur/hauteur de l'écran /////////////////////////////
static void KeepAspectRatio(int *x, int *y)
{
 float expectedAspectRatio = (float)XBITMAP/(float)(2*YBITMAP);
 float aspectRatio = (float)*x/(float)*y;

 if(aspectRatio != expectedAspectRatio)
 {
  if(aspectRatio > expectedAspectRatio)
  {
   *x = expectedAspectRatio * *y;
  }
  else
  {
   *y = (1.f / expectedAspectRatio) * *x;
  }
 }
}

// Resize screen //////////////////////////////////////////////////////////////
void Resizescreen(int x, int y)
{
 int savepause6809;
 int ystatus;
 savepause6809 = pause6809;
 pause6809 = 1; SDL_Delay(200);
 //effacement surface de l'ecran
 ClearScreen();

 ystatus = is_fullscreen ? 0 : YSTATUS; // Disable status bar in fullscreen mode
 y -= ystatus;
 options.xclient = (x < XBITMAP/2) ? XBITMAP/2 : x;
 options.yclient = (y < YBITMAP) ? YBITMAP : y;
 KeepAspectRatio(&options.xclient, &options.yclient);

 if (window == NULL)
 {
  window = SDL_CreateWindow(_(MSG_PROGNAME),
                           SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED,
                           options.xclient, options.yclient + ystatus,
                           is_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_RESIZABLE);
  if (window == NULL) { SDL_error(__func__, "SDL_CreateWindow"); return; }
  SDL_SetWindowIcon(window, SDL_LoadBMP_RW(SDL_RWFromMem(dcto8dicon, sizeof(dcto8dicon)), 1));
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (renderer == NULL) { SDL_error(__func__, "SDL_CreateRenderer"); return; }
 }
 SDL_RenderSetLogicalSize(renderer, options.xclient, options.yclient + ystatus);
 SDL_SetWindowSize(window, options.xclient, options.yclient + ystatus);
 screen = SDL_CreateRGBSurface(0, options.xclient, options.yclient + ystatus, 32,
                                         0x00FF0000,
                                         0x0000FF00,
                                         0x000000FF,
                                         0xFF000000);
 if (screen == NULL) { SDL_error(__func__, "SDL_CreateRGBSurface"); return; }
 texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                options.xclient, options.yclient + ystatus);
 if (texture == NULL) { SDL_error(__func__, "SDL_CreateTexture"); return; }

 pmin = (int*)(screen->pixels) + ystatus * options.xclient;
 pmax = pmin + options.yclient * options.xclient;
 //rafraichissement de l'ecran
 InitScreen();
 pause6809 = savepause6809;
}

// Switch between fullscreen and windowed modes
void SwitchFullScreenMode(void)
{
 SDL_Delay(200);
 int result;
 if (is_fullscreen)
 {
  is_fullscreen = false;
  result = SDL_SetWindowFullscreen(window, 0);
 }
 else
 {
  is_fullscreen = true;
  result = SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
 }
 if (result != 0)
 {
  SDL_error(__func__, "SDL_SetWindowFullscreen");
 }
}

// Returns true if fullscreen mode is enabled, false otherwise
bool IsFullScreenMode(void)
{
 return is_fullscreen;
}
#endif

#ifdef __LIBRETRO__
uint32_t* CreateLibRetroVideoBuffer()
{
  uint32_t *video_buffer = (uint32_t *)malloc(XBITMAP * YBITMAP * 2 * sizeof(uint32_t));
  screen = SDL_CreateRGBSurfaceWithFormatFrom(video_buffer, XBITMAP, YBITMAP*2,
      sizeof(uint32_t), sizeof(uint32_t)*XBITMAP, SDL_PIXELFORMAT_ARGB8888);
  pmin = (int*)(screen->pixels);
  pmax = pmin + XBITMAP * YBITMAP * 2;
  InitScreen();
  ClearScreen();
  return video_buffer;
}
#endif
