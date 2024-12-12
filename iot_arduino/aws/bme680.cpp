#include "bme680.h"
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME680 bme;

void BME680_setup() {
    Serial.begin(115200);
    while (!bme.begin(0x76)) {
        Serial.println("Could not find a valid BME680 sensor, check wiring!");
        delay(1000);
    }
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150);
}

void BME680_ask_and_wait_data() {
    unsigned long endTime = bme.beginReading();
    delay(50);
    if (!bme.endReading()) {
        Serial.println(F("Failed to complete reading :("));
        return;
    }

}

float BME680_readTemperature() {
    return bme.temperature;
}

float BME680_readHumidity() {
    return bme.humidity;
}

float BME680_readPressure() {
    return bme.pressure / 100.0;
}

float BME680_readGas() {
    return bme.gas_resistance / 1000.0;
}

float BME680_readAltitude() {
    return bme.readAltitude(SEALEVELPRESSURE_HPA);
}
