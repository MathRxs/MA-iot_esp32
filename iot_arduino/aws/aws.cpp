#include "HardwareSerial.h"
#include "aws.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include "bme680.h"
#include "nvs.hpp"

#define AWS_IOT_CLAIM_TOPIC "$aws/certificates/create/json"
#define AWS_IOT_RECIEVE_CERT_TOPIC "$aws/certificates/create/json/accepted"
#define AWS_IOT_REGISTER_TOPIC "$aws/provisioning-templates/ma_iot_template/provision/json"

#define MQTT_BUFFER_SIZE (4096)

static void AWS_claim_certificate();
static void messageHandler(String& topic, String& payload);

static void AWS_register_device(const char* certificateOwnershipToken, const char* certificateId);


WiFiClientSecure net;
MQTTClient client(MQTT_BUFFER_SIZE);
unsigned long lastPublishTime = 0;
String thingname = THINGNAME;
String certificate = AWS_CERT_CRT;
String private_key = AWS_CERT_PRIVATE;



void AWS_connect() {

  if (NVS_get_register() == false) {
    Serial.println("Device is not registered!");
    // use factory certificate and private key
  } else {
    Serial.println("Device is registered!");
    NVS_get_thingsname(&thingname);
    NVS_get_certificate(&certificate);
    NVS_get_private_key(&private_key);
  }

  net.setCACert(AWS_CERT_CA);
  net.setCertificate(certificate.c_str());
  net.setPrivateKey(private_key.c_str());
  client.begin(AWS_IOT_ENDPOINT, 8883, net);
  client.onMessage(messageHandler);
  Serial.println(thingname);
  Serial.println("Connecting to AWS IoT");
  while (!client.connect(thingname.c_str())) {
    delay(100);
    Serial.print(".");
  }
  if (!client.connected()) {
    Serial.println("ESP32 - AWS IoT Timeout!");
    return;
  }

  if (NVS_get_register() == false) {
    Serial.println("Claiming certificate from AWS!");
    AWS_claim_certificate();
  }
}


void AWS_loop() {
  client.loop();
}


bool AWS_should_publish_data() {
  return (millis() - lastPublishTime > PUBLISH_INTERVAL);
}

void AWS_send() {
  BME680_ask_and_wait_data();
  lastPublishTime = millis();
  StaticJsonDocument<200> message;
  message["state"]["reported"]["id"] = thingname;
  message["state"]["reported"]["new_data"]["temperature"] = BME680_readTemperature();
  message["state"]["reported"]["new_data"]["humidity"] = BME680_readHumidity();
  message["state"]["reported"]["new_data"]["pressure"] = BME680_readPressure();
  message["state"]["reported"]["new_data"]["gas"] = BME680_readGas();
  message["state"]["reported"]["new_data"]["altitude"] = BME680_readAltitude();
  message["state"]["reported"]["new_data"]["wind_speed"] = BME680_readAltitude();
  message["state"]["reported"]["new_data"]["wind_direction"] = BME680_readAltitude();
  char messageBuffer[512];
  serializeJson(message, messageBuffer);
  Serial.println(messageBuffer);
  char topic[100];
  sprintf(topic, "$aws/things/%s/shadow/update", thingname.c_str());
  client.publish(topic, messageBuffer);
  Serial.println("Data sent to AWS!");
}

static void messageHandler(String& topic, String& payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  if (topic == AWS_IOT_RECIEVE_CERT_TOPIC) {
    Serial.println("Received certificate from AWS!");
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

    AWS_register_device(certificateOwnershipToken, certificateId);
    NVS_set_certificate(String(certificatePem));
    NVS_set_private_key(String(PrivateKey));

    String Chip_id;
    AWS_chip_id(&Chip_id);
    NVS_set_thingsname("esp_" + String(Chip_id));
    NVS_set_register();

    ESP.restart();
  }
}

static void AWS_claim_certificate() {
  client.subscribe(AWS_IOT_RECIEVE_CERT_TOPIC);
  client.publish(AWS_IOT_CLAIM_TOPIC, "");
}

void AWS_chip_id(String* Chip_id) {
  uint64_t chipid = ESP.getEfuseMac();
  char unique_id[13];
  snprintf(unique_id, 13, "%04X%08X", (uint16_t)(chipid >> 32), (uint32_t)chipid);
  *Chip_id = String(unique_id);
  Serial.print("Generated unique ID: ");
  Serial.println(*Chip_id);
}

static void AWS_register_device(const char* certificateOwnershipToken, const char* certificateId) {
  StaticJsonDocument<200> message;
  String Chip_id;
  AWS_chip_id(&Chip_id);
  message["certificateOwnershipToken"] = certificateOwnershipToken;
  message["parameters"]["ChipID"] = Chip_id;
  message["parameters"]["AWS::IoT::Certificate::Id"] = certificateId;
  char messageBuffer[MQTT_BUFFER_SIZE];
  serializeJson(message, messageBuffer);
  client.publish(AWS_IOT_REGISTER_TOPIC, messageBuffer);
  Serial.println("Device registered to AWS!");
}

