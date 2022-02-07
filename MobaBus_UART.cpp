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

#include "MobaBus_UART.h"

MobaBus_UART::MobaBus_UART(Stream &serial){
    this->_interface = &serial;
}
    
bool MobaBus_UART::begin(){
    //Clear the serial buffer
    while (_interface->available())
    {
        _interface->read();
    }
    return true;
}

bool MobaBus_UART::Send(MobaDevType type, uint16_t target, uint8_t port, MobaCmd cmd,  uint8_t len, uint8_t *data){
    _interface->print('#');
    _interface->print(type);
    _interface->print(',');
    _interface->print(target);
    _interface->print(',');
    _interface->print(port);
    _interface->print(',');
    _interface->print(cmd);
    _interface->print(',');
    _interface->print(len);
    for(int i = 0; i < len; i++){
        _interface->print(',');
        _interface->print(data[i]);
    }
    _interface->println();
    return true;
}

bool MobaBus_UART::Receive(MobaBus_Packet &pkg){
    if(_interface->available()){
        if(_interface->read() != '#'){
            _interface->readStringUntil('\n');
            return false;
        }
        pkg.meta.type = (MobaDevType)_interface->parseInt();
        pkg.meta.address = _interface->parseInt();
        pkg.meta.port = _interface->parseInt();
        pkg.meta.cmd = (MobaCmd)_interface->parseInt();
        pkg.len = _interface->parseInt();
        for(int i = 0; i < pkg.len; i++){
            pkg.data[i] = _interface->parseInt();
        }
        #ifdef DEBUG
        Serial.println("MobaBus - Packet received!");
        Serial.print("Dev Type = 0x");
        Serial.println(pkg.meta.type, HEX);
        Serial.print("Address = 0x");
        Serial.println(pkg.meta.address, HEX);
        Serial.print("Cmd = 0x");
        Serial.println(pkg.meta.cmd, HEX);
        Serial.print("Data Len = ");
        Serial.println(pkg.len);
        Serial.print("DATA: ");
        for (int i = 0; i < pkg.len; i++){
        Serial.print(pkg.data[i]);
        Serial.print("; ");
        }
        Serial.println();
        #endif
        return true;
    }
    return false;
}