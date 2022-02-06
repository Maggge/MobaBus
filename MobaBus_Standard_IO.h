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

#ifndef __MOBABUS_STANDARD_IO__
#define __MOBABUS_STANDARD_IO__

#include "MobaBus_Module.h"
#include "MobaBus.h"

class MobaBus_Sensor : public MobaBus_Module{
private:
    uint8_t pin[8];
    bool pullUp;

    bool state[8];

    uint32_t debounce[8];

    void sendStates();
    
public:
    /**
     * Constructor
     * @param pinX number, 0 if not used!
     * @param pullup if true pinMode is INPUT_PULLUP else INPUT
     */
    MobaBus_Sensor(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7, uint8_t pin8, bool pullup);

    uint8_t begin(bool useEEPROM, uint16_t address);
    void loop();

    void loadConf();
    void storeConf();

    void processPkg(MobaBus_Packet *pkg);
    
    uint8_t programmAddress(uint16_t addr);
};

class MobaBus_Output : public MobaBus_Module{
private:
    uint8_t pin;
    bool inverted;
    bool state;

public:
    MobaBus_Output(uint8_t pin, bool inverted);

    uint8_t begin(bool useEEPROM, uint16_t address);
    void loop();

    void loadConf();
    void storeConf();


    void processPkg(MobaBus_Packet *pkg);
  
    uint8_t programmAddress(uint16_t addr);

    void setTurnout(bool dir);

};

#define AUTO_POWER_OFF 500 // ms after switching the pins turn off to prevent a burn out of the turnout drive

class MobaBus_Output_2pin : public MobaBus_Module{
private:
     uint8_t pin[2];
     bool inverted;
     bool state;

     bool active;
     uint32_t powerOffT;

     bool autoPowerOff;   

public:
    MobaBus_Output_2pin(uint8_t pin1, uint8_t pin2, bool inverted, bool autoPowerOff);

    uint8_t begin(bool useEEPROM, uint16_t address);
    void loop();

    void loadConf();
    void storeConf();


    void processPkg(MobaBus_Packet *pkg);
  
    uint8_t programmAddress(uint16_t addr);

    void setTurnout(bool dir, bool power);
};
#endif