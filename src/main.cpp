#include "header.h"

void setup() {
    Serial.begin(115200);
    while(!Serial);

    init_nvs();
    init_network();
}

void loop() {

}
