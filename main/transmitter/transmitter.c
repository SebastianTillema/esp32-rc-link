#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "comm.h"
#include "packet.h"

#define POT_CHANNEL ADC_CHANNEL_0
#define ADC_UNIT ADC_UNIT_1

static const char *TAG = "TX";

static void on_send(const esp_now_send_info_t *info, esp_now_send_status_t status)
{
    ESP_LOGI(TAG, "Transmitted status: %.*s", status);
}

static void on_recv(const esp_now_recv_info_t *info, const uint8_t *data, int len)
{
    ESP_LOGI(TAG, "Received: %.*s", len, data);
}

void app_main(void)
{
    uint8_t broadcast_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    comm_init(on_send, on_recv);
    comm_add_peer(broadcast_mac);

    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, POT_CHANNEL, &config));
    while (1)
    {
        int raw = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, POT_CHANNEL, &raw));

        int8_t throttle = (int8_t)((raw * 255 / 4095) - 128);
        control_packet_t pkt = {
            .steering = 0, // fixed steering for now
            .throttle = throttle};

        esp_now_send(broadcast_mac, (const uint8_t *)&pkt, sizeof(pkt));
        ESP_LOGI(TAG, "Throttle raw=%d mapped=%d", raw, throttle);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
