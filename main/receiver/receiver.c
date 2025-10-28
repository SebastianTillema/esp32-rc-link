#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "pwm.h"
#include "comm.h"
#include "packet.h"

#define SERVO_MIN_US 1000
#define SERVO_MAX_US 2000
#define ESC_MIN_US 1000
#define ESC_MAX_US 2000
#define RX_QUEUE_LEN 10

static const char *TAG = "RX";

static void on_send(const esp_now_send_info_t *info, esp_now_send_status_t status)
{
    ESP_LOGI(TAG, "Transmitted status: %.*s", status);
}

static inline uint16_t map_control_to_us(int8_t value, uint16_t min_us, uint16_t max_us)
{
    float normalized = (value + 128) / 255.0f; // range [0.0, 1.0]
    return (uint16_t)(min_us + normalized * (max_us - min_us));
}

int iteration = 0;
static QueueHandle_t rx_queue = NULL;
static void on_recv(const esp_now_recv_info_t *info, const uint8_t *data, int len)
{
    control_packet_t pkt;
    memcpy(&pkt, data, sizeof(pkt));
    ESP_LOGI(TAG, "Throttle: %d, Steering: %d", pkt.throttle, pkt.steering);
    if (rx_queue != NULL)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(rx_queue, &pkt, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken)
        {
            portYIELD_FROM_ISR();
        }
    }
    if (++iteration % 100 == 0)
    {
        ESP_LOGI(TAG, "Received: %.*s", len, data);
    }

    // control_packet_t pkt;
    // if (len != sizeof(control_packet_t))
    // {
    //     ESP_LOGW(TAG, "Invalid packet size %d", len);
    //     return;
    // }

    // memcpy(&pkt, data, sizeof(pkt));
    // uint16_t steering_us = map_control_to_us(pkt.steering, SERVO_MIN_US, SERVO_MAX_US);
    // uint16_t throttle_us = map_control_to_us(pkt.throttle, ESC_MIN_US, ESC_MAX_US);

    // // ESP_LOGI(TAG, "Steering=%d → %dus, Throttle=%d → %dus", pkt.steering, steering_us, pkt.throttle, throttle_us);

    // set_pwm_pulsewidth_us(PWM_SERVO_CHANNEL, steering_us);
    // set_pwm_pulsewidth_us(PWM_ESC_CHANNEL, throttle_us);
}

static void rx_task(void *arg)
{
    control_packet_t pkt;
    while (1)
    {
        if (xQueueReceive(rx_queue, &pkt, portMAX_DELAY))
        {
            // Handle the control packet safely here
            ESP_LOGI(TAG, "Throttle: %d, Steering: %d", pkt.throttle, pkt.steering);

            uint16_t steering_us = map_control_to_us(pkt.steering, SERVO_MIN_US, SERVO_MAX_US);
            uint16_t throttle_us = map_control_to_us(pkt.throttle, ESC_MIN_US, ESC_MAX_US);
            set_pwm_pulsewidth_us(PWM_SERVO_CHANNEL, steering_us);
            set_pwm_pulsewidth_us(PWM_ESC_CHANNEL, throttle_us);
        }
    }
}

void app_main(void)
{
    pwm_init();

    set_pwm_pulsewidth_us(PWM_ESC_CHANNEL, 1500);
    set_pwm_pulsewidth_us(PWM_SERVO_CHANNEL, 1500);
    
    uint8_t transmitter_mac[] = {0x78, 0x21, 0x84, 0x80, 0x3d, 0x30};
    comm_init(on_send, on_recv);
    comm_add_peer(transmitter_mac);

    rx_queue = xQueueCreate(RX_QUEUE_LEN, sizeof(control_packet_t));
    if (rx_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create rx_queue");
        return;
    }

    // Create a task to process packets
    xTaskCreate(rx_task, "rx_task", 4096, NULL, 5, NULL);

    // Sweep servo
    // while (1)
    // {
    //     for (int us = 1000; us <= 2000; us += 100)
    //     {
    //         set_pwm_pulsewidth_us(PWM_SERVO_CHANNEL, us);
    //         vTaskDelay(pdMS_TO_TICKS(500));
    //     }
    // }
}
