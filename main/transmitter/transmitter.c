#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "comm.h"
#include "packet.h"

static const char *TAG = "TX";

static void on_send(const esp_now_send_info_t *info, esp_now_send_status_t status) {
    ESP_LOGI(TAG, "Transmitted status: %.*s", status);
}

static void on_recv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    ESP_LOGI(TAG, "Received: %.*s", len, data);
}

void app_main(void)
{
    uint8_t broadcast_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    comm_init(on_send, on_recv);
    comm_add_peer(broadcast_mac);

    while (1) {
        control_packet_t pkt = {
            .steering = 127,
            .throttle = 0
        };
        esp_now_send(broadcast_mac, (const uint8_t *)&pkt, sizeof(pkt));
        ESP_LOGI(TAG, "Sent message");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
