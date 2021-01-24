#ifndef OPEN_WM_H
#define OPEN_WM_H

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

#include "protocol_examples_common.h"

#include "cJSON.h"

#define WEB_SERVER "api.openweathermap.org"
#define WEB_PORT "80"
#define WEB_PATH "/data/2.5/weather?q=Indianapolis&appid=22b03431ced097033d68e5e4d5477279&units=imperial&mode=json"


typedef struct owm_data_t {
    double temp_now;
    double temp_high;
    double temp_low;
    double temp_feel;
};

static const char *TAG = "OWM REQUEST";
static const char *REQUEST = "GET " WEB_PATH " HTTP/1.0\r\n"
    "Host: "WEB_SERVER":"WEB_PORT"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
    "\r\n";


char * http_extract_json_string(char *);

void wifi_http_get();

struct owm_data_t get_owm_data(); 

#endif
