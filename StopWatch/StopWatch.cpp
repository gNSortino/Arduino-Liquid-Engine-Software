/*
 Title: StopWatch.h
  Author/Date: gNSortino@yahoo.com / 2014-01-11
  Description: This library performs the basic functions
	of a stop watch and is used to simplify the process of 
	keeping track of time on an arduino. All methods are 
	documented in the body below

	This code uses unsigned longs which can store 32 bits
	or 4 bytes of information on an arduino uno. In practical
	terms this equates to:
		4,294,967.29 seconds (2^32 - 1)
		   71,582.79 minutes
		    1,193.05 hours 
		       49.71 days
	
  Revision History:
	(GNS) 2014-01-11: inital version
*/


#include "Arduino.h"
#include "StopWatch.h"

/*
	Empty Constructor
*/
StopWatch::StopWatch ()
{
}

/*
	sets a timer to run for 'x' milliseconds
*/
void StopWatch::startTimer (unsigned long millisToTime)
{
  _startTime = millis();
  _stopTime = _startTime + millisToTime;
}


/* 
	Returns true if the timer is still active. False otherwise
*/
boolean StopWatch::timerStatus ()
{
  if ( (millis() >= _startTime) && (millis() <= _stopTime) )
    return true;
  else
    return false;
}

/*
	Returns the elapsed time since the timer was started. 
*/
unsigned long StopWatch::timeElapsed ()
{
  return millis() - _startTime;
}

/*
	A sleep without delay function
*/
void StopWatch::millisToSleep (unsigned long sleepTime)
{
  unsigned long wakeupTime = millis() + sleepTime;
  
  while(true)
  {
    if (millis() >= wakeupTime)
      break;
  }
}