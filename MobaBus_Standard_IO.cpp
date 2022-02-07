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

MobaBus_Sensor::MobaBus_Sensor(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7, uint8_t pin8, bool pullup) {
    init("MobaBus_Sensor", FEEDBACK, 100, 1);
    pullUp = pullup;
    pin[0] = pin1;
    pin[1] = pin2;
    pin[2] = pin3;
    pin[3] = pin4;
    pin[4] = pin5;
    pin[5] = pin6;
    pin[6] = pin7;
    pin[7] = pin8;
}

bool MobaBus_Sensor::begin(){
    for (int i = 0; i < 8; i++){
        pinMode(pin[i], pullUp ? INPUT_PULLUP : INPUT);
    }
    return true;
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
    controller->sendPkg(FEEDBACK, address(), SET, 1, &states);
}

int MobaBus_Sensor::loadConf(uint16_t eepromAddress){
    int idx = MobaBus_Module::loadConf(eepromAddress);
    if(idx < 0){
        return idx;
    }
    return idx;
}

int MobaBus_Sensor::storeConf(uint16_t eepromAddress){
    int idx = MobaBus_Module::storeConf(eepromAddress);
    if(idx < 0){
        return idx;
    }
    return idx;
}

void MobaBus_Sensor::processPkg(MobaBus_Packet *pkg){
    if(!initialized()) return;
    if(pkg->meta.type == getModuleType() && pkg->meta.address == address() && pkg->meta.cmd == GET){
        sendStates();
    }
}

//----------------------   MobaBus_Output   -------------------------------------

MobaBus_Output::MobaBus_Output(uint8_t pin, bool inverted) {
    init("MobaBus_Output", ACCESSORIE, 1, 1);
    this->pin = pin;
    this->inverted = inverted;
}

bool MobaBus_Output::begin(){
    pinMode(this->pin, OUTPUT);
    digitalWrite(this->pin, inverted);
    return true;
}

void MobaBus_Output::loop() {
    return;
}

int MobaBus_Output::loadConf(uint16_t eepromAddress){
    int idx = MobaBus_Module::loadConf(eepromAddress);
    if(idx < 0){
        return -2;
    }
    return idx;
}

int MobaBus_Output::storeConf(uint16_t eepromAddress){
    int idx = MobaBus_Module::storeConf(eepromAddress);
    if(idx < 0){
        return -2;
    }
    return idx;
}

void MobaBus_Output::processPkg(MobaBus_Packet *pkg){
    if(pkg->meta.cmd == SET){
        setTurnout((bool)pkg->data[0]);
    }
    else if(pkg->meta.cmd == GET){
        uint8_t data[] = {state};
        controller->sendPkg(ACCESSORIE, address(), INFO, 1, data);
    }
}

void MobaBus_Output::setTurnout(bool dir){
    digitalWrite(pin, inverted ? !dir : dir);
    state = dir;
    uint8_t data[] = {dir};
    controller->sendPkg(ACCESSORIE, address(), INFO, 1, data);
}

//----------------------   MobaBus_Turnout_2pin   --------------------------------

MobaBus_Output_2pin::MobaBus_Output_2pin(uint8_t pin1, uint8_t pin2, bool inverted, uint16_t autoPowerOff){
    init("MobaBus_Output_2pin", ACCESSORIE, 2, 1);
    pin[0] = pin1;
    pin[1] = pin2;
    this->inverted = inverted;
    this->autoPowerOff = autoPowerOff;
}

bool MobaBus_Output_2pin::begin(){
    pinMode(this->pin[0], OUTPUT);
    pinMode(this->pin[1], OUTPUT);
    digitalWrite(this->pin[0], inverted);
    digitalWrite(this->pin[1], inverted);
    return true;
}

void MobaBus_Output_2pin::loop(){
    if(autoPowerOff && active && powerOffT <= millis()){
        setTurnout(state, false);
    }
}

int MobaBus_Output_2pin::loadConf(uint16_t eepromAddress){
    int idx = MobaBus_Module::loadConf(eepromAddress);
    if(idx < 0){
        return -2;
    }
    return idx;
}

int MobaBus_Output_2pin::storeConf(uint16_t eepromAddress){
    int idx = MobaBus_Module::storeConf(eepromAddress);
    if(idx < 0){
        return -2;
    }
    return idx;
}


void MobaBus_Output_2pin::processPkg(MobaBus_Packet *pkg){
    if(!initialized()) return;
    if(pkg->meta.type == getModuleType() && pkg->meta.address == address()){
        if(pkg->meta.cmd == SET){
            setTurnout((bool)pkg->data[0], (bool)pkg->data[1]);
        }
        else if(pkg->meta.cmd == GET){
            uint8_t data[] = {state};
            controller->sendPkg(ACCESSORIE, address(), INFO, 1, data);
        }
    }
}

void MobaBus_Output_2pin::setTurnout(bool dir, bool power){
    if(power){
        bool p = dir ? inverted : !inverted;
        digitalWrite(pin[0], p);
        digitalWrite(pin[1], !p);
        state = dir;
        active = true;
        powerOffT = millis() + autoPowerOff;
        uint8_t data[] = {dir};
        controller->sendPkg(ACCESSORIE, address(), INFO, 1, data);
    }
    else{
        digitalWrite(pin[0], inverted);
        digitalWrite(pin[1], inverted);
        active = false;
    }
}
