#include "at_cmd.h"
#include "drv_uart/drv_uart.h"
#include "string.h"
#include "cmsis_os.h"

#include "main.h"

#include "tests.h"

AT_Request_pack_t AT_request_pack = {
		.header = 0x4154, // "AT",
		.connector = '+',
		.mesh_str = 0x4853454D, //"MESH"
		.delimiter = 0x00, // 00
		.addr = 0x0001,
		.data = 0x00000000,
		.tail = 0x0d0a
};

AT_Receive_pack_t AT_receive_pack_template = {
		.header = 0x00F1DD09,
		.source_addr = 0x0000,
		.target_addr = 0x0000,
		.tail = 0x0d0a
};

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

	if(memcmp(rxbuf,AT_receive_pack_template.header,3) == 0){
		LOG("Received from edge\n");
		uint16_t current_addr = (rxbuf[3] << 8) + rxbuf[4];
		LOG(current_addr);
		AT_Device_insert(current_addr);

	}
	return AT_OK;
}

AT_Device_Table_t AT_device_table = {
	.Size = 0
};

enum{
	ALREADY_EXIST,
	ADD_SUCCESS
};

int AT_Device_insert(uint16_t addr){
	int Size = AT_device_table.Size;
	for(int i = Size; i > 0;i --){
		if(AT_device_table.Device[i-1].address == addr){
			LOG("add_Failed");
			return ALREADY_EXIST;
		}
	}
	Size++;
	AT_device_table.Device[Size - 1].address = addr;
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
		decode_AT();
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

	//test
	test();
	//test_end

	for(int i = 5; i > 0; i--){
		if(AT_first_request() == AT_OK){
			LOG("init_sucess\n");
			return AT_OK;
		}
		osDelay(200);
	}

	LOG("Please Reset");
	osDelay(500);

	return AT_ERROR;
}
