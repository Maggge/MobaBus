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

#ifndef __MOBABUS_SERVO__
#define __MOBABUS_SERVO__

#include "MobaBus.h"
#include "MobaBus_Module.h"

#include <Servo.h>

#define SERVO_POWER_OFF 100 // ms after last step to power off the servo if autoPowerOff = true

class MobaBus_Servo : public MobaBus_Module{
private:
    Servo servo;

    uint8_t pin;

    uint8_t angles[2];
    uint8_t actualAngle;
    uint8_t targetAngle;
    bool active;

    uint32_t lastMove;
    uint8_t moveSpeed;

    bool autoPowerOff;
    uint32_t powerOffT;

public:
    /**
     * Constructor for Servo
     */
    MobaBus_Servo(uint8_t pin, uint8_t angle0, uint8_t angle1, uint8_t speed, bool autoPowerOff);

    uint8_t begin(bool useEEPROM, uint16_t address);
    void loop();

    void loadConf();
    void storeConf();


    void processPkg(MobaBus_Packet *pkg);
  
    uint8_t programmAddress(uint16_t addr);

    void setTurnout(bool dir, bool power);
};

#endif