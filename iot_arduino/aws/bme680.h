#ifndef BME680_H
#define BME680_H

void  BME680_setup();
void  BME680_ask_and_wait_data();
float BME680_readTemperature();
float BME680_readHumidity();
float BME680_readPressure();
float BME680_readGas();
float BME680_readAltitude();

#endif