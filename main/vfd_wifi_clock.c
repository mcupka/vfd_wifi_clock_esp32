/* Hello World Example
This example code is in the Public Domain (or CC0 licensed, at your option.) 
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"

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

#include "protocol_examples_common.h"

#include "cJSON.h"

#define WEB_SERVER "api.openweathermap.org"
#define WEB_PORT "80"
#define WEB_PATH "/data/2.5/weather?q=Indianapolis&appid=22b03431ced097033d68e5e4d5477279&units=imperial&mode=json"

#define VFD_HOST VSPI_HOST
#define DMA_CHAN     2
#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define PIN_LE       32
#define PIN_STROBE   14



void vfd_send(spi_device_handle_t *vfd_dev, uint data);
void wifi_http_get();


char * http_extract_json_string(char *);

static const char *TAG = "example_http";

static const char *REQUEST = "GET " WEB_PATH " HTTP/1.0\r\n"
    "Host: "WEB_SERVER":"WEB_PORT"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
    "\r\n";


void app_main(void)
{
    wifi_http_get();

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


void vfd_send(spi_device_handle_t *vfd_dev, uint data) {
   
    esp_err_t ret;

    spi_transaction_t vfd_trans = {
        .flags = SPI_TRANS_USE_TXDATA,
        .cmd = 0,
        .addr = 0,
        .length = 32,
        .rxlength = 0,
        .user = -1,
        .tx_buffer = NULL,
        .tx_data = {data >> 24, data >> 16, data >> 8,  data},
        .rx_buffer = NULL,
        .rx_data = {0x00, 0x00, 0x00, 0x00}
    };

  
    ret = spi_device_transmit(*vfd_dev, &vfd_trans);
    if (ret != ESP_OK) printf("Error: failed to complete transaction S\n"); 

    //SET le high
    gpio_set_level(PIN_LE, 1);

    //SET le low
    gpio_set_level(PIN_LE, 0);

}

void wifi_http_get() {
    ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(example_connect());
    
        const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    char recv_buf[1000];

    while(1) {
        int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

        if(err != 0 || res == NULL) {
            ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        /* Code to print the resolved IP.

           Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
        addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

        s = socket(res->ai_family, res->ai_socktype, 0);
        if(s < 0) {
            ESP_LOGE(TAG, "... Failed to allocate socket.");
            freeaddrinfo(res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... allocated socket");

        if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
            ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
            close(s);
            freeaddrinfo(res);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }

        ESP_LOGI(TAG, "... connected");
        freeaddrinfo(res);

        if (write(s, REQUEST, strlen(REQUEST)) < 0) {
            ESP_LOGE(TAG, "... socket send failed");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... socket send success");

        struct timeval receiving_timeout;
        receiving_timeout.tv_sec = 5;
        receiving_timeout.tv_usec = 0;
        if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                sizeof(receiving_timeout)) < 0) {
            ESP_LOGE(TAG, "... failed to set socket receiving timeout");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... set socket receiving timeout success");

        printf("%s\n-+-+-+-+\n", recv_buf);

        /* Read HTTP response */
        do {
            bzero(recv_buf, sizeof(recv_buf));
            r = read(s, recv_buf, sizeof(recv_buf)-1);
            //for(int i = 0; i < r; i++) {
            //    putchar(recv_buf[i]);
            //}

            if (r > 0) {

                printf("\nPRINTING HTTP\n");
                printf("%s\n", recv_buf);
                printf("\n\nDONE PRINTING HTTP\n");

                char * http_json = http_extract_json_string(&(recv_buf[0]));
                const char * jsonstring = http_json; 
                cJSON * jsonstruct = cJSON_Parse(jsonstring);

                cJSON * json_main = cJSON_GetObjectItem(jsonstruct, "main");
                if (json_main == NULL) printf("JSONMAIN = NULL\n");
                double temp = cJSON_GetObjectItem(json_main, "temp")->valuedouble;
           
                printf("\nTemp = %d\n", (int)round(temp));
            }
            } while(r > 0);

        ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d.", r, errno);
        close(s);
        for(int countdown = 10; countdown >= 0; countdown--) {
            ESP_LOGI(TAG, "%d... ", countdown);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "Starting again!");
    }
}


void getTemp() {

}

//This function takes the http response from the website in a string, and returns a string containing only the JSON line
char * http_extract_json_string(char * original) {

    char * token = strtok(original, "\n");

    char JSONSearchString [] = "{\"coord\":{";
    int isNotJSON;

    while (token != NULL) {
        isNotJSON = strncmp(token, JSONSearchString, strlen(JSONSearchString));
        if (!isNotJSON) return token;
        token = strtok(NULL, "\n");
    }

    //if no json weather string found, return NULL
    return NULL;
}
