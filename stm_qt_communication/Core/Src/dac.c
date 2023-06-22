/*
 * dac.c
 *
 *  Created on: Jun 13, 2023
 *      Author: malin
 */
#include "dac.h"
#include "spi.h"
#include "stm32l0xx_hal.h"

void DAC_SetVoltage( uint16_t voltage)
{
    /* Convert voltage to DAC value (12-bit resolution) */
    //uint16_t dacValue = voltage* 4095 / 3.3;  // Assuming 3.3V reference

    /* Send command over SPI */
//    SPI_SendData(dacValue);
    SPI_SendData(voltage);


}

