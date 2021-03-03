void animate_lights(float primarySwitch, float horizontal) {
  if(!isToggled(primarySwitch)) {
    Serial.print(" Primary: lights");

    if(abs(horizontal) > SWITCH_TOLERANCE) {
      if(sign(horizontal) == -1)
        Serial.print(" Next lighting scheme");
      else
        Serial.print(" Previous lighting scheme");
    }
  }
}
