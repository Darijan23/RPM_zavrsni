/*

Arduino library for ClosedCube TMP116 ?0.2?C (max) High-Accuracy Low-Power I2C Temperature Sensor breakout board

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

#ifndef _CLOSEDCUBE_TMP116_h
#define _CLOSEDCUBE_TMP116_h

#include <ClosedCube_I2C.h>

enum TMP116_REGISTERS
{
	TMP116_REG_TEMP	= 0x00,
	TMP116_REG_CONFIG = 0x01,
	TMP116_REG_HIGH_LIMIT = 0x02,
	TMP116_REG_LOW_LIMIT= 0x03,	
	TMP116_REG_DEVICE_ID = 0x0F,
};

namespace ClosedCube
{
	namespace Sensor
	{
		
		class TMP116
		{
		public:

			TMP116();
			TMP116(uint8_t address);

			void address(uint8_t address);

			double readTemperature();
			double readT();

			double readHighLimit();
			double readLowLimit();

			void writeHighLimit(double limit);
			void writeLowLimit(double limit);

			uint16_t readDeviceId();

		private:

			ClosedCube::Driver::I2CDevice _sensor;
		};

	}
}

#endif

