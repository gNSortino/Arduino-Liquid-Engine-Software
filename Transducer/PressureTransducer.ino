#include <Transducer.h>
#include <stdio.h> //For dtostrf function http://www.nongnu.org/avr-libc/user-manual/group__avr__stdlib.html#ga060c998e77fb5fc0d3168b3ce8771d42

Transducer transducer;
char s[64];  //buffer for dtostrf
float raw;
float pressures[5];


void setup() {
  Serial.begin(9600);
  Serial.println ("Signal,Voltage,PSI,Pa,MPa");
}

void loop() {
  raw = analogRead(A2);
  transducer.getPressures (raw, pressures);
  Serial.println ((String)dtostrf (pressures[0], 2,6, s) + "," +    //Analog Signal
                  (String)dtostrf (pressures[1], 2,6, s) + "," +    //Voltage 
                  (String)dtostrf (pressures[2], 5,6, s) + "," +    //Pounds per Square Inch
                  (String)dtostrf (pressures[3], 8,6, s) + "," +    //Pascal
                  (String)dtostrf (pressures[4], 2,10, s) + ",");   //MegaPascal
  delay(2000);
}
