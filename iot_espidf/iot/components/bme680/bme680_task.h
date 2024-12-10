#ifndef BME680_TASK_H
#define BME680_TASK_H


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bme680.h"

#define I2C_BUS       0
#define I2C_SCL_PIN   6
#define I2C_SDA_PIN   5
#define I2C_FREQ      I2C_FREQ_100K

#define MEASURE_DELAY 1000

// Task to read data from the BME680 sensor
void bme_task(void *pvParameters);
void bme_task_get_value(bme680_values_float_t *values);
int bme_task_init_sensor();
#endif // BME680_TASK_H