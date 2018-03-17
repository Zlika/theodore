//////////////////////////////////////////////////////////////////////////////
// DCTO8DMAIN.H - dcto8d emulator main program
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

#ifndef __DCTO8DMAIN_H
#define __DCTO8DMAIN_H

//processor pause state
extern int pause6809;

void About();

// Message d'erreur SDL //////////////////////////////////////////////////////
void SDL_error(const char* function, const char* message);
// Message d'erreur emulateur ////////////////////////////////////////////////
void Erreur(const char* function, const char* message);

#endif /* __DCTO8DMAIN_H */
