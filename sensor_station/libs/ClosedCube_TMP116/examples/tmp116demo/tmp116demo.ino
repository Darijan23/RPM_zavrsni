/**************************************************************************************

This is example for 
ClosedCube TMP116 ±0.2°C (max) High-Accuracy Low-Power I2C Temperature Sensor breakout board

Initial Date: 13-Feb-2018

Hardware connections for Arduino Uno:
VDD to 3.3V DC
SCL to A5
SDA to A4
GND to common ground

**************************************************************************************/

#include "ClosedCube_TMP116.h"

ClosedCube::Sensor::TMP116 tmp116;

void setup()
{
	Wire.begin();
	Serial.begin(9600);

	Serial.println("ClosedCube TMP116 Arduino Test");
	
	tmp116.address(0x48); // Sensor I2C address either 0x48 or x49

	Serial.print("Device ID: 0x");
	Serial.println(tmp116.readDeviceId(), HEX); // Device ID = 0x116	
}

void loop()
{
	Serial.print("T=");
	Serial.print(tmp116.readTemperature());
	Serial.println("C");

	delay(200);
}
