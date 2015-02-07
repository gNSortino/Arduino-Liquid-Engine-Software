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
 Change Log:
	GNS 2013-07-28: Corrected incorrect gas-flow calculation. Removed input
		value for static constant 'r' Gas Coefficient
	GNS 2014-01-20: added thrustCalc method to return the engine thrust in lbf
	GNS 2014-05-18: added support for dynamic calculation of choked and non-choked gas flow
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
									float den,  	// Liquid Density (kg/m^3)
									float p1,  	// Inlet Pressure (psi) 
									float p2,  	// Outlet Pressure (psi)
									float a)  		// Orifice Area (m^2)
{
  //Convert psi to kg/(m*s^2)
  p1 = (p1*6894.75729);
  p2 = (p2*6894.75729);
  float pdrop = p1 - p2;  //pressure drop across orifice
  float lasmf =a*(cd*sqrt(2*pdrop*den));
  return lasmf;
}

/*
	Function determines whether flow is choked (or not) and then 
	calculates flow appropriately.
*/
float EngineMath::GasMassFlow (	float cd,  		// Coefficient of Discharge (Dimensionless)
								float g,    	// Gravity 9.80665 (m/sec^2)
								float k,   		// Gas Specific Heat Ratio (Dimensionless)
								float z,    	// Gas Compressability Factor (Dimensionless) 
								float temp, 	// Gas Temperature at inlet (Kelvin)
								float m,    	// Gas Molecular Mass  (mol)
								float p1,  		// Inlet Pressure (psi) 
								float p2,  		// Outlet Pressure (psi)
								float a)   		// Orifice Area (m^2)
{
  float r = 8314.4621;	// 	J/Kg^-1*mol^-1
  //Convert psi to kg/(m*s^2)
  p1 = (p1*6894.75729);
  p2 = (p2*6894.75729);
  float pratio = p2 /p1;
  float pcritical = pow((2 / (k + 1)),( k / (k -1)));
  float density = p1  / (z * (r / m) * temp);
  float gasmf;
  if ((pcritical * p1) > p2) // choked
	gasmf = cd * a * sqrt( k * density * p1 * pow ((2 / (k + 1)), ((k + 1) / (k - 1))));
  else // non-choked
	gasmf = a*(cd*p1*sqrt(((2*m*g)/(z*r*temp))*(k/(k-1)) * (pow(pratio,(2/k))- pow(pratio,((k+1)/k)))));
  return gasmf;
}

/*
	Returns thrust in lbf given the below inputs
*/
float EngineMath::thrustCalc (	float k,		// specific heat ratio for the engine
								float p1PSI, 	// chamber pressure (PSI)
								float p2PSI, 	// exit pressure (PSI)
								float p3PSI, 	// atmospheric pressure (PSI)
								float aExit, 	// Nozzle exit area m^2
								float aThroat)	// Nozzle throat area m^2
{
	float p1Mpa = p1PSI * 0.00689475729;
	float p2Mpa = p2PSI * 0.00689475729;
	float p3Mpa = p3PSI * 0.00689475729;
	float Cf = sqrt(((2.0 * pow(k,2.0))/(k - 1.0)) * pow(2.0 / (k + 1.0), (k + 1.0)/(k - 1.0)) * (1.0 - pow(p2Mpa / p1Mpa, (k - 1.0) / k) )) + ((p2Mpa - p3Mpa)/ p1Mpa)*(aExit / aThroat);
	return Cf * p1Mpa * aThroat * 1000000.0 * 0.22481;
}