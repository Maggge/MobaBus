/**
  MobaBus Standard Inputs/Outputs

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

#include "MobaBus_Standard_IO.h"

MobaBus_Sensor::MobaBus_Sensor(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7, uint8_t pin8, bool pullup){
    pullUp = pullup;
    pin[0] = pin1;
    pin[1] = pin2;
    pin[2] = pin3;
    pin[3] = pin4;
    pin[4] = pin5;
    pin[5] = pin6;
    pin[6] = pin7;
    pin[7] = pin8;
    type = FEEDBACK;
}

uint8_t MobaBus_Sensor::begin(bool useEEPROM, uint16_t address){
    Serial.println("Module begin");
    programmAddress(address);

    for (int i = 0; i < 8; i++){
        pinMode(pin[i], pullUp ? INPUT_PULLUP : INPUT);
    }
    intitialized = true;
    return 8;
}

void MobaBus_Sensor::loop(){
    bool newState;
    for(int i = 0; i < 8; i++){
        if(debounce[i] < millis()){
            newState = digitalRead(pin[i]) ? !pullUp : pullUp;
            if(newState != state[i]){
                debounce[i] = millis() + 80;
                state[i] = newState;
                sendStates();
            }
        }
        
    }
}

void MobaBus_Sensor::sendStates(){
    uint8_t states;
    for (int i = 0; i < 8; i++){
        states = states | (state[i] << i);
    }
    controller->sendPkg(FEEDBACK, address, SET, 1, &states);
}

void MobaBus_Sensor::loadConf(){
    if(controller == NULL) return;
    
    uint16_t configAddr = controller->getEEPROMAddress(moduleID);
    uint16_t storedAddress;
    configAddr += sizeof(EEPROM.get(configAddr, storedAddress));
    
    if(address != storedAddress) return;
}

void MobaBus_Sensor::storeConf(){
    if(controller == NULL) return;
    
    uint16_t configAddr = controller->getEEPROMAddress(moduleID);

    configAddr += sizeof(EEPROM.put(configAddr, address));

}

void MobaBus_Sensor::processPkg(MobaBus_Packet *pkg){
    if(!intitialized) return;
    if(pkg->meta.type == type && pkg->meta.address == address && pkg->meta.cmd == GET){
        sendStates();
    }
}

uint8_t MobaBus_Sensor::programmAddress(uint16_t addr){
    address = addr;
    Serial.print("Set Module Address ");
    Serial.print(address);
    return 8;
}

//----------------------   MobaBus_Turnout   -------------------------------------

MobaBus_Output::MobaBus_Output(uint8_t pin, bool inverted){
    this->pin = pin;
    this->inverted = inverted;
    type = ACCESSORIE;
}

uint8_t MobaBus_Output::begin(bool useEEPROM, uint16_t address){
    Serial.println("Module begin");
    pinMode(this->pin, OUTPUT);
    digitalWrite(this->pin, inverted);
    programmAddress(address);
    intitialized = true;
    return 1;
}

void MobaBus_Output::loop() {
    return;
}

void MobaBus_Output::loadConf() {
    return;
}

void MobaBus_Output::storeConf(){
    return;
}

void MobaBus_Output::processPkg(MobaBus_Packet *pkg){
    if(pkg->meta.type == type && pkg->meta.address == address){
        if(pkg->meta.cmd == SET){
            setTurnout((bool)pkg->data[0]);
        }
        else if(pkg->meta.cmd == GET){
            uint8_t data[] = {state};
            controller->sendPkg(ACCESSORIE, address, INFO, 1, data);
        }
    }
}

uint8_t MobaBus_Output::programmAddress(uint16_t addr){
    address = addr;
    Serial.print("Set Address ");
    Serial.println(address);
    return 1;
}

void MobaBus_Output::setTurnout(bool dir){
    digitalWrite(pin, inverted ? !dir : dir);
    state = dir;
    uint8_t data[] = {dir};
    controller->sendPkg(ACCESSORIE, address, INFO, 1, data);
}

//----------------------   MobaBus_Turnout_2pin   --------------------------------

MobaBus_Output_2pin::MobaBus_Output_2pin(uint8_t pin1, uint8_t pin2, bool inverted, bool autoPowerOff){
    pin[0] = pin1;
    pin[1] = pin2;
    this->inverted = inverted;
    this->autoPowerOff = autoPowerOff;
    type = ACCESSORIE;
}

uint8_t MobaBus_Output_2pin::begin(bool useEEPROM, uint16_t address){
    Serial.println("Module begin");
    pinMode(this->pin[0], OUTPUT);
    pinMode(this->pin[1], OUTPUT);
    digitalWrite(this->pin[0], inverted);
    digitalWrite(this->pin[0], inverted);
    programmAddress(address);
    intitialized = true;
    return 1;
}

void MobaBus_Output_2pin::loop(){
    if(autoPowerOff && active && powerOffT <= millis()){
        setTurnout(state, false);
    }
}

void MobaBus_Output_2pin::loadConf() {
    return;
}

void MobaBus_Output_2pin::storeConf() {
    return;
}


void MobaBus_Output_2pin::processPkg(MobaBus_Packet *pkg){
    if(pkg->meta.type == type && pkg->meta.address == address){
        if(pkg->meta.cmd == SET){
            setTurnout((bool)pkg->data[0], (bool)pkg->data[1]);
        }
        else if(pkg->meta.cmd == GET){
            uint8_t data[] = {state};
            controller->sendPkg(ACCESSORIE, address, INFO, 1, data);
        }
    }
}

uint8_t MobaBus_Output_2pin::programmAddress(uint16_t addr){
    address = addr;
    Serial.print("Set Address ");
    Serial.println(address);
    return 1;
}

void MobaBus_Output_2pin::setTurnout(bool dir, bool power){
    if(power){
        bool p = dir ? inverted : !inverted;
        digitalWrite(pin[0], p);
        digitalWrite(pin[1], !p);
        state = dir;
        active = true;
        powerOffT = millis() + AUTO_POWER_OFF;
        uint8_t data[] = {dir};
        controller->sendPkg(ACCESSORIE, address, INFO, 1, data);
    }
    else{
        digitalWrite(pin[0], inverted);
        digitalWrite(pin[1], inverted);
        active = false;
    }
}
