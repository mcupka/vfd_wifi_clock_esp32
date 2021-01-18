#ifndef VFD_DRIVER_H
#define VFD_DRIVER_H

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "vfd_chars.h"


#define VFD_HOST VSPI_HOST
#define DMA_CHAN     2
#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define PIN_LE       32
#define PIN_STROBE   14

//SPI device used to send VFD data
spi_device_handle_t vfd_dev;

//Global variable to store data displayed on VFD
uint vfd_grid_data[8];

//Configure SPI device using specified pins.
void vfd_config();
//Send one 32-bit value to VFD controller
void vfd_send(spi_device_handle_t *vfd_dev, uint data);
//cycle through grids, sending current data for each one
void vfd_onecycle();
//OR bits of a mask into a grid's data
void vfd_set_grid_bits(int grid, uint mask);
//RESET bits of a mask into a grid's data
void vfd_reset_grid_bits(int grid, uint mask);

#endif
