#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "pwm.h"
#include "communication.h"

static const char *TAG = "RX";

static void on_send(const esp_now_send_info_t *info, esp_now_send_status_t status) {
    ESP_LOGI(TAG, "Transmitted status: %.*s", status);
}

static void on_recv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    ESP_LOGI(TAG, "Received: %.*s", len, data);
}

void app_main(void)
{
    pwm_init();

    set_pwm_pulsewidth_us(PWM_ESC_CHANNEL, 1500);
    set_pwm_pulsewidth_us(PWM_SERVO_CHANNEL, 1500);

    comm_init(on_send, on_recv);

    // Sweep servo
    while (1) {
        for (int us = 1000; us <= 2000; us += 100) {
            set_pwm_pulsewidth_us(PWM_SERVO_CHANNEL, us);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}
