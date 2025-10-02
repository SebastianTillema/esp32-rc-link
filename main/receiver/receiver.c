#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "pwm.h"
#include "comm.h"
#include "packet.h"

#define SERVO_MIN_US   1000
#define SERVO_MAX_US   2000
#define ESC_MIN_US     1000
#define ESC_MAX_US     2000

static const char *TAG = "RX";

static void on_send(const esp_now_send_info_t *info, esp_now_send_status_t status) {
    ESP_LOGI(TAG, "Transmitted status: %.*s", status);
}

static inline uint16_t map_control_to_us(int8_t value, uint16_t min_us, uint16_t max_us) {
    float normalized = (value + 128) / 255.0f; // range [0.0, 1.0]
    return (uint16_t)(min_us + normalized * (max_us - min_us));
}

static void on_recv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    ESP_LOGI(TAG, "Received: %.*s", len, data);
    control_packet_t pkt;
    if (len != sizeof(control_packet_t)) {
        ESP_LOGW(TAG, "Invalid packet size %d", len);
        return;
    }

    memcpy(&pkt, data, sizeof(pkt));
    uint16_t steering_us = map_control_to_us(pkt.steering, SERVO_MIN_US, SERVO_MAX_US);
    uint16_t throttle_us = map_control_to_us(pkt.throttle, ESC_MIN_US, ESC_MAX_US);

    ESP_LOGI(TAG, "Steering=%d → %dus, Throttle=%d → %dus", pkt.steering, steering_us, pkt.throttle, throttle_us);

    set_pwm_pulsewidth_us(PWM_SERVO_CHANNEL, steering_us);
    set_pwm_pulsewidth_us(PWM_ESC_CHANNEL, throttle_us);
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
