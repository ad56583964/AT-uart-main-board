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




typedef enum _AT_Status_t
{
	AT_OK,
	AT_ERROR
} AT_Status_t;

//typedef enum
//{
//	SHELL_OK,
//	SHELL_ERROR
//} SHELL_StatusTypeDef;

typedef enum _AT_Request_Type_t{
	REG_DEVICE,
	DOOR_DEVICE,
	SMOKE_DEVICE,
	IR_DEVICE,
	MAIN_ACK,
	EDGE_ACK
}AT_Request_Type_t;

typedef enum _EDGE_Type_t{
	EDGE_DOOR,
	EDGE_SMOKE,
	EDGE_IR,
}EDGE_Type_t;

enum{
	ALREADY_EXIST,
	ADD_SUCCESS
};

enum {
	AT_TEST = 0U,
	AT_DEVICE = 1U
};

typedef union _AT_Request_Pack_t{
	struct __attribute__((packed)){
		uint8_t header[2];
		uint8_t connector;
		uint8_t mesh_str[4];
		uint8_t delimiter;
		uint8_t addr[2];
		uint8_t type[1];
		uint8_t	data[2];
		uint8_t tail[2];
	};
	uint8_t raw[15];
} AT_Request_Pack_t;

typedef union AT_Receive_pack_t{
	struct {
		uint8_t header[2];
		uint8_t connecter;
		uint8_t source_addr[2];
		uint8_t target_addr[2];
		uint8_t type;
		uint8_t data[2];
		uint8_t tail[2];
	}__attribute__((packed));
	uint8_t raw[12];
} AT_Receive_Pack_t;

typedef struct _AT_Receive_Read_t{
		uint16_t header;
		uint16_t tail;
		uint16_t source_addr;
		uint16_t target_addr;
		uint16_t data;
		uint8_t type;
} AT_Receive_Read_t;

typedef uint16_t addr_t;

typedef struct {
	addr_t address;
	uint8_t type;
} AT_Device_Handle_t;

typedef struct {
	AT_Device_Handle_t Device[20];
	uint8_t Size;
} AT_Device_Table_t;

typedef struct _AT_Request_Set_t {
	uint16_t addr;
	uint8_t type;
	uint16_t data;
} AT_Request_Set_t;

AT_Status_t AT_main_schedule();

//extern AT_Receive_pack_t AT_receive_pack_template;
extern AT_Request_Pack_t AT_request_pack;

#define LOG uart1_write
#define AT_Send( str , size ) _uart2_write( (uint8_t*)str , size )

AT_Status_t AT_Init();
AT_Status_t AT_check_reply();
AT_Status_t AT_confirm_return(uint16_t addr);

int AT_Device_insert(uint16_t addr,uint8_t type);

int AT_check_addr();
int init_receive();

int wait_receive();
int open_receive();
int start_receive();
AT_Status_t clear_semaphore();
int decode_AT();

AT_Status_t AT_request (AT_Request_Set_t* pack,AT_Receive_Read_t* get_pack);
AT_Status_t AT_request_send_pack (AT_Request_Set_t* pack);
AT_Status_t AT_receive_read_pack (AT_Receive_Read_t* pack);

typedef enum{
	UNKNOWN,
	MAIN_DEVICE,
	EDGE_DEVICE
} AT_Device_Mode_t;

extern AT_Device_Mode_t AT_device_mode;
extern osSemaphoreId_t at_receiveHandle;
extern const osSemaphoreAttr_t ATReceive_attributes;

#endif /* AT_CMD_AT_CMD_H_ */
