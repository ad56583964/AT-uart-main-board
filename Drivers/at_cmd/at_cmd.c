#include "at_cmd.h"
#include "drv_uart/drv_uart.h"

typedef enum _AT_Request_Set_Eunm_t{
	AT_ADDR = 9,
	AT_DATA = 11
} AT_Request_Set_Eunm_t;


AT_Request_Pack_t AT_request_pack = {
		.header = {0x41,0x54},//0x4154, // "AT",
		.connector = '+',
		.mesh_str = {0x4D,0x45,0x53,0x48},//0x4853454D, //"HSEM" <- "MESH"
		.delimiter = 0x00, // 00
		.addr = {0x00,0x01},
		.type = {0},
		.data = {0,0},
		.tail = {0x0d,0x0a},//0x0d0a
};

AT_Status_t AT_request_send_pack (AT_Request_Set_t* pack){
	AT_request_pack.addr[0] = pack->addr >> 8;
	AT_request_pack.addr[1] = pack->addr&0xff;
	AT_request_pack.type[0] = pack->type;
	AT_request_pack.data[0] = pack->data >> 8;
	AT_request_pack.data[1] = pack->data&0xff;

	AT_Send((uint8_t*)&AT_request_pack, 15);
	return AT_OK;
}

AT_Status_t _check_header_tail(AT_Receive_Read_t* pack){
	if(pack->header != 0xF1DD || pack->tail != 0x0A0D){
		return AT_ERROR;
	}
	return AT_OK;
}


AT_Status_t AT_receive_read_pack (AT_Receive_Read_t* pack){

	AT_Receive_Pack_t* raw_pack = (union AT_Receive_pack_t *)&rxbuf;
	pack->header = (raw_pack->header[0] << 8) + raw_pack->header[1];
	pack->tail = (raw_pack->tail[0] << 8) + raw_pack->tail[1];

	_check_header_tail(pack);

	pack->data = (raw_pack->data[0] << 8) + raw_pack->data[1];
	pack->source_addr = (raw_pack->source_addr[0] << 8) + raw_pack->source_addr[1];

	return AT_OK;
}

AT_Status_t AT_confirm_return(uint16_t addr){

	AT_Request_Set_t pack;
	pack.addr = addr;
	pack.type = MAIN_ACK;
	pack.data = 0x00;

	AT_request_send_pack(&pack);
	return AT_OK;
}



AT_Device_Mode_t AT_device_mode = UNKNOWN;

AT_Status_t set_mode(AT_Device_Mode_t mode){
	AT_device_mode = mode;
	return AT_OK;
};

AT_Status_t AT_check_reply(int reply_type){

	int result = 0;
	if(reply_type == AT_TEST){
		result = strncmp(rxbuf,"OK\r\n",4);
		if(result == 0){
			LOG("init_sucess\n");
		}
		else{
			LOG("init is not ok\n");
		}
	}
	return AT_OK;
}

int init_receive(){
	if(osSemaphoreGetCount(at_receiveHandle) > 0){
		osSemaphoreAcquire(at_receiveHandle, osWaitForever);
	}
	return AT_OK;
}

int wait_receive(){
	osSemaphoreAcquire(at_receiveHandle, osWaitForever);
	return AT_OK;
}

int start_receive(){
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)&rxbuf,RX_BUF_SIZE);
	return AT_OK;
}

int decode_AT(){


	return AT_OK;
}

AT_Device_Table_t AT_device_table = {
	.Size = 0
};

int AT_Device_insert(uint16_t addr,uint8_t type){
	int Size = AT_device_table.Size;
	for(int i = Size; i > 0;i --){
		if(AT_device_table.Device[i-1].address == addr){
			LOG("add_Failed");
			return ALREADY_EXIST;
		}
	}
	Size++;
	AT_device_table.Device[Size - 1].address = addr;
	AT_device_table.Device[Size - 1].type = type;
	AT_device_table.Size = Size;
	return ADD_SUCCESS;
}

int init_device_table(){


	return AT_OK;
}

AT_Status_t AT_main_schedule(){

	init_device_table();
	while(1){
		start_receive();
		wait_receive();
		LOG(rxbuf);
		osDelay(1);
	}
	return AT_OK;
}

AT_Status_t AT_first_request(){
	start_receive();
	AT_Send("AT\r\n",4);
	int result = 0;
	wait_receive();
	result = strncmp(rxbuf,"OK\r\n",4);
	if(result != 0){
		LOG("retry\n");
		return AT_ERROR;
	}
	else {
		return AT_OK;
	}
}

int AT_check_addr(){
	start_receive();
	AT_Send("AT+MADDR\r\n",10);
	wait_receive();

	int result = 0;
	result = strncmp("+MADDR=0001\r\n",rxbuf,13);
	if(result == 0){
		LOG("IS MainMachine\n");
		set_mode(MAIN_DEVICE);
	}
	else{
		LOG("IS Edge Board\n");
		set_mode(EDGE_DEVICE);
	}
	return AT_OK;
}

AT_Status_t AT_Init(){
	init_receive();

	for(int i = 5; i > 0; i--){
		if(AT_first_request() == AT_OK){
			LOG("init_sucess\n");
			return AT_OK;
		}
		osDelay(200);
	}

	LOG("Please Reset\n");
	osDelay(500);

	return AT_ERROR;
}
