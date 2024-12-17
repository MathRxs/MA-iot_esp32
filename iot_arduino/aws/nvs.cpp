#include <Preferences.h>
#include "nvs.hpp"
Preferences prefs;

void NVS_setup() {
    prefs.begin("nvs", false);
}

void NVS_set_register() {
    prefs.putString("is_registered", "true");
}
bool NVS_get_register() {
    String oe = prefs.getString("is_registered","false");
    Serial.println("AAAAAAAAAAAAAAA is_registered: " + oe);
    return oe == "true";
}

void NVS_get_thingsname(String *thingname) {
    *thingname = prefs.getString("thingname");
}

void NVS_set_thingsname(String thingname) {
    prefs.putString("thingname", thingname);
}

void NVS_get_certificate(String *certificate) {
    *certificate = prefs.getString("certificate");
}

void NVS_set_certificate(String certificate) {
    prefs.putString("certificate", certificate);
}

void NVS_get_private_key(String *private_key) {
    *private_key = prefs.getString("private_key");
}

void NVS_set_private_key(String private_key) {
    prefs.putString("private_key", private_key);
}

void NVS_claim_certificate(String *certificate) {
    *certificate = prefs.getString("claim_cert");
}

void NVS_set_claim_certificate(String certificate) {
    prefs.putString("claim_cert", certificate);
}

void NVS_set_claim_private_key(String private_key) {
    prefs.putString("claim_private_key", private_key);
}

void NVS_claim_private_key(String *private_key) {
    *private_key = prefs.getString("claim_private_key");
}

void set_not_first_boot() {
    prefs.putString("first_boot", "false");
}

bool is_first_boot() {
    String oe = prefs.getString("first_boot","true");
    Serial.println("AAAAAAAAAAAAAAA first_boot: " + oe);
    return oe == "true";
}
void NVS_clear() {
    prefs.clear();
}

