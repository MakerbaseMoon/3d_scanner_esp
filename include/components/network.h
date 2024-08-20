// Path: include/components/network.h
#ifndef __3D_SCANNER_NETWORK_H__
#define __3D_SCANNER_NETWORK_H__

#include <Arduino.h>

#include <WiFi.h>
#include <ESPmDNS.h>
#include <DNSServer.h>

#include <string.h>

#include "components/data.h"

#define NETWORK_TAG_NAME "NETWORK"

void init_network();

#endif // __3D_SCANNER_NETWORK_H__