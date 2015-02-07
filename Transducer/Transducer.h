/*
 Title: Transducer.h
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
	Change Log:
		GNS 2013-07-28: updated psi to reflect atmospheric (psia)
			rather than gauge
*/
#ifndef Transducer_h
#define Transducer_h

#include "Arduino.h"

class Transducer
{
  public:
    Transducer ();	
	void getPressures (int analogSignalF, float pressures[]);
    float getVoltage (float analogSignal);
    float getPSI (float voltage);
	float getPa (float psi);
	float getMPa (float psi);
};

#endif