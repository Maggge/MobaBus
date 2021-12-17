/**
  MobaBus Module

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

#ifndef __MOBA_BUS_MODULE__
#define __MOBA_BUS_MODULE__

#include <Arduino.h>
#include <EEPROM.h>

#include "MobaBus_Protocol.h"

#define MODULE_MAX_CONFIG_SIZE 16

class MobaBus;

class MobaBus_Module{

public:

    uint8_t moduleID;

    uint16_t address;

    MobaDevType type;

    MobaBus *controller;

    bool useConfigStorage;

    uint8_t channels;

    bool intitialized = false;

    /**
     * @return channels used
     */
    virtual uint8_t begin(bool useEEPROM, uint16_t address);
    virtual void loop();

    virtual void loadConf();
    virtual void storeConf();

    virtual void processPkg(MobaBus_Packet *pkg);

    /**
        * Addresses the Module
        * @param first address of the module
        * @return the amount of used addresses
        */
    virtual uint8_t programmAddress(uint16_t addr);
};



#endif