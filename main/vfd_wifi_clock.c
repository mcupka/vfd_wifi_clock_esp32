
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
#include "vfd_chars.h"
#include "open_wm.h"

#define MS_BETWEEN_OWM_UPDATES 10000

void task_display_data();
void task_update_temperature();

int wifi_connected;

void app_main(void)
{
    vfd_config();

    ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());
  
    wifi_connected = 1;

    uint broil = 0b00000000000000000001110110110010;


    vfd_enable_grid(0);
    vfd_enable_grid(1);
    vfd_enable_grid(2);
    vfd_set_grid_bits(0, vfd_asciitable['S']);
    vfd_set_grid_bits(1, vfd_asciitable['O']);
    vfd_set_grid_bits(2, vfd_asciitable['P']);
    vfd_set_grid_bits(7, broil);

    vfd_showint(68, VFD_SSEG_LARGE, VFD_ALIGN_RIGHT, VFD_CLEAR_DISPLAY);

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
        if (wifi_connected == 1) {
            struct owm_data_t dat = get_owm_data();
            printf("\nTEMP=%.2f\n", dat.temp_now);
            printf("\nTEMPINT=%d\n", (int)dat.temp_now);
            printf("TEMPREMAINDER=%d\n", (int)(100.0 * (dat.temp_now - (float)((int) dat.temp_now))));
            vfd_showint((int)dat.temp_now, VFD_SSEG_LARGE, VFD_ALIGN_LEFT, VFD_NOCLEAR_DISPLAY);
            vfd_showint((int)(100.0 * (dat.temp_now - (float)((int) dat.temp_now))), VFD_SSEG_LARGE, VFD_ALIGN_RIGHT, VFD_NOCLEAR_DISPLAY);
            vfd_set_grid_bits(4, VFD_AN_BOTCOLON); //decimal point
            wifi_http_get();
        }
        else {
            ESP_ERROR_CHECK(example_connect());
            wifi_connected = 1;
        }
       vTaskDelay(MS_BETWEEN_OWM_UPDATES/portTICK_PERIOD_MS); 
    }    

}
