/**
  MobaBus Interface

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

#ifndef __Moba_Interface__
#define __Moba_Interface__

#include "MobaBus_Protocol.h"

class MobaBus_Interface
{
public: 

    virtual bool begin() = 0;
    virtual bool Send(MobaDevType type, uint16_t target, uint8_t port, MobaCmd cmd,  uint8_t len, uint8_t *data) = 0;
    virtual bool Receive(MobaBus_Packet &pkg)=0;
};



#endif