/*
 * Flash_Write.c
 *
 *  Created on: Oct 20, 2014
 *      Author: Rahul
 */

#include "common_headers.h"
#include "sp2_string.h"
MQX_FILE_PTR   flash_file;
extern uint8_t Write_Strings(void);
extern void Write_Flash_DFU();
extern void Check_Serial();
char Serial_Numbr[15];
void Write_Flash(uint8_t buffer[32],uint8_t len);
extern uint8_t Write_All_Calib_Dat();
extern uint8_t Read_All_Calib_Dat();
extern uint8_t Write_Acc_Calib_Dat();

uint8_t Write_Strings(void)
{
	uint_32 ioctl_param;
	uint8_t err_ctr=0;
	while(1)
	{
		/* Open the flash device */
		flash_file = fopen(FLASH_NAME, NULL);
		if (flash_file == NULL) 
		{
			printf("\nUnable to open file %s", FLASH_NAME);
			return 0;
		}
		else 
		{
			printf("\nFlash file %s opened", FLASH_NAME);
		}
	
		/* Get the size of the flash file */
		fseek(flash_file, STRING_SECTOR, IO_SEEK_SET);
	
		/* Disable sector cache */
		ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
		printf("\nFlash sector cache enabled.");
	
		/* Unprotecting the the FLASH might be required */
		ioctl_param = 0;
		ioctl(flash_file, FLASH_IOCTL_WRITE_PROTECT, &ioctl_param);
	
		_ICACHE_DISABLE();
		ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
		_ICACHE_DISABLE();
		//get size of the array to be written
		int string_struct_size = getStringStructSize();
		printf("string struct size = %d", string_struct_size);
		
		if (string_struct_size != write(flash_file, string_array, string_struct_size)) 
		{
			printf("\nError writing to the file. Error code: %d", _io_ferror(flash_file));
			err_ctr++;
			if(err_ctr>=5)
			{
				printf("Flash Memory Corrupted....\n");
				_ICACHE_DISABLE();
				ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
				_ICACHE_DISABLE();
				_ICACHE_DISABLE();
				ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
				_ICACHE_DISABLE();
				_ICACHE_DISABLE();
				ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
				_ICACHE_DISABLE();
				fclose(flash_file);
				return 0;
			}
		}
		else 
		{
			printf("\nData written to the flash.\n");
			fclose(flash_file);
			printf("\nFlash Write function End.");
			return 1;
		}
		fclose(flash_file);
	}
}
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
	uint8_t err_ctr=0;
	uint8_t buffer[1];
	while(1)
	{
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
		fseek(flash_file, DFU_SECTOR, IO_SEEK_SET);

		/* Disable sector cache */
		ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
		printf("\nFlash sector cache enabled.");

		/* Unprotecting the the FLASH might be required */
		ioctl_param = 0;
		ioctl(flash_file, FLASH_IOCTL_WRITE_PROTECT, &ioctl_param);

		_ICACHE_DISABLE();
		ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
		_ICACHE_DISABLE();

		/* Get the size of the flash file */
		fseek(flash_file, DFU_ADDR_LOC, IO_SEEK_SET);
		
		buffer[0]=1;
		if (1 != write(flash_file, buffer, 1)) 
		{
			printf("\nError writing to the file. Error code: %d", _io_ferror(flash_file));
			err_ctr++;
			if(err_ctr>=5)
			{
				printf("Flash Memory Corrupted....\n");
				_ICACHE_DISABLE();
				ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
				_ICACHE_DISABLE();
				_ICACHE_DISABLE();
				ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
				_ICACHE_DISABLE();
				_ICACHE_DISABLE();
				ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
				_ICACHE_DISABLE();
				fclose(flash_file);
				return ;
			}
		}
		else 
		{
			printf("\nData written to the flash.\n");
			fclose(flash_file);
			printf("\nFlash Write function End.");
			return;
		}
		fclose(flash_file);
	}
}

void Check_Serial()
{
	char buffer[32];
	uint8_t err_ctr=0;
	MQX_FILE_PTR   flash_file,ser_fd_ptr;
	
	strcpy(Serial_Numbr,"NO NUMBER");
	/* Open the flash device */
	flash_file = fopen(FLASH_NAME, NULL);
	if (flash_file == NULL) 
	{
		printf("\nUnable to open file %s", FLASH_NAME);
		return;
	}

	/* Get the size of the flash file */
	fseek(flash_file, SERIAL_ADDR_LOC, IO_SEEK_SET);

	/* Disable sector cache */
	ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);

	if (15 != read(flash_file, buffer, 15)) 
	{
		printf("\nERROR! Could not read from flash. Exiting...");
		fclose(flash_file);
		return;
	}
	fclose(flash_file);
	
	if((buffer[0]=='S')&&(buffer[1]=='P')&&(buffer[2]=='2'))
	{
		buffer[14] = 0x00;
		strcpy(Serial_Numbr,buffer);
		printf("Current serial number is %s\n",Serial_Numbr);
	}
	else
	{
		ser_fd_ptr = fopen("a:serial.txt", "r");
		if (ser_fd_ptr == NULL)
		{
			printf("No image file found\n");
		}
		else
		{
			memset(Serial_Numbr,0x00,15);
			fread(Serial_Numbr,14,1,ser_fd_ptr);
			printf("Serial number from file is %s\n",Serial_Numbr);
			fclose(ser_fd_ptr);	
			while(1)
			{
				/* Open the flash device */
				flash_file = fopen(FLASH_NAME, NULL);
				if (flash_file == NULL) 
				{
					printf("\nUnable to open file %s", FLASH_NAME);
//					fclose(ser_fd_ptr);	
					return;
				}

				/* Get the size of the flash file */
				fseek(flash_file, SERIAL_ADDR_LOC, IO_SEEK_SET);

				/* Disable sector cache */
				ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);

				_ICACHE_DISABLE();
				ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
				_ICACHE_DISABLE();

				if (15 != write(flash_file, Serial_Numbr, 15)) 
				{
					err_ctr++;
					if(err_ctr>=5)
					{
						strcpy(Serial_Numbr,"NO NUMBER");
						printf("Flash Memory Corrupted....\n");
						_ICACHE_DISABLE();
						ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
						_ICACHE_DISABLE();
						_ICACHE_DISABLE();
						ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
						_ICACHE_DISABLE();
						_ICACHE_DISABLE();
						ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
						_ICACHE_DISABLE();
						fclose(flash_file);
						return ;
					}
				}
				else
				{
					fclose(flash_file);
					return;
				}
				fclose(flash_file);
			}
		}
	}
}

void Write_Calib(char * buff,uint32_t Addr ,uint8_t len)
{
#if 0
	Write_Flash((uint8_t *)buff,len);
#else
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
	fseek(flash_file, Addr, IO_SEEK_SET);

	/* Disable sector cache */
	ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
	printf("\nFlash sector cache enabled.");

	/* Unprotecting the the FLASH might be required */
	ioctl_param = 0;
	ioctl(flash_file, FLASH_IOCTL_WRITE_PROTECT, &ioctl_param);

	if (len != write(flash_file, buff, len)) 
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
#endif
}

void Read_Calib(char * buff,uint32_t Addr ,uint8_t len)
{
#if 0
	Write_Flash((uint8_t *)buff,len);
#else
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
	fseek(flash_file, Addr, IO_SEEK_SET);

	/* Disable sector cache */
	ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
	printf("\nFlash sector cache enabled.");

	if (len != read(flash_file, buff, len)) 
	{
		printf("\nERROR! Could not read from flash. Exiting...");
		fclose(flash_file);
		return;
	}
	
	fclose(flash_file);
	function_end:
	printf("\nFlash Write function End.");
#endif
}

uint8_t Write_All_Calib_Dat()
{
	uint_32        ioctl_param;
	uint8_t err_ctr=0;
	while(1)
	{
		/* Open the flash device */
		flash_file = fopen(FLASH_NAME, NULL);
		if (flash_file == NULL) 
		{
			printf("\nUnable to open file %s", FLASH_NAME);
			return 0;
		}
		else 
		{
			printf("\nFlash file %s opened", FLASH_NAME);
		}
	
		/* Get the size of the flash file */
		fseek(flash_file, CALIB_SECTOR, IO_SEEK_SET);
	
		/* Disable sector cache */
		ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
		printf("\nFlash sector cache enabled.");
	
		/* Unprotecting the the FLASH might be required */
		ioctl_param = 0;
		ioctl(flash_file, FLASH_IOCTL_WRITE_PROTECT, &ioctl_param);
	
		_ICACHE_DISABLE();
		ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
		_ICACHE_DISABLE();
		
		if (sizeof(Calib_Flash_Buf) != write(flash_file, Calib_Flash_Buf, sizeof(Calib_Flash_Buf))) 
		{
			printf("\nError writing to the file. Error code: %d", _io_ferror(flash_file));
			err_ctr++;
			if(err_ctr>=5)
			{
				printf("Flash Memory Corrupted....\n");
				_ICACHE_DISABLE();
				ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
				_ICACHE_DISABLE();
				_ICACHE_DISABLE();
				ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
				_ICACHE_DISABLE();
				_ICACHE_DISABLE();
				ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
				_ICACHE_DISABLE();
				fclose(flash_file);
				return 0;
			}
		}
		else 
		{
			printf("\nData written to the flash.\n");
			fclose(flash_file);
			printf("\nFlash Write function End.");
			return 1;
		}
		fclose(flash_file);
	}
}


uint8_t Read_Acc_Calib_Dat()
{
	//uint_32        ioctl_param;
	/* Open the flash device */
	flash_file = fopen(FLASH_NAME, NULL);
	if (flash_file == NULL) 
	{
		printf("\nUnable to open file %s", FLASH_NAME);
		return 1;
	}
	else 
	{
		printf("\nFlash file %s opened", FLASH_NAME);
	}

	/* Get the size of the flash file */
	fseek(flash_file, ACC_CALIB_SECTOR, IO_SEEK_SET);

	/* Disable sector cache */
	ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
	printf("\nFlash sector cache enabled.");

	if (96 != read(flash_file, ACC_Data.ACC_char, 96)) 
	{
		printf("\nERROR! Could not read from flash. Exiting...");
	}
	fclose(flash_file);
	
	
	printf("\nFlash Write function End.");
	
	printf("\nDouble Values are  \n%0.15f \n%0.15f \n%0.15f\n", ACC_Data.data.ACC00, ACC_Data.data.ACC01, ACC_Data.data.ACC02);
	printf("\nDouble Values are  \n%0.15f \n%0.15f \n%0.15f\n", ACC_Data.data.ACC10, ACC_Data.data.ACC11, ACC_Data.data.ACC12);
	printf("\nDouble Values are  \n%0.15f \n%0.15f \n%0.15f\n", ACC_Data.data.ACC20, ACC_Data.data.ACC21, ACC_Data.data.ACC22);
	printf("\nDouble Values are  \n%0.15f \n%0.15f \n%0.15f\n", ACC_Data.data.ACC30, ACC_Data.data.ACC31, ACC_Data.data.ACC32);
	
	return 0;
}


uint8_t Write_Acc_Calib_Dat()
{
	uint_32        ioctl_param;
	uint8_t err_ctr=0;

	
	while(1)
	{
		/* Open the flash device */
		flash_file = fopen(FLASH_NAME, NULL);
		if (flash_file == NULL)
		{
			printf("\nUnable to open file %s", FLASH_NAME);
			return 0;
		}
		else 
		{
			printf("\nFlash file %s opened", FLASH_NAME);
		}
	
		/* Get the size of the flash file */
		fseek(flash_file, ACC_CALIB_SECTOR, IO_SEEK_SET);
	
		/* Disable sector cache */
		ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
		printf("\nFlash sector cache enabled.");
	
		/* Unprotecting the the FLASH might be required */
		ioctl_param = 0;
		ioctl(flash_file, FLASH_IOCTL_WRITE_PROTECT, &ioctl_param);
	
		_ICACHE_DISABLE();
		ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
		_ICACHE_DISABLE();
		
		if (96 != write(flash_file, ACC_Data.ACC_char, 96)) 
		{
			printf("\nError writing to the file. Error code: %d", _io_ferror(flash_file));
			err_ctr++;
			if(err_ctr>=5)
			{
				printf("Flash Memory Corrupted....\n");
				_ICACHE_DISABLE();
				ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
				_ICACHE_DISABLE();
				_ICACHE_DISABLE();
				ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
				_ICACHE_DISABLE();
				_ICACHE_DISABLE();
				ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
				_ICACHE_DISABLE();
				fclose(flash_file);
				return 0;
			}
		}
		else 
		{
			printf("\nData written to the flash.\n");
			fclose(flash_file);
			printf("\nFlash Write function End.");
			return 1;
		}
		fclose(flash_file);
	}
}

uint8_t Read_All_Calib_Dat()
{
	//uint_32        ioctl_param;
	/* Open the flash device */
	flash_file = fopen(FLASH_NAME, NULL);
	if (flash_file == NULL) 
	{
		printf("\nUnable to open file %s", FLASH_NAME);
		return 0;
	}
	else 
	{
		printf("\nFlash file %s opened", FLASH_NAME);
	}

	/* Get the size of the flash file */
	fseek(flash_file, CALIB_SECTOR, IO_SEEK_SET);

	/* Disable sector cache */
	ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
	printf("\nFlash sector cache enabled.");

	if (sizeof(Calib_Flash_Buf) != read(flash_file, Calib_Flash_Buf, sizeof(Calib_Flash_Buf))) 
	{
		printf("\nERROR! Could not read from flash. Exiting...");
	}
	fclose(flash_file);
	printf("\nFlash Write function End.");
	return 1;
}
