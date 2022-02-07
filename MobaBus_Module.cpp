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

#include "MobaBus_Module.h"

void MobaBus_Module::init(String name, MobaDevType type, uint8_t typeID, uint8_t usedChannels){
  _moduleName = name;
  _moduleTypeID = typeID;
  _channels = usedChannels;
  _type = type;
}

String MobaBus_Module::moduleName(){
  return _moduleName;
}

uint8_t MobaBus_Module::usedChannels(){
  return _channels;
}

uint8_t MobaBus_Module::programmAddress(uint16_t address){
  Serial.print("Set address ");
  Serial.print(address);
  Serial.print("....");
  if(address == 0) {
    Serial.println("Failed!");
    return 0;
  }
  _address = address;
  _intitialized = true;
  Serial.println("OK!");
  return _channels;
}

MobaDevType MobaBus_Module::getModuleType(){
  return _type;
}

int MobaBus_Module::loadConf(uint16_t eepromAddress){
    if(EEPROM.read(eepromAddress) == _moduleTypeID){
        int addr;
        EEPROM.get(eepromAddress+1, addr);
        programmAddress(addr);
        return eepromAddress+3;
    }
    return -1;
}

int MobaBus_Module::storeConf(uint16_t eepromAddress){
    EEPROM.update(eepromAddress, _moduleTypeID);
    EEPROM.put(eepromAddress+1, _address);
    return eepromAddress+3;
}

uint16_t MobaBus_Module::address(){
    return _address;
}

bool MobaBus_Module::initialized(){
    return _intitialized;
}