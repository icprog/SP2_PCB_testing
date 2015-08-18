/*
 * spi_config.c
 *
 *  Created on: Jan 27, 2014
 *      Author: GADGEON-03
 */


#include <mqx.h>
#include <bsp.h>
#include "spi_config.h"

const char_ptr device_mode[] =
{
		"SPI_DEVICE_MASTER_MODE",
		"SPI_DEVICE_SLAVE_MODE",
};

const char_ptr clock_mode[] =
{
		"SPI_CLK_POL_PHA_MODE0",
		"SPI_CLK_POL_PHA_MODE1",
		"SPI_CLK_POL_PHA_MODE2",
		"SPI_CLK_POL_PHA_MODE3"
};


uint_8 frame_buff[12000]={0xFF};

/*-----------------------------------------------------------------------------
 *************************		END		*******************************
-----------------------------------------------------------------------------*/


