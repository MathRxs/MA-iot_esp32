#ifndef AWS_H
#define AWS_H

#define THINGNAME "need_to_find_a_name"
#define PUBLISH_INTERVAL_SECONDS (20)
#define PUBLISH_INTERVAL (PUBLISH_INTERVAL_SECONDS*1000)  // 4 seconds

void AWS_connect();
void AWS_loop();
bool AWS_should_publish_data();
void AWS_send();
void AWS_chip_id(String* Chip_id);

#endif
