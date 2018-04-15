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

/* Keyboard input */

#ifndef __KEYBOARD_H
#define __KEYBOARD_H

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
