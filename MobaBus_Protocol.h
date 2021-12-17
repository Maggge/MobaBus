/**
  MobaBus Protocol

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

#ifndef __MobaProtocol__
#define __MobaProtocol__

#include <Arduino.h>

enum MobaDevType{
    ALL         = 0x00,
    BOOSTER     = 0x01,
    ACCESSORIE  = 0x02,
    FEEDBACK    = 0x03,
};

enum MobaCmd{
    GET         = 0x01,
    SET         = 0x02,
    INFO        = 0x03,
};

struct MobaBus_Meta{
    MobaDevType type;

    uint16_t address;

    uint8_t port;

    MobaCmd cmd;
};

struct MobaBus_Packet
{
    MobaBus_Meta meta;

    char busId = -1;

    uint8_t len = 0;

    uint8_t data[8];
};


#endif