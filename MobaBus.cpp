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
**/


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
            if(attachModule(module, i)){
                return i;
            }
            else{
                return 254;
            }
        }
    }
    return 255;

}

bool MobaBus::attachModule(MobaBus_Module *module, uint8_t id){
    module->controller = this;
    module->moduleID = id; 
    Serial.print("Attach " + module->moduleName() + " with ID ");
    Serial.print(module->moduleID);
    Serial.print("...");
    if(module->begin()){
        modules[id] = module;
        
        Serial.println(" OK!");
        if(useEEPROM){
            Serial.print("Load config on EEPROM Addr ");
            Serial.print(getEEPROMAddress(id));
            Serial.print("....");
            Serial.println(module->loadConf(getEEPROMAddress(id)));
            if(module->address() == 0){
                addressModule(id);
                module->storeConf(getEEPROMAddress(id));
            }
        } 
    }
    else{
        Serial.println("Failed!");
        return false;
    }
    return true;
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
    return eepromStart + 1 + ((moduleId) * MODULE_MAX_CONFIG_SIZE);
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

void MobaBus::begin(bool routing=true, void (*firstboot)()=NULL){
    this->routing = routing;
    if(useEEPROM){
        pinMode(progPin, INPUT_PULLUP);
        pinMode(led, OUTPUT);
        digitalWrite(led, HIGH);
        delay(1000);
        digitalWrite(led,LOW);
        if(EEPROM.read(eepromStart) != 21){
            if(firstboot != NULL){
                firstboot();
            }
            EEPROM.update(eepromStart, 21);
        }
        return;
    }
    if(firstboot != NULL){
        firstboot();
    }
}


void MobaBus::loop(){
    MobaBus_Packet pkg;
    loop(&pkg);
}

bool MobaBus::loop(MobaBus_Packet *pkg){
    if(progButton() >= 10000){
        reset();
    }
    for(int i = 0; i < MAX_MODULES; i++){
        if(modules[i] != NULL && modules[i]->initialized()){
            modules[i]->loop();
        }
    }

    return receive(pkg);
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
    else if(!pressed && digitalRead(progPin) == LOW){
        lastChange = millis();
        pressed = true;
        Serial.println("Button pressed");
    }
    else if(pressed && digitalRead(progPin) == HIGH){
        pressed = false;
        lastChange = millis();
        Serial.println("Button released");
        return 0;
    }
    return 0;
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
            if(modules[i]->initialized() && pkg->meta.type == modules[i]->getModuleType() && pkg->meta.address >= modules[i]->address() && pkg->meta.address < modules[i]->address()+modules[i]->usedChannels()){
                modules[i]->processPkg(pkg);
            }
        }
    }
}

bool MobaBus::addressModule(uint8_t id){
    Serial.println("Start Addressing...");
    bool progMode = false;
    bool ledState = false;
    uint32_t lastBlink = 0;
    uint32_t progStart = 0;
    uint16_t d;
    MobaBus_Packet pkg;
    switch (modules[id]->getModuleType())
    {
    case FEEDBACK:
        d = 1000;
        break;
    case ACCESSORIE:
        d = 500;
        break;
    default:
        d = 2000;
        break;
    } 
    while (true){
        uint32_t actualTime = millis();
        if((lastBlink + (progMode ? 200 : d)) <= actualTime){
            digitalWrite(led, !ledState);
            ledState = !ledState;
            lastBlink = actualTime;
        }

        if(!progMode && progButton() >= 5000 ){
            progMode = true;
            progStart = actualTime;
            Serial.println("Wait for Address");
        }
        else if(progButton() >= 10000){
            reset();
        }

        if(progMode && progStart + 30000 <= actualTime){
            progMode= false;
            Serial.println("Exit progmode");
        }

        if(receive(&pkg) && progMode){
            if(pkg.meta.type == modules[id]->getModuleType() && pkg.meta.cmd == SET){
                modules[id]->programmAddress(pkg.meta.address);
                digitalWrite(led, LOW);
                return true;
            }
        }
    }
}

void MobaBus::reset(){
    Serial.println("Reset....");
    for(int i = 0; i < 10; i++){
        digitalWrite(led, HIGH);
        delay(200);
        digitalWrite(led, LOW);
        delay(200);
    }
    if(useEEPROM){
        for(int i = 0; i < EEPROM.length(); i++){
            EEPROM.update(i, 0);
        }
    }
    digitalWrite(led, HIGH);
    Serial.println("Reboot");
    delay(1000);
    wdt_enable(WDTO_15MS);
    while (true);
}