#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "pwm.h"

void app_main(void)
{
    pwm_init();

    set_pwm_pulsewidth_us(PWM_ESC_CHANNEL, 1500);
    set_pwm_pulsewidth_us(PWM_SERVO_CHANNEL, 1500);

    // Sweep servo
    while (1) {
        for (int us = 1000; us <= 2000; us += 100) {
            set_pwm_pulsewidth_us(PWM_SERVO_CHANNEL, us);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}
