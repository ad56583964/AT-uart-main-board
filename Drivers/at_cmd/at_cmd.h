/*
 * at_commands.h
 *
 *  Created on: 2023年3月10日
 *      Author: ad565
 */

#ifndef AT_CMD_AT_CMD_H_
#define AT_CMD_AT_CMD_H_

#include "cmsis_os.h"
#include "string.h"

#include "drv_uart/drv_uart.h"


#include "main.h"

#include "tests.h"

typedef enum
{
	AT_OK,
	AT_ERROR
} AT_Status_t;

typedef enum
{
	SHELL_OK,
	SHELL_ERROR
} SHELL_StatusTypeDef;

enum {
	AT_TEST = 0U,
	AT_DEVICE = 1U
};

typedef union {
	struct __attribute__((packed)){
		uint16_t header;
		uint8_t connector;
		uint32_t mesh_str;
		uint8_t delimiter;
		uint16_t addr;
		uint32_t data;
		uint16_t tail;
	};
	uint8_t raw[16];
} AT_Request_pack_t;

typedef union {
	struct {
		uint16_t header_H;
		uint8_t header_L;
		uint16_t source_addr;
		uint16_t target_addr;
		uint32_t data;
		uint16_t tail;
	}__attribute__((packed));
	uint8_t raw[13];
} AT_Receive_pack_t;

extern AT_Receive_pack_t AT_receive_pack_template;
extern AT_Request_pack_t AT_request_pack;

#define LOG uart1_write
#define AT_Send( str , size ) _uart2_write( (uint8_t*)str , size )

AT_Status_t AT_Init();
AT_Status_t AT_check_reply();

typedef uint16_t addr_t;

typedef struct {
	addr_t address;
	uint8_t type;
} AT_Device_Handle_t;

typedef struct {
	AT_Device_Handle_t Device[20];
	uint8_t Size;
} AT_Device_Table_t;


AT_Status_t AT_main_schedule();

int AT_Device_insert(uint16_t addr);

int AT_check_addr();
int init_receive();

int wait_receive();

int start_receive();

int decode_AT();

typedef enum{
	UNKNOWN,
	MAIN_DEVICE,
	EDGE_DEVICE
} AT_Device_Mode_t;

extern AT_Device_Mode_t AT_device_mode;
extern osSemaphoreId_t at_receiveHandle;
extern const osSemaphoreAttr_t ATReceive_attributes;

#endif /* AT_CMD_AT_CMD_H_ */
