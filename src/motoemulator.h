/*
 * This file is part of theodore (https://github.com/Zlika/theodore),
 * a Thomson emulator based on Daniel Coulom's DCTO8D emulator
 * (http://dcto8.free.fr/).
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

#ifndef __TOEMULATOR_H
#define __TOEMULATOR_H

#include <stdint.h>
#include "boolean.h"

// Size of RAM (512K)
#define RAM_SIZE 0x80000
// Size of cartridge memory space (4x16K)
#define CARTRIDGE_MEM_SIZE 0x10000
// Size of I/O ports space
#define IO_MEM_SIZE 0x40

// memory
//espace cartouche 4x16K
extern char car[];
//ram 512K
extern char ram[];
//ports d'entree/sortie (0xE7C0 -> 0xE7FF)
extern char port[];

//flags cartouche
//type de cartouche (0=simple 1=switch bank, 2=os-9)
extern int cartype;
//bits0,1,4=bank, 2=cart-enabled, 3=write-enabled
extern int carflags;

//lightpen coordinates
extern int xpen, ypen;
//lightpen button state
extern int penbutton;

//affichage
//compteur ligne (0-63)
extern int videolinecycle;
//numero de ligne video affichee (0-311)
extern int videolinenumber;
//couleur de la bordure de l'écran
extern int bordercolor;
//pointeur page video affichee
extern char *pagevideo;

typedef enum { JOY0_UP, JOY0_DOWN, JOY0_LEFT, JOY0_RIGHT,
               JOY1_UP, JOY1_DOWN, JOY1_LEFT, JOY1_RIGHT,
               JOY0_FIRE, JOY1_FIRE } JoystickAxis;

typedef enum { TO8, TO8D, TO9, TO9P, MO5, MO6, PC128, TO7, TO7_70 } ThomsonModel;

// Returns the current level of the speaker as a signed 16-bit integer
int16_t GetAudioSample(void);
// Joystick emulation
void Joysemul(JoystickAxis axis, bool isOn);
// Keyboard emulation
void keyboard(int scancode, bool down);
// Initialisation of the computer
void Initprog(void);
// Execution of n CPU cycles
int Run(int ncyclesmax);
// Hardreset of the computer
void Hardreset(void);
// Sets the Thomson model emulated (default=TO8)
void SetThomsonModel(ThomsonModel model);
// Gets the currently emulated Thomson model
ThomsonModel GetThomsonModel(void);

// The following functions are used for libretro's save states feature.
// Returns the amount of data required to serialize the whole state of the emulator.
unsigned int toemulator_serialize_size(void);
// Serializes the whole state of the emulator.
void toemulator_serialize(void *data);
// Unserializes the whole state of the emulator.
void toemulator_unserialize(const void *data);

#endif /* __TOEMULATION_H */
