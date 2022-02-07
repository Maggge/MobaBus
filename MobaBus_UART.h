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

#ifndef __MobaBus_UART__
#define __MobaBus_UART__

#include "MobaBus_Interface.h"


class MobaBus_UART: public MobaBus_Interface{
private:
    Stream *_interface;
public:
    MobaBus_UART(Stream &serial);
    
    bool begin();
    bool Send(MobaDevType type, uint16_t target, uint8_t port, MobaCmd cmd,  uint8_t len, uint8_t *data);
    bool Receive(MobaBus_Packet &pkg);
};

#endif
