# Arduino Nano RC Controls for 1/72 Scale Frégate Forbin Model
![Forbin](https://www.corsenetinfos.corsica/photo/art/grande/8284150-12962402.jpg)
This sketch is intended to control a 3D printed two-meter scale model from Arduino Nano.

*Note: because this program is in construction, its outputs have not been tested and may not work.*

## What it Does
First, intercepts the PWM signals emitted by my receiver. Then, mixes the throttle and rudder channel to drive the motors asymmetrically and control the rudder. This uses only two channels (throttle and rudder), so everything else is free to control decorative systems. A normal radio controller only mix would either take up both sticks (driving like a tank) or require mixes between channels; while mixes are fine, they are proprietary and limited by some systems, which would mean upgrading to a more expensive radio. Also, buying a DC motor board is cheaper than getting RC speed controllers and ESCs. Additionally, custom programming keeps possibilities open for limitless model fonctionalities later.

With a 6 channel Spektrum Tx/Rx, the left stick and right stick both are two linear channels, for a total of 4 (all from PWM 1000 to 2000, with a midpoint at 1500); while the gear and flap switches are boolean toggles (either PWM 1000 or 2000).

With switches controlling modes, one could operate cranes, cannons, move doors up and down, and more with the horizontal and vertical axes. In my case, I'd like to switch lights to different modes, and control three separate bay doors. I could also switch the radar's rotation on or off.

### Dual Thrust
Both motors operate independently, and provide asymmetric thrust in coordination with the rudder, both forwards and backwards. When the ship is stopped, the motors can be activated in opposite directions for in-place turns. The rudder signal is intercepted but not changed.

### Lighting
The lighting scheme will be chosen based on whether the ship is motionless (at anchor) or under way (steaming lights). Otherwise, the helipad, bridge, and deck lights also will have manual control.

### Radar
The radar can be activated from one of the radio switches at a distance.

### Gun Rotation
The guns will rotate to face a constant direction based on a magnetometer (compass).

### Garage Doors
Bay doors will have the possibility of opening and closing based on radio inputs.

### Sound
For now, the plan is to add a horn and perhaps a missile siren. These will be artificially generated sound waves that do not require an SD card.

## Electronics
- 12V SLA battery
   - Cytron 10A 5-30V Dual DC Motor Driver
      - RS-755 12V Brushed DC Motors (2x) [driving propellers]
   - 4-40V to 1.5-35V Buck Converter
      - Arduino Nano
         - PCA9685 16 Channel 12-Bit PWM Servo Motor Driver IIC [LED driver]
      - Continuous Rotation Servo [rotating main radar]
   - 2.5kg Goteck Metal Gear Micro Servo [directing rudder]

### Nano Pinout Reference
![Nano Pinout](arduino-nano-pinout.jpg)

## Thanks
Thanks to the code by Kelvin Nelson as found here: [Read PWM, Decode RC Receiver Input, and Apply Fail-Safe](https://create.arduino.cc/projecthub/kelvineyeone/read-pwm-decode-rc-receiver-input-and-apply-fail-safe-6b90eb)
