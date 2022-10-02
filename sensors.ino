OneWire oneWire(TEMP_PIN);
DallasTemperature temperatureSensor(&oneWire);

extern "C"
{
  uint8_t temprature_sens_read();
}

//=======================================================
//  readSensors: Read all sensors and battery voltage
//=======================================================
void readSensors(struct sensorData *environment)
{
  readWindSpeed(environment);
  readWindDirection(environment);
  readTemperature(environment);
  readLux(environment);
  //readPR(environment);
  // need overload readBME(environment);
  readUV(environment);
  //readBattery(hardware);
  //readESPCoreTemp(hardware);
  readBME(environment);
}

//=======================================================
//  readSystemSensors: Hardware health and diagnostics
//=======================================================
void readSystemSensors(struct diagnostics *hardware)
{
  readBME(hardware);
  readBattery(hardware);
  readESPCoreTemp(hardware);
}
//=======================================================
//  readTemperature: Read 1W DS1820B
//=======================================================
void readTemperature (struct sensorData *environment)
{
  MonPrintf("Requesting temperatures...\n");
  temperatureSensor.requestTemperatures();
  environment->temperatureC = temperatureSensor.getTempCByIndex(0);

  // Check if reading was successful
  if (environment->temperatureC != DEVICE_DISCONNECTED_C)
  {
    //environment->temperatureF = environment->temperatureC * 9 / 5 + 32;
    MonPrintf("Temperature for the device 1 (index 0) is: %5.1f C\n", environment->temperatureC);
  }
  else
  {
    MonPrintf("Error: Could not read temperature data\n");
    //environment->temperatureF = -40;
    environment->temperatureC = -40;
  }
}

//=======================================================
//  readBattery: read analog volatage divider value
//=======================================================
void readBattery (struct diagnostics *hardware)
{
  hardware->batteryADC = analogRead(VOLT_PIN);
  hardware->batteryVoltage = hardware->batteryADC * batteryCalFactor;
  MonPrintf("Battery digital ADC :%i voltage: %6.2f\n", hardware->batteryADC, hardware->batteryVoltage);
  //check for low battery situation
  if (hardware->batteryVoltage < batteryLowVoltage)
  {
    lowBattery = true;
  }
  else
  {
    lowBattery = false;
  }
}

//=======================================================
//  checkBatteryVoltage: set/reset low voltage flag only
//=======================================================
void checkBatteryVoltage (void)
{
  int adc;
  float voltage;
  adc = analogRead(VOLT_PIN);
  voltage = adc * batteryCalFactor;
  //check for low battery situation
  if (voltage < batteryLowVoltage)
  {
    lowBattery = true;
  }
  else
  {
    lowBattery = false;
  }
}
//=======================================================
//  readLux: LUX sensor read
//=======================================================
void readLux(struct sensorData *environment)
{
#ifdef BH1750Enable
  if (status.lightMeter)
  {
    environment->lux = lightMeter.readLightLevel();
  }
  else
  {
    environment->lux = -1;
  }
#else
  environment->lux = -3;
#endif
  MonPrintf("LUX value: %6.2f\n", environment->lux);
}

//=======================================================
//  readPR: photoresistor ADC read
//=======================================================
void readPR(struct sensorData *environment)
{
  int vin;

  vin = analogRead(PR_PIN);

  MonPrintf("photoresistor value: %i photoresistor\n", vin);
  //environment->photoresistor = vin;
}

//=======================================================
//  readBME: BME sensor read
//=======================================================
void readBME(struct diagnostics *hardware)
{
  float relHum, pressure;
  if (status.bme)
  {
    bme.read(pressure, hardware->BMEtemperature, relHum, BME280::TempUnit_Celsius, BME280::PresUnit_Pa);
  }
  else
  {
    //set to insane values
   //environment->barometricPressure = -100;
    hardware->BMEtemperature = -100;
    //environment->humidity = -100;
  }
  MonPrintf("BME case temperature: %6.2f\n", hardware->BMEtemperature);

}

//=======================================================
//  readBME: BME sensor read
//=======================================================
void readBME(struct sensorData *environment)
{
  float case_temperature;
  if (status.bme)
  {

    bme.read(environment->barometricPressure, case_temperature, environment->humidity, BME280::TempUnit_Celsius, BME280::PresUnit_Pa);
    environment->barometricPressure += ALTITUDE_OFFSET_METRIC;
  }
  else
  {
    //set to insane values
    environment->barometricPressure = -100;
    //environment->BMEtemperature = -100;
    environment->humidity = -100;
  }
  MonPrintf("BME barometric pressure: %6.2f  BME humidity: %6.2f\n", environment->barometricPressure, environment->humidity);

}

//=======================================================
//  readUV: get implied uv sensor value
//=======================================================
void readUV(struct sensorData *environment)
{
  if (status.uv)
  {
    environment->UVIndex = (float) uv.readUV() / 100;
  }
  else
  {
    environment->UVIndex = -1;
  }
  MonPrintf("UV Index: %f\n", environment->UVIndex);
  MonPrintf("Vis: %i\n", uv.readVisible());
  MonPrintf("IR: %i\n", uv.readIR());
}

void readESPCoreTemp(struct diagnostics *hardware)
{
  unsigned int coreF, coreC;
  coreF = temprature_sens_read();
  coreC = (coreF - 32) * 5 / 9;
  hardware->coreC = coreC;
}

/*
  void printADCLCD( void)
  {
  int adc;
  adc = analogRead(VOLT_PIN);
  display.printf("ADC: %i\n", adc);
  }*/

//===========================================
// sensorEnable: Initialize i2c and 1w sensors
//===========================================
void sensorEnable(void)
{
  status.temperature = 1;
  //Wire.setClock(100000);
  status.bme = bme.begin();
  status.uv = uv.begin();
  status.lightMeter = lightMeter.begin();

  temperatureSensor.begin();                //returns void - cannot directly check
}

//===========================================
// sensorStatusToConsole: Output .begin return values
//===========================================
void sensorStatusToConsole(void)
{
  MonPrintf("----- Sensor Statuses -----\n");
  MonPrintf("BME status:         %i\n", status.bme);
  MonPrintf("UV status:          %i\n", status.uv);
  MonPrintf("lightMeter status:  %i\n", status.lightMeter);
  MonPrintf("temperature status: %i\n\n", status.temperature);
}
