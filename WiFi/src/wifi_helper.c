#include "wifi_helper.h"

// Broadcast-Adresse
const uint8_t PEER_MAC_BCAST[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

// Helper Function 
void mac2str(const uint8_t mac[6], char out[18]) {
    snprintf(out, 18, "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void wifi_init(void) {
    // Init
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta = esp_netif_create_default_wifi_sta();
    (void)sta;

    // Config
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    wifi_config_t wc = {0};
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wc));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Set Channel (no AP)
    ESP_ERROR_CHECK(esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE));

    ESP_ERROR_CHECK(esp_now_init());
    
    // Get MAC
    uint8_t mac[6]; esp_wifi_get_mac(WIFI_IF_STA, mac);
    char mac_string[18]; mac2str(mac, mac_string);
    ESP_LOGI(TAG, "WIFI ready on channel %d, MAC %s", WIFI_CHANNEL, mac_string);
}