//////////////////////////////////////////////////////////////////////////////
// DCTO8DKEYB.H - TO8D keyboard definition
// Author   : Daniel Coulom - danielcoulom@gmail.com
// Web site : http://dcto8.free.fr
// Created  : December 2007
//
// This file is part of DCTO8D.
//
// DCTO8D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DCTO8D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DCTO8D.  If not, see <http://www.gnu.org/licenses/>.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __DCTO8DKEYB_H
#define __DCTO8DKEYB_H

// Initialisation du clavier /////////////////////////////////////////////////
void Keyboardinit();
//Traitement des clics boite de dialogue clavier /////////////////////////////
void Keyclick();
//Traitement des clics boite de dialogue manettes ////////////////////////////
void Joyclick();
// Key down //////////////////////////////////////////////////////////////////
void Keydown(int sym, int scancode);
// Key up ////////////////////////////////////////////////////////////////////
void Keyup(int keysym, int scancode);
//Creation de la boite de dialogue du clavier ////////////////////////////////
void Drawkeyboardbox();
//Creation de la boite de dialogue des manettes //////////////////////////////
void Drawjoystickbox();
// Test touches SHIFT, ALT et CTRL gauche et droite //////////////////////////
void Testshiftkey();

#endif
