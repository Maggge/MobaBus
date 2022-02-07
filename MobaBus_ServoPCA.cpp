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


MobaBus_ServoPCA::MobaBus_ServoPCA(uint8_t angle0, uint8_t angle1, uint8_t speed, uint16_t autoPowerOff) {
    MobaBus_ServoPCA(0x40, angle0, angle1, speed, autoPowerOff);
}

MobaBus_ServoPCA::MobaBus_ServoPCA(uint16_t addr, uint8_t angle0, uint8_t angle1, uint8_t speed, uint16_t autoPowerOff) {
    init("MobaBus_ServoPCA", ACCESSORIE, 4, 16);
    servoBoard = Adafruit_PWMServoDriver(addr);
    this->autoPowerOff = autoPowerOff;
    angles[0] = angle0;
    angles[1] = angle1;
    moveSpeed = (uint8_t)255 - speed;
}

bool MobaBus_ServoPCA::begin(){
    servoBoard.begin();
    servoBoard.setPWMFreq(60);

    for(int i = 0; i < usedChannels(); i++){
        int startAngle = (int)(((float)angles[0] + (float)angles[1]) / 2.0);
        servoBoard.setPWM(i, 0, map(startAngle, 0, 180, SERVOMIN, SERVOMAX));
        actualAngle[i] = startAngle;
        setTurnout(i, 0);
        delay(20);
    }
    return true;
}
void MobaBus_ServoPCA::loop(){
    uint32_t actualTime = millis();
    if(actualTime >= lastMove + moveSpeed){
        lastMove = actualTime;
        for(int i = 0; i < usedChannels(); i++){
            if(actualAngle[i] != targetAngle[i]){
                actualAngle[i] += targetAngle[i] > actualAngle[i] ? 1 : -1;
                servoBoard.setPWM(i, 0, map(actualAngle[i], 0, 180, SERVOMIN, SERVOMAX));
                active[i] = true;
                remainingTime[i] = autoPowerOff;
            }
            else if(active[i] && autoPowerOff){
                    remainingTime[i] -= moveSpeed;
                    if(remainingTime[i] <= 0){
                        active[i] = false;
                        servoBoard.setPWM(i, 0, 4096);
                    }
            }
        }
    }
}

int MobaBus_ServoPCA::loadConf(uint16_t eepromAddress){
    int idx = MobaBus_Module::loadConf(eepromAddress);
    if(idx < 0){
        return -2;
    }
    idx += sizeof(EEPROM.get(idx, angles));
    return idx;
}

int MobaBus_ServoPCA::storeConf(uint16_t eepromAddress){
    int idx = MobaBus_Module::storeConf(eepromAddress);
    if(idx < 0){
        return -2;
    }
    idx += sizeof(EEPROM.put(idx, angles));
    return idx;
}

void MobaBus_ServoPCA::processPkg(MobaBus_Packet *pkg){
    if(pkg->meta.cmd == SET){
        setTurnout(pkg->meta.address - address(), pkg->data[0], pkg->data[1]);
    }
    else if(pkg->meta.cmd == GET){
        uint8_t data[] = {(targetAngle[pkg->meta.address - address()] == angles[0] ? 0 : 1)};
        controller->sendPkg(ACCESSORIE, pkg->meta.address, INFO, 1, data);
    }
}

void MobaBus_ServoPCA::setTurnout(uint8_t pin, uint8_t dir, bool power){

    if(power){
        targetAngle[pin] = angles[dir];
        uint8_t data[] = {dir};
        controller->sendPkg(ACCESSORIE, address() + pin, INFO, 1, data);
    }
    else{
        servoBoard.setPWM(pin, 0, 4096);
        targetAngle[pin] = actualAngle[pin];
        active[pin] = false;
    }
}


 