/*

Arduino library for ClosedCube TMP116 ±0.2°C (max) High-Accuracy Low-Power I2C Temperature Sensor breakout board

---

The MIT License (MIT)

Copyright (c) 2018 ClosedCube Limited

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include "ClosedCube_TMP116.h"

#include <ClosedCube_I2C.h>

ClosedCube::Sensor::TMP116::TMP116()
{
	
}

ClosedCube::Sensor::TMP116::TMP116(uint8_t address) 
{
	_sensor.address(address);
}


void ClosedCube::Sensor::TMP116::address(uint8_t address)
{
	_sensor.address(address);
}

double ClosedCube::Sensor::TMP116::readT()
{
	return readTemperature();
}

double ClosedCube::Sensor::TMP116::readTemperature()
{
	return _sensor.readWordFromReg(TMP116_REG_TEMP,10) * 0.0078125;
}


double ClosedCube::Sensor::TMP116::readHighLimit()
{
	return  _sensor.readWordFromReg(TMP116_REG_HIGH_LIMIT, 10) * 0.0078125;
}

double ClosedCube::Sensor::TMP116::readLowLimit()
{
	return  _sensor.readWordFromReg(TMP116_REG_LOW_LIMIT, 10) * 0.0078125;
}


void ClosedCube::Sensor::TMP116::writeHighLimit(double limit)
{
	_sensor.writeWordToReg(TMP116_REG_HIGH_LIMIT, (uint16_t)(limit/0.0078125));
}


void ClosedCube::Sensor::TMP116::writeLowLimit(double limit)
{
	_sensor.writeWordToReg(TMP116_REG_LOW_LIMIT, (uint16_t)(limit / 0.0078125));
}

uint16_t ClosedCube::Sensor::TMP116::readDeviceId()
{
	return _sensor.readWordFromReg(TMP116_REG_DEVICE_ID);
}





