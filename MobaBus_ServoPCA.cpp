/**
  MobaBus Servo PCA9685

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

#include "MobaBus_ServoPCA.h"


MobaBus_ServoPCA::MobaBus_ServoPCA(uint8_t angle0, uint8_t angle1, uint8_t speed, bool autoPowerOff) {
    MobaBus_ServoPCA(0x40, angle0, angle1, speed, autoPowerOff);
}

MobaBus_ServoPCA::MobaBus_ServoPCA(uint16_t addr, uint8_t angle0, uint8_t angle1, uint8_t speed, bool autoPowerOff) {
    servoBoard = Adafruit_PWMServoDriver(addr);
    powerOff = autoPowerOff;
    angles[0] = angle0;
    angles[1] = angle1;
    moveSpeed = (uint8_t)255 - speed;
    channels = 16;
    type = ACCESSORIE;
}

uint8_t MobaBus_ServoPCA::begin(bool useEEPROM, uint16_t address){
    Serial.println("Module begin");
    servoBoard.begin();
    servoBoard.setPWMFreq(60);
    
    programmAddress(address);

    loadConf();
    for(int i = 0; i < channels; i++){
        int startAngle = (int)(((float)angles[0] + (float)angles[1]) / 2.0);
        servoBoard.setPWM(i, 0, map(startAngle, 0, 180, SERVOMIN, SERVOMAX));
        actualAngle[i] = startAngle;
        setTurnout(i, 0);
        delay(20);
    }
    intitialized = true;
    return channels;
}
void MobaBus_ServoPCA::loop(){
    uint32_t actualTime = millis();
    if(actualTime >= lastMove + moveSpeed){
        lastMove = actualTime;
        for(int i = 0; i < channels; i++){
            if(actualAngle[i] != targetAngle[i]){
                actualAngle[i] += targetAngle[i] > actualAngle[i] ? 1 : -1;
                servoBoard.setPWM(i, 0, map(actualAngle[i], 0, 180, SERVOMIN, SERVOMAX));
                active[i] = true;
                remainingTime[i] = SERVO_POWER_OFF;
            }
            else if(active[i] && powerOff){
                    remainingTime[i] -= moveSpeed;
                    if(remainingTime[i] <= 0){
                        active[i] = false;
                        servoBoard.setPWM(i, 0, 4096);
                    }
            }
        }
    }
}

void MobaBus_ServoPCA::loadConf(){
    if(controller == NULL) return;
    
    uint16_t configAddr = controller->getEEPROMAddress(moduleID);
    uint16_t storedAddress;
    configAddr += sizeof(EEPROM.get(configAddr, storedAddress));
    
    if(address != storedAddress) return;

    configAddr += sizeof(EEPROM.get(configAddr, angles[0]));
    configAddr += sizeof(EEPROM.get(configAddr, angles[1]));
}

void MobaBus_ServoPCA::storeConf(){
    if(controller == NULL) return;
    
    uint16_t configAddr = controller->getEEPROMAddress(moduleID);
    
    configAddr += sizeof(EEPROM.put(configAddr, address));
    configAddr += sizeof(EEPROM.put(configAddr, angles[0]));
    configAddr += sizeof(EEPROM.put(configAddr, angles[1]));

}

void MobaBus_ServoPCA::processPkg(MobaBus_Packet *pkg){
    if(!intitialized) return;
    Serial.print("First Address: ");
    Serial.print(address);
    Serial.print(" Last Address: ");
    Serial.println(address + channels);
    if(pkg->meta.type == type && pkg->meta.address >= address && pkg->meta.address < address+channels){
        if(pkg->meta.cmd == SET){
            setTurnout(pkg->meta.address - address, pkg->data[0], pkg->data[1]);
        }
        else if(pkg->meta.cmd == GET){
            uint8_t data[] = {(targetAngle[pkg->meta.address - address] == angles[0] ? 0 : 1)};
            controller->sendPkg(ACCESSORIE, pkg->meta.address, INFO, 1, data);
        }
    }
}

uint8_t MobaBus_ServoPCA::programmAddress(uint16_t addr){
    address = addr;
    Serial.print("Set Address ");
    Serial.print(address);
    Serial.print(" - ");
    Serial.println(address + channels - 1);
    return channels;
}

void MobaBus_ServoPCA::setTurnout(uint8_t pin, uint8_t dir, bool power){

    if(power){
        targetAngle[pin] = angles[dir];
        uint8_t data[] = {dir};
        controller->sendPkg(ACCESSORIE, address + pin, INFO, 1, data);
    }
    else{
        servoBoard.setPWM(pin, 0, 4096);
        targetAngle[pin] = actualAngle[pin];
        active[pin] = false;
    }
}


 