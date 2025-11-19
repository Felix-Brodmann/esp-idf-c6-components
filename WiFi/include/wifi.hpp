#pragma once

#include <string>
#include <cstring>
#include <array>
#include <esp_timer.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <freertos/semphr.h>
extern "C" {
#include "wifi_helper.h"
}

class WiFi
{
  public:

    /**
     * @brief A Endpoint represents a communication endpoint, identified by a MAC address.
     */
    struct Endpoint
    {
        uint8_t mac[6];
        bool    is_broadcast;
        Endpoint(const uint8_t mac[6]) : is_broadcast(false)
        {
            for (int i = 0; i < 6; i++)
            {
                this->mac[i] = mac[i];
            }
        }
        Endpoint() : is_broadcast(true)
        {
            for (int i = 0; i < 6; i++)
            {
                this->mac[i] = PEER_MAC_BCAST[i];
            }
        }
    };

  private:
    struct PeerInfo
    {
        bool used = false;
        uint64_t last_send = 0;
        uint8_t mac[6] = {0};
    };

    static const size_t MAX_PEERS = 20;
    static const size_t RAW_MAX = 255;
    static WiFi* instance;

    PeerInfo peers[MAX_PEERS];
    payload_t last_payload{};
    payload3_t last_payload3{};
    std::array<uint8_t, RAW_MAX> last_raw{};
    uint8_t last_mac[6]         = {0};
    size_t last_raw_len         = 0;
    portMUX_TYPE last_mac_mux   = portMUX_INITIALIZER_UNLOCKED;
    portMUX_TYPE data_mux       = portMUX_INITIALIZER_UNLOCKED;
    bool has_payload_flag       = false;
    bool has_payload3_flag      = false;
    bool has_raw_flag           = false;
    QueueHandle_t queue         = nullptr;

    static void on_data_send(const wifi_tx_info_t* tx_info, esp_now_send_status_t status);
    static void on_data_receive(const esp_now_recv_info* recv_info, const uint8_t* data, int len);

    void handle_receive(const uint8_t* data, int len);
    std::string mac_to_hash_string(const uint8_t mac[6]) const;
    esp_err_t ensurePeer(const uint8_t mac[6]);

  public:
    /**
     * @brief Creates the WiFi instance
     * @details This initializes the ESP-NOW functionality of the WiFi module.
     * There should be only one instance of this class.
     */
    WiFi();

    /**
     * @brief Attaches the WiFi to a queue
     * @details The WiFi instance sends events to the queue when it receives data.
     * @param queue The queue, to which the WiFi should attach to
     */
    void attach_queue(QueueHandle_t queue);
    
    /**
     * @brief Sends a payload to the provided endpoint
     * @param endpoint The endpoint to send the payload to
     * @param message The payload to send
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t send(const Endpoint& endpoint, const payload_t& message);

    /**
     * @brief Sends a payload3 to the provided endpoint
     * @param endpoint The endpoint to send the payload to
     * @param message The payload3 to send
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t send(const Endpoint& endpoint, const payload3_t& message);

    /**
     * @brief Sends a string message to the provided endpoint
     * @param endpoint The endpoint to send the message to
     * @param message The string message to send
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t send(const Endpoint& endpoint, const std::string& message);

    /**
     * @brief Gets the MAC address of this device
     * @return A pointer to the MAC address (6 bytes)
     */
    uint8_t* get_my_own_mac() const;

    /**
     * @brief Gets the MAC address of the last received message
     * @return A pointer to the MAC address (6 bytes)
     */
    uint8_t* get_last_received_mac();

    /**
     * @brief Checks if a payload has been received
     * @return `true` if a payload has been received, `false` otherwise
     */
    bool has_received_payload() const;

    /**
     * @brief Gets the last received payload
     * @return The last received payload
     */
    payload_t get_last_payload();

    /**
     * @brief Checks if a payload3 has been received
     * @return `true` if a payload3 has been received, `false` otherwise
     */
    bool has_received_payload3() const;

    /**
     * @brief Gets the last received payload3
     * @return The last received payload3
     */
    payload3_t get_last_payload3();

    /**
     * @brief Checks if raw data has been received
     * @return `true` if raw data has been received, `false` otherwise
     */
    bool has_received_raw() const;

    /**
     * @brief Gets the last received raw data
     * @return The last received raw data as an array of bytes
     */
    std::array<uint8_t, RAW_MAX> get_last_raw();

};