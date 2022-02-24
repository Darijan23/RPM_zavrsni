#include <Wire.h>
#include "ClosedCube_TMP116.h"
#include <ClosedCube_OPT3001.h>
#include <Adafruit_DPS310.h>
#include <sSense-HDC2010.h>

ClosedCube::Sensor::TMP116 tmp116;
HDC2010 ssenseHDC2010(HDC2010_I2C_ADDR);
ClosedCube_OPT3001 opt3001;
Adafruit_DPS310 dps;
Adafruit_Sensor *dps_temp = dps.getTemperatureSensor();
Adafruit_Sensor *dps_pressure = dps.getPressureSensor();

#define BAUD 115200
#define OPT3001_ADDRESS 0x44
#define TMP116_ADDRESS 0x48
#define TEMP_INTERVAL 5
#define HUM_INTERVAL 5
#define LUX_FREQ 4
#define PRESS_FREQ 4

unsigned long long int temp_stamp = 0, hum_stamp = 0, lux_stamp = 0, press_stamp = 0;

int light_pin = LED_BUILTIN, humidifier_pin = 10, heating_pin = 11, cooling_pin = 12;

void config_home_devices() {
  pinMode(light_pin, OUTPUT);
  pinMode(humidifier_pin, OUTPUT);
  pinMode(heating_pin, OUTPUT);
  pinMode(cooling_pin, OUTPUT);
}

void set_home_devices(char light, char heat, char cool, char hum) {
  if (light == '1') {
    digitalWrite(light_pin, HIGH);
  } else {
    digitalWrite(light_pin, LOW);
  }

  if (heat == '1') {
    digitalWrite(heating_pin, HIGH);
  } else {
    digitalWrite(heating_pin, LOW);
  }

  if (cool == '1') {
    digitalWrite(cooling_pin, HIGH);
  } else {
    digitalWrite(cooling_pin, LOW);
  }

  if (hum == '1') {
    digitalWrite(humidifier_pin, HIGH);
  } else {
    digitalWrite(humidifier_pin, LOW);
  }
}

void configHDC2010() {
  ssenseHDC2010.begin();
  ssenseHDC2010.reset();

  ssenseHDC2010.setMeasurementMode(TEMP_AND_HUMID);
  ssenseHDC2010.setRate(TWO_HZ);
  ssenseHDC2010.setTempRes(FOURTEEN_BIT);
  ssenseHDC2010.setHumidRes(FOURTEEN_BIT);
}

void configOPT3001() {
  opt3001.begin(OPT3001_ADDRESS);
  OPT3001_Config newConfig;
  
  newConfig.RangeNumber = B1100;  
  newConfig.ConvertionTime = B0;
  newConfig.Latch = B1;
  newConfig.ModeOfConversionOperation = B11;

  OPT3001_ErrorCode errorConfig = opt3001.writeConfig(newConfig);
}

void configDPS310() {
  dps.begin_I2C();
  dps.configurePressure(DPS310_8HZ, DPS310_16SAMPLES);
  dps.configureTemperature(DPS310_8HZ, DPS310_16SAMPLES);
}

void setup() {
  Wire.begin();
  Serial.begin(BAUD);
  while(!Serial);

  config_home_devices();
 
  tmp116.address(TMP116_ADDRESS);
  
  configHDC2010();
  
  configOPT3001();

  configDPS310();

  ssenseHDC2010.triggerMeasurement();
}

void loop() {
  // Čitanje naredbi s računala
  if (Serial.available()) {
    String input = Serial.readStringUntil('|');
  
    char light = input[0];
    char heat = input[1];
    char cool = input[2];
    char hum = input[3];

    set_home_devices(light, heat, cool, hum);
  }
  
  if (lux_stamp > millis()) {
    lux_stamp = 0;
    press_stamp = 0;
    temp_stamp = 0;
    hum_stamp = 0;
  }
  
  // Temperatura
  if (millis() - temp_stamp >= (TEMP_INTERVAL * 1000)) {
    temp_stamp = millis();
    
    // TMP116
    Serial.print("TMP116, T, ");
    Serial.println(tmp116.readTemperature());
  
    // HDC2010
    Serial.print("HDC2010, T, ");
    Serial.println(ssenseHDC2010.readTemp());
  
    // DPS310
    sensors_event_t temp_event;
    while (!dps.temperatureAvailable());
    if (dps.temperatureAvailable()) {
      dps_temp->getEvent(&temp_event);
      Serial.print("DPS310, T, ");
      Serial.println(temp_event.temperature);
    }
  }
  
  // Relativna vlažnost zraka
  if (millis() - hum_stamp >= (HUM_INTERVAL * 1000)) {
    hum_stamp = millis();
    Serial.print("HDC2010, H, ");
    Serial.println(ssenseHDC2010.readHumidity());
  }
  
  // Svjetlost
  if (millis() - lux_stamp >= (1000 / LUX_FREQ)) {
    lux_stamp = millis();
    
    Serial.print("OPT3001, L, ");
    Serial.println(opt3001.readResult().lux);
  }
  
  // Tlak
  if (millis() - press_stamp >= (1000 / PRESS_FREQ)) {
    press_stamp = millis();
    sensors_event_t pressure_event;
    
    while (!dps.pressureAvailable());
    if (dps.pressureAvailable()) {
      dps_pressure->getEvent(&pressure_event);
      Serial.print("DPS310, P, ");
      Serial.println(pressure_event.pressure * 100);
    }
  }
}
