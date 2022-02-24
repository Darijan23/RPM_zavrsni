/***************************************************
 * HDC2010 library class v0.21 / 20190808
 * HDC2010 high accuracy temperature and humidity sensors are manufactured by Texas Instruments.
 *
 * This HDC2010 temperature and humidity class it's based on class developed by Brandon Fisher (see bellow). 
 * Thank you Brandon! Great job! 
 * We've just add add some variables, functions and functionalities.
 *
 * This library it's compatible with:
 *		s-Sense HDC2010 I2C sensor breakout [PN: SS-HDC2010#I2C, SKU: ITBP-6005], info https://itbrainpower.net/sensors/HDC2010-TEMPERATURE-HUMIDITY-I2C-sensor-breakout 
 *		s-Sense CCS811 + HDC2010 I2C sensor breakout [PN: SS-HDC2010+CCS811#I2C, SKU: ITBP-6006], info https://itbrainpower.net/sensors/CCS811-HDC2010-CO2-TVOC-TEMPERATURE-HUMIDITY-I2C-sensor-breakout
 * 
 * 
 * 
 * HDC2010 definitions are placed bellow, in lines 44-46
 *
 * READ HDC2010 documentation! https://itbrainpower.net/downloadables/hdc2010.pdf
 * 
 * You are legaly entitled to use this SOFTWARE ONLY IN CONJUNCTION WITH s-Sense HDC2010 I2C sensors DEVICES USAGE. Modifications, derivates and redistribution 
 * of this software must include unmodified this COPYRIGHT NOTICE. You can redistribute this SOFTWARE and/or modify it under the terms 
 * of this COPYRIGHT NOTICE. Any other usage may be permited only after written notice of Dragos Iosub / R&D Software Solutions srl.
 * 
 * This SOFTWARE is distributed is provide "AS IS" in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *  
 *  
 * itbrainpower.net invests significant time in design phase of our IoT products and in associated software and support resources.
 * Support us by purchasing our environmental and air quality sensors from here https://itbrainpower.net/order#s-Sense
 *
 *
 *
 * Dragos Iosub, Bucharest 2019.
 * https://itbrainpower.net
 */
/*
	HDC2010.h
	Created By: Brandon Fisher, August 1st 2017
	
	This code is release AS-IS into the public domain, no guarantee or warranty is given.
	
	Description: This header file accompanies HDC2010.cpp, and declares all methods, fields,
	and constants used in the source code. 
*/

#define HDC2010_I2C_ADDR 0x40
#define HDC2010_IRQ_PIN 	6


#ifndef HDC2010_H
#define HDC2010_h

#include <Arduino.h>

#ifdef  __SAMD21G18A__
  #define DebugPort SerialUSB
#else
  #define DebugPort Serial 
#endif

#include <Wire.h>

//  Constants for setting measurement resolution
#define FOURTEEN_BIT 0
#define ELEVEN_BIT 1
#define NINE_BIT  2

//  Constants for setting sensor mode
#define TEMP_AND_HUMID 0
#define TEMP_ONLY	   1
#define HUMID_ONLY	   2
#define ACTIVE_LOW	   0
#define ACTIVE_HIGH	   1
#define LEVEL_MODE		0
#define COMPARATOR_MODE 1

//  Constants for setting sample rate
#define MANUAL			0
#define TWO_MINS		1
#define ONE_MINS		2
#define TEN_SECONDS		3 
#define	FIVE_SECONDS	4
#define ONE_HZ			5
#define TWO_HZ			6
#define FIVE_HZ			7	

class HDC2010
{
	public:
		HDC2010(uint8_t addr);					// Initialize the HDC2010
		void begin(void);  						// Join I2C bus
		boolean checkSensorType(void);				// check sensor type ID to be 0x07D0 
		boolean checkSensorManufacturer(void);		// check sensor Manufacturer ID to be 0x5449 
		float readTemp(void);					// Returns the temperature in degrees C
		float readHumidity(void);				// Returns the relative humidity
		void enableHeater(void);				// Enables the heating element
		void disableHeater(void);				// Disables the heating element
		void setLowTemp(float temp);			// Sets low threshold temperature (in c)
		void setHighTemp(float temp);			// Sets high threshold temperature (in c)
		void setHighHumidity(float humid);		// Sets high Humiditiy threshold
		void setLowHumidity(float humid);		// Sets low Humidity threshold 
		float readLowHumidityThreshold(void);	// Returns contents of low humidity threshold register
		float readHighHumidityThreshold(void);	// Returns contents of high humidity threshold register
		float readLowTempThreshold(void);		// Returns contents of low temperature threshold register (in C)
		float readHighTempThreshold(void);		// Returns contents of high temperature threshold register (in C)
		void triggerMeasurement(void);			// Triggers a manual temperature/humidity reading
		void reset(void); 						// Triggers a software reset
		void enableInterrupt(void);				// Enables the interrupt/DRDY pin
		void disableInterrupt(void); 			// Disables the interrupt/DRDY pin (High Z)
		uint8_t readInterruptStatus(void); 		// Reads the status of the interrupt register
		void clearMaxTemp(void);				// Clears the Maximum temperature register
		void clearMaxHumidity(void);			// Clears the Maximum humidity register
		float readMaxTemp(void); 				// Reads the maximum temperature register
		float readMaxHumidity(void);			// Reads the maximum humidity register
		void enableThresholdInterrupt(void);	// Enables high and low temperature/humidity interrupts
		void disableThresholdInterrupt(void);	// Disables high and low temperature/humidity interrupts
		void enableDRDYInterrupt(void);			// Enables data ready interrupt
		void disableDRDYInterrupt(void);		// Disables data ready interrupt
		
		
		
		/* Sets Temperature & Humidity Resolution, 3 options
		   0 - 14 bit
		   1 - 11 bit
		   2 - 9 bit
		   default - 14 bit							*/
		void setTempRes(int resolution);		
		void setHumidRes(int resolution);	

		/* Sets measurement mode, 3 options
		   0 - Temperature and Humidity
		   1 - Temperature only
		   2 - Humidity only
		   default - Temperature & Humidity			*/
		void setMeasurementMode(int mode);
		
		/* Sets reading rate, 8 options
		   0 - Manual
		   1 - reading every 2 minutes
		   2 - reading every minute
		   3 - reading every ten seconds
		   4 - reading every 5 seconds
		   5 - reading every second
		   6 - reading at 2Hz
		   7 - reading at 5Hz
		   default - Manual		*/		
		void setRate(int rate);
		
		/* Sets Interrupt polarity, 2 options
		   0 - Active Low
		   1 - Active High
		   default - Active Low			*/		
		void setInterruptPolarity(int polarity);
		
		/* Sets Interrupt mode, 2 options
		   0 - Level sensitive
		   1 - Comparator mode
		   default - Level sensitive	*/		
		void setInterruptMode(int polarity);
		
		/*value..read manual*/
		void setTemperatureOffset(uint8_t value);

		
	private:
		int _addr; 									// Address of sensor 
		void openReg(uint8_t reg); 	    			// Points to a given register
		uint8_t readReg(uint8_t reg);				// Reads a given register, returns 1 byte
		void writeReg(uint8_t reg, uint8_t data); 	// Writes a byte of data to one register
		

};


#endif