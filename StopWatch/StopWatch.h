/*
 Title: StopWatch.h
  Author/Date: gNSortino@yahoo.com / 2014-01-11
  Description: This library performs the basic functions
	of a stop watch and is used to simplify the process of 
	keeping track of time on an arduino.
	
	This code uses unsigned longs which can store 32 bits
	or 4 bytes of information on an arduino uno. In practical
	terms this equates to:
		4,294,967.29 seconds (2^32 - 1)
		   71,582.79 minutes
		    1,193.05 hours 
		       49.71 days

	Function descriptions can be found in the .cpp file
	of the same name.
*/


#ifndef StopWatch_h
#define StopWatch_h

#include "Arduino.h"

class StopWatch
{
	public:
		StopWatch ();
		void startTimer (unsigned long millisToTime);
		boolean timerStatus ();
		unsigned long timeElapsed ();
		void millisToSleep (unsigned long sleepTime);
	private:
		unsigned long _startTime;
		unsigned long _stopTime;
};

#endif