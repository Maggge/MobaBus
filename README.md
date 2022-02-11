# MobaBus
MobaBus is an arduino based system to connect turnouts, sensors and other model railroad accessories over can-bus.
You can build custom components with sensors, accessories and others combined on one Arduino. At the moment its possible to combine 10 modules per Arduino (in future maybe more).

At the moment are included following modules:
-  digital outputs over arduino pins
-  digital outputs wit 2 pins(for turnouts without end switch)
-  digital inputs (like sensors or buttons) over arduino pins
-  servos over arduino pins
-  servos over PCA9685 16-channel pwm driver

Libraries needed:</br>
<a href="https://github.com/coryjfowler/MCP_CAN_lib">MCP_CAN_lib</a></br>
<a href="https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library">Adafruit-PWM-Servo-Driver-Library</a>
