#include "vfd_driver.h"
#include "string.h"

extern uint vfd_grid_data[8];

//set up the SPI peripheral for VFD use using pins specified in vfd_controller.h
void vfd_config() {

    //initialize sseg asciitable
    vfd_init_asciitable();
 
    //initialize global grid data variables 
    for (int a = 0; a < 8; a++) {
        vfd_grid_data[a] = 0;
    } 
    
    //Set gpio pins up for le and strobe pins
    gpio_config_t io_conf;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = (1 << PIN_LE) | (1 << PIN_STROBE); //pins 4 and 5
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    //SET le low
    gpio_set_level(PIN_LE, 0);

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
}


//Send one 32-bit value to VFD controller over the given SPI device
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

void vfd_set_grid_bits(int grid, uint mask) {
    vfd_grid_data[grid] |= mask;
}

void vfd_reset_grid_bits(int grid, uint mask) {
    vfd_grid_data[grid] &= ~mask;
}

void vfd_enable_grid(int grid) {
    vfd_set_grid_bits(grid, (VFD_GRIDS_HIGH >> grid));
}

void vfd_disable_grid(int grid) {
    vfd_reset_grid_bits(grid, (VFD_GRIDS_HIGH >> grid));
}


//This function cycles through each grid, quickly sending the data so that it appears that all grids are on at once. Needs to be called in a loop if you want to be able to see it
void vfd_onecycle() {

        for (int a = 0; a < 8; a++) {
            vfd_send(&vfd_dev, vfd_grid_data[a]);
            vTaskDelay(1);
        }

}

//put an integer into the small / large displays on the VFD 
void vfd_showint(uint n, uint display_sel, uint alignleft, uint clear_display) {
    char buf[5] = {'\0', '\0', '\0', '\0', '\0'};
    snprintf(buf, 4, "%d", n);

    if (display_sel == VFD_SSEG_SMALL) {
        //Grids 0, 1, and 2
        if (clear_display == VFD_CLEAR_DISPLAY) {
            vfd_reset_grid_bits(0, VFD_AN_SSEG_ALL);
            vfd_reset_grid_bits(1, VFD_AN_SSEG_ALL);
            vfd_reset_grid_bits(2, VFD_AN_SSEG_ALL);
        }
        if (alignleft == VFD_ALIGN_LEFT) {
           for (int a = 0; (a < 3) && (buf[a] != '\0'); a++) {
               vfd_enable_grid(a);
               vfd_reset_grid_bits(a, VFD_AN_SSEG_ALL);
               vfd_set_grid_bits(a, vfd_asciitable[(int)(buf[a])]);
           } 
        }
        else {
            int b = 0;
            for (int a = 3 - strlen(buf); (a >= 0) && (buf[b] != '\0'); a++) {
                vfd_reset_grid_bits(a, VFD_AN_SSEG_ALL);
                vfd_enable_grid(a);
                vfd_set_grid_bits(a, vfd_asciitable[(int)(buf[b])]);
                b++;
            }
        }
    }
    else {
        //Grids 3, 4, 5, and 6 
        if (clear_display == VFD_CLEAR_DISPLAY) {
            vfd_reset_grid_bits(3, VFD_AN_SSEG_ALL);
            vfd_reset_grid_bits(4, VFD_AN_SSEG_ALL);
            vfd_reset_grid_bits(5, VFD_AN_SSEG_ALL);
            vfd_reset_grid_bits(6, VFD_AN_SSEG_ALL);
        }
        if (alignleft == VFD_ALIGN_LEFT) {
           for (int a = 0; (a < 3) && (buf[a] != '\0'); a++) {
               vfd_enable_grid(a + 3);
               vfd_reset_grid_bits(a + 3, VFD_AN_SSEG_ALL);
               vfd_set_grid_bits(a + 3, vfd_asciitable[(int)(buf[a])]);
           } 
        }
        else {
            int b = 0;
            for (int a = 4 - strlen(buf); (a >= 0) && (buf[b] != '\0'); a++) {
                vfd_enable_grid(a + 3);
                vfd_reset_grid_bits(a + 3, VFD_AN_SSEG_ALL);
                vfd_set_grid_bits(a + 3, vfd_asciitable[(int)(buf[b])]);
                b++;
            }
        }  
    }    

}

