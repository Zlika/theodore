//////////////////////////////////////////////////////////////////////////////
// DCTO8DMSG.H - message text used by dcto8d
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

#ifndef __DCTO8DMSG_H
#define __DCTO8DMSG_H

#include "../config.h"

//2 langues : 0=francais 1=anglais
extern int language;

#define _(X) gettext(X)

// Renvoie la chaine de caracteres en fonction de la langue selectionnee.
const char* gettext(const char *msg[2]);

extern const char* MSG_ABOUT_TITLE[];
extern const char* MSG_ABOUT[];
extern const char* MSG_PROGNAME[];
extern const char* MSG_MENU_SETTINGS[];
extern const char* MSG_MENU_KEYBOARD[];
extern const char* MSG_MENU_JOYSTICKS[];
extern const char* MSG_MENU_DISASSEMBLY[];
extern const char* MSG_BTN_NONE[];
extern const char* MSG_EMULATED_KEY[];
extern const char* MSG_EMULATED_FUNC[];
extern const char* MSG_KEYBOARD_LAYOUT[];
extern const char* MSG_PRESS_KEY[];
extern const char* MSG_CLICK_PICTURE[];
extern const char* MSG_JOY_EMULATION[];
extern const char* MSG_UNLOAD[];
extern const char* MSG_BACK_TO_FIRST[];
extern const char* MSG_NEXT[];
extern const char* MSG_SETTINGS_LANG_FR[];
extern const char* MSG_SETTINGS_LANG_EN[];
extern const char* MSG_SETTINGS_FPS[];
extern const char* MSG_SETTINGS_PROC[];
extern const char* MSG_SETTINGS_WRITE[];
extern const char* MSG_YES[];
extern const char* MSG_NO[];
extern const char* MSG_SETTINGS_NUMPAD[];

extern const char *msg_btn[][2];

#endif
