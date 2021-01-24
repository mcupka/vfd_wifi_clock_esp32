#include "open_wm.h"

//#define DEBUG_PRINT

struct owm_data_t owm_data = {
    .temp_now = 0.0,
    .temp_high = 0.0,
    .temp_low = 0.0,
    .temp_feel = 0.0
};


extern int wifi_connected;

void wifi_http_get() {
    
        const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    char recv_buf[1000];

    int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

    if(err != 0 || res == NULL) {
        ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        return;
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
        return;
    }
    ESP_LOGI(TAG, "... allocated socket");

    if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
        ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
        close(s);
        freeaddrinfo(res);
        wifi_connected = 0;
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        return;
    }

    ESP_LOGI(TAG, "... connected");
    freeaddrinfo(res);

    if (write(s, REQUEST, strlen(REQUEST)) < 0) {
        ESP_LOGE(TAG, "... socket send failed");
        close(s);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        return;
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
        return;
    }
    ESP_LOGI(TAG, "... set socket receiving timeout success");

            bzero(recv_buf, sizeof(recv_buf));
            r = read(s, recv_buf, sizeof(recv_buf) - 1);
#ifdef DEBUG_PRINT 
            printf("\nPRINTING HTTP\n");
            printf("%s\n", recv_buf);
            printf("\n\nDONE PRINTING HTTP\n");
#endif
            char * http_json = http_extract_json_string(&(recv_buf[0]));
            const char * jsonstring = http_json; 
            cJSON * jsonstruct = cJSON_Parse(jsonstring);

            cJSON * json_main = cJSON_GetObjectItem(jsonstruct, "main");
            double temp = cJSON_GetObjectItem(json_main, "temp")->valuedouble;
            double temp_feel = cJSON_GetObjectItem(json_main, "feels_like")->valuedouble;
            double temp_low = cJSON_GetObjectItem(json_main, "temp_min")->valuedouble;
            double temp_high = cJSON_GetObjectItem(json_main, "temp_max")->valuedouble;

#ifdef DEBUG_PRINT            
            printf("\nTemp = %d\nFeels Like = %d\nLow = %d\nHigh = %d\n", (int)round(temp), (int)round(temp_feel), (int)round(temp_low), (int)round(temp_high));
#endif

            owm_data.temp_now = temp;
            owm_data.temp_low = temp_low;
            owm_data.temp_high = temp_high;
            owm_data.temp_feel = temp_feel;

    ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d.", r, errno);
    close(s);
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

struct owm_data_t get_owm_data() {return owm_data;}
