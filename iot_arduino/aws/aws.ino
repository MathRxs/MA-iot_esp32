#include "setup.h"
#include "aws.h"
#include "bme680.h"
#include "nvs.h"

void setup() {
    NVS_setup();
    // NVS_clear();
    // while(1);

    BME680_setup();
    setupWiFi();
    AWS_connect();
}

void loop() {
    AWS_loop();
    
    if (AWS_should_publish_data()) {
        AWS_send();
    }
}