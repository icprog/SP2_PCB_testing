/*
 * Flash_Write.c
 *
 *  Created on: Oct 20, 2014
 *      Author: Rahul
 */

#include "common_headers.h"
MQX_FILE_PTR   flash_file;
#if MQX_ROM_VECTORS
#define FLASH_NAME 			"flashx:bank3"
#else
#define FLASH_NAME 			"flashx:bank0"
#endif
#define SERIAL_ADDR_LOC 	0x24B60
#define DFU_ADDR_LOC		0x24B70
#define TEST_ID_ADDR_LOC	0x24B71
extern void Write_Flash_DFU();

void Write_Flash(uint8_t buffer[32],uint8_t len)
{
	uint_32        ioctl_param;

	/* Open the flash device */
	flash_file = fopen(FLASH_NAME, NULL);
	if (flash_file == NULL) 
	{
		printf("\nUnable to open file %s", FLASH_NAME);
		goto function_end;
	}
	else 
	{
		printf("\nFlash file %s opened", FLASH_NAME);
	}

	/* Get the size of the flash file */
	fseek(flash_file, TEST_ID_ADDR_LOC, IO_SEEK_SET);

	/* Disable sector cache */
	ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
	printf("\nFlash sector cache enabled.");

	/* Unprotecting the the FLASH might be required */
	ioctl_param = 0;
	ioctl(flash_file, FLASH_IOCTL_WRITE_PROTECT, &ioctl_param);

	if (len != write(flash_file, buffer, len)) 
	{
		printf("\nError writing to the file. Error code: %d", _io_ferror(flash_file));
	}
	else 
	{
		printf("\nData written to the flash.\n");
	}
	fclose(flash_file);
	function_end:
	printf("\nFlash Write function End.");
}


void Read_Flash(uint8_t buffer[32],uint8_t len)
{
	MQX_FILE_PTR   flash_file;

	/* Open the flash device */
	flash_file = fopen(FLASH_NAME, NULL);
	if (flash_file == NULL) 
	{
		printf("\nUnable to open file %s", FLASH_NAME);
		return;
	}
	else 
	{
		printf("\nFlash file %s opened", FLASH_NAME);
	}

	/* Get the size of the flash file */
	fseek(flash_file, TEST_ID_ADDR_LOC, IO_SEEK_SET);

	/* Disable sector cache */
	ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
	printf("\nFlash sector cache enabled.");

	if (len != read(flash_file, buffer, len)) 
	{
		printf("\nERROR! Could not read from flash. Exiting...");
		fclose(flash_file);
		printf("\nFlash Read function End.");
		return;
	}
	else
	{
		printf("\nReading last 32 Bytes Success");
	}

	fclose(flash_file);
	printf("\nFlash Read function End.");
}

void Write_Flash_DFU()
{
	uint_32        ioctl_param;
	uint8_t buffer[1];

	/* Open the flash device */
	flash_file = fopen(FLASH_NAME, NULL);
	if (flash_file == NULL) 
	{
		printf("\nUnable to open file %s", FLASH_NAME);
		goto function_end;
	}
	else 
	{
		printf("\nFlash file %s opened", FLASH_NAME);
	}

	/* Get the size of the flash file */
	fseek(flash_file, DFU_ADDR_LOC, IO_SEEK_SET);

	/* Disable sector cache */
	ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
	printf("\nFlash sector cache enabled.");

	/* Unprotecting the the FLASH might be required */
	ioctl_param = 0;
	ioctl(flash_file, FLASH_IOCTL_WRITE_PROTECT, &ioctl_param);
	buffer[0]=1;
	if (1 != write(flash_file, buffer, 1)) 
	{
		printf("\nError writing to the file. Error code: %d", _io_ferror(flash_file));
	}
	else 
	{
		printf("\nData written to the flash.\n");
	}
	fclose(flash_file);
	function_end:
	printf("\nFlash Write function End.");
}
