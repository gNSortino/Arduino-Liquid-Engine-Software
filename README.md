Arduino-Liquid-Engine-Software
==============================

Control and Data Capture Software for My Liquid Fueled Engines

Author: Graham N. Sortino
Web Site: http://wiki.fubarlabs.org/fubarwiki/Small-Liquid-Fueled-Rocket-Engines.ashx
Git Hub URL: https://github.com/gNSortino/Arduino-Liquid-Engine-Software

**Start Here**
This file contains information about the contents of the software included in this
repository. It also contains some quick usage instructions. I try to keep most of 
the documentation w/in the libraries themselves in order to prevent the readme
file from getting out of date so please consultant the relevant libraries 
for further information.

**What is This???**
Over the past year I developed a small liquid fueled rocket engine and some 
softawre that was used to control/analyze it. The results of which can be found 
here. The software was written to run on an Arduino Uno (rev3) micro-controller 
but could easily be ported to another micro. 

**What is in Here???**
With the exception of "EngineMath" all of the software libraries are stored in their own 
folder and operate as self sustained libraries. So, for example, if you are just interested 
in using the Thermocouple libraries to handle temperature for a project then you can just 
grab the relevant piece of code without worrying about doing a complicated build process. 
Additionally, each library contains a starter program used to show how to call the variuos 
routines.

There is also the main library called igniter launch sequence which is used to actually
control the liquid fueled rocket engine.

**Libraries**
ThermoTemp - Reads the output of a type K thermocouple connected to an AD595AQ thermocouple. See
the datasheet: http://www.analog.com/static/imported-files/data_sheets/AD594_595.pdf for more info.
The library calculates temperature in Celsius, Kelvin, Fahrenheit, and Rankine. The sample routine
included in this library outputs the aforementioned data in CSV format.

Transducer - Reads data from an analog pressure transducer and outputs Pounds Per Square Inch (PSI), 
Pascal (Pa), and Mega Pascal (MPa). The library was specifically designed to work with 2 brands of 
pressure transducers: SSI 1000psi absolute gauge transducer and MSI 1000psi Ratio Metric. However,
it could easily be converted to support other vendors.

Engine Math - This sophisticated library calculates the thermodynamic properties of Gas and Liquid
mass flow given pressure, orifice area, density, and a few other relevant parameters. It was used
to calculate the mass-flow characters of my engine in real-time. The sample library outputs a variety
of engine data-points in a CSV format

igniter_launch_sequence - This is the main library and is responsible for controlling the engine and
sending back relevant data. It performs the following functions (see software documentation for 
full details).
	1. Nitrogen Purge
	2. Run Engine (Hot Fire)
	3. Test Spark Igniter
	4. Test Valves
	5. Test Sensors (CSV sensor output)
	6. Valve Response Time Test
	7. Igniter Pulse Test (Engine Pulsing. Not Currently Used)
	8. Heat Glow Plug (not currently used) 
