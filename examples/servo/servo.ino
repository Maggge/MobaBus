#include <Arduino.h>

#include <MobaBus.h>

#include <MobaBus_Servo.h>

MobaBus mobaBus(0, 8, 9); //MobaBus-controller instance with EEPROM, progamming Button and statusLED

MobaBus_CAN can(10, CAN_125KBPS, MCP_8MHZ, 2); //Can Bus interface

MobaBus_Servo servo(7, 70, 110, 230, true); // Servo attached to pin 7, angle0=70°, angle1=110°, moving speed=230 and autoPowerOff=on

void setup() {

  mobaBus.begin(); //initialize the controller

  mobaBus.attachInterface(&can); //add the can bus interface to the controller
  mobaBus.attachModule(&servo); // add the Servo module to the controller

}

void loop() {

  mobaBus.loop();
}