#include "pwm.h"
#include "driver/ledc.h"
#include "esp_log.h"

static const char *TAG = "PWM";

void pwm_init(void)
{
    // Configure PWM timer
    ledc_timer_config_t timer_conf = {
        .speed_mode       = PWM_MODE,
        .timer_num        = PWM_TIMER,
        .duty_resolution  = PWM_RESOLUTION,
        .freq_hz          = PWM_FREQ,
        .clk_cfg          = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer_conf);

    // Configure ESC channel
    ledc_channel_config_t esc_channel_conf = {
        .gpio_num   = PWM_ESC_GPIO,
        .speed_mode = PWM_MODE,
        .channel    = PWM_ESC_CHANNEL,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = PWM_TIMER,
        .duty       = 0,
        .hpoint     = 0,
    };
    ledc_channel_config(&esc_channel_conf);

    // Configure Servo channel
    ledc_channel_config_t servo_channel_conf = {
        .gpio_num   = PWM_SERVO_GPIO,
        .speed_mode = PWM_MODE,
        .channel    = PWM_SERVO_CHANNEL,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = PWM_TIMER,
        .duty       = 0,
        .hpoint     = 0,
    };
    ledc_channel_config(&servo_channel_conf);

    // ESP_LOGI(TAG, "PWM initialized: ESC on GPIO%d, Servo on GPIO%d", PWM_ESC_GPIO, PWM_SERVO_GPIO);
}

void set_pwm_pulsewidth_us(uint32_t channel, uint32_t pulse_width_us)
{
    uint32_t period_us = 1000000UL / PWM_FREQ;
    uint32_t max_duty = (1 << PWM_RESOLUTION) - 1;

    uint32_t duty = (pulse_width_us * max_duty) / period_us;

    if (duty > max_duty) {
        duty = max_duty; // clamp
    }

    ledc_set_duty(PWM_MODE, channel, duty);
    ledc_update_duty(PWM_MODE, channel);

    // ESP_LOGI(TAG, "Channel %u: %u us (duty=%u)", channel, pulse_width_us, duty);
}
