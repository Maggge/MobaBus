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


#include "MobaBus.h"

MobaBus::MobaBus(){
    useEEPROM = false;
}

    
MobaBus::MobaBus(uint16_t EEPROMstart, uint8_t progPin, uint8_t statusLed){
    useEEPROM = true;
    eepromStart = EEPROMstart;
    this->progPin = progPin;
    led = statusLed;
}

uint8_t MobaBus::attachInterface(MobaBus_Interface *interface){
    for(uint8_t i = 0; i < MAX_BUS_INTERFACES; i++){
        if(bus[i] == interface){
            return i;
        }
    }

    for(uint8_t i = 0; i < MAX_BUS_INTERFACES; i++){
        if(bus[i] == NULL){
            bus[i] = interface;
            interface->begin();
            return i;
        }
    }
    return 255;
}


bool MobaBus::dettachInterface(MobaBus_Interface *interface){
    for(uint8_t i = 0; i < MAX_BUS_INTERFACES; i++){
        if(bus[i] == interface){
            bus[i] = NULL;
            return true;
        }
    }
    return false;
}

bool MobaBus::dettachInterface(uint8_t id){
    if(id >= MAX_BUS_INTERFACES) return false;
    bus[id] = NULL;
    return true;
}

uint8_t MobaBus::attachModule(MobaBus_Module *module){
    for(uint8_t i = 0; i < MAX_MODULES; i++){
        if(modules[i] == module){
            return i;
        }
    }

    for(uint8_t i = 0; i < MAX_MODULES; i++){
        if(modules[i] == NULL){
            modules[i] = module;
            module->controller = this;
            module->moduleID = i;
            switch (module->type)
            {
            case ACCESSORIE:
                Serial.println("Accessorie");
                if(accessorieAddr == 0){
                    accessorieAddr = programmMode(ACCESSORIE);                 
                }
                accessorieAddr += module->begin(useEEPROM, accessorieAddr);             
                break;
            case FEEDBACK:
                if(feedbackAddr == 0){
                    feedbackAddr = programmMode(FEEDBACK);
                }
                feedbackAddr += module->begin(useEEPROM, feedbackAddr);
                break;
            case BOOSTER:
                if(boosterAddr == 0){
                    boosterAddr = programmMode(BOOSTER);
                }
                boosterAddr += module->begin(useEEPROM, boosterAddr);
                break;
            default:
                break;
            }
            return i;
        }
    }
    return 255;

}
    
bool MobaBus::dettachModule(MobaBus_Module *module){
    for(uint8_t i = 0; i < MAX_MODULES; i++){
        if(modules[i] == module){
            uint16_t a = getEEPROMAddress(i);
            for(int i = a; i < a+MODULE_MAX_CONFIG_SIZE; i++){
                EEPROM.update(i, 0);
            }
            modules[i] = NULL;
            return true;
        }
    }
    return false;
}

bool MobaBus::dettachModule(uint8_t id){
    if(id >= MAX_MODULES) return false;
    modules[id] = NULL;
    return true;
}


int16_t MobaBus::getEEPROMAddress(uint8_t moduleId){
    if(!useEEPROM || moduleId >= MAX_MODULES) return -1;
    return eepromStart + ((moduleId+1) * MODULE_MAX_CONFIG_SIZE);
}

uint8_t MobaBus::sendPkg(MobaBus_Packet *pkg){
    return sendPkg(pkg->meta.type, pkg->meta.address, pkg->meta.port, pkg->meta.cmd, pkg->len, pkg->data, pkg->busId);
}

uint8_t MobaBus::sendPkg(MobaDevType type, uint16_t addr, MobaCmd cmd, uint8_t len, uint8_t *data, char skip){
    return sendPkg(type, addr, 0, cmd, len, data, skip);
}

uint8_t MobaBus::sendPkg(MobaDevType type, uint16_t addr, uint8_t port, MobaCmd cmd, uint8_t len, uint8_t *data, char skip){
    if(addr > 0x3FFF || port > 0x7F) return 255;
    uint8_t err = 0;
    for (int i = 0; i < MAX_BUS_INTERFACES; i++){
        if(bus[i] != NULL && i != skip){
            err *= bus[i]->Send(type, addr, port, cmd, len, data) ? 0 : 1;
        }
    }
    return err;
}

void MobaBus::loop(){
    MobaBus_Packet pkg;
    loop(&pkg);
}

void MobaBus::begin(uint16_t accessorieAddress, uint16_t feedbackAddress, uint16_t boosterAddress){

    pinMode(progPin, INPUT_PULLUP);
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);

    bool firstboot = (EEPROM.read(eepromStart) != uint8_t(21));
    Serial.print("Firstboot: ");
    Serial.println(firstboot);

    if(useEEPROM &! firstboot){
        EEPROM.get(eepromStart+1, boosterAddr);
        EEPROM.get(eepromStart+3, accessorieAddr);
        EEPROM.get(eepromStart+5, feedbackAddr);
        
    }
    else{
        accessorieAddr = accessorieAddress;
        feedbackAddr = feedbackAddress;
        boosterAddr = boosterAddress;
    }
    
}

uint32_t MobaBus::progButton(){
    if(millis() - lastChange < 50){
        if(pressed){
            return millis() - lastChange;
        }
        else{
            return 0;
        }   
    }
    else if(pressed && digitalRead(progPin) == LOW){
        uint32_t t = millis() - lastChange;
        return t;
    }
    else if(!pressed && digitalRead(progPin)){
        lastChange = millis();
        pressed = true;
    }
    else if(pressed && digitalRead(progPin) == HIGH){
        pressed = false;
        lastChange = millis();
        return 0;
    }
    return 0;
}

uint16_t MobaBus::programmMode(MobaDevType type){
    MobaBus_Packet pkg;
    bool prog = false;

    uint32_t ledTime = 0;
    uint32_t ledIntervall = 5000;
    Serial.print("Wait for addressing!");
    switch (type)
    {
    case ACCESSORIE:
        ledIntervall = 1000;
        break;
    case FEEDBACK:
        ledIntervall = 2000;
        break;
    case BOOSTER:
        ledIntervall = 3000;
        break;
    default:
        break;
    }

    digitalWrite(led, HIGH);

    while (true)
    {   
        if(progButton() >= 5000 && !prog){   
            prog = true;
            ledIntervall = 200;
            Serial.println("Prog...");
        }

        if(millis() - ledTime > ledIntervall){
            ledTime = millis();
            digitalWrite(led, !digitalRead(led));
        }     

        if(receive(&pkg) && prog && pkg.meta.type == type && pkg.meta.cmd == SET){
            if(useEEPROM){
                EEPROM.update(eepromStart, (uint8_t)21);
                EEPROM.put(eepromStart+(type * 2) - 1, pkg.meta.address);
            }
            return pkg.meta.address;
        }
    }   
}

bool MobaBus::loop(MobaBus_Packet *pkg){
    for(int i = 0; i < MAX_MODULES; i++){
        if(modules[i] != NULL){
            modules[i]->loop();
        }
    }

    return receive(pkg);
}

bool MobaBus::receive(MobaBus_Packet *pkg){
    for(int i = 0; i < MAX_BUS_INTERFACES; i++){
        if(bus[i] != NULL){
            if(bus[i]->Receive(*pkg)){
                pkg->busId = i;
                if(routing){
                    sendPkg(pkg);
                }
                processPkg(pkg);
                return true;
            }
        }
    }
    return false;
}

void MobaBus::processPkg(MobaBus_Packet *pkg){
    for(int i = 0; i < MAX_MODULES; i++){
        if(modules[i] != NULL){
            if(pkg->meta.type == modules[i]->type){
                modules[i]->processPkg(pkg);
            }
        }
    }
}