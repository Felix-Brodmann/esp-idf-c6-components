#include <wifi.hpp>

void WiFi::handle_receive(const uint8_t* data, int len)
{
    if (queue != nullptr)
    {
        if (len == sizeof(payload_t)) {
            payload_t msg;
            memcpy(&msg, data, sizeof(msg));
            xQueueSendFromISR(queue, &msg, NULL);
        } else if (len == sizeof(payload3_t)) {
            payload3_t msg;
            memcpy(&msg, data, sizeof(msg));
            xQueueSendFromISR(queue, &msg, NULL);
        } else {
            size_t copy_len = len > 255 ? 255 : len;
            char raw_data[255];
            memcpy(raw_data, data, copy_len);
            xQueueSendFromISR(queue, &raw_data, NULL);
        }
    }
    portENTER_CRITICAL(&data_mux);
    if (len == sizeof(payload_t)) {
        has_payload_flag = true;
        memcpy(&last_payload, data, sizeof(last_payload));
    } else if (len == sizeof(payload3_t)) {
        has_payload3_flag = true;
        memcpy(&last_payload3, data, sizeof(last_payload3));
    } else {
        has_raw_flag = true;
        last_raw_len = len > RAW_MAX ? RAW_MAX : len;
        for (size_t i = 0; i < last_raw_len; ++i)
            last_raw[i] = data[i];
    }
    portEXIT_CRITICAL(&data_mux);
}

void WiFi::on_data_send(const wifi_tx_info_t* tx_info, esp_now_send_status_t status)
{
    const uint8_t* mac_addr = tx_info->des_addr;
    char mac_str[18];
    mac2str(mac_addr, mac_str);
    printf("\n __________________________________\n");
    printf("|                                  |\n");
    printf("| [ESP-NOW] Data sent              |\n");
    printf("| [ESP-NOW] To: %s  |\n", mac_str);
    printf("| [ESP-NOW] Status: %s           |\n", status == ESP_NOW_SEND_SUCCESS ? "OK  " : "FAIL");
    printf("|__________________________________|\n\n");
}

void WiFi::on_data_receive(const esp_now_recv_info* recv_info, const uint8_t* data, int len)
{
    const uint8_t* mac = recv_info->src_addr;
    char mac_str[18];
    mac2str(mac, mac_str);
    printf("\n ______________________________________________\n");
    printf("|                                              |\n");
    printf("| [ESP-NOW] Data received                      |\n");
    printf("| [ESP-NOW] From: %s            |\n", mac_str);
    printf("| [ESP-NOW] Length: %d                          |\n", len);

    if (len == sizeof(payload_t)) {
        payload_t msg;
        memcpy(&msg, data, sizeof(msg));
        printf("| [ESP-NOW] Type: payload_t:                   |\n");
        printf("| [ESP-NOW]    seq=%-6" PRIu32 "                      |\n", msg.seq);
        printf("| [ESP-NOW]    type=%u                              |\n", msg.type);
        printf("| [ESP-NOW]    value=%02X                        |\n", msg.value);
    }
    else if (len == sizeof(payload3_t)) {
        payload3_t msg;
        memcpy(&msg, data, sizeof(msg));
        printf("| [ESP-NOW] Type: payload3_t:                  |\n");
        printf("| [ESP-NOW]    seq=%-6" PRIu32 "                      |\n", msg.seq);
        printf("| [ESP-NOW]    type=%u                          |\n", msg.type);
        printf("| [ESP-NOW]    value1=%02X                       |\n", msg.value1);
        printf("| [ESP-NOW]    value2=%02X                       |\n", msg.value2);
        printf("| [ESP-NOW]    value3=%02X                       |\n", msg.value3);
    }
    else {
        printf ("| [ESP-NOW] Type: Raw data:                      |\n");
        printf("| [ESP-NOW] Data: ");
        for (int i = 0; i < len; i++) {
            printf("%02X ", data[i]);
        }
        printf("       |\n");
    }
    printf("|______________________________________________|\n\n");
    portENTER_CRITICAL_ISR(&instance->last_mac_mux);
    for (int i = 0; i < 6; i++) instance->last_mac[i] = mac[i];
    portEXIT_CRITICAL_ISR(&instance->last_mac_mux);
    instance->handle_receive(data, len);
}

esp_err_t WiFi::ensurePeer(const uint8_t mac[6])
{
    uint64_t now = esp_timer_get_time();

    // If the peer is already known, update and return
    for (size_t i = 0; i < MAX_PEERS; i++)
    {
        if (peers[i].used && memcmp(peers[i].mac, mac, 6) == 0)
        {
            peers[i].last_send = now;
            return ESP_OK;
        }
    }

    // If we reached the max number of peers, remove the oldest one
    for (size_t i = 0; i < MAX_PEERS; i++)
    {
        if (!peers[i].used)
        {
            memcpy(peers[i].mac, mac, 6);
            peers[i].last_send = now;
            peers[i].used = true;

            // Peer in ESP-NOW registrieren
            esp_now_peer_info_t info = {};
            memcpy(info.peer_addr, mac, 6);
            info.channel = WIFI_CHANNEL;
            info.encrypt = false;
            return esp_now_add_peer(&info);
        }
    }
    size_t oldest_idx = 0;
    for (size_t i = 1; i < MAX_PEERS; i++)
    {
        if (peers[i].last_send < peers[oldest_idx].last_send)
            oldest_idx = i;
    }
    esp_err_t delRes = esp_now_del_peer(peers[oldest_idx].mac);
    memcpy(peers[oldest_idx].mac, mac, 6);
    peers[oldest_idx].last_send = now;

    // Add the new peer
    esp_now_peer_info_t info = {};
    memcpy(info.peer_addr, mac, 6);
    info.channel = WIFI_CHANNEL;
    info.encrypt = false;
    esp_err_t addRes = esp_now_add_peer(&info);
    peers[oldest_idx].used = true;

    if (delRes != ESP_OK) return delRes;
    return addRes;
}

WiFi::WiFi()
{
    instance = this;
    wifi_init();
    ESP_ERROR_CHECK(esp_now_register_send_cb(on_data_send));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(on_data_receive));
}

void WiFi::attach_queue(QueueHandle_t queue)
{
    this->queue = queue;
}

esp_err_t WiFi::send(const Endpoint& endpoint, const payload_t& message)
{
    if (!endpoint.is_broadcast)
    {
        if (esp_err_t res = ensurePeer(endpoint.mac); res != ESP_OK)
            return res;
    }
    return esp_now_send(endpoint.mac,
                        reinterpret_cast<const uint8_t*>(&message), 
                        sizeof(payload_t));
}

esp_err_t WiFi::send(const Endpoint& endpoint, const payload3_t& message)
{
    if (!endpoint.is_broadcast)
    {
        if (esp_err_t res = ensurePeer(endpoint.mac); res != ESP_OK)
            return res;
    }
    return esp_now_send(endpoint.mac,
                        reinterpret_cast<const uint8_t*>(&message), 
                        sizeof(payload3_t));
}

esp_err_t WiFi::send(const Endpoint& endpoint, const std::string& message)
{
    if (message.size() > 250) return ESP_ERR_INVALID_SIZE;
    if (!endpoint.is_broadcast)
    {
        if (esp_err_t res = ensurePeer(endpoint.mac); res != ESP_OK)
            return res;
    }
    return esp_now_send(endpoint.mac,
                        reinterpret_cast<const uint8_t*>(message.data()),
                        message.size());
}

uint8_t* WiFi::get_my_own_mac() const
{
    static uint8_t mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, mac);
    return mac;
}

uint8_t* WiFi::get_last_received_mac() 
{
    portENTER_CRITICAL(&last_mac_mux);
    static uint8_t mac_copy[6];
    memcpy(mac_copy, last_mac, 6);
    portEXIT_CRITICAL(&last_mac_mux);
    return mac_copy;
}

bool WiFi::has_received_payload() const
{
    return has_payload_flag;
}

payload_t WiFi::get_last_payload()
{
    payload_t copy;
    portENTER_CRITICAL(&data_mux);
    copy = last_payload;
    portEXIT_CRITICAL(&data_mux);
    return copy;
}

bool WiFi::has_received_payload3() const
{
    return has_payload3_flag;
}

payload3_t WiFi::get_last_payload3()
{
    payload3_t copy;
    portENTER_CRITICAL(&data_mux);
    copy = last_payload3;
    portEXIT_CRITICAL(&data_mux);
    return copy;
}

bool WiFi::has_received_raw() const
{
    return has_raw_flag;
}

std::array<uint8_t, WiFi::RAW_MAX> WiFi::get_last_raw()
{
    std::array<uint8_t, RAW_MAX> copy{};
    portENTER_CRITICAL(&data_mux);
    for (size_t i = 0; i < last_raw_len; ++i)
        copy[i] = last_raw[i];
    portEXIT_CRITICAL(&data_mux);
    return copy;
}


WiFi* WiFi::instance = nullptr;