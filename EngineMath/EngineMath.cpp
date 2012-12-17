/*
 Title: EngineMath.cpp
  Author/Date: gNSortino@yahoo.com / 2012-05-26
  Description: This library is used to calculate properties
	of a rocket engine. Currently it will calculate the following
	properties
		(1) GasMassFlow. 
			-> Returns kg/sec as a float 
		(2) LiquidMassFlow. 
			-> Returns kg/sec as a float 
		(3) MassFlowConvert. Convers kg/sec to:
			-> [0] kg/sec 
			-> [1] kg/min
			-> [2] lbs/sec
			-> [3] lbs/min
    Note that this library will not setup any pins. It
	is expected that these will be defined by the calling 
	program.
*/

#include "Arduino.h"
#include "EngineMath.h"

//Empty Constructor
EngineMath::EngineMath()
{

}

/*
	Note that the array mf is passed by reference so
	the calling function should use this to view results.
*/
void EngineMath::MassFlowConvert (	float mf[])	// Mass Flow array
{
  mf[0] = mf[0];				// kg/sec
  mf[1] = mf[0] * 60.0;  		// kg/min
  mf[2] = mf[0] *2.20462262;	// lbs/sec
  mf[3] = mf[2] * 60;			// lbs/min
}

float EngineMath::LiquidMassFlow ( float cd,   	// Coefficient of Discharge (Dimensionless)
									float g,    	// Gravity 9.80665 (m/sec2)
									float den,  	// Liquid Density (kg/m^3)
									float p1,  	// Inlet Pressure (psi) 
									float p2,  	// Outlet Pressure (psi)
									float a)  		// Orifice Area (m^2)
{
  //Convert psi to kg/(m*s^2)
  p1 = (p1/0.001422);
  p2 = (p2/0.001422);
  float pdrop = p1 - p2;  //pressure drop across orifice
  float lasmf =a*(cd*sqrt(2*g*pdrop*den));
  return lasmf;
}

float EngineMath::GasMassFlow (	float cd,  		// Coefficient of Discharge (Dimensionless)
									float g,    	// Gravity 9.80665 (m/sec^2)
									float k,   		// Gas Specific Heat Ratio (Dimensionless)
									float r,   		// Universal Gas constant 8,314.4621000 (J/kg mol-K)
									float z,    	// Gas Compressability Factor (Dimensionless) 
									float temp, 	// Gas Temperature at inlet (Kelvin)
									float m,    	// Gas Molecular Mass  (mol)
									float p1,  	// Inlet Pressure (psi) 
									float p2,  	// Outlet Pressure (psi)
									float a)   	// Orifice Area (m^2)
{
  //Convert psi to kg/(m*s^2)
  p1 = (p1/0.001422);
  p2 = (p2/0.001422);
  float pratio = p2 /p1;
  float gasmf = a*(cd*p1*sqrt((2*m*g)/(z*r/10*temp)*(k/(k-1)) * (pow(pratio,(2/k))- pow(pratio,((k+1)/k)))));
  return gasmf;
}