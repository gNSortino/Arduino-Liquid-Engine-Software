/*
 Title: ThermoTemp.cpp
  Author/Date: gNSortino@yahoo.com / 2012-05-05
  Description: This library will take the input from a 
    K type thermocouple sensor and return the following
    values as an array:
		[0] raw interval
		[1] voltage (in milivolts)
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

#include "Arduino.h"
#include "ThermoTemp.h"

//Empty Constructor
ThermoTemp::ThermoTemp()
{

}

/*
	Note that the array tempsF is passed by reference so
	the calling function should use this to view results.
*/
void ThermoTemp::getTemps (int analogSignalF, float tempsF[])
{
  tempsF[0] = analogSignalF;
  tempsF[1] = getVoltage (tempsF[0]);  
  tempsF[2] = getCelsius (tempsF[1]);
  tempsF[3] = getKelvin (tempsF[2]);
  tempsF[4] = getFahrenheit (tempsF[2]);
  tempsF[5] = getRankine (tempsF[2]);
}

float ThermoTemp::getVoltage (float analogSignal)
{
  float voltage = ((5.0 * analogSignal) / 1024.0);
  return voltage;
  
}
float ThermoTemp::getCelsius (float voltage)
{
  float celsius = (voltage * 100);
  return celsius;
}

float ThermoTemp::getKelvin (float celsius)
{
  float kelvin = (celsius + 273.15);
  return kelvin;
}
float ThermoTemp::getFahrenheit (float celsius)
{
  float fahrenheit = (((celsius * 9) / 5.0) + 32);
  return fahrenheit;
}
float ThermoTemp::getRankine (float celsius)
{
  float rankine = (9.0 / 5.0) * (celsius + 273.15 );
  return rankine;
}