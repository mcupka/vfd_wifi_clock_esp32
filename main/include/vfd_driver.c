#include "vfd_driver.h"

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


