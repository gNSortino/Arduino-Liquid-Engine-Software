#include <EngineMath.h>
#include <Transducer.h>
#include <ThermoTemp.h>
#include <string.h>
#include <stdio.h> //For dtostrf function http://www.nongnu.org/avr-libc/user-manual/group__avr__stdlib.html#ga060c998e77fb5fc0d3168b3ce8771d42
/*
  Title: Igniter Launch Sequence
  Author/Date: gNSortino@yahoo.com / 2012-08-19
  Description: This code is for safely lighting my
    aluminum liquid fueled rocket engine, which generates
    5lbf of thrust. It is actuated via 3 solenoid valves
    and a spark plug (ignition source). The spark plug
    uses Pulse Width Modulation to generate a spark
  
  Acknowledgements: PWM Code was generated with the help 
    of the following resources:
      http://www.arduino.cc/playground/Main/TimerPWMCheatsheet
      http://arduino.cc/en/Tutorial/SecretsOfArduinoPWM   
    Rick suggested using F("String") to write strings to flash mem instead of wasting main memory
*/
////////////////////////////////////

/*Declare Global Variables*/
//Main Vars (Configurable)
int solenoidPurge = 12;
int solenoidFuelAcutator = 9;
int solenoidOxActuator = 10;
int igniterPin = 3;              // Choices are 3 or 11
int tempPin = A0;                // Thermocouple Analog Pin
int tempPowerPin = 13;           // Thermocouple 5V power pin
int pressurePin = A2;            // Pressure Transducer Analog Pin
int glowControl = 8;             // Control Pin for the Glow Plug Relay.
//Gas Flow Properties (Compressed Air)
float gcd = 0.7;                 // Coefficient of Discharge (Dimensionless)
float  gk = 1.227;               // Gas Specific Heat Ratio (Dimensionless)
float gz = 0.975;                // Gas Compressability Factor (Dimensionless) 
float gtemp = 296.0;             // Gas Temperature at inlet (Kelvin)
float gm = 28.9;                 // Gas Molecular Mass  (mol)
float gp2 = 300;                 // Gas Outlet Pressure (psi)
float ga = 0.0000011133709258;   // Orifice Area (m^2) =((PI()*((3/64")/2)^2)*0.00064516)
//Liquid Flow Properties (Ethanol)
float lcd = 0.6;   	         // Coefficient of Discharge (Dimensionless)
float lden = 785; 	         // Liquid Density (kg/m^3)
float lp2 = 200;  	         // Gas Outlet Pressure (psi)
float la = 5.06707479E-08;       // Orifice Area (m^2) =(PI()*(0.010"/2)^2)*0.00064516
//Additonal Vars (Not Typically Changed)
float pressures[5];              // Thermocouple Pressure Array
float gasmf[4];                  // Gas Mass Flow Array
float liquidmf[4];               // Liquid Mass Flow Array
float temps[6];                  // Temperature Array
EngineMath em;
Transducer transducer;
ThermoTemp thermoTemp;
char s[64];                      // buffer for dtostrf
char out[128];                   // Text array to hold output data so that we don't need to use 
                                 // the String data type, which apparently eats a lot of memory
float raw;                       // holds raw analog read

float r = 8314.4621;             // Universal Gas Constant (J/kg mol-K)
float g = 9.80665;               // Gravity m/sec^2
int prescalerVal = 0x07;
int igniterDutyCycle = 100;      // (values between 0-255)
int serialData;
int inbyte;

void setup ()
{
  //Set our Comms Speed
  Serial.begin(57600);
  
  //Setup our pins
  pinMode (solenoidPurge, OUTPUT);
  pinMode (solenoidFuelAcutator, OUTPUT);
  pinMode (solenoidOxActuator, OUTPUT);
  pinMode (igniterPin, OUTPUT);
  pinMode (tempPowerPin, OUTPUT);
  pinMode (glowControl, OUTPUT);
  digitalWrite(tempPowerPin, HIGH);
  
  
  //Configure Arduino for Fast non-inverted PWM
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  
  //Clear Timer 2 Prescaler Bits
  bitClear(TCCR2B, CS22);
  bitClear(TCCR2B, CS21);
  bitClear(TCCR2B, CS20);

  //Set the prescaller:
  TCCR2B = TCCR2B & 0b11111000 | prescalerVal;
  
  //Set the duty cycle timings (values between 0-255) for 
  //Pins 3 and 11 respectively
  OCR2A = -1;  //67;     // start at -1 so igniter is off
  OCR2B = -1; //67;      // ""
}


//This is the main function, which performs the following tasks:
  //1) Pressurization of Liquid Fuel
  //2) Start Igniter
  //3) Start Oxidizer Flow
  //4) Start Fuel Flow (At this point ignition should begin)
  //5) Stop all flow and turn off igniter.
void loop () 
{
  //set variables:
  long counter = 0;                  // Generic counter variable for various timers (specified in mili-seconds)
  int cycletime = 10;                // Time for each cycle (specified in miliseconds)
  int countdown_timer = 5;           // Engine fire countdown timer
  unsigned long startTime;           // A timer variable
  
  
  delay(1000);
  strcpy (out,  "[1] Nitrogen Purge:");
  Serial.println(out);
  strcpy (out,  "[2] Run engine\n"
                "[3] TestSparkIgniter.");
  Serial.println(out);
  strcpy (out,  "[4] TestValves.\n"
                "[5] TestSensors.");
  Serial.println(out);
  strcpy (out,  "[6] ValveResponseTimeTest.\n"
                "[7] IginterPulseTest");              
  Serial.println(out);
  strcpy (out,  "[8] HeatGlowPlug:");
  Serial.println(out);
  
  getSerial(0);
  switch (serialData)
  {
      case 1:
      {
        //Nitrogen Purge
        strcpy(out, "Run Purge For (in sec): ");
        Serial.println (out);
        getSerial(0);
        Serial.print (strcpy(out, "Purging... "));
        controlValve (solenoidPurge, true);
        //May need to add logic to convert serialdata to int
        counter = int(serialData);
        while (counter > 0)
        {
          Serial.print (strcpy (out, strcat (itoa(counter, out, 10), ", ")));
          delay (1000); //1 second timer
          counter = counter - 1;
        }
        controlValve (solenoidPurge, false);
        Serial.println (strcpy(out, "Done."));
        break;
      }
      case 2:
      {
        //run engine
        heatGlowPlug(45, true);
        strcpy(out, "EngineRunTime(inMilisecs)? Enter[0]ToAbort: ");
        Serial.println (out);
        getSerial(0);
        //May need to add logic to convert serialdata to int
        counter = (int)serialData;
        if (counter <= 0)
        {
          Serial.println (strcpy (out,"Abort"));
          break;
        }
        Serial.println (strcpy(out,"SensorReadInterval(inMilis): "));
        getSerial(0);
        cycletime = serialData;
        delay (500);
        
        Serial.println (strcpy(out, "StartingEngineIn:"));
        while (countdown_timer > 0)
        {
          Serial.print (strcpy (out, strcat (itoa(countdown_timer, out, 10), ", ")));
          delay (1000); //1 second timer
          countdown_timer = countdown_timer - 1;
        }
        Serial.println ("");
        
        // Setup the timers
        startTime = millis();
        sensorRead();
        sensorWrite(true, 0);        
        sensorCycle(&startTime, 500, cycletime);
        
        controlIgniter (true);
        Serial.println ("IgniterStarted");
        controlValve (solenoidFuelAcutator, true);
        Serial.println ("FuelValveOpen... ");
        delay(50);
        controlValve (solenoidOxActuator, true);
        Serial.print ("OxValveOpen... Running\n");
        
        sensorCycle(&startTime, counter, cycletime);

        Serial.print ("Finished. Shutting down... ");
        controlValve (solenoidFuelAcutator, false);
        Serial.print ("FuelValveClosed ");
        controlValve (solenoidOxActuator, false);
        Serial.print ("OxValveClosed ");
        Serial.println ("GlowPlugOff");
        controlValve (glowControl, false);
        sensorCycle(&startTime, 1000, cycletime);
        controlIgniter (false);
        Serial.println ("IgniterOff.");
        Serial.println ("EngineShutdownComplete");
        
        
        //Safety Shutdown
        delay(250);
        controlIgniter (false);
        controlValve (glowControl, false);
        controlValve (solenoidFuelAcutator, false);
        controlValve (solenoidOxActuator, false);
        delay(250);
        controlIgniter (false);
        controlValve (glowControl, false);
        controlValve (solenoidFuelAcutator, false);
        controlValve (solenoidOxActuator, false);
        break;
    }
    case 3:  //Spark Igniter Test
    {
      Serial.println ("TestSparkIgniter:");
      controlIgniter (true);
      Serial.print ("SparkIgniterStarted: ");
      while (countdown_timer > 0)
      {
        Serial.print (strcpy (out, strcat (itoa(countdown_timer, out, 10), ", ")));
        delay(1000);
        countdown_timer = countdown_timer -1;
      }
      Serial.print ("TestComplete.");
      controlIgniter (false);
      Serial.println ("SparkIgniterIsOff");
      break;
    }
    case 4:  //Valve Test
    {  
      Serial.println (strcpy(out,"PurgeValveOpen"));
      controlValve (solenoidPurge, true);
      delay (2000);
      Serial.println (strcpy(out,"FuelActValveOpen"));
      controlValve (solenoidFuelAcutator, true);
      delay (2000);
      Serial.println (strcpy(out,"OxValveOpen"));
      controlValve (solenoidOxActuator, true);
      delay (2000);
      
      Serial.println (strcpy(out,"PurgeValveClosed"));
      controlValve (solenoidPurge, false);
      delay (2000);
      Serial.println (strcpy(out,"FuelActuatorValveClosed"));
      controlValve (solenoidFuelAcutator, false);
      delay (2000);
      Serial.println (strcpy(out,"OxValveClosed"));
      controlValve (solenoidOxActuator, false);
      break;
    }
    case 5:  // Sensor Test
    {
      Serial.println ("SensorRead.AnyKeyExits");
      getSerial(-1);
      break;
    }
    case 6:  // Valve Response Time Test
    {
      int valve;    // Valve to Open
      Serial.println (strcpy(out, "WhichValve?([1]Fuel,[2]Ox): "));
      getSerial(0);
      valve = serialData;
      Serial.println (strcpy(out,"SensorReadInterval(inMilis): "));
      getSerial(0);
      cycletime = serialData;
      Serial.println (strcpy(out,"ValeOpenTime(inMilis): "));
      getSerial(0);
      
      // Setup the timers
      startTime = millis();
      
      sensorRead();
      sensorWrite(true, 0);
      
      //Run the Test
      sensorCycle(&startTime, 500, cycletime);
      if (valve == 1)
        controlValve (solenoidFuelAcutator, true);
      else if (valve == 2)
        controlValve (solenoidOxActuator, true);
      Serial.println(strcpy(out,"ValveOpen"));
      sensorCycle(&startTime, (int)serialData, cycletime);
      if (valve == 1)
        controlValve (solenoidFuelAcutator, false);
      else if (valve == 2)
        controlValve (solenoidOxActuator, false);
      Serial.println("ValveClosed");
      sensorCycle(&startTime, 500, cycletime);
     Serial.println("ValveResponseTimeTestComplete.\n");
     break;
    }
   case 7:  // Igniter Pulse Test
    {
      heatGlowPlug(45, true);
      Serial.println (strcpy(out,"SensorReadInterval(inMilis): "));
      getSerial(0);
      cycletime = serialData;
      Serial.println (strcpy(out,"MaxPulseTime(inMilis): "));
      getSerial(0);
      
      while (countdown_timer > 0)
      {
        Serial.print (strcpy (out, strcat (itoa(countdown_timer, out, 10), ", ")));
        delay (1000); //1 second timer
        countdown_timer = countdown_timer - 1;
      }
      
      // Setup the timers
      startTime = millis();
      sensorRead();
      sensorWrite(true, 0);
      
      sensorCycle (&startTime, 500, cycletime);
      controlIgniter (true);
      Serial.print ("IgniterStarted. ");
      Serial.println ("BeginPulses:");
      igniterPulseStartup ((int)serialData, 750, &startTime, cycletime);

      Serial.println ("EngineFireFinished.TestComplete:");
      controlValve (solenoidFuelAcutator, false);
      controlValve (solenoidOxActuator, false);
      controlValve (glowControl, false);
      controlIgniter (false);
      sensorCycle(&startTime, 500, cycletime);

      //Safety Shutdown
      delay(250);
      controlIgniter (false);
      controlValve (glowControl, false);
      controlValve (solenoidFuelAcutator, false);
      controlValve (solenoidOxActuator, false);
      delay(250);
      controlIgniter (false);
      controlValve (glowControl, false);
      controlValve (solenoidOxActuator, false);
      break;
    }
     case 8:  // Glow Plug Heater
    { 
      heatGlowPlug(45, false);
      break;
    }
  }
}

//Reads Serial information from the user terminal until a newline character
//is received. Results are echoed back and stored in the serialData String
//if run_time is set to a value > 0 then the getSerial loop will print
//sensor data at a 1 second interval.
void getSerial(unsigned int run_time)
{
  serialData = 0; //clear any old serial data before proceeding.
  int newline = '/';
  unsigned long startMillis = millis();
  unsigned long currentMillis = startMillis;
  unsigned long prevMillis = currentMillis;
  unsigned long endMillis = currentMillis + (run_time * 1000);

  if (run_time != 0)
  {
      sensorRead();
      sensorWrite(true, currentMillis - startMillis);
  } 
  while (inbyte !=  newline)
  {
    inbyte = Serial.read(); 
    if (inbyte > 0 && inbyte != newline)
      serialData = serialData * 10 + inbyte - '0';
    //Serial.println(serialData);
    if ((currentMillis - prevMillis > 1000) && run_time != 0)
    {
      prevMillis = currentMillis;
      sensorRead();
      sensorWrite(false, currentMillis - startMillis);
      if ((currentMillis >= endMillis) && run_time > 0)
        break;
    }
    currentMillis = millis(); 
  }
  inbyte = 0;
  Serial.println(serialData);
}

//Simple function to control the Solenoid Valves
boolean controlValve (int valve, boolean state)
{
  if (state == true)
  {
    digitalWrite(valve, HIGH);
    return true;
  }
  else if (state == false)
  {
    digitalWrite(valve, LOW);
    return true;
  }

  return false;
}

//Simple function to control the Spark Igniter
boolean controlIgniter (boolean state)
{
 if (state == true)
 {
     // Turn igniter on
    OCR2A = igniterDutyCycle;
    OCR2B = igniterDutyCycle;
    return true;
 }
 else if (state == false)
 {
  //Turn igniter off
  OCR2A = -1;
  OCR2B = -1;
  return true;
 }
 else
   return false;
}

//Sensor Read. Reads Sensors and returns arrays containing:
  //Pressure Data
  //gas mass flow
  //liquid mass flow
  //thermocouple temperature
void sensorRead ()
{
  raw = analogRead(pressurePin);
  transducer.getPressures (raw, pressures);
  liquidmf[0] = em.LiquidMassFlow (lcd, g, lden,  pressures[2],  lp2, la);
  em.MassFlowConvert (liquidmf);
  gasmf[0] = em.GasMassFlow (gcd, g, gk, r, gz, gtemp, gm, pressures[2], gp2, ga); 
  em.MassFlowConvert (gasmf);
  thermoTemp.getTemps(analogRead(tempPin), temps);
}

//Outputs key sensor data (configure as needed)
void sensorWrite (boolean writeHeader, unsigned int cycleTime)
{
  if (writeHeader == true)
    Serial.println("CycleTime,Signal,Voltage,PSI,GF kg per sec, LF kg per sec, Temp in Celsius");
  Serial.print (cycleTime);                                                // Timepoint durring test cycle. Starts from 0 and calculated in ms
  Serial.print (", ");
  strcpy (out,  strcat (dtostrf (pressures[0], 2,3, s), ", "));            // Analog Signal
  strcat (out, dtostrf (pressures[1], 2,3, s));                            // Voltage 
  strcat (out, ", ");
  strcat (out, dtostrf (pressures[2], 2,3, s));                            // Pounds per Square Inch
  strcat (out, ", ");
  Serial.print (out);
  strcpy (out,  strcat (dtostrf (gasmf[0], 3,7, s), ", "));                // Gas Flow kg/sec
  strcat (out, dtostrf (liquidmf[0], 3,7, s));                             // Liquid Flow kg/sec
  strcat (out, ", ");
  strcat (out, dtostrf (temps[2], 6,4, s));                                // Temperature in Celsius
  Serial.println (out);
}

//Runs through a cycle of sensor reads/outputs for a give time, sensor interval and set of sensor data.
void sensorCycle (unsigned long *startMillis, int intervalMillis, int cycleMillis)
{
  unsigned long currentMillis = millis();
  unsigned long prevMillis = currentMillis;
  unsigned long endMillis = currentMillis + intervalMillis;
  
  sensorRead();
  sensorWrite(false, currentMillis - *startMillis);
  
  while (currentMillis < endMillis)
  {
    if (currentMillis - prevMillis > cycleMillis)
    {
      prevMillis = currentMillis;
      sensorRead();
      sensorWrite(false, currentMillis - *startMillis);
    }
    currentMillis = millis();
  }
}

// A different technique for starting the igniter. It consists of a series of short oxidizer/fuel pulses in order to build up pressure.
// Inputs are standard start/stop times and sensorData arrays. There are also 2 special new parameters called maxStartTime and maxPSI
// which are thresholds for the maximum time (in miliseconds) the engine will pulse for and the maximumPSI the engine will support.
// If either of these thresholds are broken the engine is shutdown
void igniterPulseStartup (int maxRunTime, int maxPSI, unsigned long *startMillis, int sensorCycleMillis)
{
  int pulseCycleOn = 25;                                    // time for each pulse of ox/fuel in miliseconds
  int pulseCycleOff = 100;                                  // time for each pulse of ox/fuel in miliseconds
  int pulseCycleTime = pulseCycleOn + pulseCycleOff;
  boolean ison = false;                                     // determines whether we turn the valves on or off
  unsigned long currentMillis = millis();
  unsigned long endMillis = currentMillis + maxRunTime;
  unsigned long prevMillis = currentMillis;
 
  sensorRead();
  sensorWrite(false, currentMillis - *startMillis);

  while (currentMillis < endMillis)
  {
    // Engine Pulse Cycle
    if ((currentMillis % pulseCycleTime) <= pulseCycleOn) 
    {
      if (ison == false)
      {
        Serial.println("on: " + (String) (currentMillis - *startMillis));
        controlValve (solenoidFuelAcutator, true);
        controlValve (solenoidOxActuator, true);
        ison = true;
      }  
    }
    else
    {
      if (ison == true)
      {
        Serial.println("off: " + (String) (currentMillis - *startMillis));
        controlValve (solenoidFuelAcutator, false);
        controlValve (solenoidOxActuator, false);
        ison = false;
      }  
    }
    
    // sensor cycle
    if (currentMillis - prevMillis > sensorCycleMillis)
    {
      prevMillis = currentMillis;
      sensorRead();
      sensorWrite(false, currentMillis - *startMillis);
    }
    
    currentMillis = millis();
  }
}
// A simple function to heat the glow plug for x seconds or until an interrupt is sent.
// if state = true then the glowplug is not turned off after the timer. If state = false
// then it is turned off.
void heatGlowPlug (int heatTime, boolean state)
{
  Serial.print ("GlowPlugOnFor: ");
  Serial.print (heatTime);
  Serial.println (" sesc.PressAnyKeyToInterrupt");
  controlValve (glowControl, true);
  getSerial(heatTime);
  if (state == false)
  {
    controlValve (glowControl, false);
    Serial.println ("GlowPlugOff");
  }
}
