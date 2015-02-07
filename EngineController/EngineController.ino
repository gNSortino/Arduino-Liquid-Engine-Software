  /*
  Title: Engine Controller v2
  Author: gNSortino@yahoo.com
  Description: This code is for safely lighting my regenerative 
    engine. It is comprised of the main engine element and a small
    igniter engine. The main engine is actuated via 2 servo valves 
    and the igniter is actuated via 2 solenoid valves and ignited 
    via a spark plug. This code measures the following:
      - engine/igniter temp
      - engine pressure/flow
      - igniter pressure
      - fuel pressure/flow
      - ox pressure/flow
  Safety Features:
    - Upon resetting the code shuts off all vales
      and the ignition source.
    - Any input durring the test automatically aborts the engine
  Configuration: Configurable variables can be found in the 
    Global Vars section. Anything that can be configured will
    be documented.
  Revision History:
    2014-07-20 - Original Version
    2014-09-08 - Added support for dynamic input of orifice diameters
*/
////////////////////////////////////
#include <EngineMath.h>
#include <Transducer.h>
#include <Adafruit_MAX31855.h>
#include <StopWatch.h>
#include <SoftwareSerial.h>
#include <PMCtrl.h>
#include <LoadCell.h>

///////////////////////////////////////
// Start of Global Variables Section //
///////////////////////////////////////
// Pins (Configurable)
int servoWrite = 12;
int servoRead = 11;
int igniterPin = 10;
int solenoidFuelValve = 9;
int solenoidOxValve = 8;
int thermoDO = 7;
int igniterThermoCS = 6;    // Igniter Thermocouple Pin
int engineThermoCS = 5;     // Engine Thermocouple Pin
int thermoCLK = 4;
int fuelPSIpin = A0;
int oxPSIpin = A1;
int igniterPSIpin = A2;
int enginePSIpin = A3;
int loadCellPin = A4;

// mathematical constants
const float pi = 3.141592654;

// Servo Properties
int servoClosed = 800;
int servoOpened = 1600;
unsigned char fuelChannel = 0;
unsigned char oxChannel = 1;
int deviceID = 12;               //servo device ID # (Default is 12)


// Known Engine Properties (Configurable)
float kI = 1.155;	         // specific heat ratio for the igniter
float aThroatI = 0.00001371;	 // Nozzle throat area for the igniter m^2
float aExitI = 0.0000251; 	 // Nozzle exit area for the igniter m^2

// Known Engine Properties (Configurable)
float kE = 1.22;	         // specific heat ratio for the engine
float aThroatE = 0.00017;	 // Nozzle throat area for the engine m^2
float aExitE = 0.00038; 	 // Nozzle exit area for the engine m^2

// additional properties
float p2PSI = 14.696; 	         // exit pressure (PSI)
float p3PSI = 14.696; 	         // atmospheric pressure (PSI)

// Engine Gas (Ox) Flow Properties (Configurable)
float gcd = 0.32;                // Coefficient of Discharge (Dimensionless)
float gk = 1.40;                 // Gas Specific Heat Ratio (Dimensionless)
float gz = 0.98;                 // Gas Compressability Factor (Dimensionless). Typically .975
float gtemp = 277.0;             // Gas Temperature at inlet (Kelvin)
float gm = 32;                   // Gas Molecular Mass  (mol). Ox is 32, Nitrogen, 28.02, Air = 28.97
float gd = 0.141;                // Orifice Diameter in (in^2)
float ga = orificeArea(gd);      // Orifice Area (m^2)
           
// Engine Liquid (Fuel) Flow Properties (Configurable)
float lcd = 0.7;   	         // Coefficient of Discharge (Dimensionless) *** 0.65 igniter, .47 main engine
float lden = 800;     	         // Liquid Density (kg/m^3) *** 800 ethanol, 1000 water
float ld = 0.023;                // Orifice Diameter (in^2)
float la = orificeArea(ld);      // Orifice Area (m^2)

// Load Cell Calibration Parameters
float inV = 5.0;		        // input supply voltage
float noLoadCalcV = 0.547;		// no load calculated voltage (used for calibration)
float loadMassV = 4.0;			// the calibrated output voltage at full mass
float loadMassLBF = 100.0;		// the mass of the calibration input;

// do not edit past this line
float fuelPSI;
float fuelFlow;            // kg/sec
float oxPSI;
float oxFlow;              // kg/sec
float igniterPSI;
double igniterTemp;        // Celsius
float igniterForce;        // lbf
float enginePSI;
float engineFlow;          // kg/sec
double engineTemp;         // Celsius
float engineForceCalc;     // calculated value (lbf)
float engineForceSensor;   // read from a load cell (lbf)
float g = 9.80665;         // Gravity m/sec^2
long serialData;
StopWatch sw;
EngineMath em;
Transducer transducer;
Adafruit_MAX31855 igniterThermo(thermoCLK, igniterThermoCS, thermoDO); // igniter thermocouple
Adafruit_MAX31855 engineThermo(thermoCLK, engineThermoCS, thermoDO);   // engine thermocouple
PMCtrl servoCtrl (servoRead, servoWrite, 57600);                       // RX, TX, Baud
LoadCell loadCell (inV, noLoadCalcV, loadMassV, loadMassLBF);          // load cell calibration

//////////////////////////////////////
// End of Global Variables Section //
//////////////////////////////////////

void setup ()
{
  Serial.begin(57600); //57600 needed for xbee modules
  
  // Setup Pins
  pinMode (solenoidFuelValve, OUTPUT);
  pinMode (solenoidOxValve, OUTPUT);
  pinMode (igniterPin, OUTPUT);   
  
  // Set the global servo speed (Can be modified further below). Note - 50 is ~1 second to open and hgher numbers are faster
 //servoCtrl.setServoSpeed (25, fuelChannel, deviceID);
 //servoCtrl.setServoSpeed (200, oxChannel, deviceID);
}

void loop () 
{
  emergencyStop();
  
  //////////////////////
  ///// Main Menu /////
  /////////////////////
  Serial.println (F("(1) Test Sensors"));
  Serial.println (F("(2) Test Valves + Igniter"));
  Serial.println (F("(3) Manual Valve Check"));
  Serial.print (F("(4) Set Orifice Diameters (currently "));
  Serial.print (ld,3);
  Serial.print (F("/"));
  Serial.print (gd,3);
  Serial.println (F(" F/O in)"));
  Serial.println (F("(5) Run Engine"));

  getSerial();
  switch (serialData)
  {
      case 1: // Test Sensors
      {
        testSensors();
         break;
      }
      case 2: // Test Valves + Igniter
      {
        testControl();
        break;
      }
      case 3: // Manual Valve Check
      {
        manualValveCheck();
        break;
      }
      case 4: // Set the Orifice Diameters
      {
        setOrificeDiameters();
        break;
      }
      case 5: // Run Engine
      {
        runEngine();
        break;
      }
  }
}
////////////////////////
// Supporting Methods //
////////////////////////

/*
  Output Sensor Data until user interupt is received
*/
void testSensors()
{
  Serial.println(F("Press any key to interupt."));
  sw.millisToSleep(1500);
  sw.startTimer(0);
  sensorDisplay(true);
  while (isAbort() == false)
  {  
    sw.millisToSleep(2000);
    sensorDisplay(false);
  }
}

/*
  Runs through a series of valve and igniter tests to ensure they 
  are working correctly.
*/
void testControl()
{
  Serial.println(F("Opening fuel igniter valve"));
  digitalWrite(solenoidFuelValve, HIGH); 
  if (isAbortAutoCheck(2000) == true)
    return;

  Serial.println(F("Opening ox igniter valve"));
  digitalWrite(solenoidOxValve, HIGH); 
  if (isAbortAutoCheck(2000) == true)
    return;

  Serial.println(F("Turning on igniter"));
  digitalWrite(igniterPin, HIGH); 
  if (isAbortAutoCheck(2000) == true)
    return;
  
  Serial.println(F("Closing fuel igniter valve"));
  digitalWrite(solenoidFuelValve, LOW); 
  if (isAbortAutoCheck(2000) == true)
    return;

  Serial.println(F("Closing ox igniter valve"));
  digitalWrite(solenoidOxValve, LOW); 
  if (isAbortAutoCheck(2000) == true)
    return;

  Serial.println(F("Turning off igniter"));
  digitalWrite(igniterPin, LOW); 
  if (isAbortAutoCheck(2000) == true)
    return;
  
  Serial.println(F("Turning on engine fuel valve"));
  servoCtrl.setTarget (servoOpened, fuelChannel, deviceID);
  if (isAbortAutoCheck(3000) == true)
    return;

  Serial.println(F("Turning on engine ox valve"));
  servoCtrl.setTarget (servoOpened, oxChannel, deviceID);
  if (isAbortAutoCheck(3000) == true)
    return;

  Serial.println(F("Turning off engine fuel valve"));
  servoCtrl.setTarget (servoClosed, fuelChannel, deviceID);
  if (isAbortAutoCheck(3000) == true)
    return;

  Serial.println(F("Turning on engine ox valve"));
  servoCtrl.setTarget (servoClosed, oxChannel, deviceID);
  if (isAbortAutoCheck(3000) == true)
    return;


  Serial.println(F("On all at once"));
  digitalWrite(solenoidFuelValve, HIGH); 
  digitalWrite(solenoidOxValve, HIGH);
  digitalWrite(igniterPin, HIGH);
  servoCtrl.setTarget (servoOpened, fuelChannel, deviceID);
  servoCtrl.setTarget (servoOpened, oxChannel, deviceID);
  if (isAbortAutoCheck(5000) == true)
    return;
  
  Serial.println(F("Off all at once"));
  digitalWrite(solenoidFuelValve, LOW); 
  digitalWrite(solenoidOxValve, LOW);
  digitalWrite(igniterPin, LOW);
  servoCtrl.setTarget (servoClosed, fuelChannel, deviceID);
  servoCtrl.setTarget (servoClosed, oxChannel, deviceID);
}

/*
  Allows the user to manually open a valve for a set period of time  
*/
void manualValveCheck()
{
  unsigned long valveOpenTime;
  
  Serial.println (F("How long should the valve stay open?"));
  getSerial();
  valveOpenTime = serialData;
  
  Serial.println(F("Which valve? 1=IgniterFuel, 2=IgniterOx, 3=EngineFuel, 4=EngineOx"));
  getSerial();
  switch (serialData)
  {
      case 1:
      {
        digitalWrite(solenoidFuelValve, HIGH); 
        break;
      }
      case 2:
      {
        digitalWrite(solenoidOxValve, HIGH);
        break;
      }
      case 3:
      {
        servoCtrl.setTarget (servoOpened, fuelChannel, deviceID);
        break;
      }
      case 4:
      {
        servoCtrl.setTarget (servoOpened, oxChannel, deviceID);
        break;
      }
      default:
        return;
  }
  if (isAbortAutoCheck(valveOpenTime) == true)
    return;
}

/*
  Fires the engine for 'x' milliseconds
*/
void runEngine ()
{
    unsigned long engineRunTime;
    Serial.println(F("Run engine for how many milliseconds?"));
    getSerial();
    if (serialData <= 0)
      return;
    engineRunTime = serialData;
    Serial.print(F("Starting Engine in: "));
    for (int i=5; i>0; i--)
    {
      if (isAbortAutoCheck(1000) == true)
        return;
      Serial.print (i);
      Serial.print (F(", "));
    }
    Serial.print(F("\n"));
    digitalWrite(igniterPin, HIGH);
    sw.millisToSleep(425);
    digitalWrite(solenoidOxValve, HIGH);
    sw.millisToSleep(75);
    digitalWrite(solenoidFuelValve, HIGH); 
    sw.startTimer(engineRunTime);
    sensorDisplay(true);
    // run for up to 1000ms to give it a chance for pressure to come up
    while (sw.timeElapsed() < 1000 && (sw.timerStatus() == true))
    {
      if (isAbort() == true)
        return; 
      sensorDisplay(false);
    }
    Serial.println(F("Initial Startup Complete. Opening Main Valves..."));
    servoCtrl.setServoSpeed (25, fuelChannel, deviceID);
    servoCtrl.setServoSpeed (200, oxChannel, deviceID);
    servoCtrl.setTarget (servoOpened, fuelChannel, deviceID);
    servoCtrl.setTarget (servoOpened, oxChannel, deviceID);
    while (sw.timeElapsed() < 2000 && (sw.timerStatus() == true))
    {
      sensorDisplay(false);
      if (/*(isDanger(1) == true) OR */(isAbort() == true))
        return; 
      if ((servoCtrl.getPosition(fuelChannel, deviceID) >= servoOpened - 10) &&
          (servoCtrl.getPosition(oxChannel, deviceID) >= servoOpened - 10))
          break;
    }
    digitalWrite(solenoidFuelValve, LOW); 
    digitalWrite(solenoidOxValve, LOW);
    Serial.println(F("Main Valves Open. Firing..."));
    while (sw.timerStatus() == true)
    {
      sensorDisplay(false); 
      // Check for Dangerous Conditions or Aborts
      if (/*(isDanger() == true) OR */(isAbort() == true))
        return;
    }
    digitalWrite(solenoidFuelValve, LOW); 
    digitalWrite(solenoidOxValve, LOW);
    servoCtrl.setServoSpeed (200, fuelChannel, deviceID);
    servoCtrl.setServoSpeed (25, oxChannel, deviceID);
    servoCtrl.setTarget (servoClosed, fuelChannel, deviceID);
    servoCtrl.setTarget (servoClosed, oxChannel, deviceID);
    Serial.println(F("Run Complete. Shutting Down..."));
    digitalWrite(igniterPin, LOW);
    while ((igniterPSI > 30) && (sw.timeElapsed() < engineRunTime + 3000))
    {
      if (/*(isDanger(0) == true) OR */(isAbort() == true))
        return;
      sensorDisplay(false);
    }
}

/*
    Dynamically set the Orifice Diameters and calculate the resulting orifice Areas
*/
void setOrificeDiameters ()
{
  Serial.println (F("Orifice diameters are currently: "));
  Serial.print (ld,3);
  Serial.print (F("/"));
  Serial.print (gd,3);
  Serial.println (F(" F/O in. Type '1' to change")); 
  getSerial();
  
  if (serialData == 1) {
    Serial.println (F("Enter new fuel Orifice Diameter: "));
    getSerial();
    ld = (serialData / 1000.0);
    la = orificeArea (ld);
    Serial.println (F("Enter new oxidizer Orifice Diameter: "));
    getSerial();
    gd = (serialData / 1000.0);
    ga = orificeArea (gd);
    Serial.println (F("Orifice diameters are now: "));
    Serial.print (ld,3);
    Serial.print (F("/"));
    Serial.print (gd,3);
    Serial.println (F(" F/O in."));
  }
}
/*
  Gets Sensor data and sets the corresponding sensor values  
*/
void sensorRead()
{
  

  fuelPSI = transducer.getPSI(transducer.getVoltage(analogRead(fuelPSIpin)));
  oxPSI = transducer.getPSI(transducer.getVoltage(analogRead(oxPSIpin)));
  igniterPSI = transducer.getPSI(transducer.getVoltage(analogRead(igniterPSIpin)));
  enginePSI = transducer.getPSI(transducer.getVoltage(analogRead(enginePSIpin)));
  fuelFlow = em.LiquidMassFlow (lcd, lden, fuelPSI, enginePSI, la);
  oxFlow = em.GasMassFlow (gcd, g, gk, gz, gtemp, gm, oxPSI, enginePSI, ga);   
  igniterTemp = igniterThermo.readCelsius();
  igniterForce = em.thrustCalc(kI, igniterPSI, enginePSI, p3PSI, aExitI, aThroatI);
  engineFlow = oxFlow + fuelFlow; // Can this be made more sophisticated?
  engineTemp = engineThermo.readCelsius();
  engineForceCalc = em.thrustCalc(kE, enginePSI, p2PSI, p3PSI, aExitE, aThroatE);
  engineForceSensor = loadCell.getForce (analogRead(loadCellPin));
}

/*
  Displays sensor information to the client. An optional boolean flag
  if set to true tells the method to display the column headers
*/
void sensorDisplay(boolean showHeader)
{
  sensorRead();
  
  if (showHeader == true)
  {
    Serial.println(F("Millis,fuelPos(us),fuelPSI,fuelFlow(kg/sec),oxPos(us),oxPSI,oxFlow(kg/sec),igniterPSI,igniterTemp(C),igniterForce(lbf),enginePSI,engineFlow(kg/sec),engineTemp(C),engineForceCalc(lbf),engineForceSensor(lbf)"));
  }
  Serial.print(sw.timeElapsed());
  Serial.print ((char) ',');
  Serial.print (servoCtrl.getPosition(fuelChannel, deviceID));
  Serial.print ((char) ',');
  Serial.print(fuelPSI);
  Serial.print ((char) ',');
  Serial.print(fuelFlow,8);
  Serial.print ((char) ',');
  Serial.print (servoCtrl.getPosition(oxChannel, deviceID));
  Serial.print ((char) ',');
  Serial.print(oxPSI);
  Serial.print ((char) ',');
  Serial.print(oxFlow,8);
  Serial.print ((char) ',');
  Serial.print(igniterPSI);
  Serial.print ((char) ',');
  Serial.print(igniterTemp);
  Serial.print ((char) ',');
  Serial.print(igniterForce);
  Serial.print ((char) ',');
  Serial.print(enginePSI);
  Serial.print ((char) ',');
  Serial.print(engineFlow,8);
  Serial.print ((char) ',');
  Serial.print(engineTemp);
  Serial.print ((char) ',');
  Serial.print(engineForceCalc);
  Serial.print ((char) ',');
  Serial.println(engineForceSensor);
}

/*
  Reads Serial information from the user terminal until a newline character
  is received. Results are echoed back and saved to the serial buffer.
*/
void getSerial()
{
  int newline = '/';
  int inbyte;
  serialData = 0; //clear any old serial data before proceeding.

  while (inbyte !=  newline)
  {
    inbyte = Serial.read(); 
    if (inbyte > 0 && inbyte != newline)
      serialData = serialData * 10 + inbyte - '0';
  }
  Serial.println(serialData);
}

/*
  Reads from the user input serial buffer to see if any data 
  is present. If data is present assume the user is trying to
  abort the current operation and return true. Otherwise
  false is returned.
*/
boolean isAbort ()
{
  int inbyte;
  inbyte = Serial.read(); 
  if (inbyte > 0)
    return true;
  else
    return false;
}

/*
  A function that calls isAbort repeatedly for 'x' milliseconds.
  It will return true if an abort is detected.
*/
boolean isAbortAutoCheck (unsigned long sleepTime)
{
   sw.startTimer(sleepTime);
   while (sw.timerStatus() == true)
   {
     if (isAbort() == true)
     {
       return true;
     }
   }
   return false;
}

/*
  Checks for dangerous conditions. If one is found true is returned.
  False otherwise.
  0 = [Default] check engine and igniter
  1 = check igniter only
  2 = check engine only
  * = (Anthying Else) return true
*/
boolean isDanger(int toCheck = 0)
{
    switch (serialData)
  {
      case 0: // check engine and igniter
      {
        if ((isDanger(1) == true) && (isDanger(2) == true))
          return true;
        else 
          return false;
      }
      case 1: // check igniter only
      {
        if ((igniterPSI > 150) || (igniterTemp > 400)) // Check for excessive conditions
          return true;
        else if (igniterPSI < 50) // Check for no-go conditions
          return true;
        else 
          return false;
      }
      case 2: // check engine only
      {
        if ((enginePSI > 200) || (engineTemp > 400)) // Check for excessive conditions
          return true;
        else if (enginePSI < 50) // Check for no-go conditions
          return true;
        else 
          return false;
        break;
      }
      default: // (Anthying Else) return true
        return true;
  }
}

/*
  Shuts down all controllers (valves & igniter)
*/
void emergencyStop ()
{
    digitalWrite (solenoidFuelValve, LOW);
    digitalWrite (solenoidOxValve, LOW);
    digitalWrite (igniterPin, LOW);
    servoCtrl.setTarget (servoClosed, fuelChannel, deviceID);
    servoCtrl.setTarget (servoClosed, oxChannel, deviceID);
    sw.millisToSleep(50); 
    digitalWrite (solenoidFuelValve, LOW);
    digitalWrite (solenoidOxValve, LOW);
    digitalWrite (igniterPin, LOW);
    servoCtrl.setTarget (servoClosed, fuelChannel, deviceID);
    servoCtrl.setTarget (servoClosed, oxChannel, deviceID);
    sw.millisToSleep(100); 
    digitalWrite (solenoidFuelValve, LOW);
    digitalWrite (solenoidOxValve, LOW);
    digitalWrite (igniterPin, LOW);
    servoCtrl.setTarget (servoClosed, fuelChannel, deviceID);
    servoCtrl.setTarget (servoClosed, oxChannel, deviceID);
    sw.millisToSleep(500); 
    digitalWrite (solenoidFuelValve, LOW);
    digitalWrite (solenoidOxValve, LOW);
    digitalWrite (igniterPin, LOW);
    servoCtrl.setTarget (servoClosed, fuelChannel, deviceID);
    servoCtrl.setTarget (servoClosed, oxChannel, deviceID);
}


/*
  computes the orifice area in m^2 for a given orifice diameter supplied in in
*/
float orificeArea (float orificeDiameter)
{
  const float pi = 3.141592654;
  const float in2m2 = 0.00064516;
  return (pi * pow ((orificeDiameter / 2), 2) * in2m2);
}
