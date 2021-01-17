
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sdkconfig.h"
#include "esp_log.h"

#include "esp_wifi.h"
#include "esp_event.h"

#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"


#include "vfd_driver.h"
#include "open_wm.h"


void app_main(void)
{
    //wifi_http_get();
    vfd_config();

    uint sophie_s =     0b00000000000010000000110111010010;
    uint sophie_o =     0b00000000000001000000111101110010;
    uint sophie_p =     0b00000000000000100000111110100010;
    uint sophie_h =     0b00000000000000010000101111100010;
    uint sophie_i =     0b00000000000000001000101001000011;
    uint sophie_e =     0b00000000000000000100110110110010;
    uint sophie_broil = 0b00000000000000000001110110110010;
    int cnt = 0;
    
    gpio_set_level(PIN_STROBE, 0); //toggle once every ~600ms
    while (1) {
        cnt += 1;

 
        vfd_send(&vfd_dev, sophie_s);
        vTaskDelay(1);
        vfd_send(&vfd_dev, sophie_o);
        vTaskDelay(1);
        vfd_send(&vfd_dev, sophie_p);
        vTaskDelay(1);
        vfd_send(&vfd_dev, sophie_h);
        vTaskDelay(1);
        vfd_send(&vfd_dev, sophie_i);
        vTaskDelay(1);
        vfd_send(&vfd_dev, sophie_e);
        vTaskDelay(1);
        vfd_send(&vfd_dev, sophie_broil);
        vTaskDelay(1);
        
        gpio_set_level(PIN_STROBE, (cnt / 130) % 2); //toggle once every ~600ms
        
    }

    esp_restart();
}

void getTemp() {
}

