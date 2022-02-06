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

#ifndef __MOBA_SERVO_PCA__
#define __MOBA_SERVO_PCA__

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include "MobaBus.h"
#include "MobaBus_Module.h"


#define SERVOMIN  100 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  550 // this is the 'maximum' pulse length count (out of 4096)575

#define SERVO_POWER_OFF 100 // ms after last step to power off the servo if autoPowerOff = true



class MobaBus_ServoPCA : public MobaBus_Module{
private:
    Adafruit_PWMServoDriver servoBoard;
    
    uint8_t angles[2];

    uint8_t actualAngle[16];
    uint8_t targetAngle[16];
    bool active[16];
    int16_t remainingTime[16];

    uint32_t lastMove;
    uint8_t moveSpeed;

    bool powerOff;


public:

    /**
     * Constructor for Turnouts with one PCA9685 module
     * initialized with standard Address 0x40
     */
    MobaBus_ServoPCA(uint8_t angle0, uint8_t angle1, uint8_t speed, bool autoPowerOff);

    /**
     * Constructor for Turnouts with one or multiple PCA9685 modules
     * @param addr of the module (standard = 0x40)
     */
    MobaBus_ServoPCA(uint16_t addr, uint8_t angle0, uint8_t angle1, uint8_t speed, bool autoPowerOff);


    uint8_t begin(bool useEEPROM, uint16_t address);
    void loop();

    void loadConf();
    void storeConf();


    void processPkg(MobaBus_Packet *pkg);
  
    uint8_t programmAddress(uint16_t addr);

    void setTurnout(uint8_t pin, uint8_t dir, bool power = true);
};




#endif