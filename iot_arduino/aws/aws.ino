#include "setup.h"
#include "aws.h"
#include "bme680.h"
#include "nvs.hpp"

void setup() {
    Serial.begin(115200);
    NVS_setup();
    // NVS_clear();
    // while(1);
    if (is_first_boot()) {
        NVS_clear();

        // return name 
        String Chip_id;
        AWS_chip_id(&Chip_id);
        Serial.println("id: " + String(Chip_id));
        NVS_set_thingsname("NR_" + String(Chip_id));
        while (Serial.available() == 0) {
            // Wait for user input
        }
        
        String certificate = Serial.readString();
        boolean receivingCertificate;
        while(1){
          if (Serial.available()) {
            String line = Serial.readStringUntil('\n'); // Lit une ligne jusqu'à la fin de ligne (retour à la ligne)

            // Vérifie les délimiteurs de début et de fin du certificat
            if (line.startsWith("-----BEGIN CERTIFICATE-----")) {
              receivingCertificate = true;
              certificate = line + "\n"; // Ajoute la ligne au certificat
            } else if (line.startsWith("-----END CERTIFICATE-----")) {
              receivingCertificate = false;
              certificate += line + "\n"; // Ajoute la ligne finale
              Serial.println("Certificat reçu:");
              Serial.println(certificate);
              break;
              certificate = ""; // Réinitialise pour la réception suivante
            } else if (receivingCertificate) {
              certificate += line + "\n"; // Continue à ajouter les lignes au certificat
            }
          }
        }
        Serial.println("AKK");
        NVS_set_certificate(certificate);
        while (Serial.available() == 0) {
            // Wait for user input
        }
        String privateKey = Serial.readString();
        NVS_set_private_key(privateKey);
        Serial.println("AKK");
        set_not_first_boot();
    }
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