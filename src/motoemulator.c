/*
 * This file is part of theodore (https://github.com/Zlika/theodore),
 * a Thomson emulator based on Daniel Coulom's DCTO8D/DCTO9P/DCMO5
 * emulators (http://dcmoto.free.fr/).
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

/* Thomson MO/TO emulator */

#include "motoemulator.h"
#ifdef THEODORE_DASM
#include "debugger.h"
#endif

#include <string.h>
#include <time.h>

#include "6809cpu.h"
#include "debugger.h"
#include "devices.h"
#include "video.h"
#include "rom/rom_to8.inc"
#include "rom/rom_to8d.inc"
#include "rom/rom_to9.inc"
#include "rom/rom_to9p.inc"
#include "rom/rom_mo5.inc"
#include "rom/rom_mo6.inc"
#include "rom/rom_pc128.inc"
#include "rom/rom_to7.inc"
#include "rom/rom_to770.inc"
#include "rom/basic-1_memo7.inc"
#include "rom/basic-128_memo7.inc"

#define VBL_NUMBER_MAX  2
// Number of keys of the keyboard
#define KEYBOARDKEY_MAX 84
#define PALETTE_SIZE    32
// Sound level on 6 bits
#define MAX_SOUND_LEVEL 0x3f

typedef struct
{
  char *basic;        // "BASIC and other embedded software" part of the ROM
  int *basic_patch;   // Patch to apply to the "BASIC and other embedded software" part of the ROM
  char *monitor;      // Pointer to the beginning of the "monitor" part of the ROM
  int *monitor_patch; // Patch to apply to the "monitor" part of the ROM
  char *disk_drive_monitor;      // MO5/MO6: disk driver monitor
  int *disk_drive_monitor_patch; // MO5/MO6: Patch to apply to the disk drive monitor
  bool is_mo;                    // If it is a MO or TO system
  bool is_mo6;                   // If it is a MO6 or a PC128
} SystemRom;

static SystemRom ROM_TO8 = { to8_basic_rom, to8_basic_patch, to8_monitor_rom, to8_monitor_patch, NULL, NULL, false, false };
static SystemRom ROM_TO8D = { to8_basic_rom, to8_basic_patch, to8d_monitor_rom, to8d_monitor_patch, NULL, NULL, false, false };
static SystemRom ROM_TO9 = { to9_basic_rom, to9_basic_patch, to9_monitor_rom, to9_monitor_patch, NULL, NULL, false, false };
static SystemRom ROM_TO9P = { to9p_basic_rom, to9p_basic_patch, to9p_monitor_rom, to9p_monitor_patch, NULL, NULL, false, false };
static SystemRom ROM_MO5 = { mo5_v2_basic_rom, mo5_v2_basic_patch, mo5_v2_monitor_rom, mo5_v2_monitor_patch, cd90_640_rom, cd90_640_patch, true, false };
static SystemRom ROM_MO6 = { mo6_v3_basic128_rom, mo6_v3_basic128_patch, mo6_v3_basic1_rom, mo6_v3_basic1_patch, cd90_640_rom, cd90_640_patch, true, true };
static SystemRom ROM_PC128 = { pc128_basic128_rom, pc128_basic128_patch, pc128_basic1_rom, pc128_basic1_patch, cd90_640_rom, cd90_640_patch, true, true };
static SystemRom ROM_TO770 = { NULL, NULL, to770_monitor_rom, to770_monitor_patch, NULL, NULL, false, false };
static SystemRom ROM_TO7 = { NULL, NULL, to7_monitor_rom, to7_monitor_patch, NULL, NULL, false, false };

static ThomsonModel currentModel = TO8;
static SystemRom *rom = &ROM_TO8;

// memory
char car[CARTRIDGE_MEM_SIZE];   //espace cartouche 4x16K
char ram[RAM_SIZE];             //ram 512K
char port[IO_MEM_SIZE];         //ports d'entree/sortie (0xE7C0 -> 0xE7FF)
static char x7da[PALETTE_SIZE]; //stockage de la palette de couleurs
// pointers
char *pagevideo;            //pointeur page video affichee
static char *ramvideo;      //pointeur couleurs ou formes
static char *ramuser;       //pointeur ram utilisateur fixe
static char *rambank;       //pointeur banque ram utilisateur
static char *romsys;        //pointeur rom systeme
static char *rombank;       //pointeur banque rom ou cartouche
//flags cartouche
int cartype;         //type de cartouche (0=simple, 1=switch bank, 2=os-9)
int carflags = 0;    //bits0,1,4=bank, 2=cart-enabled, 3=write-enabled
//keyboard, joysticks, mouse
static int touche[KEYBOARDKEY_MAX]; //etat touches
static int capslock;         //1=capslock, 0 sinon
static int joysposition;     //position des manches
static int joysaction;       //position des boutons d'action
int xpen, ypen;              //lightpen coordinates
int penbutton;               //lightpen button state
//affichage
int videolinecycle;         //compteur ligne (0-63)
int videolinenumber;        //numero de ligne video affichee (0-311)
static int vblnumber;       //compteur du nombre de vbl avant affichage
static int displayflag;     //indicateur pour l'affichage
int bordercolor;            //couleur de la bordure de l'écran
//divers
static int sound;                  //niveau du haut-parleur
static int mute;                   //mute flag
static int timer6846;       //compteur du timer 6846
static int latch6846;       //registre latch du timer 6846
static int keyb_irqcount;   //nombre de cycles avant la fin de l'irq clavier
static int timer_irqcount;  //nombre de cycles avant la fin de l'irq timer
//reserved data in serialization for future use
static int reserved1 = 0;
static int reserved2 = 0;
static int reserved3 = 0;
static int reserved4 = 0;

//Forward declarations
static char MgetTo(unsigned short a);
static void MputTo(unsigned short a, char c);
static char MgetMo(unsigned short a);
static void MputMo(unsigned short a, char c);
static char MgetTo7(unsigned short a);
static void MputTo7(unsigned short a, char c);

void (*selectVideoRam)(void);
void (*selectRomBank)(void);

//Table de conversion scancode TO9 --> code ASCII
const int to9key[0xa0] =
{
//code ASCII non shiftes
/*00*/0x91,0x5f,0x79,0x68,0x0b,0x09,0x1e,0x6e,
      0x92,0x28,0x74,0x67,0x3d,0x08,0x1c,0x62,
/*10*/0x93,0x27,0x72,0x66,0x16,0x31,0x1d,0x76,
      0x94,0x22,0x65,0x64,0x37,0x34,0x30,0x63,
/*20*/0x90,0x80,0x7a,0x73,0x38,0x32,0x2e,0x78,
      0x23,0x2a,0x61,0x71,0x5b,0x35,0x36,0x77,
/*30*/0x02,0x81,0x75,0x6a,0x20,0x39,0x0d,0x2c,
      0xb0,0x21,0x69,0x6b,0x24,0x0a,0x5d,0x3b,
/*40*/0xb7,0x82,0x6f,0x6c,0x2d,0x84,0x0d,0x3a,
      0xb3,0x83,0x70,0x6d,0x29,0x5e,0x33,0x3e,
//code ASCII shiftes
/*00*/0x96,0x36,0x59,0x48,0x0b,0x09,0x0c,0x4e,
      0x97,0x35,0x54,0x47,0x2b,0x08,0x1c,0x42,
/*10*/0x98,0x34,0x52,0x46,0x16,0x31,0x7f,0x56,
      0x99,0x33,0x45,0x44,0x37,0x34,0x30,0x43,
/*20*/0x95,0x32,0x5a,0x53,0x38,0x32,0x2e,0x58,
      0x40,0x31,0x41,0x51,0x7b,0x35,0x36,0x57,
/*30*/0x03,0x37,0x55,0x4a,0x20,0x39,0x0d,0x3f,
      0xb0,0x38,0x49,0x4b,0x26,0x0a,0x7d,0x2e,
/*40*/0xb8,0x39,0x4f,0x4c,0x5c,0x25,0x0d,0x2f,
      0xb3,0x30,0x50,0x4d,0x86,0x85,0x33,0x3c
};

// Registres du 6846 (TO) /////////////////////////////////////////////////////
/*
E7C0= registre d'etat composite port C (CSR)
- csr0= timer interrupt flag
- csr1= cp1 interrupt flag (clavier)
- csr2= cp2 interrupt flag
- csr3-csr6 unused and set to zeroes
- csr7= composite interrupt flag (si au moins un interrupt flag a 1)

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

int16_t GetAudioSample(void)
{
  return mute ? 0 : (sound * 65535 / MAX_SOUND_LEVEL) - (65536 / 2);
}

void SetThomsonModel(ThomsonModel model)
{
  if (model != currentModel)
  {
    switch (model)
    {
      case TO8:
        rom = &ROM_TO8;
        break;
      case TO8D:
        rom = &ROM_TO8D;
        break;
      case TO9:
        rom = &ROM_TO9;
        break;
      case TO9P:
        rom = &ROM_TO9P;
        break;
      case MO5:
        rom = &ROM_MO5;
        break;
      case MO6:
        rom = &ROM_MO6;
        break;
      case PC128:
        rom = &ROM_PC128;
        break;
      case TO7:
        rom = &ROM_TO7;
        LoadMemoFromArray(basic_1_memo7_rom, basic_1_memo7_rom_len);
        break;
      case TO7_70:
        rom = &ROM_TO770;
        LoadMemoFromArray(basic_128_memo7_rom, basic_128_memo7_rom_len);
        break;
      default:
        return;
    }
    currentModel = model;
    SetModeTO(!rom->is_mo);
    Hardreset();
  }
}

ThomsonModel GetThomsonModel(void)
{
  return currentModel;
}

// Emulation du clavier TO8/TO9 ///////////////////////////////////////////////
void keyboard(int scancode, bool down)
{
  int i;
  // Filter false key down events when the key was already down
  if (down && !touche[scancode]) return;
  touche[scancode] = down ? 0x00 : 0x80;
  if ((rom->is_mo) || (currentModel == TO7) || (currentModel == TO7_70))
  {
    return;
  }
  if (!down) //touche relachee
  {
    //s'il reste une touche enfoncee, ne rien faire
    for(i = 0; i < 0x50; i++) if(touche[i] == 0) return;
    //si toutes les touches sont relachees
    port[0x08] = 0x00; //bit 0 de E7C8 = 0 (toutes les touches relachees)
    keyb_irqcount = 0;
    return;
  }
  //touche enfoncee
  if(scancode == 0x50) capslock = 1 - capslock; //capslock
  if(scancode > 0x4f) return;           //touches shift, ctrl et joysticks
  i = 0;
  if(!touche[0x51]) i = 0x80; //SHIFT gauche
  if(!touche[0x52]) i = 0x80; //SHIFT droit
  if(capslock) switch(scancode)
  {
    case 0x02: case 0x03: case 0x07: case 0x0a: case 0x0b: case 0x0f: case 0x12:
    case 0x13: case 0x17: case 0x1a: case 0x1b: case 0x1f: case 0x22: case 0x23:
    case 0x27: case 0x2a: case 0x2b: case 0x2f: case 0x32: case 0x33: case 0x3a:
    case 0x3b: case 0x42: case 0x43: case 0x4a: case 0x4b: i = 0x80; break;
  }
  if (currentModel == TO8 || currentModel == TO8D)
  {
    rom->monitor[0x30f8] = scancode | i;         //scancode + indicateur de touche SHIFT
    rom->monitor[0x3125] = touche[0x53] ? 0 : 1; //indicateur de touche CTRL
    port[0x08] |= 0x01; //bit 0 de E7C8 = 1 (touche enfoncee)
    port[0x00] |= 0x82; //bit CP1 = interruption clavier
    keyb_irqcount = 500000; //positionne le signal d'irq pour 500 ms maximum
    dc6809_irq = 1;
  }
  else // TO9 / TO9+
  {
    if (i == 0x80) scancode += 0x50;
    i = to9key[scancode]; //code ASCII
    if (!touche[0x53])    //touche CNT
    {
      if (i == 23) i = 0;
      if ((i > 0x3f) & (i < 0x60)) i -= 0x40;
      if ((i > 0x60) & (i < 0x80)) i -= 0x60;
    }
    port[0x08] = 0x01; //bit 0 de E7C8 = 1 (touche enfoncee)
    port[0x1e] = 0x01; //touche en attente
    port[0x1f] = i;    //code ASCII TO9-TO9+
  }
}

// Selection de banques memoire //////////////////////////////////////////////
static void selectVideoRamTo(void)
{
  int nsystbank;  //numero banque systeme (00-01)
  int nvideopage; //numero page video (00-01)
  nvideopage = port[0x03] & 1;
  // The "video" data (either from RAMA or RAMB) is mapped in memory at 0x4000-0x5FFF
  ramvideo = ram - 0x4000 + (nvideopage << 13);
  nsystbank = (currentModel != TO9) ? (port[0x03] & 0x10) >> 4 : 0;
  // The "monitor" software is mapped in memory starting at address 0xe000
  romsys = rom->monitor - 0xe000 + (nsystbank << 13);
}

static void selectVideoRamTo7(void)
{
  int nvideopage; //numero page video (00-01)
  nvideopage = port[0x03] & 1;
  // The "video" data (either from RAMA or RAMB) is mapped in memory at 0x4000-0x5FFF
  ramvideo = ram - 0x4000 + (nvideopage << 13);
  // The "monitor" software is mapped in memory starting at address 0xe800
  romsys = rom->monitor - 0xe800;
  if (currentModel == TO7)
  {
    bordercolor = (port[0x03] >> 4) & 0x07;
  }
  else
  {
    // TO7/70 (Pastel + BGR)
    bordercolor = ((port[0x03] >> 4) & 0x07) | ((~port[0x03] & 0x04) << 1);
  }
}

static void selectVideoRamMo5(void)
{
  int nvideopage = port[0] & 1; //numero page video (00-01)
  // The "video" data (either from RAMA or RAMB) is mapped in memory at 0x0000-0x1FFF
  ramvideo = ram + (nvideopage << 13);
  // The "monitor" software is mapped in memory starting at address 0xf000
  romsys = rom->monitor - 0xf000;
  bordercolor = (port[0] >> 1) & 0x0f;
}

static void selectVideoRamMo6(void)
{
  int nvideopage = port[0] & 1; //numero page video (00-01)
  // The "video" data (either from RAMA or RAMB) is mapped in memory at 0x0000-0x1FFF
  ramvideo = ram + (nvideopage << 13);
  // The "monitor" software is mapped in memory starting at address 0xf000
  romsys = rom->monitor + ((port[0] & 0x20) << 9) + 0x3000 - 0xf000;
}

static void selectRamBankTo(void)
{
  int nrambank; // RAM page (TO8 mode) or bank (TO7/TO9 mode) number

  // TO8 mode (5 lower bits of e7e5 = RAM page number)
  // (bit D4 of gate array mode page's "system 1" register at e7e7 = 0
  // if RAM bank switching is done via PIA bits for TO7-70/TO9 emulation)
  if ((port[0x27] & 0x10) && (currentModel != TO9) && (currentModel != TO7_70))
  {
    nrambank = port[0x25] & 0x1f; // RAM page number (0-31)
    rambank = ram - 0xa000 + (nrambank << 14);
  }
  else
  {
    // TO7-70/TO9 compatibility mode via e7c9
    switch (port[0x09] & 0xf8)
    {
      case 0x08: nrambank = 0; break;
      case 0x10: nrambank = 1; break;
      case 0xe0: nrambank = 2; break;
      case 0xa0: nrambank = ((currentModel == TO9) || (currentModel == TO7_70)) ? 4 : 3; break;  // banks 3 and 4
      case 0x60: nrambank = ((currentModel == TO9) || (currentModel == TO7_70)) ? 3 : 4; break;  // inverted/TO7-70&TO9
      case 0x20: nrambank = 5; break;
      default: return;
    }
    // RAM bank 0 = RAM page 2 at physical address 0x8000 and logical address 0xa000
    // RAM bank n = RAM page n+2 at physical address 0x4000*(n+2) and logical address 0xa000
    rambank = ram - (0xa000 - 0x8000) + (nrambank << 14);
  }
}

static void selectRamBankMo6(void)
{
  int nrampage; // RAM page number
  nrampage = port[0x25] & 0x1f;
  rambank = ram - 0x6000 + (nrampage << 14);
}

static void selectRomBankTo(void)
{
  int nrombank;        //numero banque rom (00-07)
  if (currentModel != TO9)
  {
    //romsys = rom + 0x2000 + ((cnt[0x7c3] & 0x10) << 9);
    //si le bit 0x20 de e7e6 est positionne a 1 l'espace ROM est recouvert
    //par la banque RAM definie par les 5 bits de poids faible de e7e6
    //subtilite : les deux segments de 8K de la banque sont inverses.
    if (port[0x26] & 0x20)
    {
      rombank = ram + ((port[0x26] & 0x1f) << 14);
    }
    //sinon le bit2 de e7c3 commute entre ROM interne et cartouche
    else if (port[0x03] & 0x04)
    {
      nrombank = carflags & 3;
      rombank = rom->basic + (nrombank << 14);
    }
    else
    {
      rombank = car + ((carflags & 3) << 14);
    }
  }
  else // TO9
  {
    // bits P5 & P4 from the 6846 (at 0xe7c3) are used to select the ROM slot.
    // slot 0 = BASIC 128 (bank 0), extramon (bank 1), BASIC 1 (bank 2), iconic DOS (bank 3)
    // slot 1/2 = PARAGRAPHE and FICHES ET DOSSIERS software
    // slot 3 = cartridge
    switch ((port[0x03] & 0x30) >> 4)
    {
      case 0: // slot 0 (64ko, 4 banks)
        nrombank = carflags & 3;
        rombank = rom->basic + (nrombank << 14);
        break;
      case 1: // slot 1 (32ko, 2 banks)
        nrombank = 4 + (carflags & 1);
        rombank = rom->basic + (nrombank << 14);
        break;
      case 2: // slot 2 (32ko, 2 banks)
        nrombank = 6 + (carflags & 1);
        rombank = rom->basic + (nrombank << 14);
        break;
      case 3: // cartridge
        rombank = car + ((carflags & 3) << 14);
        break;
      default: break;
    }
  }
}

static void selectRomBankTo7(void)
{
  rombank = car + ((carflags & 3) << 14);
}

static void selectRomBankMo5(void)
{
  if ((carflags & 4) == 0)
  {
    rombank = rom->basic - 0xc000;
  }
  else
  {
    rombank = car - 0xb000 + ((carflags & 0x03) << 14);
    if ((cartype == 2) && (carflags & 0x10)) rombank += 0x10000;
  }
}

static void selectRomBankMo6(void)
{
  if ((carflags & 4) == 0)
  {
    if (port[0x1d] & 0x10)
    {
      // BASIC128 EPROM selected
      rombank = rom->basic + ((port[0x00] & 0x20) << 9) - 0xb000;
    }
    else
    {
      // BASIC1 EPROM selected
      rombank = rom->monitor + ((port[0x00] & 0x20) << 9) - 0xc000;
    }
    romsys = rom->monitor + ((port[0x00] & 0x20) << 9) + 0x3000 - 0xf000;
  }
  else
  {
    // Cartridge enabled
    rombank = car - 0xb000 + ((carflags & 0x03) << 14);
    if ((cartype == 2) && (carflags & 0x10)) rombank += 0x10000;
  }
}

static void SwitchMemo5Bank(int a)
{
  if(cartype != 1) return;
  // Read in $BFFC-$BFFF changes the cartridge ROM bank (2 LSB bits = bank number)
  if((a & 0xfffc) != 0xbffc) return;
  carflags = (carflags & 0xfc) | (a & 3);
  selectRomBank();
}

static void videopage_bordercolor(char c)
{
  port[0x1d] = c;
  pagevideo = ram + ((c & 0xc0) << 8);
  bordercolor = c & 0x0f;
}

// Selection video ////////////////////////////////////////////////////////////
static void selectVideomode(char c)
{
  port[0x1c] = c;
  switch(c)
  {
    case 0x21: SetVideoMode(VIDEO_320X4); break;
    case 0x2a: SetVideoMode(VIDEO_640X2); break;
    case 0x41: SetVideoMode(VIDEO_320X4_SPECIAL); break;
    case 0x7b: SetVideoMode(VIDEO_160X16); break;
    default:   if (rom->is_mo)
                 SetVideoMode(VIDEO_320_16_MO5);
               else SetVideoMode(VIDEO_320X16);
               break;
  }
}

// Selection d'une couleur de palette /////////////////////////////////////////
static void Palettecolor(char c)
{
  int i = port[0x1b];
  x7da[i] = c;
  port[0x1b] = (port[0x1b] + 1) & 0x1f;
  if((i & 1))
  {
    char c1 = x7da[i & 0x1e];
    Palette(i >> 1, c1 & 0x0f, (c1 & 0xf0) >> 4, c & 0x0f);
  }
}

// Line sync signal //////////////////////////////////////////////////////////
static int Iniln(void)
{
  // Duration of 1 line = 64 microseconds.
  // The useful part of the screen (working window) represents a zone of
  // 40 microseconds wide, surrounded by 2 unused zones (frame) of 12 microseconds each.
  // 11 microsecondes - 41 microsecondes - 12 microsecondes
  if(videolinecycle < 11) return 0;
  if(videolinecycle > 51) return 0;
  return 0x20;
}

// Frame sync signal /////////////////////////////////////////////////////////
static int Initn(void)
{
  // The useful part of the screen (working window) is composed of 200 lines of 64 microseconds.
  // It starts at 12 microsecondes line 56, and ends at 51 microsecondes line 255.
  if(videolinenumber < 56) return 0;
  if(videolinenumber > 255) return 0;
  if(videolinenumber == 56) if(videolinecycle < 12) return 0;
  if(videolinenumber == 255) if(videolinecycle > 50) return 0;
  return 0x80;
}

// Joystick emulation ////////////////////////////////////////////////////////
void Joysemul(JoystickAxis axis, bool isOn)
{
  //PA0=0 nord   PA1=0 sud   PA2=0 ouest   PA3=0 est   PB6=0 action
  //PA4=1 nord   PA5=1 sud   PA6=1 ouest   PA7=1 est   PB7=1 action
  int n;
  n = 0;
  switch(axis)
  {
    case 0: if(joysposition & 0x02) n = 0x01; break;
    case 1: if(joysposition & 0x01) n = 0x02; break;
    case 2: if(joysposition & 0x08) n = 0x04; break;
    case 3: if(joysposition & 0x04) n = 0x08; break;
    case 4: if(joysposition & 0x20) n = 0x10; break;
    case 5: if(joysposition & 0x10) n = 0x20; break;
    case 6: if(joysposition & 0x80) n = 0x40; break;
    case 7: if(joysposition & 0x40) n = 0x80; break;
    case 8: if(!isOn) joysaction |= 0x40; else joysaction &= 0xbf; return;
    case 9: if(!isOn) joysaction |= 0x80; else joysaction &= 0x7f; return;
  }
  if(n > 0) {if(!isOn) joysposition |= n; else joysposition &= (~n);}
}

// Initialisation of the emulated computer ////////////////////////////////////
void Initprog(void)
{
  int i;
  // keyboards's keys released
  for(i = 0; i < KEYBOARDKEY_MAX; i++) touche[i] = 0x80;
  // joystick centered and buttons released
  joysposition = 0xff;
  joysaction = 0xc0;
  carflags &= 0xec;

  SetVideoMode(VIDEO_320X16);

  if (currentModel == MO5)
  {
    ramuser = ram + 0x2000;
    SetVideoMode(VIDEO_320_16_MO5);
    pagevideo = ram;
    Mputc = MputMo;
    Mgetc = MgetMo;
    selectVideoRam = selectVideoRamMo5;
    selectRomBank = selectRomBankMo5;
  }
  else if (rom->is_mo6)
  {
    ramuser = ram + 0x2000;
    Mputc = MputMo;
    Mgetc = MgetMo;
    selectVideoRam = selectVideoRamMo6;
    selectRomBank = selectRomBankMo6;
    port[0x25] = 0x02; // RAM bank 0 selected
    selectRamBankMo6();
  }
  else if ((currentModel == TO7) || (currentModel == TO7_70))
  {
    ramuser = ram - 0x2000;
    Mputc = MputTo7;
    Mgetc = MgetTo7;
    selectVideoRam = selectVideoRamTo7;
    selectRomBank = selectRomBankTo7;
    videopage_bordercolor(port[0x1d]);
    if (currentModel == TO7_70)
    {
      port[0x09] = 0x0f; // RAM bank 0 selected
      selectRamBankTo();
    }
  }
  else
  {
    ramuser = ram - 0x2000;
    Mputc = MputTo;
    Mgetc = MgetTo;
    selectVideoRam = selectVideoRamTo;
    selectRomBank = selectRomBankTo;
    videopage_bordercolor(port[0x1d]);
    port[0x09] = 0x0f; // RAM bank 0 selected
    selectRamBankTo();
  }

  selectVideoRam();
  selectRomBank();
  Reset6809();
}

// Patch of the ROM ///////////////////////////////////////////////////////////
static void patch_rom(char rom_data[], int patch[])
{
  int i, j, a, n;
  i = 0;
  while((n = patch[i++]))
  {
    a = patch[i++];  //debut de la banque
    a += patch[i++]; //adresse dans la banque
    for(j = 0; j < n; j++) rom_data[a++] = patch[i++];
  }
}

// Write the current date in the ROM //////////////////////////////////////////
static void set_current_date(void)
{
  time_t curtime;
  struct tm *loctime;
  if (currentModel == TO8 || currentModel == TO8D || currentModel == TO9P)
    {
      //en rom : remplacer jj-mm-aa par la date courante
      curtime = time(NULL);
      loctime = localtime(&curtime);
      strftime(rom->basic + 0xeb90, 9, "%d-%m-%y", loctime);
      rom->basic[0xeb98] = 0x1f;
      //en rom : au reset initialiser la date courante
      //24E2 8E2B90  LDX  #$2B90
      //24E5 BD29C8  BSR  $29C8
      rom->basic[0xe4e2] = 0x8e; rom->basic[0xe4e3] = 0x2b;
      rom->basic[0xe4e4] = 0x90; rom->basic[0xe4e5] = 0xbd;
      rom->basic[0xe4e6] = 0x29; rom->basic[0xe4e7] = 0xc8;
    }
}

// Hardreset of the emulated computer /////////////////////////////////////////
void Hardreset(void)
{
  unsigned int i;
  for(i = 0; i < sizeof(ram); i++)
  {
    ram[i] = -((i & 0x80) >> 7);
  }
  for(i = 0; i < sizeof(port); i++)
  {
    port[i] = 0;
  }
  // Reset cartridge space only if no cartridge is present
  if (carflags == 0)
  {
    memset(car, 0, CARTRIDGE_MEM_SIZE);
  }
  RewindTape();

  // Patch the ROM
  if ((rom->basic != NULL) && (rom->basic_patch != NULL))
  {
    patch_rom(rom->basic, rom->basic_patch);
  }
  if ((rom->monitor != NULL) && (rom->monitor_patch != NULL))
  {
    patch_rom(rom->monitor, rom->monitor_patch);
  }
  if ((rom->disk_drive_monitor != NULL) && (rom->disk_drive_monitor_patch != NULL))
  {
    patch_rom(rom->disk_drive_monitor, rom->disk_drive_monitor_patch);
  }
  // Set the current date
  set_current_date();

  keyb_irqcount = 0;
  timer_irqcount = 0;
  videolinecycle = 0;
  videolinenumber = 0;
  vblnumber = 0;
  InitPalette();
  Initprog();
  latch6846 = 65535;
  timer6846 = 65535;
  sound = 0;
  mute = 0;
  penbutton = 0;
  capslock = 1;
}

// Timer control /////////////////////////////////////////////////////////////
static void Timercontrol(void)
{
  if(port[0x05] & 0x01) timer6846 = latch6846 << 3;
}

// Execution n cycles processeur 6809 ////////////////////////////////////////
int Run(int ncyclesmax)
{
  int ncycles, opcycles;
  ncycles = 0;
  while(ncycles < ncyclesmax)
  {
#ifdef THEODORE_DASM
    debug(dc6809_pc & 0xFFFF);
#endif
    //execution d'une instruction
    opcycles = Run6809();
    if(opcycles < 0) {RunIoOpcode(-opcycles); opcycles = 64;}
    ncycles += opcycles;
    videolinecycle += opcycles;
    if(displayflag) Displaysegment();
    // Attente d'une fin de ligne
    if(videolinecycle >= 64)
    {
      videolinecycle -= 64;
      if(displayflag) Nextline();
      // Attente d'une fin de trame
      if(++videolinenumber > 311)
        //valeurs de videolinenumber :
        //000-047 hors ecran, 048-055 bord haut
        //056-255 zone affichable
        //256-263 bord bas, 264-311 hors ecran
      {
        videolinenumber -= 312;
        if(++vblnumber >= VBL_NUMBER_MAX) vblnumber = 0;
        if (rom->is_mo) Irq();
      }
      displayflag = ((vblnumber == 0) && (videolinenumber > 47) && (videolinenumber < 264));
    }
    if (!rom->is_mo)
    {
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
  }
  return(ncycles - ncyclesmax); //retour du nombre de cycles en trop (extracycles)
}

// TO8/TO9 memory write /////////////////////////////////////////////////////
static void MputTo(unsigned short a, char c)
{
#ifdef THEODORE_DASM
  debug_mem_write(a);
#endif
  switch(a >> 12)
  {
    case 0x0: case 0x1:
      if (currentModel != TO9)
      {
        //subtilite :
        //quand la rom est recouverte par la ram, les 2 segments de 8 Ko sont inverses
        if(!(port[0x26] & 0x20)) {carflags = (carflags & 0xfc) | (a & 3); selectRomBank();}
        if((port[0x26] & 0x60) != 0x60) return;
        if(port[0x26] & 0x20) rombank[a + 0x2000] = c; else rombank[a] = c; return;
      }
      else
      {
        carflags = (carflags & 0xfc) | (a & 3);
        selectRomBank();
        return;
      }
    case 0x2: case 0x3: if((port[0x26] & 0x60) != 0x60) return;
    if(port[0x26] & 0x20) rombank[a - 0x2000] = c; else rombank[a] = c; return;
    case 0x4: case 0x5: ramvideo[a] = c; return;
    case 0x6: case 0x7: case 0x8: case 0x9: ramuser[a] = c; return;
    case 0xa: case 0xb: case 0xc: case 0xd: rambank[a] = c; return;
    case 0xe:
      switch(a)
      {
        case 0xe7c0: port[0x00] = c; return;
        case 0xe7c1: port[0x01] = c; mute = c & 8; return;
        case 0xe7c3: port[0x03] = (c & 0x3d); if((c & 0x20) == 0) keyb_irqcount = 0;
        selectVideoRam(); selectRomBank(); return;
        case 0xe7c5: port[0x05] = c; Timercontrol(); return; //controle timer
        case 0xe7c6: latch6846 = (latch6846 & 0xff) | ((c & 0xff) << 8); return;
        case 0xe7c7: latch6846 = (latch6846 & 0xff00) | (c & 0xff); return;
        case 0xe7c9: port[0x09] = c; selectRamBankTo(); return;
        // Extension musique et jeux (Motorola 6821)
        //e7cc= registre de direction ou de donnees port A (6821 systeme)
        //e7cd= registre de direction ou de donnees port B
        //e7ce= registre de controle port A (CRA)
        //e7cf= registre de controle port B (CRB)
        case 0xe7cc: port[0x0c] = c; return;
        case 0xe7cd: if(port[0x0f] & 4) sound = c & MAX_SOUND_LEVEL; else port[0x0d] = c; return;
        case 0xe7ce: port[0x0e] = c; return; //registre controle position joysticks
        case 0xe7cf: port[0x0f] = c; return; //registre controle action - musique
        case 0xe7d0: port[0x10] = c; return; //save the value written to know if an
                                             //intelligent function of the floppy controller is used
        case 0xe7d8: return;
        case 0xe7da: Palettecolor(c); return;
        case 0xe7db: port[0x1b] = c; return;
        case 0xe7dc: selectVideomode(c); return;
        case 0xe7dd: videopage_bordercolor(c); return;
        case 0xe7e4: port[0x24] = c; return;
        case 0xe7e5: port[0x25] = c; selectRamBankTo(); return;
        case 0xe7e6: port[0x26] = c; selectRomBank(); return;
        case 0xe7e7: port[0x27] = c; selectRamBankTo(); return;
        default: return;
      }
      return;
    default: return;
  }
}

// Basic emulation of the floppy controller to workaround some game protections.
static char floppy_controller_emu(unsigned short a)
{
  // FIL protection (Atomik, Avenger, Way of the Tiger):
  // This protection makes a direct access to the floppy controller to read out-of-bound data
  // on a track/sector and compares with the expected value.
  // - read at e7d0 must return 0x82 (operation pending)
  // - read at e7d1 must return 0x02 (floppy ready)
  // - read at e7d3 must return the value expected by the game to work around the protection.
  // Example of code:
  // ADDR OPCODE   MNEMONIC      REGISTERS
  // 8D71 E603     LDB  $03,X    A=03 B=82 X=E7D0 Y=74FF, CC=D8 <= Load content at 0xe7d3 in register B
  // 8D73 C1FB     CMPB #$FB     A=03 B=00 X=E7D0 Y=74FF, CC=D4 <= Compare with 0xFB
  // 8D75 26F3     BNE  $8D6A    A=03 B=00 X=E7D0 Y=74FF, CC=D1 <= If not equal, infinite loop
  // The workaround checks if the code executed is "E603C1" (LDB $03,X followed by CMPB ??)
  // and then returns the byte just after, which is the value expected by the game.
  //
  // Infogrames protection (Dieux du stade 2, Dossier Boerhaave, Le temps d'une histoire):
  // - read at e7d0 must return 0x80 (intelligent function of the disk controller not used)
  // - read at e7d1 must return 0x03 (floppy ready)
  //                          + 0x08 (floppy track 0 detection)
  //                          + 0x40 (floppy index detected)
  // - read at e7d3 : same kind of protection than the "FIL" protection, but uses
  // the A register instead of the B register.
  switch (a)
  {
    case 0xe7d0:
      // Opération intelligente en cours
      if ((port[a & 0x3f] & 0x03) != 0)
        return 0x82; // 10000010 : bit 7 = demande d'opération,
                     //            bit 1 = identique bit 7 pour opérations intelligentes
      // Pas d'opération intelligente en cours
      return 0x80;   // 10000000 : bit 7 = demande d'opération
    case 0xe7d1: return 0x4a; // 01001010 : bit 6 = détection d'index pour le floppy
                              //            bit 3 = détection de la piste 0 pour le floppy
                              //            bit 1 = information "ready" du lecteur
    case 0xe7d3:
      // Detect sequence LDB $03,X / CMPB #$??
      if (((Mgetc(dc6809_pc) & 0xff) == 0xc1) && ((Mgetc(dc6809_pc-1) & 0xff) == 0x03)
          && ((Mgetc(dc6809_pc-2) & 0xff) == 0xe6)) return Mgetc(dc6809_pc+1);
      // Detect sequence LDA $03,X / CMPA #$??
      else if (((Mgetc(dc6809_pc) & 0xff) == 0x81) && ((Mgetc(dc6809_pc-1) & 0xff) == 0x03)
          && ((Mgetc(dc6809_pc-2) & 0xff) == 0xa6)) return Mgetc(dc6809_pc+1);
      else return port[a & 0x3f];
    default: return port[a & 0x3f];
  }
}

// TO8/TO9 memory read //////////////////////////////////////////////////////
static char MgetTo(unsigned short a)
{
#ifdef THEODORE_DASM
  debug_mem_read(a);
#endif
  switch(a >> 12)
  {
    //subtilite : quand la rom est recouverte par la ram, les 2 segments de 8 Ko sont inverses
    case 0x0: case 0x1: return (port[0x26] & 0x20) ? rombank[a + 0x2000] : rombank[a];
    case 0x2: case 0x3: return (port[0x26] & 0x20) ? rombank[a - 0x2000] : rombank[a];
    case 0x4: case 0x5: return ramvideo[a];
    case 0x6: case 0x7: case 0x8: case 0x9: return ramuser[a];
    case 0xa: case 0xb: case 0xc: case 0xd: return rambank[a];
    case 0xe:
      switch(a)
      {
        //e7c0 = 6846 composite status register
        //csr0 = timer interrupt flag
        //csr1 = cp1 interrupt flag (keyboard)
        //csr2 = cp2 interrupt flag
        //csr3-csr6 unused and set to zeroes
        //csr7 = composite interrupt flag (if at least one interrupt flag is set)
        case 0xe7c0: return((port[0]) ? (port[0] | 0x80) : 0);
        case 0xe7c3: return(port[0x03] | 0x80 | (penbutton << 1));
        case 0xe7c6: return (timer6846 >> 11 & 0xff);
        case 0xe7c7: return (timer6846 >> 3 & 0xff);
        case 0xe7ca: return (videolinenumber < 200) ? 0 : 2; //non, registre de controle PIA
        // Extension musique et jeux (Motorola 6821)
        //e7cc= registre de direction ou de donnees port A (6821 systeme)
        //e7cd= registre de direction ou de donnees port B
        //e7ce= registre de controle port A (CRA)
        //e7cf= registre de controle port B (CRB)
        case 0xe7cc: return((port[0x0e] & 4) ? joysposition : port[0x0c]);
        case 0xe7cd: return((port[0x0f] & 4) ? joysaction | sound : port[0x0d]);
        case 0xe7ce: return 0x04;
        case 0xe7da: return x7da[port[0x1b]++ & 0x1f];
        case 0xe7df: port[0x1e] = 0; return(port[0x1f]);
        case 0xe7e4: return port[0x1d] & 0xf0;
        case 0xe7e5: return port[0x25] & 0x1f;
        case 0xe7e6: return port[0x26] & 0x7f;
        case 0xe7e7: return (port[0x24] & 0x01) | Initn() | Iniln();
        default:
          if (a >= 0xe7d0 && a <= 0xe7d3) return floppy_controller_emu(a);
          if (a < 0xe7c0) return romsys[a];
          if (a < 0xe800) return port[a & 0x3f];
      }
      return romsys[a];
    default: return romsys[a];
  }
}

// TO7-TO7/70 memory write /////////////////////////////////////////////////////
static void MputTo7(unsigned short a, char c)
{
#ifdef THEODORE_DASM
  debug_mem_write(a);
#endif
  switch(a >> 12)
  {
    // 0000->3fff: Cartouche enfichable MEMO7
    case 0x0: case 0x1:
      carflags = (carflags & 0xfc) | (a & 3);
      selectRomBank();
      return;
    case 0x2: case 0x3: if((port[0x26] & 0x60) != 0x60) return;
      if(port[0x26] & 0x20) rombank[a - 0x2000] = c; else rombank[a] = c; return;
    // 4000->5fff: Memoire Ecran
    case 0x4: case 0x5: ramvideo[a] = c; return;
    // 6000->dfff: Memoire
    case 0x6: case 0x7: case 0x8: case 0x9: ramuser[a] = c; return;
    case 0xa: case 0xb: case 0xc: case 0xd:
      if (currentModel == TO7) ramuser[a] = c; else rambank[a] = c; return;
    case 0xe:
      switch(a)
      {
        // e000->e7bf: Moniteur controlleur disque (e000->e7af, libre au dela)
        // e7c0->e7c7: PIA 6846
        // e7c0: Composite Status Register
        // e7c1: Control Register Port C
        // e7c2: Direction Register Port C
        // e7c3: Data Register Port C (b7=LEP, b6-4=border color, b3=keyboard led,
        //                             b1=lightpen button, b0=RAM forme/fond)
        // e7c5: Timer Control Register
        // e7c6: Timer MSB
        // e7c7: Timer LSB
        case 0xe7c0: port[0x00] = c; return;
        case 0xe7c1: port[0x01] = c; mute = c & 8; return;
        case 0xe7c3: port[0x03] = (c & 0x7d);
        selectVideoRam(); selectRomBank(); return;
        case 0xe7c5: port[0x05] = c; Timercontrol(); return; //controle timer
        case 0xe7c6: latch6846 = (latch6846 & 0xff) | ((c & 0xff) << 8); return;
        case 0xe7c7: latch6846 = (latch6846 & 0xff00) | (c & 0xff); return;
        // e7c8->e7cb: PIA 6821
        // e7c8: Data Register Port A (input keyboard matrix)
        // e7c9: TO7: Data Register Port B (output keyboard matrix)
        //       TO7/70: RAM banks and coded output keyboard matrix
        // e7ca: Control Register Port A (CA1 (input): read  INITRAME,
        //                                CA2 (output): command tape drive)
        // e7cb: Control Register Port B
        // (CB1 (output): OUTPUT_ENABLE command (TO7) or compositing command (TO7/70))
        case 0xe7c9: port[0x09] = c; if (currentModel == TO7_70) selectRamBankTo(); return;
        // e7cc->e7cf: Extension musique et jeux (Motorola 6821)
        //e7cc: registre de direction ou de donnees port A
        //e7cd: registre de direction ou de donnees port B
        //e7ce: registre de controle port A (CRA)
        //e7cf: registre de controle port B (CRB)
        case 0xe7cc: port[0x0c] = c; return;
        case 0xe7cd: if(port[0x0f] & 4) sound = c & MAX_SOUND_LEVEL; else port[0x0d] = c; return;
        case 0xe7ce: port[0x0e] = c; return; //registre controle position joysticks
        case 0xe7cf: port[0x0f] = c; return; //registre controle action - musique
        // e7d0->e7df: Controlleur disque
        // e7e0->e7e3: PIA 6821 (RS232/Parellel interfaces)
        // e7e4->e7ff: Libre pour extension PIA et ACIA
        default: return;
      }
      return;
    default: return;
  }
}

/*
 * Read Port A (8 columns of the keyboard matrix) of the 6821 System PIA.
 * The TO7 keyboard is a 8x8 matrix:
 * - 8 lines connected to the Port B of the 6821 System PIA
 * - 8 columns connected to the Port A of the 6821 System PIA
 * Port A is configured as input, with pull-up resistances.
 * Port B is configured as output.
 * The TO7 set successively the outputs of the port B to 0V (scanning)
 * when port A is read. Each bit of A represents one of the column and
 * is set to 0 if the corresponding key is pressed.
 */
static char readTo7KeybPortA()
{
  int keyb_matrix_line, keyb_matrix_column, matrix_offset, porta_inv;
  char result;
  if (currentModel == TO7)
  {
    int portb_inv= (~port[0x09] & 0xff);
    keyb_matrix_line = 7;
    while (portb_inv != 1)
    {
      portb_inv >>= 1;
      keyb_matrix_line--;
    }
  }
  else
  {
    // TO7/70: only bits PB0,PB1,PB2 are used to code the line number (0->7)
    keyb_matrix_line = port[0x09] & 0x07;
  }
  matrix_offset = keyb_matrix_line * 8;
  porta_inv = 0;
  // Check if each key on the current line is pressed or not
  for (keyb_matrix_column = 0; keyb_matrix_column < 8; keyb_matrix_column++)
  {
    if (touche[matrix_offset + keyb_matrix_column] == 0)
    {
      porta_inv |= (1 << keyb_matrix_column);
    }
  }
  result = (char)(~porta_inv);
  return result;
}

// TO7-TO7/70 memory read //////////////////////////////////////////////////////
static char MgetTo7(unsigned short a)
{
#ifdef THEODORE_DASM
  debug_mem_read(a);
#endif
  switch(a >> 12)
  {
    // 0000->3fff: Cartouche enfichable MEMO7
    case 0x0: case 0x1: case 0x2: case 0x3: return rombank[a];
    // 4000->5fff: Memoire Ecran
    case 0x4: case 0x5: return ramvideo[a];
    // 6000->dfff: Memoire
    //   6000->60ff: page 0 reservee au systeme,
    //   TO7:
    //     6100->7fff: memoire utilisateur,
    //     8000->bfff: extension memoire,
    //     c000->dfff: RAM non utilisée
    //   TO7/70:
    //     6100->9fff: memoire utilisateur
    //     a000->dfff: extension memoire paginable
    case 0x6: case 0x7: case 0x8: case 0x9: return ramuser[a];
    case 0xa: case 0xb: case 0xc: case 0xd: return currentModel == TO7 ? ramuser[a] : rambank[a];
    case 0xe:
      switch(a)
      {
        // e000->e7bf: Moniteur controlleur disque (e000->e7af, libre au dela)
        // e7c0->e7c7: PIA 6846
        // e7c0: Composite Status Register
        // e7c1: Control Register Port C
        // e7c2: Direction Register Port C
        // e7c3: Data Register Port C (b7=LEP, b6-4=border color, b3=keyboard led,
        //                             b1=lightpen button, b0=RAM forme/fond)
        // e7c5: Timer Control Register
        // e7c6: Timer MSB
        // e7c7: Timer LSB
        case 0xe7c0: return((port[0]) ? (port[0] | 0x80) : 0);
        case 0xe7c3: return(port[0x03] | 0x80 | (penbutton << 1));
        case 0xe7c6: return (timer6846 >> 11 & 0xff);
        case 0xe7c7: return (timer6846 >> 3 & 0xff);
        // e7c8->e7cb: PIA 6821
        // e7c8: Data Register Port A (input keyboard matrix)
        // e7c9: Data Register Port B (output keyboard matrix)
        // e7ca: Control Register Port A (CA1 (input): read  INITRAME,
        //                                CA2 (output): command tape drive)
        // e7cb: Control Register Port B
        // (CB1 (output): OUTPUT_ENABLE command (TO7) or compositing command (TO7/70))
        case 0xe7c8: return readTo7KeybPortA();
        case 0xe7ca: return (videolinenumber < 200) ? 0 : 2; //non, registre de controle PIA
        // e7cc->e7cf: Extension musique et jeux (Motorola 6821)
        //e7cc: registre de direction ou de donnees port A (lecture direction joysticks)
        //e7cd: registre de direction ou de donnees port B
        //      PB0-5 (input): CNA sur 6 bits
        //      PB6/7 (input): Bouton Action joysticks 0/1
        //e7ce: registre de controle port A (CRA)
        //e7cf: registre de controle port B (CRB)
        case 0xe7cc: return((port[0x0e] & 4) ? joysposition : port[0x0c]);
        case 0xe7cd: return((port[0x0f] & 4) ? joysaction | sound : port[0x0d]);
        case 0xe7ce: return 0x04;
        // e7e0->e7e3: PIA 6821 (RS232/Parellel interfaces)
        // e7e4->e7ff: Libre pour extension PIA et ACIA
        default:
          if (a < 0xe7c0) return 0;
          if (a < 0xe800) return port[a & 0x3f];
      }
      return romsys[a];
    default: return romsys[a];
  }
}

// MO5/MO6 memory write ///////////////////////////////////////////////////////////
static void MputMo(unsigned short a, char c)
{
#ifdef THEODORE_DASM
  debug_mem_write(a);
#endif
  switch(a >> 12)
  {
    case 0x0: case 0x1: ramvideo[a] = c; return;
    case 0x2: case 0x3: case 0x4: case 0x5: ramuser[a] = c; return;
    case 0x6: case 0x7: case 0x8: case 0x9:
      if (rom->is_mo6) rambank[a] = c; else ramuser[a] = c; return;
    case 0xa:
      switch(a)
      {
        // A7C0->A7C3 : PIA 6821 Systeme
        case 0xa7c0: if (currentModel == MO5) { port[0] = c & 0x5f; selectVideoRam(); }
                     else { port[0] = c & 0x39; selectVideoRam(); selectRomBank(); } return;
        case 0xa7c1: port[1] = c & 0x7f; sound = (c & 1) << 5; return;
        case 0xa7c2: port[2] = c & 0x3f; return;
        case 0xa7c3: port[3] = c & 0x3f; return;
        // A7CB is used by the Jane cartridge and the 64k RAM extension
        case 0xa7cb: carflags = c; selectRomBank(); break;
        // A7CC->A7CF : Music and Game Extension
        case 0xa7cc: port[0x0c] = c; return;
        case 0xa7cd: port[0x0d] = c; sound = c & MAX_SOUND_LEVEL; return;
        case 0xa7ce: port[0x0e] = c; return; //registre controle position joysticks
        case 0xa7cf: port[0x0f] = c; return; //registre controle action - musique
        // A7DA->A7DB : Gate Palette Registers
        case 0xa7da: if (rom->is_mo6) { Palettecolor(c); } return;
        case 0xa7db: if (rom->is_mo6) { port[0x1b] = c; } return;
        // A7DC->A7DD : Gate Mode Page Registers
        case 0xa7dc: if (rom->is_mo6) { selectVideomode(c); } return;
        case 0xa7dd: if (rom->is_mo6) { videopage_bordercolor(c); carflags = (carflags & ~0x04) | ((~c & 0x20) >> 3); selectRomBankMo6(); } return;
        // A7E4->A7E7 : Gate Mode Page Registers
        case 0xa7e4: if (rom->is_mo6) { port[0x24] = c & 0x01; } return;
        case 0xa7e5: if (rom->is_mo6) { port[0x25] = c; selectRamBankMo6(); } return;
      }
      return;
    case 0xb: case 0xc: case 0xd: case 0xe:
      if ((carflags & 8) && (cartype == 0)) rombank[a] = c;
      return;
    case 0xf: return;
    default: ramuser[a] = c;
 }
}

static char mo6keybPB7()
{
  int line, col, scancode;
  // column = PB4-PB6 (0-7)
  col = (port[1] & 0x70) >> 4;
  // line = PA3,PB1-PB3 (0-8)
  line = (port[0] & 0x08) | ((port[1] & 0x0e) >> 1);
  // I don't know why but Saphir (and maybe other games) always set PA3 to 1 on MO6
  // whereas it should be set to 0. This gives either illegal or wrong scancodes.
  // This is just a dirty workaround that don't even work for some keys, but
  // I don't know how to manage that cleanly.
  if ((line > 8) // line cannot be > 8
    || ((line == 8) && (col > 4))) // line 8 only have 5 keys
  {
    line &= 0x07;
  }
  scancode = (line << 3) | col;
  return touche[scancode];
}

// MO5/MO6 memory read ////////////////////////////////////////////////////////////
static char MgetMo(unsigned short a)
{
#ifdef THEODORE_DASM
  debug_mem_read(a);
#endif
  switch(a >> 12)
  {
    case 0x0: case 0x1: return ramvideo[a];
    case 0x2: case 0x3: case 0x4: case 0x5: return ramuser[a];
    case 0x6: case 0x7: case 0x8: case 0x9:
          if (rom->is_mo6) return rambank[a]; else return ramuser[a];
    case 0xa:
      switch(a)
      {
        // A7C0->A7C3 : PIA 6821 Systeme
        case 0xa7c0: return (currentModel == MO5) ? port[0] | 0x80 | (penbutton << 5) : port[0] | 0x80 | (penbutton << 1);
        case 0xa7c1: return (currentModel == MO5) ? port[1] | touche[(port[1] & 0xfe) >> 1]
                     : port[1] | mo6keybPB7();
        case 0xa7c2: return port[2];
        case 0xa7c3: return port[3] | ~Initn();
        // A7CB is used by the Jane cartridge and the 64k RAM extension
        case 0xa7cb: return (carflags&0x3f)|((carflags&0x80)>>1)|((carflags&0x40)<<1);
        // A7CC->A7CF : Music and Game Extension
        case 0xa7cc: return((port[0x0e] & 4) ? joysposition : port[0x0c]);
        case 0xa7cd: return((port[0x0f] & 4) ? joysaction | sound : port[0x0d]);
        case 0xa7ce: return 4;
        // A7DA->A7DB : Gate Palette Registers
        case 0xa7da: return (rom->is_mo6) ? x7da[port[0x1b]++ & 0x1f] : port[a & 0x3f];
        case 0xa7d8: return ~Initn(); //octet etat disquette
        case 0xa7e1: return 0xff;     //zero provoque erreur 53 sur imprimante
        // A7E4->A7E7 : Gate Mode Page Registers
        case 0xa7e4: return (rom->is_mo6) ? port[0x1d] & 0xf0 : port[a & 0x3f];
        case 0xa7e6: return Iniln() << 1;
        case 0xa7e7: return (currentModel == MO5) ? Initn() : (port[0x24] & 0x01) | Initn() | Iniln();
        default: if(a < 0xa7c0) return(cd90_640_rom[a & 0x7ff]);
             if(a < 0xa800) return(port[a & 0x3f]);
             return(0);
      }
      return 0;
    case 0xb: SwitchMemo5Bank(a); return rombank[a];
    case 0xc: case 0xd: case 0xe: return rombank[a];
    case 0xf: return romsys[a];
    default: return ramuser[a];
 }
}

unsigned int toemulator_serialize_size(void)
{
  return sizeof(currentModel) + cpu_serialize_size() + video_serialize_size()
      + sizeof(ram) + sizeof(port) + sizeof(x7da) + device_serialize_size()
      + sizeof(reserved1) + sizeof(reserved2) + sizeof(reserved3) + sizeof(reserved4)
      + sizeof(carflags) + sizeof(touche) + sizeof(capslock) + sizeof(joysposition)
      + sizeof(joysaction) + sizeof(xpen) + sizeof(ypen) + sizeof(penbutton)
      + sizeof(videolinecycle) + sizeof(videolinenumber) + sizeof(vblnumber)
      + sizeof(displayflag) + sizeof(bordercolor) + sizeof(sound) + sizeof(mute)
      + sizeof(timer6846) + sizeof(latch6846) + sizeof(keyb_irqcount) + sizeof(timer_irqcount);
}

void toemulator_serialize(void *data)
{
  int offset = 0;
  char *buffer = (char *) data;

  memcpy(buffer+offset, &currentModel, sizeof(currentModel));
  offset += sizeof(currentModel);

  cpu_serialize(buffer+offset);
  offset += cpu_serialize_size();
  video_serialize(buffer+offset);
  offset += video_serialize_size();

  memcpy(buffer+offset, ram, sizeof(ram));
  offset += sizeof(ram);
  memcpy(buffer+offset, port, sizeof(port));
  offset += sizeof(port);
  memcpy(buffer+offset, x7da, sizeof(x7da));
  offset += sizeof(x7da);

  device_serialize(buffer+offset);
  offset += device_serialize_size();

  memcpy(buffer+offset, &reserved1, sizeof(reserved1));
  offset += sizeof(reserved1);
  memcpy(buffer+offset, &reserved2, sizeof(reserved2));
  offset += sizeof(reserved2);
  memcpy(buffer+offset, &reserved3, sizeof(reserved3));
  offset += sizeof(reserved3);
  memcpy(buffer+offset, &reserved4, sizeof(reserved4));
  offset += sizeof(reserved4);
  memcpy(buffer+offset, &carflags, sizeof(carflags));
  offset += sizeof(carflags);
  memcpy(buffer+offset, touche, sizeof(touche));
  offset += sizeof(touche);
  memcpy(buffer+offset, &capslock, sizeof(capslock));
  offset += sizeof(capslock);
  memcpy(buffer+offset, &joysposition, sizeof(joysposition));
  offset += sizeof(joysposition);
  memcpy(buffer+offset, &joysaction, sizeof(joysaction));
  offset += sizeof(joysaction);
  memcpy(buffer+offset, &xpen, sizeof(xpen));
  offset += sizeof(xpen);
  memcpy(buffer+offset, &ypen, sizeof(ypen));
  offset += sizeof(ypen);
  memcpy(buffer+offset, &penbutton, sizeof(penbutton));
  offset += sizeof(penbutton);
  memcpy(buffer+offset, &videolinecycle, sizeof(videolinecycle));
  offset += sizeof(videolinecycle);
  memcpy(buffer+offset, &videolinenumber, sizeof(videolinenumber));
  offset += sizeof(videolinenumber);
  memcpy(buffer+offset, &vblnumber, sizeof(vblnumber));
  offset += sizeof(vblnumber);
  memcpy(buffer+offset, &displayflag, sizeof(displayflag));
  offset += sizeof(displayflag);
  memcpy(buffer+offset, &bordercolor, sizeof(bordercolor));
  offset += sizeof(bordercolor);
  memcpy(buffer+offset, &sound, sizeof(sound));
  offset += sizeof(sound);
  memcpy(buffer+offset, &mute, sizeof(mute));
  offset += sizeof(mute);
  memcpy(buffer+offset, &timer6846, sizeof(timer6846));
  offset += sizeof(timer6846);
  memcpy(buffer+offset, &latch6846, sizeof(latch6846));
  offset += sizeof(latch6846);
  memcpy(buffer+offset, &keyb_irqcount, sizeof(keyb_irqcount));
  offset += sizeof(keyb_irqcount);
  memcpy(buffer+offset, &timer_irqcount, sizeof(timer_irqcount));
}

void toemulator_unserialize(const void *data)
{
  int offset = 0;
  const char *buffer = (const char *) data;
  ThomsonModel model;

  memcpy(&model, buffer+offset, sizeof(model));
  offset += sizeof(model);
  SetThomsonModel(model);

  cpu_unserialize(buffer+offset);
  offset += cpu_serialize_size();
  video_unserialize(buffer+offset);
  offset += video_serialize_size();
  memcpy(ram, buffer+offset, sizeof(ram));
  offset += sizeof(ram);
  memcpy(port, buffer+offset, sizeof(port));
  offset += sizeof(port);
  memcpy(x7da, buffer+offset, sizeof(x7da));
  offset += sizeof(x7da);

  device_unserialize(buffer+offset);
  offset += device_serialize_size();

  memcpy(&reserved1, buffer+offset, sizeof(reserved1));
  offset += sizeof(reserved1);
  memcpy(&reserved2, buffer+offset, sizeof(reserved2));
  offset += sizeof(reserved2);
  memcpy(&reserved3, buffer+offset, sizeof(reserved3));
  offset += sizeof(reserved3);
  memcpy(&reserved4, buffer+offset, sizeof(reserved4));
  offset += sizeof(reserved4);
  memcpy(&carflags, buffer+offset, sizeof(carflags));
  offset += sizeof(carflags);
  memcpy(touche, buffer+offset, sizeof(touche));
  offset += sizeof(touche);
  memcpy(&capslock, buffer+offset, sizeof(capslock));
  offset += sizeof(capslock);
  memcpy(&joysposition, buffer+offset, sizeof(joysposition));
  offset += sizeof(joysposition);
  memcpy(&joysaction, buffer+offset, sizeof(joysaction));
  offset += sizeof(joysaction);
  memcpy(&xpen, buffer+offset, sizeof(xpen));
  offset += sizeof(xpen);
  memcpy(&ypen, buffer+offset, sizeof(ypen));
  offset += sizeof(ypen);
  memcpy(&penbutton, buffer+offset, sizeof(penbutton));
  offset += sizeof(penbutton);
  memcpy(&videolinecycle, buffer+offset, sizeof(videolinecycle));
  offset += sizeof(videolinecycle);
  memcpy(&videolinenumber, buffer+offset, sizeof(videolinenumber));
  offset += sizeof(videolinenumber);
  memcpy(&vblnumber, buffer+offset, sizeof(vblnumber));
  offset += sizeof(vblnumber);
  memcpy(&displayflag, buffer+offset, sizeof(displayflag));
  offset += sizeof(displayflag);
  memcpy(&bordercolor, buffer+offset, sizeof(bordercolor));
  offset += sizeof(bordercolor);
  memcpy(&sound, buffer+offset, sizeof(sound));
  offset += sizeof(sound);
  memcpy(&mute, buffer+offset, sizeof(mute));
  offset += sizeof(mute);
  memcpy(&timer6846, buffer+offset, sizeof(timer6846));
  offset += sizeof(timer6846);
  memcpy(&latch6846, buffer+offset, sizeof(latch6846));
  offset += sizeof(latch6846);
  memcpy(&keyb_irqcount, buffer+offset, sizeof(keyb_irqcount));
  offset += sizeof(keyb_irqcount);
  memcpy(&timer_irqcount, buffer+offset, sizeof(timer_irqcount));

  if (currentModel != MO5)
  {
    videopage_bordercolor(port[0x1d]);
    if (rom->is_mo6) selectRamBankMo6(); else selectRamBankTo();
  }
  selectVideoRam();
  selectRomBank();
}
