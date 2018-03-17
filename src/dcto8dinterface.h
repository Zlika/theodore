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

#ifndef __DCTO8DINTERFACE_H
#define __DCTO8DINTERFACE_H

#include <SDL.h>
#include "dcto8dglobal.h"

//surface d'affichage dialogbox
extern SDL_Surface *dialogbox;
//surface de la barre de statut
extern SDL_Surface *statusbar;
//0=rien 1=message, 2=options 3=clavier 4=menu
extern int dialog;
extern const int blanc;
extern const int bleu;
extern const int gris0;
extern button bouton[BOUTON_MAX];
//editbox ayant le focus
extern const dialogeditbox *focus;
//position du curseur dans la chaine de caracteres
extern int xcursor;
//position dialogbox
extern SDL_Rect dialogrect;
//position souris dans dialogbox deplacee
extern int xmove, ymove;
//pouptable inactive par defaut
extern int popuptable;
//pointeur vers lignes de la popup table
extern const char *popuptabletext[];

//repertoires des fichiers k7, fd, memo
extern char path[3][TEXT_MAXLENGTH];

//pointeur fonction de chargement de fichier
extern void (*Load[3])(char *name);

//flag de clignotement du curseur
extern int blink;

void Drawtextbox(SDL_Surface *s, const char *txt, SDL_Rect rect, int t, int b, int r);
void Drawbutton(const dialogbutton *bouton, int push);
void Draweditbox(const dialogeditbox *box);
//Draw status bar ////////////////////////////////////////////////////////////
void Drawstatusbar();
//Index cassette /////////////////////////////////////////////////////////////
void Drawk7index();
//Draw popup directory ////////////////////////////////////////////////////////
void Drawpopupdirectory(int n);
//Draw popup table ////////////////////////////////////////////////////////////
void Drawpopuptable(int n, int x, int y);
//Draw menu box ////////////////////////////////////////////////////////////
void Drawmenubox();
//Create dialog box //////////////////////////////////////////////////////////
void Createdialogbox(int w, int h);

//Traitement des clics souris ////////////////////////////////////////////////
void Mouseclick();
//Deplacement de la boite de dialogue ////////////////////////////////////////
void Dialogmove();
//Initialisation des surfaces des polices ////////////////////////////////////
void Initfontsurfaces();
//Initialisation des surfaces des boutons ////////////////////////////////////
void Initbuttonsurfaces();
//Init status bar ////////////////////////////////////////////////////////////
void Initstatusbar();

#endif
