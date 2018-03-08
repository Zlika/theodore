//////////////////////////////////////////////////////////////////////////////
// DCTO8DDEVICES.C   Emulation des peripheriques MO5
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
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include "dcto8dglobal.h"

// Variable globales /////////////////////////////////////////////////////////
DIR *dmemo = NULL;  // pointeur directory pour recherche memo7
FILE *ffd = NULL;   // pointeur fichier disquette
FILE *fk7 = NULL;   // pointeur fichier k7
FILE *fprn = NULL;  // pointeur fichier imprimante
int k7bit = 0;      // puissance de 2 designant le bit k7 en cours
int k7octet;        // octet de la cassette en cours de traitement
int k7index;        // compteur du lecteur de cassette
int k7indexmax;     // compteur du lecteur de cassette en fin de bande
int k7protection;   // indicateur lecture seule pour la cassette
int fdprotection;   // indicateur lecture seule pour la disquette
char k7name[100];   // nom du fichier cassette
char fdname[100];   // nom du fichier disquette
char memoname[100]; // nom du fichier cartouche

//pointeurs vers fonctions d'acces memoire
extern char (*Mgetc)(unsigned short a);
extern void (*Mputc)(unsigned short a, char c);

//6809 registers
extern char dc6809_cc;
extern char *dc6809_a, *dc6809_b;
extern short dc6809_x, dc6809_y, dc6809_s;
#define CC dc6809_cc
#define A *dc6809_a
#define B *dc6809_b
#define X dc6809_x
#define Y dc6809_y
#define S dc6809_s

// Emulation imprimante //////////////////////////////////////////////////////
void Imprime()
{
 if(fprn == NULL) fprn = fopen("dcto8d-printer.txt", "ab");
 if(fprn != NULL) {fputc(B, fprn); CC &= 0xfe;};
}

// Erreur lecture/ecriture fichier qd ou fd //////////////////////////////////
void Diskerror(int n)
{
 Mputc(0x604e, n - 1); //erreur 53 = erreur entree/sortie
 CC |= 0x01; //indicateur d'erreur
 return;
}

// Lecture d'un secteur /////////////////////////////////////////////////////
void Readsector()
{
 char buffer[256];
 int i, j, u, p, s;
 if(ffd == NULL) {Diskerror(71); return;}
 u = Mgetc(0x6049) & 0xff; if(u > 03) {Diskerror(53); return;}
 p = Mgetc(0x604a) & 0xff; if(p != 0) {Diskerror(53); return;}
 p = Mgetc(0x604b) & 0xff; if(p > 79) {Diskerror(53); return;}
 s = Mgetc(0x604c) & 0xff; if((s == 0) || (s > 16)) {Diskerror(53); return;}
 s += 16 * p + 1280 * u;
 fseek(ffd, 0, SEEK_END);
 if((s << 8) > ftell(ffd)) {Diskerror(53); return;}
 for(j = 0; j < 256; j++) buffer[j] = 0xe5;
 fseek(ffd, (s - 1) << 8, SEEK_SET);
 i = ((Mgetc(0x604f) & 0xff) << 8) + (Mgetc(0x6050) & 0xff);
 if(fread(buffer, 256, 1, ffd) == 0) {Diskerror(53); return;}
 for(j = 0; j < 256; j++) Mputc(i++, buffer[j]);
}

// Ecriture d'un secteur /////////////////////////////////////////////////////
void Writesector()
{
 char buffer[256];
 int i, j, u, p, s;
 if(ffd == NULL) {Diskerror(71); return;}
 //erreur 72 = protection ecriture
 if(fdprotection == 1) {Diskerror(72); return;}
 u = Mgetc(0x6049) & 0xff; if(u > 03) {Diskerror(53); return;}
 p = Mgetc(0x604a) & 0xff; if(p != 0) {Diskerror(53); return;}
 p = Mgetc(0x604b) & 0xff; if(p > 79) {Diskerror(53); return;}
 s = Mgetc(0x604c) & 0xff; if((s == 0) || (s > 16)) {Diskerror(53); return;}
 s += 16 * p + 1280 * u;
 fseek(ffd, (s - 1) << 8, SEEK_SET);
 i = 256 * (Mgetc(0x604f) & 0xff) + (Mgetc(0x6050) & 0xff);
 for(j = 0; j < 256; j++) buffer[j] = Mgetc(i++);
 if(fwrite(buffer, 256, 1, ffd) == 0) Diskerror(53);
}

// Formatage d'un disque ////////////////////////////////////////////////////
void Formatdisk()
{
 char buffer[256];
 int i, u, fatlength;
 if(ffd == NULL) {Diskerror(71); return;}
 //erreur 72 = protection ecriture
 if(fdprotection == 1) {Diskerror(72); return;}
 u = Mgetc(0x6049) & 0xff; if(u > 03) return; //unite
 u = (1280 * u) << 8; //debut de l'unité dans le fichier .fd
 fatlength = 160;     //80=160Ko, 160=320Ko
 //rem: fatlength provisoire !!!!! (tester la variable adéquate)
 //initialisation de tout le disque avec E5
 for(i = 0; i < 256; i++) buffer[i] = 0xe5;
 fseek(ffd, u, SEEK_SET);
 for(i = 0; i < (fatlength * 8); i++)
  if(fwrite(buffer, 256, 1, ffd) == 0) {Diskerror(53); return;}
 //initialisation de la piste 20 a FF
 for(i = 0; i < 256; i++) buffer[i] = 0xff;
 fseek(ffd, u + 0x14000, SEEK_SET);
 for(i = 0; i < 16; i++)
  if(fwrite(buffer, 256, 1, ffd) == 0) {Diskerror(53); return;}
 //ecriture de la FAT
 buffer[0x00] = 0;
 buffer[0x29] = 0xfe; buffer[0x2a] = 0xfe;
 for(i = fatlength + 1; i < 256; i++) buffer[i] = 0xfe;
 fseek(ffd, u + 0x14100, SEEK_SET);
 if(fwrite(buffer, 256, 1, ffd) == 0) {Diskerror(53); return;}
}

// Chargement d'un fichier fd /////////////////////////////////////////////////
void Loadfd(char *name)
{
 char filename[256];
 extern char path[][TEXT_MAXLENGTH];
 //fermeture disquette éventuellement ouverte
 if(ffd) {fclose(ffd); ffd = NULL; fdname[0] = 0;}
 if(name[0] == 0) return;
 //ouverture de la nouvelle disquette
 strcpy(fdname, name);
 strcpy(filename, path[1]);
 strcat(filename, name);
 ffd = fopen(filename, "rb+");
 if(ffd == NULL) {fdname[0] = 0; return;}
 //fdprotection = 1;
}

// Emulation cassette ////////////////////////////////////////////////////////
void Readoctetk7()
{
 int byte = 0;
 if(fk7 == NULL) {Initprog(); Erreur(11); return;}
 byte = fgetc(fk7);
 if(byte == EOF)
 {
  Initprog(); Erreur(12);
  fseek(fk7, 0, SEEK_SET); k7index = 0; Drawk7index(); return;
 }
 A = k7octet = byte; Mputc(0x2045, byte); k7bit = 0;
 if((ftell(fk7) & 511) == 0) {k7index = ftell(fk7) >> 9; Drawk7index();}
}

void Readbitk7()
{
 int octet = Mgetc(0x2045) << 1;
 if(k7bit == 0) {Readoctetk7(); k7bit = 0x80;}
 if((k7octet & k7bit)) {octet |= 0x01; A = 0xff;} else A = 0;
 Mputc(0x2045, octet); k7bit >>= 1;
}

void Writeoctetk7()
{
 if(fk7 == NULL) {Initprog(); Erreur(11); return;}
 if(k7protection) {Initprog(); Erreur(13); return;}
 if(fputc(A, fk7) == EOF) {Initprog(); Erreur(13); return;}
 Mputc(0x2045, 0);
 if((ftell(fk7) & 511) == 0) {k7index = ftell(fk7) >> 9; Drawk7index();}
}

void Loadk7(char *name)
{
 char filename[256];
 extern char path[][TEXT_MAXLENGTH];
 if(fk7) {fclose(fk7); fk7 = NULL; k7name[0] = 0;} //fermeture cassette éventuellement ouverte
 if(name[0] == 0) return;
 strcpy(k7name, name);
 strcpy(filename, path[0]);
 strcat(filename, name);
 fk7 = fopen(filename, "rb+");
 if(fk7 == NULL) {k7name[0] = 0; return;}
 k7index = 0;
 fseek(fk7, 0, SEEK_END);
 k7indexmax = ftell(fk7) >> 9;
 fseek(fk7, 0, SEEK_SET);
 Drawk7index();
 //k7protection = 1;
}

void Rewindk7()
{
 if(fk7 == NULL) return;
 fseek(fk7, 0, SEEK_SET);
}

// Emulation cartouche memo7 /////////////////////////////////////////////////
void Loadmemo(char *name)
{
 FILE *fp = NULL;
 int i, c, carsize;
 char filename[256];
 extern int carflags, cartype;
 extern char ram[], car[];
 extern char path[][TEXT_MAXLENGTH];
 strcpy(memoname, name);
 strcpy(filename, path[2]);
 strcat(filename, name);
 //ouverture du fichier memo7
 fp = fopen(filename, "rb");
 if(fp == NULL) {carflags = 0; memoname[0] = 0; Hardreset(); return;}
 //chargement
 carsize = 0;
 while(((c = fgetc(fp)) != EOF) && (carsize < 0x10000)) car[carsize++] = c;
 fclose(fp);
 for(i = 0; i < 0xc000; i++) ram[i] = -((i & 0x80) >> 7);
 cartype = 0; //cartouche <= 16 Ko
 if(carsize > 0x4000) cartype = 1;   //bank switch system
 carflags = 4; //cartridge enabled, write disabled, bank 0;
 Initprog();   //initialisation pour lancer la cartouche
}

// Initialisation noms de fichiers et pointeur de fonction de chargement //////
void Initfilenames(char c)
{
 extern void (*Load[]);
 k7name[0] = 0;
 fdname[0] = 0;
 memoname[0] = 0;
 Load[0] = Loadk7;
 Load[1] = Loadfd;
 Load[2] = Loadmemo;
}

// Lecture boutons souris ////////////////////////////////////////////////////
void Readmousebutton()
{
 extern int penbutton;
 A = 3; if(penbutton) {A = 0; CC |= 0x05;}
}

/*
// Emulation crayon optique //////////////////////////////////////////////////
void Readpenxy()
{
 extern int CC, xpen, ypen;
 extern void Mputw(unsigned short a, short w);
 if((xpen < 0) || (xpen >= 320)) {CC |= 1; return;}
 if((ypen < 0) || (ypen >= 200)) {CC |= 1; return;}
 Mputw(S + 6, xpen); Mputw(S + 8, ypen); CC &= 0xfe;
 return;
}
*/

// Lecture position crayon(0) ou souris(1) ///////////////////////////////////
void Readpenxy(int device)
{
 int k;
 extern int xpen, ypen;
 extern char port[];
 void Mputw(unsigned short a, short w);
 if((xpen < 0) || (xpen >= 640)) {CC |= 1; return;} //x hors limites
 if((ypen < 0) || (ypen >= 200)) {CC |= 1; return;} //y hors limites
 /*
 if(device == 0) //test crayon optique sur couleur tres sombre
 {
  unsigned char *p;  //pointeur dans le bitmap du point designe
  extern void *pix;  //pointeur sur le debut des pixels du bitmap
  p = pix + (y + 16) * xbitmap + (x + 16) * 2;
  if((p[0] + p[1] + p[2]) < 10) {CC |= 1; return;}
 }
 */
 k = (port[0x1c] == 0x2a) ? 0 : 1; //mode 40 colonnes : x divise par 2
 if(device > 0) //souris
 {
  Mputw(0x60d8, xpen >> k);
  Mputw(0x60d6, ypen);
 }
 X = xpen >> k;
 Y = ypen;
 CC &= 0xfe;
}

