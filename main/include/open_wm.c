#include "open_wm.h"

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
