/**
  MobaBus Can Interface

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

#ifndef __MobaBus_CAN__
#define __MobaBus_CAN__

#include "MobaBus_Interface.h"
#include <mcp_can.h>
#include <SPI.h>

extern volatile bool MM_CANIntReq;

void MM_CANInt(void);

class MobaBus_CAN : public MobaBus_Interface {
private:
    uint8_t _csPin = 0;

    /**
     * Interrupt pin to be used
     */
    uint8_t _intPin = 0;

    /**
     * CAN Bus Speed
     */
    uint8_t _speed = 0;

    /**
     * CAN Crystal Frequency
     */
    uint8_t _clockspd = 0;

    MCP_CAN _interface; 

    /**
     * Error code of the last action - 0 = no error
     */
    uint8_t lastErr = 0;
public:
    MobaBus_CAN(uint8_t cs, uint8_t speed, uint8_t clockspd, uint8_t interrupt);
    
    bool begin();
    bool Send(MobaDevType type, uint16_t target, uint8_t port, MobaCmd cmd,  uint8_t len, uint8_t *data);
    bool Receive(MobaBus_Packet &pkg);
    bool end();

    uint32_t CanAddrAssemble(MobaDevType type, uint16_t target, uint8_t port, MobaCmd cmd);
    MobaBus_Meta CanAddrParse(uint32_t canAddr);
};



#endif