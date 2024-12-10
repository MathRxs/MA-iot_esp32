/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-aws-iot
 */

#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

Adafruit_BME680 bme; 
#define PUBLISH_INTERVAL_SECONDS (20)
#define PUBLISH_INTERVAL (PUBLISH_INTERVAL_SECONDS*1000)  // 4 seconds
#define SEALEVELPRESSURE_HPA (1013.25)
uint64_t chipid = ESP.getEfuseMac();
#define AWS_IOT_SUBSCRIBE_TOPIC "$aws/certificates/create/json/accepted"
WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(4096);
String payload = "{\"certificateId\": \"bd0201bbf873b7fb8274086910692330644e0ad4171cb2579127c0bcda040982\", \"parameters\": {\"SerialNumber\": \"123456789\", \"certificateId\" : \"bd0201bbf873b7fb8274086910692330644e0ad4171cb2579127c0bcda040982\"}}";
int oe = 0;
String chipID = String((uint32_t)(chipid >> 32), HEX) + String((uint32_t)chipid, HEX);
void setup_analog(){
  
}
unsigned long lastPublishTime = 0;
void setup_bme() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println(F("BME680 async test"));
  // myWire.begin(SDA_PIN, SCL_PIN,400000);
  // Serial.println();
  while (!bme.begin(0x76)) {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
     delay(1000);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}
void setup() {
  setup_bme();
  Serial.begin(9600);

  // set the ADC attenuation to 11 dB (up to ~3.3V input)
  analogSetAttenuation(ADC_11db);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("ESP32 connecting to Wi-Fi");


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  connectToAWS();
}

void loop() {
  client.loop();
  if (!oe){
    client.publish("$aws/certificates/create/json", "");
    
    oe =1;
  }

  if (millis() - lastPublishTime > PUBLISH_INTERVAL) {
    sendToAWS();
    lastPublishTime = millis();
  }

  // int sensorValue = analogRead(A0);
  
  // // The ADC range is 0-4095 for 0-3.1V
  // float outputVoltage = (sensorValue / 4095.0) * 3.3;
  // Serial.println(outputVoltage);
  // float windspeed = (outputVoltage - 0.4) /1.6*32.4;
  // if(windspeed<0){
  //   windspeed=0;
  // }
  // Serial.println(windspeed);

}
void read_data() {
  // Tell BME680 to begin measurement.
  unsigned long endTime = bme.beginReading();

  // instantaneous even if milli() >= endTime due to I2C/SPI latency.
  if (!bme.endReading()) {
    Serial.println(F("Failed to complete reading :("));
    return;
  }

}

void connectToAWS() {
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
  
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a handler for incoming messages
  client.onMessage(messageHandler);

  Serial.print("ESP32 connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  if (!client.connected()) {
    Serial.println("ESP32 - AWS IoT Timeout!");
    return;
  }
  // Serial.print("Connected");
  

  // Subscribe to a topic, the incoming messages are processed by messageHandler() function
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  client.publish("$aws/certificates/create/json", "");
  Serial.println("ESP32  - AWS IoT Connected!");
}

void sendToAWS() {
  char aws_shadow_topic[100];
  sprintf(aws_shadow_topic, "$aws/things/%s/shadow/update", THINGNAME);
  

  //read data from sensor
  read_data();
  StaticJsonDocument<200> message;
  message["state"]["reported"]["temperature"] = bme.temperature;
  message["state"]["reported"]["humidity"] = bme.humidity;
  message["state"]["reported"]["pressure"] = bme.pressure / 100.0;
  message["state"]["reported"]["gas"] = bme.gas_resistance / 1000.0;
  message["state"]["reported"]["altitude"] = bme.readAltitude(SEALEVELPRESSURE_HPA);
  message["state"]["reported"]["max_windspeed"] = 0.0;
  message["state"]["reported"]["mean_windspeed"] = 0.0;
  char messageBuffer[512];
  serializeJson(message, messageBuffer);  // print to client
  Serial.println(messageBuffer);
  client.publish(aws_shadow_topic, messageBuffer);
  
  // Serial.println("sent:");
  // Serial.print("- topic: ");
  // Serial.println(aws_shadow_topic);
  // Serial.print("- payload:");
  // Serial.println(messageBuffer);
}

void messageHandler(String &topic, String &payload) {
  Serial.println("received:");
  Serial.println("- topic: " + topic);
  Serial.println("- payload:");
  Serial.println(payload);
  if (topic == AWS_IOT_SUBSCRIBE_TOPIC) {
    StaticJsonDocument<200> doc;
    deserializeJson(doc, payload);
    const char* certificateId = doc["certificateId"];
    const char* certificatePem = doc["certificatePem"];
    const char* PrivateKey = doc["privateKey"];
    const char* certificateOwnershipToken = doc["certificateOwnershipToken"];

    Serial.println("Certificate ID: " + String(certificateId));
    Serial.println("Certificate PEM: " + String(certificatePem));
    Serial.println("Private Key: " + String(PrivateKey));
    Serial.println("Certificate Ownership Token: " + String(certificateOwnershipToken));

//     {
//     "certificateOwnershipToken": "string",
//     "parameters": {
//         "string": "string",
//         ...
//     }
//    } 
    StaticJsonDocument<200> message;
    message["certificateOwnershipToken"] = certificateOwnershipToken;
    message["parameters"]["ChipID"] = "123456789";
    message["parameters"]["AWS::IoT::Certificate::Id"] = certificateId;
    char messageBuffer[2048];
    serializeJson(message, messageBuffer);  // print to client
    // Register the device with the received certificate
    // This is a placeholder for the actual registration logic

    Serial.println("Registering device with AWS IoT using the received certificate...");

    client.publish("aws/provisioning-templates/ma_iot_template/provision/json", messageBuffer);
    // client.publish("$aws/provisioning-templates/ma_iot_template/provision/json", messageBuffer);

  // You can process the incoming data as json object, then control something
  /*
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  */
  }
}
