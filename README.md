# MobaBus
MobaBus is an arduino based system to connect turnouts, sensors and other model railroad accessories over can-bus.

At the moment we support digital outputs over arduino pins, outputs wit 2 pins(for turnouts without end switch), digital inputs (like sensors or buttons) over arduino pins, servos over arduino pins and servos over PCA9685 16-channel pwm driver.

Libraries needed:</br>
<a href="https://github.com/coryjfowler/MCP_CAN_lib">MCP_CAN_lib</a></br>
<a href="https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library">Adafruit-PWM-Servo-Driver-Library</a>
