/*
 Title: StopWatch (Demo)
  Author/Date: gNSortino@yahoo.com / 2014-01-14
  Description: This is a demo library that shows how to
        use the features of the StopWatch library

	Function descriptions can be found in the .cpp file
	of the same name.
*/

#include "StopWatch.h"


StopWatch sw;

void setup ()
{
	Serial.begin(115200);
}

void loop ()
{
	Serial.println ("Commence 10,000 millisecond timer countdown");  
	sw.startTimer(10000);
	while (sw.timerStatus() == true)
	{
		Serial.println (sw.timeElapsed());
		sw.millisToSleep (1000);
	}
	Serial.println ("Countdown Completed");  	
	sw.millisToSleep (1000);
	Serial.println ("waiting for a bit...");
	sw.millisToSleep (3999);
        Serial.print (sw.timeElapsed());
        Serial.println (" (Should be: 10599)");
	Serial.println ("Done!\n");
	sw.millisToSleep (2500);
}
