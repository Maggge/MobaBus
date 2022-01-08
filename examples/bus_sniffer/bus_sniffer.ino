#include <Arduino.h>

#include <MobaBus.h>

MobaBus mobaBus(0); //MobaBus-controller

MobaBus_CAN can(10, CAN_125KBPS, MCP_8MHZ, 2); //Can Bus interface

void setup() {
  Serial.begin(115200);
 
  Serial.println("MobaBus");
  Serial.println("--------------------------");
  mobaBus.begin();
  mobaBus.attachInterface(&can); //add the can bus interface to the controller

}

void loop() {

  MobaBus_Packet pkg;
  if(mobaBus.loop(&pkg)){
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
    Serial.println();
  }
}