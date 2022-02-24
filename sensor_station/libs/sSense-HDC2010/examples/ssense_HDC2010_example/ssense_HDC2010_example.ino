/* s-Sense I2C HDC2010 I2C sensor breakout example - v1.0/20190524.
 * 
 * This library it's compatible with:
 *    s-Sense HDC2010 I2C sensor breakout [PN: SS-HDC2010#I2C, SKU: ITBP-6005], info https://itbrainpower.net/sensors/HDC2010-TEMPERATURE-HUMIDITY-I2C-sensor-breakout 
 *    s-Sense CCS811 + HDC2010 I2C sensor breakout [PN: SS-HDC2010+CCS811#I2C, SKU: ITBP-6006], info https://itbrainpower.net/sensors/CCS811-HDC2010-CO2-TVOC-TEMPERATURE-HUMIDITY-I2C-sensor-breakout
 * 
 * HDC2010 temperature and humidity (pulling at 2 second) reading example - based on HDC2010 library written 
 * by Brandon Fisher and provided by TI. Thank you Brandon! Great job! Read credits bellow and inside the class.
 * We've just add add some variables, functions and functionalities.
 * 
 *  Mandatory wiring:
 *    Common for 3.3V and 5V Arduino boards:
 *        sensor I2C SDA  <------> Arduino I2C SDA
 *        sensor I2C SCL  <------> Arduino I2C SCL
 *        sensor GND      <------> Arduino GND
 *    For Arduino 3.3V compatible:
 *        sensor Vin      <------> Arduino 3.3V
 *    For Arduino 5V compatible:
 *        sensor Vin      <------> Arduino 5V
 *        
 * Leave other sensor PADS not connected.
 *  
 * SPECIAL note for some ARDUINO boards:
 *        SDA (Serial Data)   ->  A4 on Uno/Pro-Mini, 20 on Mega2560/Due, 2 Leonardo/Pro-Micro
 *        SCK (Serial Clock)  ->  A5 on Uno/Pro-Mini, 21 on Mega2560/Due, 3 Leonardo/Pro-Micro
 * 
 * WIRING WARNING: wrong wiring may damage your Arduino board MCU or your sensor! Double check what you've done.
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
 * HDC2010.ino
 * Created: August 1st 2017
 * This sketch is released AS-IS into the public domain, no guarantee or warranty is given.
 * This Code is not supported by Texas Instruments.
 * 
 * Description: This sketch configures the HDC2010 to monitor for a specific temperature 
 * and humidity range, with regular one second samples. The temperature and humidity readings
 * of the device are printed to a standard 16x2 LCD display. When the threshold values are
 * exceeded, the LCD will also have "ALT" printed in the bottom right corner. The schematic 
 * for this example is included alongside the HDC2010 Arduino Source Code.
 * 
 * Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#define SERIAL_SPEED  19200

#include <sSense-HDC2010.h>

HDC2010 ssenseHDC2010(HDC2010_I2C_ADDR);


float temperature = 0, humidity = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  DebugPort.begin(SERIAL_SPEED);
  delay(5000);
  DebugPort.println("s-Sense HDC2010 I2C sensor.");

  // Initialize HDC2010(THS) I2C communication
  ssenseHDC2010.begin();
    
  // Begin with a HDC2010(THS) reset
  ssenseHDC2010.reset();

  // Set up HDC2010(THS) temperature offset, if required
  //ssenseHDC2010.setTemperatureOffset(0b11010111);    //-6.64 degrees Celsius - determine and set your, see definitions and HDC2010 datasheet
 
  // Configure Measurements
  ssenseHDC2010.setMeasurementMode(TEMP_AND_HUMID);  // Set measurements to temperature and humidity
  ssenseHDC2010.setRate(ONE_HZ);                     // Set measurement frequency to 1 Hz
  ssenseHDC2010.setTempRes(FOURTEEN_BIT);
  ssenseHDC2010.setHumidRes(FOURTEEN_BIT);

  
  delay(1000);

  //begin HDC2010 sensor measuring
  ssenseHDC2010.triggerMeasurement();
}

void loop() {
  // read temperature and humidity
  temperature = ssenseHDC2010.readTemp();
  humidity = ssenseHDC2010.readHumidity();

  DebugPort.print("TEMP(C): ");
  DebugPort.println(temperature);

  DebugPort.print("%RH: ");
  DebugPort.println(humidity);

  // Wait 2 second for the next reading
  delay(2000);
}

