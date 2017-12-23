/*
 * ADS1015.c
 * 
 * Copyright 2017  <pi@pizero2>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "ADS1015.h"





uint16_t readADC_SingleEnded(int fd, uint8_t channel) 
{
	
	if (channel > 3)
	{
		return 0;
	}

	int m_bitShift = 4;

	// Start with default values
	uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE | // Disable the comparator (default val)
	ADS1015_REG_CONFIG_CLAT_NONLAT | // Non-latching (default val)
	ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low (default val)
	ADS1015_REG_CONFIG_CMODE_TRAD | // Traditional comparator (default val)
	ADS1015_REG_CONFIG_DR_1600SPS | // 1600 samples per second (default)
	ADS1015_REG_CONFIG_MODE_SINGLE; // Single-shot mode (default)

	// Set PGA/voltage range
	config |= ADS1015_REG_CONFIG_PGA_6_144V; // +/- 6.144V range (limited to VDD +0.3V max!)

	// Set single-ended input channel
	switch (channel)
	{
		case (0):
		config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
		break;
		case (1):
		config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
		break;
		case (2):
		config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
		break;
		case (3):
		config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
		break;
	}

	// Set ‘start single-conversion’ bit
	config |= ADS1015_REG_CONFIG_OS_SINGLE;

	// Write config register to the ADC
	wiringPiI2CWriteReg16(fd, ADS1015_REG_POINTER_CONFIG, config);

	// Wait for the conversion to complete
	delay(ADS1015_CONVERSIONDELAY);

	// Read the conversion results
	// Shift 12-bit results right 4 bits for the ADS1015
	return wiringPiI2CReadReg16(fd, ADS1015_REG_POINTER_CONVERT) >> m_bitShift;
}

int main(int argc, char **argv)
{
	uint16_t adc0, adc1, adc2, adc3;
	int fd=0;

	fd=wiringPiI2CSetup(ADS1015_ADDRESS);

	if (fd>=0)
	{
		adc0 = readADC_SingleEnded(fd,0);
		adc1 = readADC_SingleEnded(fd,1);
		adc2 = readADC_SingleEnded(fd,2);
		adc3 = readADC_SingleEnded(fd,3);

		printf("AIN0: %d\n",adc0);
		printf("AIN1: %d\n",adc1);
		printf("AIN2: %d\n",adc2);
		printf("AIN3: %d\n",adc3);
	}
	else
	{
		printf("Failed to open I2C : %d\n",errno);
	}
	
	return 0;
}

