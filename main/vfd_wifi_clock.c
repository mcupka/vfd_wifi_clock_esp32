
j#include <stdio.h>
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

    //set up gpio for le and strobe
    gpio_config_t io_conf;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = (1 << PIN_LE) | (1 << PIN_STROBE); //pins 4 and 5
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

     //SET le low
    gpio_set_level(PIN_LE, 0);


   printf("Sending SOPHIE via spi to the vfd controller\n");

    spi_bus_config_t buscfg={
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };

    //Initialize the SPI bus
    esp_err_t ret = spi_bus_initialize(VFD_HOST, &buscfg, DMA_CHAN);
    ESP_ERROR_CHECK(ret);
    if (ret != ESP_OK) printf("Error: failed to initialize spi bus\n"); 
    else printf("Success: initialized spi bus!\n"); 


    //Add "device" to spi bus

    spi_device_handle_t vfd_dev;
    spi_device_interface_config_t vfd_dev_config = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 3,
        .duty_cycle_pos = 128,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = SPI_MASTER_FREQ_10M,
        .input_delay_ns = 0,
        .spics_io_num = -1,
        .flags = 0,
        .queue_size = 8
    };


    ret = spi_bus_add_device(VFD_HOST, &vfd_dev_config, &vfd_dev);
    ESP_ERROR_CHECK(ret);
    if (ret != ESP_OK) printf("Error: failed to add device to spi bus\n"); 
    else  printf("Success: added device to spi bus!\n"); 


    //Set gpio pins up for le and strobe pins


    //send "sophie" via spi to the vfd controller 
   
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

