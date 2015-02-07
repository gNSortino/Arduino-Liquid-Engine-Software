/*
 Title: PMCtrl.h
  Author: gNSortino@yahoo.com
  Description: This library implements the main features
	of the Pololu Maestro. It was tested against the
	micro-maestro. Additional features will be added as
	needed/requested.
	
	The code is based off the Pololu user guide as
	well as the software serial** arduino library see
	the relevant links below:
		# Sofware Serial Library
			http://arduino.cc/en/Reference/SoftwareSerial
		# Pololu Arduino Tutorial
			http://forum.pololu.com/viewtopic.php?f=16&t=5696
		# Pololu User Guide
			http://www.pololu.com/docs/0J40/all#7.c  	
	
	**This code relies on the SoftwareSerial Arduino
	library. It is recommended that this library also be 
	#included in the calling code as the Arduino can
	sometimes have problems if this isn't done.
	
	Function descriptions and change history can be found in the 
	.cpp file of the same name.
*/


#ifndef PMCtrl_h
#define PMCtrl_h

#include "Arduino.h"
#include "SoftwareSerial.h"

class PMCtrl
{
	public:
		PMCtrl (int rxPin, int txPin, long baudRate);
		~PMCtrl();
		void setTarget (unsigned int pos, unsigned char channel, int deviceID);
		void setServoSpeed (unsigned int servoSpeed, unsigned char channel, int deviceID);
		void setAcceleration (unsigned int acceleration, unsigned char channel, int deviceID);
		void goHome (int deviceID);	
		unsigned int getPosition (unsigned char channel, int deviceID);
		unsigned int getErrors (unsigned char channel, int deviceID);
	private:
		int _rxPin;
		int _txPin;
		SoftwareSerial _serialCtrl;
};

#endif