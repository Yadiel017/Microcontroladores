#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>


void app_main(void) {
  while (1) {
    printf("Hola, mundo!\n");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
