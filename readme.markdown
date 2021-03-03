NOTE: THIS PROGRAM IS IN CONSTRUCTION AND ITS OUTPUTS MAY NOT WORK PROPERLY
This sketch is intended to control a 3D printed scale model that's two meters long.

## What it Does
First, intercepts the PWM signals emitted by my receiver. Then, mixes the throttle and rudder channel to drive the motors asymmetrically and control the rudder. This uses only two channels (throttle and rudder), so everything else is free to control decorative systems.

With a 6 channel Spektrum Tx/Rx, the gear and flap switches are boolean toggles (either PWM 1000 or 2000), while the vertical and horizontal axes are linear (PWM 1000 increasing to 2000, midpoint at 1500).

With switches controlling modes, one could operate cranes, cannons, move doors up and down, and more with the horizontal and vertical axes. In my case, I'd like to switch lights to different modes, and control three separate bay doors. I could also switch the radar's rotation on or off.

## Electronics
- 12V SLA battery
   - Cytron 10A 5-30V Dual DC Motor Driver
      - RS-755 12V Brushed DC Motors (2x) [driving propellers]
   - 4-40V to 1.5-35V Buck Converter
      - Arduino Nano
         - PCA9685 16 Channel 12-Bit PWM Servo Motor Driver IIC [LED driver]
      - Continuous Rotation Servo [rotating main radar]
   - 2.5kg Goteck Metal Gear Micro Servo [directing rudder]