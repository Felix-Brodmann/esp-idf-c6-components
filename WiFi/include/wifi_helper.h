#ifndef WIFI_HELPER_H
#define WIFI_HELPER_H
#pragma once

// Wifi includes
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_err.h"
#include "esp_log.h"

#define TAG "ESPNOW"
#define WIFI_CHANNEL 1 

extern const uint8_t PEER_MAC_BCAST[6]; // Broadcast-Adresse
extern const uint8_t PEER_MAC_UNICAST[6]; // Unicast-Adresse 

typedef struct __attribute__((packed)) {
    uint32_t seq;
    uint8_t  type;
    uint8_t  value;
} payload_t;

typedef struct __attribute__((packed)) {
    uint32_t seq;
    uint8_t  type;
    uint8_t  value1;
    uint8_t  value2;
    uint8_t  value3;
} payload3_t;

void mac2str(const uint8_t mac[6], char out[18]);
void wifi_init(void);

#endif