/*
 Title: ThermoTemp.h
  Author/Date: gNSortino@yahoo.com / 2012-05-05
  Description: This library will take the input from a 
    K type thermocouple sensor and return the following
    values as an array:
		[0] raw interval
		[1] voltage (in volts)
		[2] temperature in Celsius
		[3] temperature in Kelvin
		[4] temperature in Fahrenheit
		[5] temperature in Rankine
	Additionally this library has some handy methods 
	to calculate Kelvin, Fahrenheit, and Rankine given
	a Celsius measurement.
    Note that this library will not setup any pins. It
	is expected that these will be defined by the calling 
	program.
*/
#ifndef ThermoTemp_h
#define ThermoTemp_h

#include "Arduino.h"

class ThermoTemp
{
  public:
    ThermoTemp ();	
	void getTemps (int analogSignalF, float tempsF[]);
    float getVoltage (float analogSignal);
    float getCelsius (float voltage);
	float getKelvin (float clesius);
	float getFahrenheit (float celsius);
	float getRankine (float celsius); 
};

#endif