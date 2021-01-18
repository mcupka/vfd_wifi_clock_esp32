
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

#define MS_BETWEEN_OWM_UPDATES 10000

void task_display_data();
void task_update_temperature();

void app_main(void)
{
    vfd_config();

    ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());
   
    uint sophie_s =     0b00000000000010000000110111010010;
    uint sophie_o =     0b00000000000001000000111101110010;
    uint sophie_p =     0b00000000000000100000111110100010;
    uint sophie_h =     0b00000000000000010000101111100010;
    uint sophie_i =     0b00000000000000001000101001000011;
    uint sophie_e =     0b00000000000000000100110110110010;
    uint sophie_broil = 0b00000000000000000001110110110010;

    vfd_set_grid_bits(0, sophie_s);
    vfd_set_grid_bits(1, sophie_o);
    vfd_set_grid_bits(2, sophie_p);
    vfd_set_grid_bits(3, sophie_h);
    vfd_set_grid_bits(4, sophie_i);
    vfd_set_grid_bits(5, sophie_e);
    vfd_set_grid_bits(7, sophie_broil);

    //launch task to display data on VFD
    xTaskCreate(task_display_data, "DISPLAY_TASK", 1000, NULL, configMAX_PRIORITIES - 1, NULL);

    //launch task to periodically connect to wifi and update the temperature value. Set to lower priority than display task so that the display can always seem fluid
    xTaskCreate(task_update_temperature, "TEMPERATURE_TASK", 10000, NULL, configMAX_PRIORITIES - 2, NULL);

    //loop, tasks should handle everything
    while (1) {
        vTaskDelay(10000);
    }


    esp_restart();
}

    

void task_display_data() {
    //int cnt = 0;
    
    gpio_set_level(PIN_STROBE, 0); //toggle once every ~600ms
    while (1) {
        //cnt += 1;

        vfd_onecycle(); 
        //gpio_set_level(PIN_STROBE, (cnt / 130) % 2); //toggle once every ~600ms
        
    }
}

void task_update_temperature() {

    while (1) {
        wifi_http_get();
        struct owm_data_t dat = get_owm_data();
        printf("\nTEMP=%d\n", dat.temp_now);
        vTaskDelay(MS_BETWEEN_OWM_UPDATES/portTICK_PERIOD_MS); 
    }    

}
