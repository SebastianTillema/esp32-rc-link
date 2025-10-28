#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "comm.h"
#include "packet.h"
#include "input_processing.h"

#define UPDATE_FREQUENCY_MS 50

#define ADC_UNIT ADC_UNIT_1
#define STEERING_CHANNEL ADC_CHANNEL_3
#define THROTTLE_CHANNEL ADC_CHANNEL_0

static const char *TAG = "TX";

static void on_send(const esp_now_send_info_t *info, esp_now_send_status_t status)
{
    // ESP_LOGI(TAG, "Transmitted status: %.*s", status);
}

static void on_recv(const esp_now_recv_info_t *info, const uint8_t *data, int len)
{
    ESP_LOGI(TAG, "Received: %.*s", len, data);
}

void app_main(void)
{
    uint8_t receiver_mac[] = {0x78, 0x21, 0x84, 0x80, 0x4c, 0x64};
    comm_init(on_send, on_recv);
    comm_add_peer(receiver_mac);

    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, THROTTLE_CHANNEL, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, STEERING_CHANNEL, &config));

    int iteration = 0;

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(UPDATE_FREQUENCY_MS));

        int raw_throttle = 0, raw_steering = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, THROTTLE_CHANNEL, &raw_throttle));
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, STEERING_CHANNEL, &raw_steering));

        int8_t throttle = compute_throttle(raw_throttle);
        int8_t steering = compute_steering(raw_steering);
        if (!is_change_significant()) continue;

        control_packet_t pkt = {
            .steering = steering,
            .throttle = throttle
        };

        ESP_LOGI(TAG, "Throttle raw=%d mapped=%d. Steering raw=%d mapped %d", raw_throttle, throttle, raw_steering, steering);
        esp_now_send(receiver_mac, (const uint8_t *)&pkt, sizeof(pkt));
    }
}
