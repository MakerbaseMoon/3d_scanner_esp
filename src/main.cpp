#include "header.h"

void setup() {
    Serial.begin(115200);
    while(!Serial);

    init_nvs();
    init_network();
    init_server();

    module_init();
    set_command(SCANNER_COMMAND_HOME);
}

void loop() {
    scanner_loop();
    ota_loop();
}
