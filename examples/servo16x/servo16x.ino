#include <Arduino.h>

#include <MobaBus.h>

#include <MobaBus_ServoPCA.h> //Require: https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library

MobaBus mobaBus(0, 8, 9); //MobaBus-controller instance with EEPROM, progamming Button and statusLED

MobaBus_CAN can(10, CAN_125KBPS, MCP_8MHZ, 2); //Can Bus interface

MobaBus_ServoPCA servos(0x40, 70, 110, 230, 100); // PCA9685 board with address=0x40, angle0=70°, angle1=110°, moving speed=230 and autoPowerOff=100ms

void setup() {

  mobaBus.begin(); //initialize the controller

  mobaBus.attachInterface(&can); //add the can bus interface to the controller
  mobaBus.attachModule(&servos); // add the PCA9685 module to the controller

}

void loop() {

  mobaBus.loop();
}