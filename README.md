# MobaBus
MobaBus is an arduino based system to connect turnouts, sensors and other model railroad accessories over can-bus.

At the moment are included following modules:
-  digital outputs over arduino pins
-  digital outputs wit 2 pins(for turnouts without end switch)
-  digital inputs (like sensors or buttons) over arduino pins
-  servos over arduino pins
-  servos over PCA9685 16-channel pwm driver

At the moment you can use up to 10 modules per arduino (in future maybe more).

Libraries needed:</br>
<a href="https://github.com/coryjfowler/MCP_CAN_lib">MCP_CAN_lib</a></br>
<a href="https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library">Adafruit-PWM-Servo-Driver-Library</a>
