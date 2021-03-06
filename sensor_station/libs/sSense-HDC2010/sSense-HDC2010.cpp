/***************************************************
 * HDC2010 library class v0.3 / 20200624
 * HDC2010 high accuracy temperature and humidity sensors are manufactured by Texas Instruments.
 *
 * This HDC2010 temperature and humidity class it's based on class developed by Brandon Fisher (see bellow). 
 * Thank you Brandon! Great job! 
 * We've just add add some variables, functions and functionalities.
 *
 * ESP32 compatibility hints provided by Tibelea Eugen, Brasov, RO - Thank you, Eugen!
 *
 * This library it's compatible with:
 *		s-Sense HDC2010 I2C sensor breakout [PN: SS-HDC2010#I2C, SKU: ITBP-6005], info https://itbrainpower.net/sensors/HDC2010-TEMPERATURE-HUMIDITY-I2C-sensor-breakout 
 *		s-Sense CCS811 + HDC2010 I2C sensor breakout [PN: SS-HDC2010+CCS811#I2C, SKU: ITBP-6006], info https://itbrainpower.net/sensors/CCS811-HDC2010-CO2-TVOC-TEMPERATURE-HUMIDITY-I2C-sensor-breakout
 * 
 * 
 * 
 * HDC2010 definitions are placed sSense-HDC2010.h
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
	HDC2010.cpp
	Created By: Brandon Fisher, August 1st 2017
	
	This code is release AS-IS into the public domain, no guarantee or warranty is given.
	
	Description: This library facilitates communication with, and configuration of,1
	the HDC2010 Temperature and Humidity Sensor. It makes extensive use of the 
	Wire.H library, and should be useable with both Arduino and Energia. 
*/


#include <sSense-HDC2010.h>

#include <Wire.h>

//Define Register Map
#define TEMP_LOW 0x00
#define TEMP_HIGH 0x01
#define HUMID_LOW 0x02
#define HUMID_HIGH 0x03
#define INTERRUPT_DRDY 0x04
#define TEMP_MAX 0x05
#define HUMID_MAX 0x06
#define INTERRUPT_CONFIG 0x07
#define TEMP_OFFSET_ADJUST 0x08
#define HUM_OFFSET_ADJUST 0x09
#define TEMP_THR_L 0x0A
#define TEMP_THR_H 0x0B
#define HUMID_THR_L 0x0C
#define HUMID_THR_H 0x0D
#define CONFIG 0x0E
#define MEASUREMENT_CONFIG 0x0F
#define MID_L 0xFC
#define MID_H 0xFD
#define DEVICE_ID_L 0xFE
#define DEVICE_ID_H 0xFF
	
HDC2010::HDC2010(uint8_t addr)
{
  _addr = addr;
  
}

void HDC2010::begin(void)
{
	Wire.begin();
}

/* check sensor type ID to be 0x07D0  - return true if sensor it's online*/
boolean HDC2010::checkSensorType(void){
	uint8_t byteR;
	byteR = readReg(DEVICE_ID_L);	
	if (byteR != 0xD0) return 0;
	byteR = readReg(DEVICE_ID_H);	
	if (byteR != 0x07) return 0;
	
	return 1;
}

/* check sensor type ID to be 0x5449  - return true if sensor it's online*/
boolean HDC2010::checkSensorManufacturer(void){
	uint8_t byteR;
	byteR = readReg(MID_L);	
	if (byteR != 0x49) return 0;
	byteR = readReg(MID_H);	
	if (byteR != 0x54) return 0;
	
	return 1;
}

/* check sensor Manufacturer ID to be 0x5449 - return true if sensor it's online*/
boolean checkSensorManufacturer(void){
}

float HDC2010::readTemp(void)
{
	uint8_t byte[2];
	uint16_t temp;
	byte[0] = readReg(TEMP_LOW);
	byte[1] = readReg(TEMP_HIGH);
	//writeReg(TEMP_LOW, (unsigned int)0x00);
	//writeReg(TEMP_HIGH, (unsigned int)0x00);
	
	temp = (unsigned int)(byte[1]) << 8 | (unsigned int) byte[0];
	//temp = temp >> 2;
	
	return (float)(temp) * 165 / 65536 - 40;
	
}

float HDC2010::readHumidity(void)
{
	uint8_t byte[2];
	uint16_t humidity;
	byte[0] = readReg(HUMID_LOW);
	byte[1] = readReg(HUMID_HIGH);
	
	humidity = (unsigned int)byte[1] << 8 | byte[0];
	
	return (float)(humidity)/( 65536 )* 100;
	
}

void HDC2010::enableHeater(void)
{
	uint8_t configContents;	//Stores current contents of config register
	
	configContents = readReg(CONFIG);
	
	//set bit 3 to 1 to enable heater
	configContents = (configContents | 0x08);
	
	writeReg(CONFIG, configContents);
	
}

void HDC2010::disableHeater(void)
{
	uint8_t configContents;	//Stores current contents of config register
	
	configContents = readReg(CONFIG);
	
	//set bit 3 to 0 to disable heater (all other bits 1)
	configContents = (configContents & 0xF7);
	writeReg(CONFIG, configContents);
	
}

void HDC2010::openReg(uint8_t reg)
{
  Wire.beginTransmission(_addr); 		// Connect to HDC2010
  Wire.write(reg); 						// point to specified register
  Wire.endTransmission(); 				// Relinquish bus control
}

/*
uint8_t HDC2010::readReg(uint8_t reg)
{
	openReg(reg);
	uint8_t reading; 					// holds byte of read data
	Wire.requestFrom(_addr, 1); 		// Request 1 byte from open register
	Wire.endTransmission();				// Relinquish bus control
	
	if (1 <= Wire.available())
	{
		reading = (Wire.read());			// Read byte
	}
	
	return reading;
}
*/

/* next with cross compatibility to ESP32 	- update provided by Tibelea Eugen, Brasov, RO - Thank you, Eugen!*/
uint8_t HDC2010::readReg(uint8_t reg)
{
	openReg(reg);
	uint8_t reading = 0; 					// holds byte of read data
	Wire.requestFrom(_addr, 1); 		// Request 1 byte from open register
	reading = (Wire.read());			// Read byte
	Wire.endTransmission();				// Relinquish bus control
	
	return reading;
}

void HDC2010::writeReg(uint8_t reg, uint8_t data)
{
	
  Wire.beginTransmission(_addr);		// Open Device
  Wire.write(reg);						// Point to register
  Wire.write(data);						// Write data to register 
  Wire.endTransmission();				// Relinquish bus control
  
}

void HDC2010::setLowTemp(float temp)
{
	uint8_t temp_thresh_low;
	
	// Verify user is not trying to set value outside bounds
	if (temp < -40)
	{
		temp = -40;
	}
	else if (temp > 125)
	{
		temp = 125;
	}
	
	// Calculate value to load into register
	temp_thresh_low = (uint8_t)(256 * (temp + 40)/165);
	
	writeReg(TEMP_THR_L, temp_thresh_low);
	
}

void HDC2010::setHighTemp(float temp)
{ 
	uint8_t temp_thresh_high;
	
	// Verify user is not trying to set value outside bounds
	if (temp < -40)
	{
		temp = -40;
	}
	else if (temp > 125)
	{
		temp = 125;
	}
	
	// Calculate value to load into register
	temp_thresh_high = (uint8_t)(256 * (temp + 40)/165);
	
	writeReg(TEMP_THR_H, temp_thresh_high);
	
}

void HDC2010::setHighHumidity(float humid)
{
	uint8_t humid_thresh;
	
	// Verify user is not trying to set value outside bounds
	if (humid < 0)
	{
		humid = 0;
	}
	else if (humid > 100)
	{
		humid = 100;
	}
	
	// Calculate value to load into register
	humid_thresh = (uint8_t)(256 * (humid)/100);
	
	writeReg(HUMID_THR_H, humid_thresh);
	
}

void HDC2010::setLowHumidity(float humid)
{
	uint8_t humid_thresh;
	
	// Verify user is not trying to set value outside bounds
	if (humid < 0)
	{
		humid = 0;
	}
	else if (humid > 100)
	{
		humid = 100;
	}
	
	// Calculate value to load into register
	humid_thresh = (uint8_t)(256 * (humid)/100);
	
	writeReg(HUMID_THR_L, humid_thresh);
	
}

//  Return humidity from the low threshold register
float HDC2010::readLowHumidityThreshold(void)
{
	uint8_t regContents;
	
	regContents = readReg(HUMID_THR_L);
	
	return (float)regContents * 100/256;
	
}

//  Return humidity from the high threshold register
float HDC2010::readHighHumidityThreshold(void)
{
	uint8_t regContents;
	
	regContents = readReg(HUMID_THR_H);
	
	return (float)regContents * 100/256;
	
}

//  Return temperature from the low threshold register
float HDC2010::readLowTempThreshold(void)
{
	uint8_t regContents;
	
	regContents = readReg(TEMP_THR_L);
	
	return (float)regContents * 165/256 - 40;
	
}

//  Return temperature from the high threshold register
float HDC2010::readHighTempThreshold(void)
{
	uint8_t regContents;
	
	regContents = readReg(TEMP_THR_H);
	
	return (float)regContents * 165/256 - 40;
	
}


/* Upper two bits of the MEASUREMENT_CONFIG register controls
   the temperature resolution*/
void HDC2010::setTempRes(int resolution)
{ 
	uint8_t configContents;
	configContents = readReg(MEASUREMENT_CONFIG);
	
	switch(resolution)
	{
		case FOURTEEN_BIT:
			configContents = (configContents & 0x3F);
			break;
			
		case ELEVEN_BIT:
			configContents = (configContents & 0x7F);
			configContents = (configContents | 0x40);  
			break;
			
		case NINE_BIT:
			configContents = (configContents & 0xBF);
			configContents = (configContents | 0x80); 
			break;
			
		default:
			configContents = (configContents & 0x3F);
	}
	
	writeReg(MEASUREMENT_CONFIG, configContents);
	
}
/*  Bits 5 and 6 of the MEASUREMENT_CONFIG register controls
    the humidity resolution*/
void HDC2010::setHumidRes(int resolution)
{ 
	uint8_t configContents;
	configContents = readReg(MEASUREMENT_CONFIG);
	
	switch(resolution)
	{
		case FOURTEEN_BIT:
			configContents = (configContents & 0xCF);
			break;
			
		case ELEVEN_BIT:
			configContents = (configContents & 0xDF);
			configContents = (configContents | 0x10);  
			break;
			
		case NINE_BIT:
			configContents = (configContents & 0xEF);
			configContents = (configContents | 0x20); 
			break;
			
		default:
			configContents = (configContents & 0xCF);
	}
	
	writeReg(MEASUREMENT_CONFIG, configContents);	
}

/*  Bits 2 and 1 of the MEASUREMENT_CONFIG register controls
    the measurement mode  */
void HDC2010::setMeasurementMode(int mode)
{ 
	uint8_t configContents;
	configContents = readReg(MEASUREMENT_CONFIG);
	
	switch(mode)
	{
		case TEMP_AND_HUMID:
			configContents = (configContents & 0xF9);
			break;
			
		case TEMP_ONLY:
			configContents = (configContents & 0xFC);
			configContents = (configContents | 0x02);  
			break;
			
		case HUMID_ONLY:
			configContents = (configContents & 0xFD);
			configContents = (configContents | 0x04); 
			break;
			
		default:
			configContents = (configContents & 0xF9);
	}
	
	writeReg(MEASUREMENT_CONFIG, configContents);
}

/*  Bit 0 of the MEASUREMENT_CONFIG register can be used
    to trigger measurements  */
void HDC2010::triggerMeasurement(void)
{ 
	uint8_t configContents;
	configContents = readReg(MEASUREMENT_CONFIG);

	configContents = (configContents | 0x01);
	writeReg(MEASUREMENT_CONFIG, configContents);
}

/*  Bit 7 of the CONFIG register can be used to trigger a 
    soft reset  */
void HDC2010::reset(void)
{
	uint8_t configContents;
	configContents = readReg(CONFIG);

	configContents = (configContents | 0x80);
	writeReg(CONFIG, configContents);
	delay(50);
}

/*  Bit 2 of the CONFIG register can be used to enable/disable 
    the interrupt pin  */
void HDC2010::enableInterrupt(void)
{
	uint8_t configContents;
	configContents = readReg(CONFIG);

	configContents = (configContents | 0x04);
	writeReg(CONFIG, configContents);
}

/*  Bit 2 of the CONFIG register can be used to enable/disable 
    the interrupt pin  */
void HDC2010::disableInterrupt(void)
{
	uint8_t configContents;
	configContents = readReg(CONFIG);

	configContents = (configContents & 0xFB);
	writeReg(CONFIG, configContents);
}


/*  Bits 6-4  of the CONFIG register controls the measurement 
    rate  */
void HDC2010::setRate(int rate)
{ 
	uint8_t configContents;
	configContents = readReg(CONFIG);
	
	switch(rate)
	{
		case MANUAL:
			configContents = (configContents & 0x8F);
			break;
			
		case TWO_MINS:
			configContents = (configContents & 0x9F);
			configContents = (configContents | 0x10);  
			break;
			
		case ONE_MINS:
			configContents = (configContents & 0xAF);
			configContents = (configContents | 0x20); 
			break;
		
		case TEN_SECONDS:
			configContents = (configContents & 0xBF);
			configContents = (configContents | 0x30); 
			break;
		
		case FIVE_SECONDS:
			configContents = (configContents & 0xCF);
			configContents = (configContents | 0x40); 
			break;
		
		case ONE_HZ:
			configContents = (configContents & 0xDF);
			configContents = (configContents | 0x50); 
			break;
		
		case TWO_HZ:
			configContents = (configContents & 0xEF);
			configContents = (configContents | 0x60); 
			break;
		
		case FIVE_HZ:
			configContents = (configContents | 0x70); 
			break;
			
		default:
			configContents = (configContents & 0x8F);
	}
	
	writeReg(CONFIG, configContents);
}

/*  Bit 1 of the CONFIG register can be used to control the  
    the interrupt pins polarity */
void HDC2010::setInterruptPolarity(int polarity)
{
	uint8_t configContents;
	configContents = readReg(CONFIG);
	
	switch(polarity)
	{
		case ACTIVE_LOW:
			configContents = (configContents & 0xFD);
			break;
			
		case ACTIVE_HIGH:
			configContents = (configContents | 0x02);  
			break;
			
		default:
			configContents = (configContents & 0xFD);
	}
	
	writeReg(CONFIG, configContents);	
}

/*  Bit 0 of the CONFIG register can be used to control the  
    the interrupt pin's mode */
void HDC2010::setInterruptMode(int mode)
{
	uint8_t configContents;
	configContents = readReg(CONFIG);
	
	switch(mode)
	{
		case LEVEL_MODE:
			configContents = (configContents & 0xFE);
			break;
			
		case COMPARATOR_MODE:
			configContents = (configContents | 0x01);  
			break;
			
		default:
			configContents = (configContents & 0xFE);
	}
	
	writeReg(CONFIG, configContents);	
}


uint8_t HDC2010::readInterruptStatus(void)
{
	uint8_t regContents;
	regContents = readReg(INTERRUPT_DRDY);
	return regContents;
	
}

//  Clears the maximum temperature register
void HDC2010::clearMaxTemp(void)
{ 
	writeReg(TEMP_MAX, 0x00);
}

//  Clears the maximum humidity register
void HDC2010::clearMaxHumidity(void)
{ 
	writeReg(HUMID_MAX, 0x00);
}

//  Reads the maximum temperature register
float HDC2010::readMaxTemp(void)
{
	uint8_t regContents;
	
	regContents = readReg(TEMP_MAX);
	
	return (float)regContents * 165/256 - 40;
	
}

//  Reads the maximum humidity register
float HDC2010::readMaxHumidity(void)
{
	uint8_t regContents;
	
	regContents = readReg(HUMID_MAX);
	
	return (float)regContents /256 * 100;
	
}


// Enables the interrupt pin for comfort zone operation
void HDC2010::enableThresholdInterrupt(void)
{
	
	uint8_t regContents;
	regContents = readReg(INTERRUPT_CONFIG);

	regContents = (regContents | 0x78);

	writeReg(INTERRUPT_CONFIG, regContents);	
}

// Disables the interrupt pin for comfort zone operation
void HDC2010::disableThresholdInterrupt(void)
{
	uint8_t regContents;
	regContents = readReg(INTERRUPT_CONFIG);

	regContents = (regContents & 0x87);

	writeReg(INTERRUPT_CONFIG, regContents);	
}

// enables the interrupt pin for DRDY operation
void HDC2010::enableDRDYInterrupt(void)
{
	uint8_t regContents;
	regContents = readReg(INTERRUPT_CONFIG);

	regContents = (regContents | 0x80);

	writeReg(INTERRUPT_CONFIG, regContents);	
}

// disables the interrupt pin for DRDY operation
void HDC2010::disableDRDYInterrupt(void)
{
	uint8_t regContents;
	regContents = readReg(INTERRUPT_CONFIG);

	regContents = (regContents & 0x7F);

	writeReg(INTERRUPT_CONFIG, regContents);	
}

void HDC2010::setTemperatureOffset(uint8_t value)
{
	/*uint8_t temp_thresh_low;
	
	// Verify user is not trying to set value outside bounds
	if (temp < -40)
	{
		temp = -40;
	}
	else if (temp > 125)
	{
		temp = 125;
	}
	
	// Calculate value to load into register
	temp_thresh_low = (uint8_t)(256 * (temp + 40)/165);
	*/
	writeReg(TEMP_OFFSET_ADJUST, value);	
}
