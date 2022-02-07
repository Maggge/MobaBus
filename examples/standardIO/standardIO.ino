#include <Arduino.h>

#include <MobaBus.h>

#include <MobaBus_Standard_IO.h>

MobaBus mobaBus(0, 8, 9); //MobaBus-controller instance with EEPROM, progamming Button and statusLED

MobaBus_CAN can(10, CAN_125KBPS, MCP_8MHZ, 2); //Can Bus interface

MobaBus_Sensor sensor(A0, A1, A2, A3, A4, A5, A6, A7,true); // Sensor attached to pins A0-A7 and INPUT_PULLUP-mode
MobaBus_Output output( 7, false); //Digital output on pin 7, not inverted
MobaBus_Output_2pin output2(5, 6, false, 200); // Digital output on 2 pins (dir0 = pin5 high, dir6 = pin2 high), not inverted and auto Power Off after 200ms to prevent burnout (0 for deactivate)

void setup() {

  mobaBus.begin(); //initialize the controller

  mobaBus.attachInterface(&can); //add the can bus interface to the controller
  mobaBus.attachModule(&sensor); // add the Sensors module to the controller
  mobaBus.attachModule(&output); // add the output module to the controller
  mobaBus.attachModule(&output2); // add the output2 module to the controller
}


void loop() {

  mobaBus.loop();
}