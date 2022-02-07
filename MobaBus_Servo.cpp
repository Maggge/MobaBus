/**
  MobaBus Servo

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

#include "MobaBus_Servo.h"

MobaBus_Servo::MobaBus_Servo(uint8_t pin, uint8_t angle0, uint8_t angle1, uint8_t speed, uint16_t autoPowerOff) {
    init("MobaBus_Servo", ACCESSORIE, 3, 1);
    this->pin = pin;
    this->autoPowerOff = autoPowerOff;
    angles[0] = angle0;
    angles[1] = angle1;
    moveSpeed = (uint8_t)255 - speed;;
}

bool MobaBus_Servo::begin(){
    servo.attach(pin);
    servo.write(90);
    actualAngle = 90;
    if(autoPowerOff){
        delay(autoPowerOff);
        servo.detach();
    }
    return true;    
}

void MobaBus_Servo::loop(){
    uint32_t actualTime = millis();
    if(actualTime >= lastMove + moveSpeed){
        lastMove = actualTime;
        if(actualAngle != targetAngle){
            if(!active){
                servo.attach(pin);
                active = true;
            }
            actualAngle += targetAngle > actualAngle ? 1 : -1;
            servo.write(actualAngle);
            powerOffT = actualTime + autoPowerOff;
            }
        else if(autoPowerOff && active && powerOffT <= actualTime){    
            active = false;
            servo.detach();
        }
    }
}

int MobaBus_Servo::loadConf(uint16_t eepromAddress){
    int idx = MobaBus_Module::loadConf(eepromAddress);
    if(idx < 0){
        Serial.println("Load config failed!");
        return -2;
    }
    idx += sizeof(EEPROM.get(idx, autoPowerOff));
    idx += sizeof(EEPROM.get(idx, moveSpeed));
    idx += sizeof(EEPROM.get(idx, angles));
    return idx;
}

int MobaBus_Servo::storeConf(uint16_t eepromAddress){
    int idx = MobaBus_Module::storeConf(eepromAddress);
    if(idx < 0){
        Serial.println("Store config failed!");
        return -2;
    }
    idx += sizeof(EEPROM.put(idx, autoPowerOff));
    idx += sizeof(EEPROM.put(idx, moveSpeed));
    idx += sizeof(EEPROM.put(idx, angles));
    return idx;
}

void MobaBus_Servo::setAngles(uint8_t angle0, uint8_t angle1){
    angles[0] = angle0;
    angles[1] = angle1;
}

void MobaBus_Servo::setSpeed(uint8_t speed){
    moveSpeed = 255 - speed;
}

void MobaBus_Servo::setAutoPowerOff(uint16_t ms){
    autoPowerOff = ms;
}

void MobaBus_Servo::processPkg(MobaBus_Packet *pkg){
    if(pkg->meta.cmd == SET){
        setTurnout((bool)pkg->data[0], (bool)pkg->data[1]);
    }
    else if(pkg->meta.cmd == GET){
        uint8_t data[] = {(actualAngle == angles[1])};
        controller->sendPkg(ACCESSORIE, address(), INFO, 1, data);
    }
}

void MobaBus_Servo::setTurnout(bool dir, bool power){
    if(power){
        targetAngle = angles[dir];
        uint8_t data[] = {dir};
        controller->sendPkg(ACCESSORIE, address(), INFO, 1, data);
    }
    else{
        targetAngle = actualAngle;
        servo.detach();
        active = false;
    }
}