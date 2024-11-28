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
#define PUBLISH_INTERVAL 400000  // 4 seconds
#define SEALEVELPRESSURE_HPA (1013.25)
WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

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

  if (millis() - lastPublishTime > PUBLISH_INTERVAL) {
    sendToAWS();
    lastPublishTime = millis();
  }
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

  // Subscribe to a topic, the incoming messages are processed by messageHandler() function
  // client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

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
  char messageBuffer[512];
  serializeJson(message, messageBuffer);  // print to client

  client.publish(aws_shadow_topic, messageBuffer);

  Serial.println("sent:");
  Serial.print("- topic: ");
  Serial.println(aws_shadow_topic);
  Serial.print("- payload:");
  Serial.println(messageBuffer);
}

void messageHandler(String &topic, String &payload) {
  Serial.println("received:");
  Serial.println("- topic: " + topic);
  Serial.println("- payload:");
  Serial.println(payload);

  // You can process the incoming data as json object, then control something
  /*
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  */
}
