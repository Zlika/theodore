///////////////////////////////////////////////////////////////////////////////
// DCTO8DEMULATION.C - Emulateur Thomson TO8D portable
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

#include <string.h>
#include <time.h>
#include "to8dbasic.h"
#include "to8dmoniteur.h"
#include "dcto8dglobal.h"

// memory
char car[0x10000];   //espace cartouche 4x16K
char ram[0x80000];   //ram 512K
char port[0x40];     //ports d'entree/sortie
char x7da[32];       //stockage de la palette de couleurs
// pointers
char *pagevideo;     //pointeur page video affichee
char *ramvideo;      //pointeur couleurs ou formes
char *ramuser;       //pointeur ram utilisateur fixe
char *rambank;       //pointeur banque ram utilisateur
char *romsys;        //pointeur rom systeme
char *rombank;       //pointeur banque rom ou cartouche
//banques
int nvideopage;      //n° page video (00-01)
int nvideobank;      //n° banque video (00-03)
int nrambank;        //n° banque ram (00-1f)
int nrombank;        //n° banque rom (00-07)
int nsystbank;       //n° banque systeme (00-01)
int nctrlbank;       //n° banque controleur (00-03)
//flags cartouche
int cartype;         //type de cartouche (0=simple 1=switch bank, 2=os-9)
int carflags;        //bits0,1,4=bank, 2=cart-enabled, 3=write-enabled
//keyboard, joysticks, mouse
int touche[KEYBOARDKEY_MAX]; //etat touches to8d
int capslock;        //1=capslock, 0 sinon
int joysposition;    //position des manches
int joysaction;      //position des boutons d'action
int xpen, ypen;      //lightpen coordinates
int penbutton;       //lightpen button state
//affichage
int videolinecycle;  //compteur ligne (0-63)
int videolinenumber; //numero de ligne video affichée (0-311)
int vblnumber;       //compteur du nombre de vbl avant affichage
int vblnumbermax;    //nombre de vbl entre deux affichages
int displayflag;     //indicateur pour l'affichage
int bordercolor;     //couleur de la bordure de l'écran
//divers
int sound;           //niveau du haut-parleur
int mute;            // mute flag
int timer6846;       //compteur du timer 6846
int latch6846;       //registre latch du timer 6846
int keyb_irqcount;   //nombre de cycles avant la fin de l'irq clavier
int timer_irqcount;  //nombre de cycles avant la fin de l'irq timer
//variables externes
extern int dc6809_irq;

//Accès mémoire
char Mgetto8d(unsigned short a);
void Mputto8d(unsigned short a, char c);
char (*Mgetc)(unsigned short);       //pointeur fonction courante
short Mgetw(unsigned short a) {return (Mgetc(a) << 8 | (Mgetc(++a) & 0xff));}
void (*Mputc)(unsigned short, char); //pointeur fonction courante
void Mputw(unsigned short a, short w) {Mputc(a, w >> 8); Mputc(++a, w);}

//affichage
void (*Decodevideo)();     //pointeur fonction courante
void Decode320x16();       //standard TO
void Decode320x4();        //bitmap4
void Decode320x4special(); //bitmap4 special
void Decode160x16();       //bitmap16
void Decode640x2();        //80 colonnes

// Registres du 6846 //////////////////////////////////////////////////////////
/*
E7C0= registre d'etat composite port C (CSR)
- csr0= timer interrupt flag
- csr1= cp1 interrupt flag (clavier)
- csr2= cp2 interrupt flag
- csr3-csr6 unused and set to zeroes
- csr7= composite interrupt flag (si au moins un interrupt flag à 1)

E7C1= registre de controle peripherique port C (PCRC)
- pcr0= CP1 interruption mask (0=masked, 1=enabled)
- pcr1= CP1 active edge (0=descendant, 1=montant)
- pcr2= CP1 input latch (0=not latched, 1=latched on active CP1)
- pcr3=
- pcr4=
- pcr5= CP2 direction (0=input, 1=output)
- pcr6=
- pcr7= reset (0=normal operation, 1=reset CSR1, CSR2 and periph data)

E7C2= registre de direction de donnees port C (DDRC)
- ddr0-ddr7= peripheral data line 0-7 direction (0=input, 1=output)

E7C3= registre de donnees peripheriques port C (PRC)
- p0-out: commutation page video (0=couleur 1=forme) emule par le gate array
- p1-in : interrupteur du crayon optique (0=ouvert 1=ferme)
- p2-out: commutation ROM (0=cartouche 1=interne)
- p3    : inutilise (ancienne commande led clavier du 707 et TO7/70)
- p4-out: commutation banque systeme (0=banque0 1=banque1)
- p5-out: acknowledge de reception d'un code touche (0=acknowledge 1=attente)
- p6_in : signal busy de l'imprimante (0=occupee 1=disponible)
- p7-in : entree magnetophone (0=deconnecte ou signal, 1=connecte sans signal)

E7C4= registre d'etat composite Timer (CSR)

E7C5= registre controle temporisateur (TCR)

E7C6= registre temporisateur d'octet de poids fort (TMSB)

E7C7= registre temporisateur d'octet de poids faible (TLSB)

*/

// Emulation du clavier TO8 ///////////////////////////////////////////////////
void TO8key(int n)
{
 int i;
 if(touche[n]) //touche relachee
 {
  //s'il reste une touche enfoncee ne rien faire
  for(i = 0; i < 0x50; i++) if(touche[i] == 0) return;
  //si toutes les touches sont relachees
  port[0x08] = 0x00; //bit 0 de E7C8 = 0 (toutes les touches relachees)
  keyb_irqcount = 0; return;
 }
 //touche enfoncee
 if(n == 0x50) capslock = 1 - capslock; //capslock
 if(n > 0x4f) return;           //touches shift, ctrl et joysticks
 i = 0;
 if(!touche[0x51]) i = 0x80; //SHIFT gauche
 if(!touche[0x52]) i = 0x80; //SHIFT droit
 if(capslock) switch(n)
 {
  case 0x02: case 0x03: case 0x07: case 0x0a: case 0x0b: case 0x0f: case 0x12:
  case 0x13: case 0x17: case 0x1a: case 0x1b: case 0x1f: case 0x22: case 0x23:
  case 0x27: case 0x2a: case 0x2b: case 0x2f: case 0x32: case 0x33: case 0x3a:
  case 0x3b: case 0x42: case 0x43: case 0x4a: case 0x4b: i = 0x80; break;
 }
 to8dmoniteur[0x30f8] = n | i;       //scancode + indicateur de touche SHIFT
 to8dmoniteur[0x3125] = touche[0x53] ? 0 : 1;   //indicateur de touche CTRL
 port[0x08] |= 0x01; //bit 0 de E7C8 = 1 (touche enfoncee)
 port[0x00] |= 0x82; //bit CP1 = interruption clavier
 keyb_irqcount = 500000; //positionne le signal d'irq pour 500 ms maximum
 dc6809_irq = 1;
}

// Selection de banques memoire //////////////////////////////////////////////
void TO8videoram()
{
 extern int nvideopage, nsystbank;
 nvideopage = port[0x03] & 1;
 ramvideo = ram - 0x4000 + (nvideopage << 13);
 nsystbank = (port[0x03] & 0x10) >> 4;
 romsys = to8dmoniteur - 0xe000 + (nsystbank << 13);
}

void TO8rambank()
{
 extern int nrambank;
 extern void Initcontroller();
 //mode TO8 par e7e5
 if(port[0x27] & 0x10)
 {
  nrambank = port[0x25] & 0x1f;
  rambank = ram - 0xa000 + (nrambank << 14);
  return;
 }
 //mode compatibilite TO7/70 par e7c9
 switch(port[0x09] & 0xf8)
 {
  case 0x08: nrambank = 0; break;
  case 0x10: nrambank = 1; break;
  case 0xe0: nrambank = 2; break;
  case 0xa0: nrambank = 3; break;  //banques 5 et 6
  case 0x60: nrambank = 4; break;  //inversees/TO770&TO9
  case 0x20: nrambank = 5; break;
  default: return;
 }
 rambank = ram - 0x2000 + (nrambank << 14);
}

void TO8rombank()
{
 extern int nrombank;
 //romsys = rom + 0x2000 + ((cnt[0x7c3] & 0x10) << 9);
 //si le bit 0x20 de e7e6 est positionne a 1 l'espace ROM est recouvert
 //par la banque RAM definie par les 5 bits de poids faible de e7e6
 //subtilite : les deux segments de 8K de la banque sont inversés.
 if(port[0x26] & 0x20)
 {
  //e7c3 |= 4;
  rombank = ram + ((port[0x26] & 0x1f) << 14);
  //rombank += (carflags & 3) << 14;
  return;
 }
 //sinon le bit2 de e7c3 commute entre ROM interne et cartouche
 if(port[0x03] & 0x04)
 {
  nrombank = carflags & 3;
  rombank = to8dbasic + (nrombank << 14);
 }
 else
 {
  nrombank = -1;
  rombank = car + ((carflags & 3) << 14);
 }
}

void Switchmemo7(int a)
{
 carflags = (carflags & 0xfc) | (a & 3);
 rombank = car + ((carflags & 3) << 14);
}

void Videopage_bordercolor(char c)
{
 port[0x1d] = c;
 pagevideo = ram + ((c & 0xc0) << 8);
 bordercolor = c & 0x0f;
}

// Selection video ////////////////////////////////////////////////////////////
void TO8videomode(char c)
{
 port[0x1c] = c;
 switch(c)
 {
  case 0x21: Decodevideo = Decode320x4; break;
  case 0x2a: Decodevideo = Decode640x2; break;
  case 0x41: Decodevideo = Decode320x4special; break;
  case 0x7b: Decodevideo = Decode160x16; break;
  default:   Decodevideo = Decode320x16; break;
 }
}

// Selection d'une couleur de palette /////////////////////////////////////////
void Palettecolor(char c)
{
 int i = port[0x1b];
 void Palette(int n, int r, int v, int b);
 x7da[i] = c;
 port[0x1b] = (port[0x1b] + 1) & 0x1f;
 if((i & 1))
 {
  char c1 = x7da[i & 0x1e];
  Palette(i >> 1, c1 & 0x0f, (c1 & 0xf0) >> 4, c & 0x0f);
 }
}

// Signaux de synchronisation ligne et trame /////////////////////////////////
int Iniln()
{//11 microsecondes - 41 microsecondes - 12 microsecondes
 if(videolinecycle < 11) return 0;
 if(videolinecycle > 51) return 0;
 return 0x20;
}

int Initn()
{//debut à 12 microsecondes ligne 56, fin à 51 microsecondes ligne 255
 if(videolinenumber < 56) return 0;
 if(videolinenumber > 255) return 0;
 if(videolinenumber == 56) if(videolinecycle < 12) return 0;
 if(videolinenumber == 255) if(videolinecycle > 50) return 0;
 return 0x80;
}

// Joystick emulation ////////////////////////////////////////////////////////
void Joysemul(int i, int state)
{
 //PA0=0 nord   PA1=0 sud   PA2=0 ouest   PA3=0 est   PB6=0 action
 //PA4=1 nord   PA5=1 sud   PA6=1 ouest   PA7=1 est   PB7=1 action
 int n;
 n = 0;
 switch(i)
 {
  case 0: if(joysposition & 0x02) n = 0x01; break;
  case 1: if(joysposition & 0x01) n = 0x02; break;
  case 2: if(joysposition & 0x08) n = 0x04; break;
  case 3: if(joysposition & 0x04) n = 0x08; break;
  case 4: if(joysposition & 0x20) n = 0x10; break;
  case 5: if(joysposition & 0x10) n = 0x20; break;
  case 6: if(joysposition & 0x80) n = 0x40; break;
  case 7: if(joysposition & 0x40) n = 0x80; break;
  case 8: if(state) joysaction |= 0x40; else joysaction &= 0xbf; return;
  case 9: if(state) joysaction |= 0x80; else joysaction &= 0x7f; return;
 }
 if(n > 0) {if(state) joysposition |= n; else joysposition &= (~n);}
}

// Joystick move /////////////////////////////////////////////////////////////
void Joysmove(int n, int x, int y)
{
 n <<= 2;
 Joysemul(n++, (y < 27768) ? 0 : 0x80);
 Joysemul(n++, (y > 37767) ? 0 : 0x80);
 Joysemul(n++, (x < 27768) ? 0 : 0x80);
 Joysemul(n++, (x > 37767) ? 0 : 0x80);
}

// Initialisation programme de l'ordinateur émulé ////////////////////////////
void Initprog()
{
 int i;
 void Reset6809();
 for(i = 0; i < KEYBOARDKEY_MAX; i++) touche[i] = 0x80; //touches relachées
 joysposition = 0xff;                      //manettes au centre
 joysaction = 0xc0;                        //boutons relachés
 carflags &= 0xec;
 Mputc = Mputto8d;
 Mgetc = Mgetto8d;
 Decodevideo = Decode320x16;
 ramuser = ram - 0x2000;
 Videopage_bordercolor(port[0x1d]);
 TO8videoram();
 TO8rambank();
 TO8rombank();
 Reset6809();
}

// Patch de la rom ////////////////////////////////////////////////////////////
void TO8dpatch(char rom[], int patch[])
{
 int i, j, a, n;
 i = 0;
 while((n = patch[i++]))
 {
  a = patch[i++];  //debut de la banque
  a += patch[i++]; //adresse dans la banque
  for(j = 0; j < n; j++) rom[a++] = patch[i++];
 }
}

// Hardreset de l'ordinateur émulé ///////////////////////////////////////////
void Hardreset()
{
 int i;
 time_t curtime;
 struct tm *loctime;
 extern int pause6809;
 pause6809 = 1;
 for(i = 0; i < sizeof(ram); i++) ram[i] = -((i & 0x80) >> 7);
 for(i = 0; i < sizeof(port); i++) port[i] = 0; port[0x09] = 0x0f;
 for(i = 0; i < sizeof(car); i++) car[i] = 0;
 //patch de la rom
 TO8dpatch(to8dbasic, to8dbasicpatch);
 TO8dpatch(to8dmoniteur - 0xe000, to8dmoniteurpatch);
 //en rom : remplacer jj-mm-aa par la date courante
 curtime = time(NULL);
 loctime = localtime(&curtime);
 strftime(to8dbasic + 0xeb90, 9, "%d-%m-%y", loctime);
 to8dbasic[0xeb98] = 0x1f;
 //en rom : au reset initialiser la date courante
 //24E2 8E2B90  LDX  #$2B90
 //24E5 BD29C8  BSR  $29C8
 to8dbasic[0xe4e2] = 0x8e; to8dbasic[0xe4e3] = 0x2b;
 to8dbasic[0xe4e4] = 0x90; to8dbasic[0xe4e5] = 0xbd;
 to8dbasic[0xe4e6] = 0x29; to8dbasic[0xe4e7] = 0xc8;
 nvideobank = 0;
 nrambank = 0;
 nsystbank = 0;
 nctrlbank = 0;
 keyb_irqcount = 0;
 timer_irqcount = 0;
 videolinecycle = 0;
 videolinenumber = 0;
 vblnumber = 0;
 Initprog();
 latch6846 = 65535;
 timer6846 = 65535;
 sound = 0;
 mute = 0;
 penbutton = 0;
 capslock = 1;
 pause6809 = 0;
}

// Timer control /////////////////////////////////////////////////////////////
void Timercontrol()
{
 if(port[0x05] & 0x01) timer6846 = latch6846 << 3;
}

// Traitement des entrees-sorties ////////////////////////////////////////////
void Entreesortie(int io)
{
 extern void Readoctetk7(), Writeoctetk7();
 extern void Readsector(), Writesector(), Formatdisk(), Imprime();
 extern void Readmousebutton(), Readpenxy(int i);
 switch(io)
 {
  case 0x14: Readsector(); break;      //lit secteur qd-fd
  case 0x15: Writesector(); break;     //ecrit secteur qd-fd
  case 0x18: Formatdisk(); break;      //formatage qd-fd
  case 0x42: Readoctetk7(); break;     //lit octet cassette
  case 0x45: Writeoctetk7(); break;    //ecrit octet cassette
  case 0x4b: Readpenxy(0); break;      //lit position crayon
  case 0x4e: Readpenxy(1); break;      //lit position souris
  case 0x51: Imprime(); break;         //imprime un caractere
  case 0x52: Readmousebutton(); break; //test clic souris
  default: break;                      //code op. invalide
 }
}

// Execution n cycles processeur 6809 ////////////////////////////////////////
int Run(int ncyclesmax)
{
 int ncycles, opcycles;
 int Run6809();
 void Displaysegment();
 void Nextline();
 void Displayscreen();
 ncycles = 0;
 while(ncycles < ncyclesmax)
 {
  //execution d'une instruction
  opcycles = Run6809();
  if(opcycles < 0) {Entreesortie(-opcycles); opcycles = 64;}
  ncycles += opcycles;
  videolinecycle += opcycles;
  if(displayflag) Displaysegment();
  if(videolinecycle >= 64)
  {
   videolinecycle -= 64;
   if(displayflag) Nextline();
   if(++videolinenumber > 311)
   //valeurs de videolinenumber :
   //000-047 hors ecran, 048-055 bord haut
   //056-255 zone affichable
   //256-263 bord bas, 264-311 hors ecran
   {
    videolinenumber -= 312;
    if(vblnumber == 0) Displayscreen();
    if(++vblnumber >= vblnumbermax) vblnumber = 0;
   }
   displayflag = 0;
   if(vblnumber == 0)
     if(videolinenumber > 47)
       if(videolinenumber < 264)
         displayflag = 1;
  }
  //decompte du temps de presence du signal irq timer
  if(timer_irqcount > 0) timer_irqcount -= opcycles;
  if(timer_irqcount <= 0) port[0x00] &= 0xfe;
  //decompte du temps de presence du signal irq clavier
  if(keyb_irqcount > 0) keyb_irqcount -= opcycles;
  if(keyb_irqcount <= 0) port[0x00] &= 0xfd;
  //clear signal irq si aucune irq active
  if((port[0x00] & 0x07) == 0) {port[0x00] &= 0x7f; dc6809_irq = 0;}
  //countdown du timer 6846
  if((port[0x05] & 0x01) == 0) //timer enabled
  {timer6846 -= (port[0x05] & 0x04) ? opcycles : opcycles << 3;} //countdown
  //counter time out
  if(timer6846 <= 5)
  {
   timer_irqcount = 100;
   timer6846 = latch6846 << 3; //reset counter
   port[0x00] |= 0x81; //flag interruption timer et interruption composite
   dc6809_irq = 1; //positionner le signal IRQ pour le processeur
  }
 }
 return(ncycles - ncyclesmax); //retour du nombre de cycles en trop (extracycles)
}

// Ecriture memoire to8d /////////////////////////////////////////////////////
void Mputto8d(unsigned short a, char c)
{
 switch(a >> 12)
 {
  //subtilité :
  //quand la rom est recouverte par la ram, les 2 segments de 8 Ko sont inversés
  case 0x0: case 0x1:
   if(!(port[0x26] & 0x20)) {carflags = (carflags & 0xfc) | (a & 3); TO8rombank();}
   if((port[0x26] & 0x60) != 0x60) return;
   if(port[0x26] & 0x20) rombank[a + 0x2000] = c; else rombank[a] = c; return;
  case 0x2: case 0x3: if((port[0x26] & 0x60) != 0x60) return;
   if(port[0x26] & 0x20) rombank[a - 0x2000] = c; else rombank[a] = c; return;
  case 0x4: case 0x5: ramvideo[a] = c; return;
  case 0x6: case 0x7: case 0x8: case 0x9: ramuser[a] = c; return;
  case 0xa: case 0xb: case 0xc: case 0xd: rambank[a] = c; return;
  case 0xe: switch(a)
  {
   case 0xe7c0: port[0x00] = c; return;
   case 0xe7c1: port[0x01] = c; mute = c & 8; return;
   case 0xe7c3: port[0x03] = (c & 0x3d); if((c & 0x20) == 0) keyb_irqcount = 0;
                TO8videoram(); TO8rombank(); return;
   case 0xe7c5: port[0x05] = c; Timercontrol(); return; //controle timer
   case 0xe7c6: latch6846 = (latch6846 & 0xff) | ((c & 0xff) << 8); return;
   case 0xe7c7: latch6846 = (latch6846 & 0xff00) | (c & 0xff); return;
   //6821 systeme: 2 ports 8 bits
   //e7c8= registre de direction ou de donnees port A (6821 systeme)
   //e7c9= registre de direction ou de donnees port B
   //e7ca= registre de controle port A (CRA)
   //e7cb= registre de controle port B (CRB)
   case 0xe7c9: port[0x09] = c; TO8rambank(); return;
   case 0xe7cc: port[0x0c] = c; return;
   case 0xe7cd: if(port[0x0f] & 4) sound = c & 0x3f; else port[0x0d] = c; return;
   case 0xe7ce: port[0x0e] = c; return; //registre controle position joysticks
   case 0xe7cf: port[0x0f] = c; return; //registre controle action - musique
   case 0xe7d8: return;
   case 0xe7da: Palettecolor(c); return;
   case 0xe7db: port[0x1b] = c; return;
   case 0xe7dc: TO8videomode(c); return;
   case 0xe7dd: Videopage_bordercolor(c); return;
   case 0xe7e4: port[0x24] = c; return;
   case 0xe7e5: port[0x25] = c; TO8rambank(); return;
   case 0xe7e6: port[0x26] = c; TO8rombank(); return;
   case 0xe7e7: port[0x27] = c; TO8rambank(); return;
   default: return;
  }
  default: return;
 }
 return;
}

// Lecture memoire to8d //////////////////////////////////////////////////////
char Mgetto8d(unsigned short a)
{
 extern int penbutton;
 extern int joysposition, joysaction;
 switch(a >> 12)
 {
  //subtilité : quand la rom est recouverte par la ram, les 2 segments de 8 Ko sont inversés
  case 0x0: case 0x1: return (port[0x26] & 0x20) ? rombank[a + 0x2000] : rombank[a];
  case 0x2: case 0x3: return (port[0x26] & 0x20) ? rombank[a - 0x2000] : rombank[a];
  case 0x4: case 0x5: return ramvideo[a];
  case 0x6: case 0x7: case 0x8: case 0x9: return ramuser[a];
  case 0xa: case 0xb: case 0xc: case 0xd: return rambank[a];
  case 0xe: switch(a)
  {
   case 0xe7c0: return((port[0]) ? (port[0] | 0x80) : 0);
   case 0xe7c3: return(port[0x03] | 0x80 | (penbutton << 1));
   case 0xe7c6: return (timer6846 >> 11 & 0xff);
   case 0xe7c7: return (timer6846 >> 3 & 0xff);
   case 0xe7ca: return (videolinenumber < 200) ? 0 : 2; //non, registre de controle PIA
   case 0xe7cc: return((port[0x0e] & 4) ? joysposition : port[0x0c]);
   case 0xe7cd: return((port[0x0f] & 4) ? joysaction | sound : port[0x0d]);
   case 0xe7ce: return 0x04;
   case 0xe7da: return x7da[port[0x1b]++ & 0x1f];
   case 0xe7df: port[0x1e] = 0; return(port[0x1f]);
   case 0xe7e4: return port[0x1d] & 0xf0;
   case 0xe7e5: return port[0x25] & 0x1f;
   case 0xe7e6: return port[0x26] & 0x7f;
   case 0xe7e7: return (port[0x24] & 0x01) | Initn() | Iniln();
   default: if(a < 0xe7c0) return(romsys[a]);
            if(a < 0xe800) return(port[a & 0x3f]);
            return(romsys[a]);
  }
  default: return romsys[a];
 }
}
