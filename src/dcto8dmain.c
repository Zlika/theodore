//////////////////////////////////////////////////////////////////////////////
// DCTO8DMAIN.C - dcto8d emulator main program
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
#include "dcto8dglobal.h"
#include "dcto8dmsg.h"
#include "dcto8dinterface.h"

#define SDL_main main    //indispensable pour eviter l'erreur
                         //undefined reference to `WinMain@16'

// global variables //////////////////////////////////////////////////////////
SDL_AudioSpec audio;
int pause6809;        //processor pause state
int report;           //nombre de milliemes de cycle a reporter

unsigned char cursor[] = {
 128,0,192,0,160,0,144,0,136,0,132,0,130,0,129,0,128,128,128,64,128,32,129,240,
 137,0,152,128,164,128,196,64,130,64,2,32,1,32,1,16,0,176,0,192,0,0,0,0,128,0,
 192,0,224,0,240,0,248,0,252,0,254,0,255,0,255,128,255,192,255,224,255,240,255,
 0,231,128,199,128,131,192,3,192,3,224,1,224,1,240,0,240,0,192,0,0,0,0};

// About message box /////////////////////////////////////////////////////////
void About()
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

// Message d'erreur emulateur ////////////////////////////////////////////////
void Erreur(const char* function, const char* message)
{
 char string[256];
 sprintf(string, "%s : %s", function, message);
 SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                          "Emulator Error",
                          string,
                          NULL);
}

// Joysticks initialization //////////////////////////////////////////////////
void Joyinit()
{
 //int i = 0;
 //if(joySetCapture(hwndmain, JOYSTICKID1, 0, TRUE)) i += 1;
 //if(joySetCapture(hwndmain, JOYSTICKID2, 0, TRUE)) i += 2;
 //joySetThreshold(JOYSTICKID1, 2000); //10 trop peu pour Trust Predator
 //joySetThreshold(JOYSTICKID2, 2000); //2000 semble correct
}

// Play sound ////////////////////////////////////////////////////////////////
void Playsound(void *udata, Uint8 *stream, int bufferlength)
{
 int i;
 int mcycles; //nombre de milliemes de cycles entre deux echantillons
 int icycles; //nombre entier de cycles entre deux echantillons
 extern int mute, sound, frequency;
 extern int Run(int n);
 extern void Testshiftkey();
 //45 cycles 6809 a 992250 Hz = un echantillon a 22050 Hz
 for(i = 0; i < bufferlength; i++)
 {
  if(pause6809) {stream[i] = 128; continue;}
  //calcul et execution du nombre de cycles entre deux echantillons
  //nbre de cycles theoriques pour la periode en cours =
  //nbre theorique + relicat arrondi precedent - cycles en trop precedent
  mcycles = frequency * 100000 / 2205;   //milliemes de cycles theoriques
  mcycles += report;                     //milliemes de cycles corriges
  icycles = mcycles / 1000;              //nbre entier de cycles a effectuer
  report = mcycles - 1000 * icycles;     //relicat de l'arrondi a reporter
  report -= 1000 * Run(icycles);         //retrancher les milliemes en trop
  stream[i] = (mute) ? 128 : sound + 96;
 }
 Testshiftkey(); //contournement bug SDL sur l'appui simultane SHIFT, ALT, CTRL
}

//Event loop /////////////////////////////////////////////////////////////////
void Eventloop()
{
 SDL_Event event;
 extern dialogeditbox *focus;   //editbox ayant le focus
 extern int xclient, yclient, blink;
 extern int penbutton, xmouse, ymouse, xmove, ymove, xpen, ypen;
 extern void Resizescreen(int x, int y), Displayscreen();
 extern void Mouseclick(), Dialogmove();
 extern void Keydown(int sym, int scancode);
 extern void Keyup(int sym, int scancode);

 while(1)
 {
  while(SDL_PollEvent(&event))
  {
   switch(event.type)
   {
    case SDL_WINDOWEVENT:
         if (event.window.event == SDL_WINDOWEVENT_RESIZED)
         {
          Resizescreen(event.window.data1, event.window.data2);
         }
         break;
    case SDL_MOUSEBUTTONDOWN:
         penbutton = 1;
         Mouseclick();
         break;
    case SDL_MOUSEBUTTONUP:
         penbutton = 0;
         xmove = ymove = 0;
         break;
    case SDL_MOUSEMOTION:
         xmouse = event.motion.x;
         ymouse = event.motion.y;
         xpen = xmouse * XBITMAP / xclient - 16;
         ypen = (ymouse - YSTATUS) * YBITMAP / yclient - 8;
         if(xmove && ymove)
         {
           Dialogmove();
         }
         break;
    case SDL_KEYDOWN:
         Keydown(event.key.keysym.sym, event.key.keysym.scancode);
         break;
    case SDL_KEYUP:
         Keyup(event.key.keysym.sym, event.key.keysym.scancode);
         break;
    case SDL_QUIT:
         return;
   }
  }
  SDL_Delay(20); blink++;
  if(blink == 0) {if(focus != NULL) Draweditbox(focus);}
  if(blink > 20) {blink = -20; if(focus != NULL) Draweditbox(focus);}
 }
}

//Main program ///////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
 extern int xclient, yclient, screencount;
 extern void Init6809(), Initfilenames(), Hardreset();
 extern void Initoptions(), Saveoptions();
 extern void Initfontsurfaces(), Initbuttonsurfaces(), Initstatusbar();
 extern void Resizescreen(int x, int y), Displayscreen(), Keyboardinit();

 //initialisations
 Joyinit();                        //Joysticks initialization
 Initoptions();                    //Option initialization
 Initfilenames();
 Init6809();
 Keyboardinit();                   //Keyboard initialization
 Hardreset();                      //MO5 initialization

 //initialize SDL video and keyboard
 if(SDL_Init(SDL_INIT_VIDEO) < 0) SDL_error(__func__, "SDL_Init");
 atexit(SDL_Quit);

 SDL_SetCursor(SDL_CreateCursor(cursor, cursor + 48, 16, 24, 0, 0)); //curseur
 Resizescreen(xclient, yclient + YSTATUS);
 Initfontsurfaces();   //Initialisation des surfaces des polices
 Initbuttonsurfaces(); //Initialisation des surfaces des boutons
 Initstatusbar();      //Initialisation de la surface de la barre de statut
 screencount = 0;
 Displayscreen();

 //initialize SDL audio
 if(SDL_Init(SDL_INIT_AUDIO) < 0) SDL_error(__func__, "SDL_Init");
 audio.freq = 22050;
 audio.format = AUDIO_U8;
 audio.channels = 1;
 audio.samples = 1024;
 audio.callback = Playsound;
 audio.userdata = NULL;
 if(SDL_OpenAudio(&audio, NULL) < 0 ) SDL_error(__func__, "SDL_OpenAudio");
 SDL_PauseAudio(0);    //Lance l'emulation

 //test events
 Eventloop();

 //quit
 pause6809 = 1;
 SDL_PauseAudio(1);
 Saveoptions();
 SDL_Delay(100);
 SDL_Quit();
 return 0;
}
