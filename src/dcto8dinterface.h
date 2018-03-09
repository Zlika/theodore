///////////////////////////////////////////////////////////////////////////////
// DCTO8DINTERFACE.H - declaration des fonctions d'interface utilisateur
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

void Drawtextbox(SDL_Surface *s, char *txt, SDL_Rect rect, int t, int b, int r);
void Drawbutton(const dialogbutton *bouton, int push);
void Draweditbox(const dialogeditbox *box);
