#include <LoadCell.h>

int sensorPin = A0;    			// force sensor input
int sensorValue;
float calcForce;

//Calibration Parameters
float inV = 5.0;				// input supply voltage
float noLoadCalcV = 0.500;		// no load calculated voltage (used for calibration)
float loadMassV = 4.0;			// the calibrated output voltage at full mass
float loadMassLBF = 100.0;		// the mass of the calibration input;
LoadCell loadCell (inV, noLoadCalcV, loadMassV, loadMassLBF);
  
void setup() {

  Serial.begin(57600);
  Serial.println ("Signal (0 - 1023),Force (lbf)");
}

void loop() {
  sensorValue = analogRead(sensorPin);
  calcForce = loadCell.getForce (sensorValue);
  Serial.print (sensorValue);	//Analog Signal
  Serial.print (",");
  Serial.println (calcForce,3);	//Force in lbf
  delay(2000);
}
