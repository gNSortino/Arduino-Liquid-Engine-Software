# Arduino-Liquid-Engine-Software

## Control and Data Capture Software for My Liquid Fueled Engines

Author: Graham N. Sortino
Web Site: http://wiki.fubarlabs.org/fubarwiki/Small-Liquid-Fueled-Rocket-Engines.ashx
Git Hub URL: https://github.com/gNSortino/Arduino-Liquid-Engine-Software

## Start Here
This file contains information about the contents of the software included in this
repository. It also contains some quick usage instructions. I try to keep most of 
the documentation w/in the libraries themselves in order to prevent the readme
file from getting out of date so please consultant the relevant libraries 
for further information.

## What is This???
Over the past few years I developed a small liquid fueled rocket engine and some 
softawre that was used to control/analyze it. The results of which can be found 
here. The software was written to run on an Arduino Uno (rev3) micro-controller 
but could easily be ported to another micro. 

## What is in Here???
Many librarires for controlling and measuring my rocket engine and igniter plus a main EngineController.ino piece of code. The libraries may be generally useful in other projects, however, EngineController.ino is fairly bespoke to my needs. Each library is fully documented and contains it's own demo function to show how it works. So, for example, if you are just interested in using the Thermocouple libraries to handle temperature for a project then you can just 
grab the relevant piece of code without worrying about doing a complicated build process. 

## Libraries
* **ThermoTemp -** Reads the output of a type K thermocouple connected to an AD595AQ thermocouple. See
the [datasheet](http://www.analog.com/static/imported-files/data_sheets/AD594_595.pdf) for more info.
The library calculates temperature in Celsius, Kelvin, Fahrenheit, and Rankine. The sample routine
included in this library outputs the aforementioned data in CSV format.

* **MAX31855 -** reads the temperature of the adafruit [MAX31855 w/ associated breakout board](http://www.adafruit.com/product/269). This is an adafruit library (not mine) but I put it here for convenience.

* **Transducer -** Reads data from an analog pressure transducer and outputs Pounds Per Square Inch (PSI), 
Pascal (Pa), and Mega Pascal (MPa). The library was specifically designed to work with 2 brands of 
pressure transducers: SSI 1000psi absolute gauge transducer and MSI 1000psi Ratio Metric. However,
it could easily be converted to support other vendors.

* **EngineMath -** This sophisticated library calculates the thermodynamic properties of Gas and Liquid
mass flow given pressure, orifice area, density, and a few other relevant parameters. It was used
to calculate the mass-flow characters of my engine in real-time. The sample library outputs a variety
of engine data-points in a CSV format

* **LoadCell -** This library will take the input from an FC22 MSI Load Cell (0.5V - 4.5V) and convert it into lbf. However, it could easily be configured to work with other load cells.

* **PMCtrl -** This library, while included for convenience, is maintained on a separate [GitHub repository](https://github.com/gNSortino/PMCtrl). It is an interface between the Arduino and the [Pololu Maestro Server controller](https://www.pololu.com/product/1350)

* **StopWatch -** This library performs the basic functions of a stop watch and is used to simplify the process of keeping track of time on an arduino.

* **EngineController -** This is the main library and is responsible for controlling the engine and
sending back relevant data. It performs the following functions (see software documentation for 
full details).
	1. Test Sensors
	2. Test Valves + Spark Igniter
	3. Manual Valve Check
	4. Set Orifice Diameters for Measurements
	5. Run Engine

It also has various safety features built in to help mitigate any dangerous conditions.
