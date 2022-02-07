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

#include "MobaBus_CAN.h"

volatile bool MM_CANIntReq = false;

void MM_CANInt() {
    MM_CANIntReq=true;
}


MobaBus_CAN::MobaBus_CAN(uint8_t cs, uint8_t speed, uint8_t clockspd, uint8_t interrupt) :
    _interface(cs) {
        pinMode(interrupt, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(interrupt), MM_CANInt, FALLING);
        _csPin = cs;
        _intPin = interrupt;
        _speed = speed;
        _clockspd = clockspd;
    //Begin doesn't work here!
}

bool MobaBus_CAN::begin(){
    lastErr = _interface.begin(MCP_ANY, _speed, _clockspd);
    _interface.setMode(MCP_NORMAL);
    return (lastErr == 0);
}

bool MobaBus_CAN::Send(MobaDevType type, uint16_t target, uint8_t port, MobaCmd cmd,  uint8_t len, uint8_t *data) {
    uint32_t addr = CanAddrAssemble(type, target, port, cmd);
    if(addr == 0) return false;

    lastErr = _interface.sendMsgBuf(addr, 1, len, data);
    if(lastErr != CAN_OK) return false;
    return true;
}

bool MobaBus_CAN::Receive(MobaBus_Packet &pkg) {

    uint32_t rxId;
    uint8_t ext = 1;
    uint8_t len = 0;
    uint8_t rxBuf[8];

    if(_interface.checkReceive() != CAN_MSGAVAIL) return false;

    uint8_t state = _interface.readMsgBuf(&rxId, &len, rxBuf);


    if(state != CAN_OK) return false;

    pkg.meta = CanAddrParse(rxId);
    pkg.len = len;

    for(uint8_t i=0; i<len; i++) pkg.data[i] = rxBuf[i];

    return true;
}

uint32_t MobaBus_CAN::CanAddrAssemble(MobaDevType type, uint16_t target, uint8_t port, MobaCmd cmd) {
        uint32_t addr = 0x80000000;

        if(type > 0x0F) return 0;
        addr |= ((uint32_t)type << 25);

        if(target > 0x3FFF) return 0;
        addr |= ((uint32_t)target << 11);

        if(port > 0x7F) return 0;
        addr |= ((uint32_t)port << 4);

        if(cmd > 0x0F) return 0;
        addr |= (uint32_t)cmd; 

        #ifdef MobaDebug
            Serial.print("Address: ");
            Serial.println(addr);
        #endif

        return addr;
}

MobaBus_Meta MobaBus_CAN::CanAddrParse(uint32_t canAddr) {
    MobaBus_Meta temp;

    #ifdef MobaDebug
        Serial.print("Address: ");
        Serial.println(canAddr);
    #endif

    temp.type = (MobaDevType)((canAddr >> 25) & 0x0F);
    temp.address = ((canAddr >> 11) & 0x3FFF);
    temp.port = ((canAddr >> 4) & 0x7F);
    temp.cmd = (MobaCmd)((canAddr) & 0x0F);

    return temp;
}
