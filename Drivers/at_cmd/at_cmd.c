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

AT_Status_t _is_send_ok(){
	int result = 0;
	result = strncmp(rxbuf,"OK\r\n",4);
	if(result == 0){
		LOG("sendSucess\n");
	}
	else{
		LOG("sendFailed\n");
	}
	return AT_OK;
}

AT_Status_t AT_request_send_pack (AT_Request_Set_t* pack){
	AT_request_pack.addr[0] = pack->addr >> 8;
	AT_request_pack.addr[1] = pack->addr&0xff;
	AT_request_pack.type[0] = pack->type;
	AT_request_pack.data[0] = pack->data >> 8;
	AT_request_pack.data[1] = pack->data&0xff;

	AT_Send((uint8_t*)&AT_request_pack, 15);
	return AT_OK;
}

AT_Status_t AT_request (AT_Request_Set_t* pack,AT_Receive_Read_t* get_pack){

	AT_request_pack.addr[0] = pack->addr >> 8;
	AT_request_pack.addr[1] = pack->addr&0xff;
	AT_request_pack.type[0] = pack->type;
	AT_request_pack.data[0] = pack->data >> 8;
	AT_request_pack.data[1] = pack->data&0xff;

	char _state = -1;
	while(_state != osOK){
		clear_semaphore();
		start_receive();
		AT_Send((uint8_t*)&AT_request_pack, 15);
		_state = wait_receive(200);
		_is_send_ok();
		start_receive();
		_state = wait_receive(200);
	}
	AT_receive_read_pack(get_pack);
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
	pack->type = raw_pack->type;

	return AT_OK;
}

AT_Status_t AT_confirm_return(uint16_t addr){

//	AT_request(pack, get_pack);
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

AT_Status_t clear_semaphore(){
	if(osSemaphoreGetCount(at_receiveHandle) > 0){
		osSemaphoreAcquire(at_receiveHandle, osWaitForever);
	}
	return AT_OK;
}

int wait_receive(int timeout){
	return 	osSemaphoreAcquire(at_receiveHandle, timeout);
}

int start_receive(){
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)&rxbuf,RX_BUF_SIZE);
	return AT_OK;
}

//int open_receive(){
//	HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)&rxbuf,RX_BUF_SIZE);
//	return AT_OK;
//}

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
			LOG("ALREADY_EXIST\n");
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

AT_Status_t AT_process_polling(){
	for(int i = 0; i < AT_device_table.Size; i++){
		LOG("PollingOnce");
		osDelay(100);
	}
	return AT_OK;
}

AT_Status_t AT_process_reg_device(AT_Request_Set_t* request_pack,AT_Receive_Read_t* received_pack){
	uint16_t REG_addr = received_pack->source_addr;
	uint16_t REG_type = received_pack->data&0xFF;

	//MAIN IS ACK
	request_pack->addr = REG_addr;
	request_pack->type = MAIN_ACK;

	AT_request(request_pack,received_pack);
	if(received_pack->type == EDGE_ACK){
		//EDGE IS ACK

		AT_Device_insert(REG_addr, REG_type);
		LOG("REG_SUCCESS\n");
	}
	return AT_OK;
}

AT_Status_t AT_main_schedule(){

	init_device_table();
	while(1){
		start_receive();
		wait_receive(200);
		LOG(rxbuf);
		osDelay(1);
	}
	return AT_OK;
}

AT_Status_t AT_first_request(){
	start_receive();
	AT_Send("AT\r\n",4);
	int result = 0;
	wait_receive(200);
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
	wait_receive(200);

	int result = 0;
	result = strncmp("+MADDR=0001\r\n",rxbuf,13);
	UART1_printf("%s",rxbuf);
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

//void reg_device(){
//	start_receive();
//	LOG("WAIT MESSAGE");
//	wait_receive();
//
//
//	AT_Receive_Read_t received_pack;
//	AT_receive_read_pack(&received_pack);
//	/*process_pack*/
//	AT_Request_Type_t type = received_pack.type;
//
//	AT_Request_Set_t request_pack;
//	// is REG_DEVICE request
//	/*REG_DEVICE*/
//	if(type == REG_DEVICE){
//		AT_process_reg_device(&request_pack,&received_pack);
//	}
//}
