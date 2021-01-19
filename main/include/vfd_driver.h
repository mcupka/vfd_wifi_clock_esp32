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

#define VFD_ALIGN_LEFT 1
#define VFD_ALIGN_RIGHT 0

#define VFD_SSEG_LARGE 1
#define VFD_SSEG_SMALL 0


//SPI device used to send VFD data
spi_device_handle_t vfd_dev;

//Global variable to store data displayed on VFD
uint vfd_grid_data[8];

//Global array of sseg representations of ascii values
uint vfd_asciitable[128];

//Configure SPI device using specified pins.
void vfd_config();
void vfd_init_asciitable();
//Send one 32-bit value to VFD controller
void vfd_send(spi_device_handle_t *vfd_dev, uint data);
//cycle through grids, sending current data for each one
void vfd_onecycle();
//OR bits of a mask into a grid's data
void vfd_set_grid_bits(int grid, uint mask);
//RESET bits of a mask into a grid's data
void vfd_reset_grid_bits(int grid, uint mask);
//enable / disable a whole grid by turning on / off the grid bit
void vfd_enable_grid(int grid);
void vfd_disable_grid(int grid);

void vfd_showint(uint n, uint display_sel, uint alignleft);

#endif
