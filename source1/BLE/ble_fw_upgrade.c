//#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "os.h"
#include "api.h"
#include "adv.h"
#include "Bulk_transfer.h"
#include "ble_fw_upgrade.h"

static UINT16 otaWriteBufferHandle = 0x0000;

void _usage(void)
{
    printf ("\n");
    printf ("usage: fw_upgrade HEX_IMAGE\n");
    printf ("HEX_IMAGE: FW image in HEX format (e.g. v1_0_3.hex)\n");
    printf ("\n");
}

#define IDEVICES_UUID128(x) {0xf7,0xfd,0x2f,0xc5,0xd3,0x94,0x37,0x9f,0x58,0x4b,0x4b,0x4a,((UINT8)(x & 0x00ff)),((UINT8)(x >> 8)),0xac,0x64};
#define UUID128(x) {0x45, 0xdd, 0x67, 0x92, 0xbf, 0x42, 0x2e, 0xa4, 0x77, 0x4e, 0x36, 0xe7, ((UINT8)(x & 0x00ff)), ((UINT8)(x >> 8)), 0xe7, 0xd7

static int _loadOTAHandle(void)
{
    UINT16 handle;
    UINT8 ota_write_buffer_char_uuid[16] = IDEVICES_UUID128(0x0005);
    int status;
    status = iDevices_GattGetHandleFromUUID(ota_write_buffer_char_uuid, 16, &handle);
    if (handle) {
        otaWriteBufferHandle = handle;
        printf("looks like we got handle");
        return 0;
    } else {
        return -1;
    }
    
}

static int _loadHexFile(char *filename)
{
    MQX_FILE_PTR fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("failed to open file %s\n", filename);
        return -1;
    }

    UINT8 buf[16];
    int numBytes = 0;
    UINT8 line[64];
	int len = 0;
	int offset = 0;
	int cmd = 0;
	char data[2*(16+1)] = {0};
	int fcs = 0;
//    fseek(fp, 0L, IO_SEEK_END);
//    int filesize = ftell(fp);
//    fseek(fp, 0L, IO_SEEK_SET);
    //UINT8 *dataBuf = (UINT8 *)MALLOC(filesize);
    //UINT8 *dataBuf = (UINT8 *)PROCESSING_INTERMEDIATE_DATA_LOCATION;
    //memset(dataBuf, 0x00, filesize);

    int bytesRead = 0;
//    while (bytesRead < filesize) {
//        int status = fread(data, 1, 1024, fp);
//        if (status > 0) {
//
//            memcpy(&dataBuf[bytesRead], data, status);
//        	printf("data = ");
//        	for(int i = 0; i < status; i++)
//        	{
//        		printf("%c ", dataBuf[bytesRead + i]);
//        	}
//            bytesRead += status;
//            printf("bytesRead = %d\n", bytesRead);
//        } else {
//            printf("failed to read file %s w/ status = %d\n", filename, status);
//            break;
//        }
//    }
    
    while(fgets(line, sizeof(line), fp))
    {
    	fseek(fp, 1, IO_SEEK_CUR);
		//printf("line = %s\n", line);
    	sscanf(line, ":%02x%04x%02x%s\n", &len, &offset, &cmd, data);
		printf("len = %d, offset = 0x%04x, cmd = %d\n", len, offset, cmd);
		int i;
		char *bytesStr = &data[0];
		numBytes = len;
		for (i = 0; i < numBytes; i++) {
			char tmp[3] = {0};
			memcpy((char *)tmp, bytesStr, 2);
			buf[i] = (UINT8)strtol(tmp, NULL, 16);
			bytesStr+=2;
		}
		char tmp[3] = {0};
		memcpy((char *)tmp, bytesStr, 2);
		fcs = (UINT8)strtol(tmp, NULL, 16);
#if 0
		printf("data = ");
		for (i = 0; i < numBytes; i++) {
			printf("%02x ", buf[i]);
		}
		printf(", fcs = %02x\n", fcs);
#endif
		UINT8 otaData[20] = {0};
		UINT8 *otaData_p = otaData;

		// otaData[0]
		*(otaData_p++) = (UINT8)len;

		// otaData[1]
		*((UINT16 *)otaData_p) = LE_SWAP_16(offset);
		otaData_p+=sizeof(UINT16);

		// otaData[3]
		*(otaData_p++) = (UINT8)cmd;

		// otaData[4]
		memcpy(otaData_p, buf, len);
#if 0
		printf("otaData = ");
		for (i = 0; i < len+4; i++) {
			printf("%02x ", otaData[i]);
		}
		printf("\n");
#endif
	int status = iDevices_GattWriteHandle(otaWriteBufferHandle, otaData, len+4);
	if(status != len+8)
	{
		printf("OTA write failed!\n");
		break;
	}
    }
    fclose(fp);



//    char *token;
//    token = strtok(dataBuf, "\n");
//    while (token != NULL) {
//        printf("%s\n", token);

//        sscanf(token, ":%02x%04x%02x%s", &len, &offset, &cmd, data);
//        printf("len = %d, offset = 0x%04x, cmd = %d\n", len, offset, cmd);
//
//        int i;
//        char *bytesStr = &data[0];
//        numBytes = len;
//        for (i = 0; i < numBytes; i++) {
//            char tmp[3] = {0};
//            memcpy((char *)tmp, bytesStr, 2);
//            buf[i] = (UINT8)strtol(tmp, NULL, 16);
//            bytesStr+=2;
//        }
//        char tmp[3] = {0};
//        memcpy((char *)tmp, bytesStr, 2);
//        fcs = (UINT8)strtol(tmp, NULL, 16);
//#if 1
//        printf("data = ");
//        for (i = 0; i < numBytes; i++) {
//            printf("%02x ", buf[i]);
//        }
//        printf(", fcs = %02x\n", fcs);
//#endif
//        UINT8 otaData[20] = {0};
//        UINT8 *otaData_p = otaData;
//
//        // otaData[0]
//        *(otaData_p++) = (UINT8)len;
//
//        // otaData[1]
//        *((UINT16 *)otaData_p) = LE_SWAP_16(offset);
//        otaData_p+=sizeof(UINT16);
//
//        // otaData[3]
//        *(otaData_p++) = (UINT8)cmd;
//
//        // otaData[4]
//        memcpy(otaData_p, buf, len);
//#if 0
//        printf("otaData = ");
//        for (i = 0; i < len+4; i++) {
//            printf("%02x ", otaData[i]);
//        }
//        printf("\n");
//#endif
//        printf("attempting to write data to BLE module OTA");
//        int status = iDevices_GattWriteHandle(otaWriteBufferHandle, otaData, len+4);
//        token = strtok(NULL, "\n");
//    }
//
//    //free(dataBuf);
//    fclose(fp);
}

int update_ble_firmware()
{
	printf("Updating BLE firmware");
    int status;
    char *filename = "a:20732I_1.1.3.hex";

    
    /* Initialise the BLE and uart baud rate */
	Ble_set_baudrate();
		
	if(Settings_Data.BLE_power_status == BLUETOOTH_ON)
	{
		if(Ble_Module_init()){
			printf("\nBLE Init failed.. Please try again\n"); 
		}
		else{
			printf("\nBLE init Success.... \n");
		}		
		
		BLE_host_wakeup_flag = NO;
		lwgpio_int_enable(&BLE_host_interrupt, TRUE );
	}
    
    
    status = _loadOTAHandle();
    if(status != 0)
    {
    	printf("problem getting Handle from UUID. error code = %d \n", status);
    }
    else
    {
    	printf("loadOTAHAndle success?\n");
    }
    printf("loading hex file");
    status = _loadHexFile(filename);

    return status;
}
