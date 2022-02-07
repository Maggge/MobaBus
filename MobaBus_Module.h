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
private:
  String _moduleName;
  uint8_t _moduleTypeID;
  uint8_t _channels;
  MobaDevType _type;
  uint16_t _address;
  bool _intitialized = false;
public:

  uint8_t moduleID;

  MobaBus *controller;

  bool useConfigStorage;

  /**
   * @return channels used
   */
  virtual bool begin()=0;
  virtual void loop()=0;
  virtual void processPkg(MobaBus_Packet *pkg)=0;

  void init(String name, MobaDevType type, uint8_t typeID, uint8_t usedChannels);

 

  /**
   * @loadConf/storeConf
   * 
   * @param eepromAddress where to store/load the conf
   * @return next EEProm index, -1 on failure;
   */
  virtual int loadConf(uint16_t eepromAddress);
  virtual int storeConf(uint16_t eepromAddress);

  /**
      * Addresses the Module
      * @param first address of the module
      * @return the amount of used addresses
      */
  uint8_t programmAddress(uint16_t address);

  String moduleName();
  uint8_t usedChannels();
  MobaDevType getModuleType();
  uint16_t address();

  bool initialized();
};

#endif