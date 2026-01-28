#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#define LED_GPIO 2

void app_main(void) {
  gpio_reset_pin(LED_GPIO);
  gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

  while (1) {
    gpio_set_level(LED_GPIO, 1); // LED ON
    vTaskDelay(pdMS_TO_TICKS(500));
    printf("LED ON\n");

    gpio_set_level(LED_GPIO, 0); // LED OFF
    vTaskDelay(pdMS_TO_TICKS(500));
    printf("LED OFF\n");
  }
}