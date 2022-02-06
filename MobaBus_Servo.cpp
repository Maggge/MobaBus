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

MobaBus_Servo::MobaBus_Servo(uint8_t pin, uint8_t angle0, uint8_t angle1, uint8_t speed, bool autoPowerOff){
    this->pin = pin;
    this->autoPowerOff = autoPowerOff;
    angles[0] = angle0;
    angles[1] = angle1;
    moveSpeed = (uint8_t)255 - speed;
    type = ACCESSORIE;
}

uint8_t MobaBus_Servo::begin(bool useEEPROM, uint16_t address){
    Serial.println("Module begin");
    Serial.println("Drive Servo at 90 degrees");
    
    programmAddress(address);

    servo.attach(pin);
    servo.write(90);
    actualAngle = 90;
    if(autoPowerOff){
        delay(SERVO_POWER_OFF);
        servo.detach();
    }

    intitialized = true;
    return 1;    
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
            powerOffT = actualTime + SERVO_POWER_OFF;
            }
        else if(autoPowerOff && active && powerOffT <= actualTime){    
            active = false;
            servo.detach();
        }
    }
}

void MobaBus_Servo::loadConf() {
    return;
}

void MobaBus_Servo::storeConf() {
    return;
}

void MobaBus_Servo::processPkg(MobaBus_Packet *pkg){
    if(pkg->meta.type == type && pkg->meta.address == address){
        if(pkg->meta.cmd == SET){
            setTurnout((bool)pkg->data[0], (bool)pkg->data[1]);
        }
        else if(pkg->meta.cmd == GET){
            uint8_t data[] = {(actualAngle == angles[1])};
            controller->sendPkg(ACCESSORIE, address, INFO, 1, data);
        }
    }
}

uint8_t MobaBus_Servo::programmAddress(uint16_t addr){
    address = addr;
    Serial.print("Set Address ");
    Serial.println(address);
    return 1;
}

void MobaBus_Servo::setTurnout(bool dir, bool power){
    if(power){
        targetAngle = angles[dir];
        uint8_t data[] = {dir};
        controller->sendPkg(ACCESSORIE, address + pin, INFO, 1, data);
    }
    else{
        targetAngle = actualAngle;
        servo.detach();
        active = false;
    }
}