#ifndef PWM_H
#define PWM_H

#include "driver/ledc.h"
#include <stdint.h>

#define PWM_ESC_GPIO      18
#define PWM_SERVO_GPIO    19
#define PWM_FREQ          50   // Hz
#define PWM_RESOLUTION    LEDC_TIMER_16_BIT
#define PWM_TIMER         LEDC_TIMER_0
#define PWM_MODE          LEDC_LOW_SPEED_MODE
#define PWM_ESC_CHANNEL   LEDC_CHANNEL_0
#define PWM_SERVO_CHANNEL LEDC_CHANNEL_1

// Initialize PWM timer and channels
void pwm_init(void);

// Set duty cycle in microseconds
void set_pwm_pulsewidth_us(uint32_t channel, uint32_t pulse_width_us);

#endif // PWM_H