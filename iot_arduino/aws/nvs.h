#ifndef NVS_H
#define NVS_H

#include <Arduino.h>

void NVS_setup();
void NVS_set_register();
bool NVS_get_register();
void NVS_get_thingsname(String *thingname);
void NVS_set_thingsname(String thingname);
void NVS_get_certificate(String *certificate);
void NVS_set_certificate(String certificate);
void NVS_get_private_key(String *private_key);
void NVS_set_private_key(String private_key);
void NVS_clear();

#endif // NVS_H