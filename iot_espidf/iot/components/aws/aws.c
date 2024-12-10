/* thing-shadow example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "shadow.h"
#include "aws.h"
#include "demo_config.h"
#include "bme680_task.h"

int aws_iot_demo_main( int argc, char ** argv );

#include "esp_log.h"

static const char *TAG = "SHADOW_EXAMPLE";

/*
 * Prototypes for the demos that can be started from this project.  Note the
 * Shadow demo is not actually started until the network is already.
 */
static uint32_t clientToken = 0U;

int aws_shadow_update(bme680_values_float_t values){
    static char updateDocument[ SHADOW_DESIRED_JSON_LENGTH + 1 ] = { 0 };
    ( void ) memset( updateDocument,
                        0x00,
                        sizeof( updateDocument ) );

    /* Keep the client token in global variable used to compare if
        * the same token in /update/accepted. */
    clientToken = ( Clock_GetTimeMs() % 1000000 );

    sprintf(updateDocument,
             "{\"state\":{\"reported\":{\"temperature\":%.2f,\"humidity\":%.2f,\"pressure\":%.2f,\"gas\":%.2f,\"altitude\":%.2f}}}", 
             values.temperature, 
             values.humidity, 
             values.pressure,
                values.gas_resistance,
                0.0);

    int returnStatus = PublishToTopic( "$aws/things/need_to_find_a_name/shadow/update",
                                   45,
                                    updateDocument,
                                    strlen(updateDocument) );
    return returnStatus;
}
void eventCallback(){

}
void aws_task(void *pvParameter)
{
    /* Initialize NVS partition */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        /* NVS partition was truncated
        * and needs to be erased */
        ESP_ERROR_CHECK(nvs_flash_erase());

        /* Retry nvs_flash_init */
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    
    while (1)
    {   int returnStatus = EstablishMqttSession( eventCallback );
        bme680_values_float_t bme680_values_float;
        bme_task_get_value(&bme680_values_float);
        aws_shadow_update(bme680_values_float);
        returnStatus = DisconnectMqttSession();
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

// void aws()
// {
//     ESP_LOGI(TAG, "[APP] Startup..");
//     ESP_LOGI(TAG, "[APP] Free memory: %"PRIu32" bytes", esp_get_free_heap_size());
//     ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

//     esp_log_level_set("*", ESP_LOG_INFO);
    
//     /* Initialize NVS partition */
//     esp_err_t ret = nvs_flash_init();
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//         /* NVS partition was truncated
//          * and needs to be erased */
//         ESP_ERROR_CHECK(nvs_flash_erase());

//         /* Retry nvs_flash_init */
//         ESP_ERROR_CHECK(nvs_flash_init());
//     }
    
//     // ESP_ERROR_CHECK(esp_netif_init());
//     // ESP_ERROR_CHECK(esp_event_loop_create_default());

//     /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
//      * Read "Establishing Wi-Fi or Ethernet Connection" section in
//      * examples/protocols/README.md for more information about this function.
//      */
//     // ESP_ERROR_CHECK(example_connect());

//     aws_iot_demo_main(0,NULL);
// }

