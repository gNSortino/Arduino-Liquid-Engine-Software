/*
 Title: Transducer.cpp
  Author/Date: gNSortino@yahoo.com / 2012-05-06
  Description: This library will take the input from an 
    SSI 1000psi (0.5V - 4.5V) sealed gauge transducer and 
	output
		[0] raw interval
		[1] voltage (in volts)
		[2] Pounds per Square Inch (PSI)
		[3] Pascal (Pa)
		[4] Mega Pascal (MPa)
	Additionally this library has some handy methods 
	to calculate pressure in different units given a 
	voltage or psi value
    Note that this library will not setup any pins. It
	is expected that these will be defined by the calling 
	program.
*/

#include "Arduino.h"
#include "Transducer.h"

//Empty Constructor
Transducer::Transducer()
{

}

/*
	Note that the array pressures is passed by reference so
	the calling function should use this to view results.
*/
void Transducer::getPressures (int analogSignalF, float pressures[])
{
  pressures[0] = analogSignalF;
  pressures[1] = getVoltage (pressures[0]);  
  pressures[2] = getPSI (pressures[1]);
  pressures[3] = getPa (pressures[2]);
  pressures[4] = getMPa (pressures[2]);
}

float Transducer::getVoltage (float analogSignal)
{
  float voltage = ((5.0 * analogSignal) / 1024.0);
  return voltage;
  
}

/*
    Calculation for SSI 1000psi absolute gauge transducer
    Part # P51-1000-S-B-I36-4.5V-000-000 is:
    y = 246.375x - 108.69
    Output is linear with the following values provided
    by the manufacturer:
    (x1,y1) = .5, 14.5 psi
    (x2,y2) = 4.5, 1000psi
	
	Calculation for MSI Ratio Metric:
	Part #: M5131-000005-01KPG (Max PSI = 1000, Gauge)
	Rule: For the 0.5V to 4.5V option (with a nominal 5.0V), 
	the sensor output is proportional to a change in supply 
	voltage over the specified supply voltage range (4.75V 
	to 5.25V), and is ±2% of FSO. Thus, over this range the 
	output voltage will be 4.5V ±90mV.
	Calculated as: 
	PSI = ((Vin/(1+Radj))-.05)/((4.5-0.5)/1000)
		Where Radj = (.02/0.25)*(Vpmeasure-Vpnominal)
			Where 	Vpmeasured = Voltage power supply measure and
					Vpmonminal = Voltage nominal power supply (eg. 5V)
*/
float Transducer::getPSI (float voltage)
{
  //Only one method is needed comment out as required.
  
  // SSI (1000 PSI linear)
  //float psi = (246.375 * voltage) - 108.69;
  
  // MSI (1000 PSI ratiometric)
  float radj = (0.2 / 0.25) * (4.93 - 5); //I'm hard coding the Vpmeasured for now...)
  float psi = ((voltage / (1 + radj)) -0.5) / 0.004;
  return psi;
}

/*
  1 pound per square inch = 6894.75729 pascals
*/
float Transducer::getPa (float psi)
{
   float pa = psi * 6894.75729;
   return pa;
}

/*
  1 pound per square inch = 0.00689475729 megapascals
*/
float Transducer::getMPa (float psi)
{
   float mpa = psi * 0.00689475729;
   return mpa;
}
