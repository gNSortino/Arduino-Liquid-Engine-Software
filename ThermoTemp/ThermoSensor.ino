#include <ThermoTemp.h>
#include <stdio.h> //For dtostrf function http://www.nongnu.org/avr-libc/user-manual/group__avr__stdlib.html#ga060c998e77fb5fc0d3168b3ce8771d42

ThermoTemp thermoTemp;
int raw;
float temps[6];
char s[32];  //buffer for dtostrf

void setup() {
  Serial.begin(9600);
  Serial.println ("Signal,Voltage,Celsius,Kelvin,Fahrenheit,Rankine");
}

void loop() {
  raw = analogRead(A0);
  thermoTemp.getTemps(raw, temps);
  String myVal = dtostrf (1234.9999, 8,20, s);
  Serial.println ((String)dtostrf (temps[0], 4,0, s) + "," +      //Analog Signal
                  (String)dtostrf (temps[1], 4,4, s) + "," +      //Voltage 
                  (String)dtostrf (temps[2], 6,4, s) + "," +      //Celsius
                  (String)dtostrf (temps[3], 6,4, s) + "," +      //Kelvin
                  (String)dtostrf (temps[4], 6,4, s) + "," +      //Fahrenheit
                  (String)dtostrf (temps[5], 6,4, s) + ",");      //Rankine
  delay(1000);
}
