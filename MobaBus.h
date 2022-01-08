/**
  MobaBus

  © 2021, Markus Mair. All rights reserved.

  This file is part of the MobaBus Project

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef __MobaBus__
#define __MobaBus__

#include <Arduino.h>

#include <EEPROM.h>	

#include "MobaBus_Protocol.h"

#include "MobaBus_Interface.h"
#include "MobaBus_CAN.h"

#include "MobaBus_Module.h"

#define MAX_BUS_INTERFACES 3

#define MAX_MODULES 10


class MobaBus{
private:
    MobaBus_Interface *bus[MAX_BUS_INTERFACES];
    bool routing;

    MobaBus_Module *modules[MAX_MODULES];


    bool useEEPROM = false;
    uint16_t eepromStart = 0;

    uint8_t progPin;
    uint8_t led;

    //Return the milliseconds off pressing
    uint32_t lastChange = 0;
    bool pressed = false;
    uint32_t progButton();

    uint16_t accessorieAddr = 0;
    uint16_t feedbackAddr = 0;
    uint16_t boosterAddr = 0;
    
public:
    /**
     * Instantiate MobaBus without EEPROM storage (Modules not stores settings and addresses!);
     */
    MobaBus(void);

    /**
     * Instantiate MobaBus with EEPROM storage (Modules stores settings and addresses);
     * @param EEPROMstart first address of storage
     * @param progPin for programming the modules
     * @param statusLed 
     */
    MobaBus(uint16_t EEPROMstart, uint8_t progPin = 8, uint8_t statusLed = 9);

    /**
     * Attach a Bus Interface
     * @return the id of the interface, return 255 if not successfull
     */
    uint8_t attachInterface(MobaBus_Interface *interface);

    /**
     * Detach a Bus Interface
     * @return true if successfull
     */
    bool dettachInterface(MobaBus_Interface *interface);
    bool dettachInterface(uint8_t id);

    /**
     * Attach a Module
     * @return the id of the module, return 255 if not successfull
     */
    uint8_t attachModule(MobaBus_Module *module);

    /**
     * Detach a Module
     * @return true if successfull
     */
    bool dettachModule(MobaBus_Module *module);
    bool dettachModule(uint8_t id);

    int16_t getEEPROMAddress(uint8_t moduleId);

    uint8_t sendPkg(MobaBus_Packet *pkg);
    uint8_t sendPkg(MobaDevType type, uint16_t	addr, MobaCmd cmd, uint8_t len, uint8_t *data, char skip = -1);
    uint8_t sendPkg(MobaDevType type, uint16_t	addr, uint8_t port, MobaCmd cmd, uint8_t len, uint8_t *data, char skip = -1);

    void processPkg(MobaBus_Packet *pkg);

    /**
     * Must called before attaching modules
     * you can set the Addresses here for first boot, if EEPROM used, or every boot if EEPROM not used!
     */
    void begin(uint16_t accessorieAddress = 0, uint16_t feedbackAddress = 0, uint16_t boosterAddress = 0);

    void loop();
    bool loop(MobaBus_Packet *pkg);

    bool receive(MobaBus_Packet *pkg);

    uint16_t programmMode(MobaDevType type);
    
};
#endif