#ifndef VFD_DRIVER_H
#define VFD_DRIVER_H

#include <stdio.h>
#include <stdlib.h>

#include "driver/spi_master.h"
#include "driver/gpio.h"



#define VFD_HOST VSPI_HOST
#define DMA_CHAN     2
#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define PIN_LE       32
#define PIN_STROBE   14



void vfd_send(spi_device_handle_t *vfd_dev, uint data);



#endif
