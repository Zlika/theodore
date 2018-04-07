//////////////////////////////////////////////////////////////////////////////
// DCTO8DOPTIONS.C - Option setting, save & restore
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

#include <stdio.h>
#include <string.h>
#include "dcto8doptions.h"
#include "dcto8dglobal.h"

#define OPTION_FILE_ID "dcto8dini-01"

Options options = { .language = 0, .xclient = XBITMAP, .yclient = 2 * YBITMAP,
                    .frequency = 1000, .vblnumbermax = 2,
                    .k7protection = 1, .fdprotection = 1, .keybpriority = 0 };
static char default_keycode[256] = { 0 };
static char default_joycode[256] = { 0 };

// Initialize default values for to8dkeycode and to8djoycode ////////
void Initoptions(char defaultto8dkeycode[256], char defaultto8djoycode[256])
{
 memcpy(default_keycode, defaultto8dkeycode, 256);
 memcpy(default_joycode, defaultto8djoycode, 256);
}

// Reset to default values //////////////////////////////////////////
void Resetoptions()
{
 options.language = 0;          //francais
 options.xclient = XBITMAP;     //zoomx 2
 options.yclient = 2 * YBITMAP; //zoomy 2
 options.frequency = 1000;      //1000 kHz
 options.vblnumbermax = 2;      //nombre de vbl entre deux affichages
 options.k7protection = 1;      //protection cassette
 options.fdprotection = 1;      //protection disquette
 options.keybpriority = 0;      //manettes prioritaires
 options.reserved1 = 0;
 options.reserved2 = 0;
 options.reserved3 = 0;
 options.reserved4 = 0;
 options.reserved5 = 0;
 memcpy(options.to8dkeycode, default_keycode, sizeof(default_keycode));
 memcpy(options.to8djoycode, default_joycode, sizeof(default_joycode));
}

// Load options /////////////////////////////////////////////////////
void Loadoptions(char *filename)
{
 int r;
 FILE *fpi;
 char string[256];
 memset(string, 0, 256);
 Resetoptions();
 //ouverture fichier
 fpi = fopen(filename, "rb+");                 //s'il existe ouverture
 if (fpi == NULL) return;
 r = fread(string, sizeof(OPTION_FILE_ID) - 1, 1, fpi); //lecture identifiant
 if(r != 1 || strcmp(OPTION_FILE_ID, string) != 0)
 {
  printf("Wrong config file %s\n", filename);
  fclose(fpi);
  return;
 }
 //lecture des options
 r = fread(&options, sizeof(options), 1, fpi);
 if (r != 1) Resetoptions();
 //controle
 if(options.language < 0) options.language = 0;
 if(options.language > (LANGUAGE_MAX - 1)) options.language = LANGUAGE_MAX - 1;
 if(options.frequency < 100) options.frequency = 100;
 if(options.frequency > 9000) options.frequency = 9000;
 if(options.xclient < (XBITMAP / 2)) options.xclient = XBITMAP / 2;
 if(options.xclient > (2 * XBITMAP)) options.xclient = 2 * XBITMAP;
 if(options.yclient < YBITMAP) options.yclient = YBITMAP;
 if(options.yclient > (4 * YBITMAP)) options.yclient = 4 * YBITMAP;
 if(options.vblnumbermax < 1) options.vblnumbermax = 1;
 if(options.vblnumbermax > 64) options.vblnumbermax = 64;
 if(options.k7protection) options.k7protection = 1;
 if(options.fdprotection) options.fdprotection = 1;
 if(options.keybpriority) options.keybpriority = 1;

 fclose(fpi);
}

// Save options ///////////////////////////////////////////////////////////////
void Saveoptions(char *filename)
{
 FILE *fpi;
 fpi = fopen(filename, "wb+");
 if ((fpi == NULL)
   || (fwrite(OPTION_FILE_ID, sizeof(OPTION_FILE_ID) - 1, 1, fpi) != 1)
   || (fwrite(&options, sizeof(options), 1, fpi) != 1))
 {
  printf("Cannot write config file %s\n", filename);
 }
 if (fpi != NULL) fclose(fpi);
}
