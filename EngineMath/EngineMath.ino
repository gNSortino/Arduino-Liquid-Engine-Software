#include <EngineMath.h>
#include <Transducer.h>
#include <stdio.h> //For dtostrf function http://www.nongnu.org/avr-libc/user-manual/group__avr__stdlib.html#ga060c998e77fb5fc0d3168b3ce8771d42

EngineMath em;
Transducer transducer;
float pressures[5];
char s[64];                      // buffer for dtostrf
float raw;                       // holds raw analog read
float mf[4];                    // array to hold mass flow properties. Re-used for liquid and gas
float r = 8314.4621;             // Universal Gas Constant (J/kg mol-K)
float g = 9.80665;               // Gravity m/sec^2
//Gas Flow Properties
float gcd = 0.7;                // Coefficient of Discharge (Dimensionless)
float  gk = 1.227;                // Gas Specific Heat Ratio (Dimensionless)
float gz = 0.975;                // Gas Compressability Factor (Dimensionless) 
float gtemp = 296.0;             // Gas Temperature at inlet (Kelvin)
float gm = 28.9;                 // Gas Molecular Mass  (mol)
float gp1 = 459.0;              // Inlet Pressure (psi) 
float gp2 = 300.0;              // Outlet Pressure (psi)
float ga = 0.000004453483703;   // Orifice Area (m^2) =((PI()*((3/32")/2)^2)*0.00064516)
//Liquid Flow Properties
float lcd = 0.6;   	         // Coefficient of Discharge (Dimensionless)
float lden = 785.0; 	         // Liquid Density (kg/m^3)
float lp1 = 500.0;  	         // Inlet Pressure (psi) 
float lp2 = 300.0;  	         // Outlet Pressure (psi)
float la = 0.000000198132573;   // Orifice Area (m^2)
/////////////////////////////////////////////

void setup() {
  Serial.begin(57600);  
  Serial.println ("Pressure,Signal,Voltage,PSI,Pa,MPa");
  Serial.println ("LiquidFlow,kg per sec,kg per min,lbs per sec,lbs per min");
  Serial.println ("GasFlow,kg per sec,kg per min,lbs per sec,lbs per min");
}

void loop() {
  raw = analogRead(A1);
  transducer.getPressures (raw, pressures);
  Serial.println ("Pressure," +
                  (String)dtostrf (pressures[0], 2,6, s) + "," +    // Analog Signal
                  (String)dtostrf (pressures[1], 2,6, s) + "," +    // Voltage 
                  (String)dtostrf (pressures[2], 5,6, s) + "," +    // Pounds per Square Inch
                  (String)dtostrf (pressures[3], 8,6, s) + "," +    // Pascal
                  (String)dtostrf (pressures[4], 2,10, s) + ",");   // MegaPascal
  mf[0] = em.LiquidMassFlow (lcd, g, lden,  lp1,  lp2, la);
  em.MassFlowConvert (mf);
  Serial.println ("LiquidFlow," +
                (String)dtostrf (mf[0], 2,15, s) + "," +   // kg/sec
                (String)dtostrf (mf[1], 3,10, s) + "," +   // kg/min 
                (String)dtostrf (mf[2], 3,15, s) + "," +   // lbs/sec
                (String)dtostrf (mf[3], 3,15, s));          // lbs/min
  mf[0] = em.GasMassFlow (gcd, g, gk, r, gz, gtemp, gm, gp1, gp2, ga); 
  em.MassFlowConvert (mf);
  Serial.println ("GasFlow," +
                (String)dtostrf (mf[0], 2,15, s) + "," +   // kg/sec
                (String)dtostrf (mf[1], 3,10, s) + "," +   // kg/min 
                (String)dtostrf (mf[2], 3,15, s) + "," +   // lbs/sec
                (String)dtostrf (mf[3], 3,15, s));          // lbs/min
  delay(100000);
}
