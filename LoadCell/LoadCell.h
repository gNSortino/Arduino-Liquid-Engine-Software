/*
 Title: LoadCell.h
  Author/Date: gNSortino@yahoo.com / 2014-07-26
  Description: This library will take the input from an 
    FC22 MSI Load Cell (0.5V - 4.5V) and convert it into lbf
	configuration inputs:
		inV:
			the supply voltage (should be 5 volts)
		noLoadCalcV:
			the no load calculated voltage
		loadMassV:
			the full load mass voltage
		loadMassLBF:
			the full load mass in pounds force
	measurement input:
		loadCellAnalogIn:
	output:
		mass:
			outputs the calculated mass given the aforementioned
			inputs.
    Note that this library will not setup any pins. It
	is expected that these will be defined by the calling 
	program.
	Change Log:
		GNS 2014-07-26: initial version
*/
#ifndef LoadCell_h
#define LoadCell_h

#include "Arduino.h"
#include "LoadCell.h"

class LoadCell
{
	public:
		LoadCell (float inV, float noLoadCalcV, float loadMassV, float loadMassLBF);	
		float getForce (int loadCellAnalogIn);
	private:
		float getVoltage (int loadCellAnalogIn);
		float _ratiometricScaleFactor;
		float _calibratedSpan;
		float _noLoadCalcV;

};

#endif