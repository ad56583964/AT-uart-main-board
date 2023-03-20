/*
 * tests.c
 *
 *  Created on: Mar 18, 2023
 *      Author: ad565
 */

#include "at_cmd.h"

//	不会传出去的字节序也是乱的吧
void test_send(){
	uint8_t* cur_pack = (uint8_t*)&AT_request_pack;
	AT_Send(cur_pack,13);
}

void test_setpack(){
	AT_Request_Set_t pack;
	pack.addr = 0x0001;
	pack.data = 0xefab;
	pack.type = 0xad;
	AT_request_send_pack(&pack);
}

void test_register(){

	start_receive();
	wait_receive();

	AT_Receive_Read_t pack;
	AT_receive_read_pack(&pack);
}

void test_connect_24m(){
	AT_Init();
	AT_check_addr();

	if(AT_device_mode == MAIN_DEVICE){
		AT_main_schedule();
	}
}

void test_connect_receive(){
	AT_Init();
	AT_check_addr();

	switch(AT_device_mode){
		case MAIN_DEVICE :{
			AT_main_schedule();
			break;
		}
		case EDGE_DEVICE :{
			/*AT_EDHE_schedule();*/
			break;
		}
		case UNKNOWN :{
			break;
		}
	}
}

void test_receive_pack_REG_DEVICE_once(){
	AT_Init();
	AT_check_addr();

	/*AT_main_schedule()*/
	/*loop_once*/
	start_receive();
	wait_receive();

	AT_Receive_Read_t pack;
	AT_receive_read_pack(&pack);
	/*process_pack*/
	AT_Request_Type_t type = pack.type;
	uint16_t data = pack.data;

	/*REG_DEVICE*/
	AT_Device_insert(pack.source_addr,pack.data&0xff);

	start_receive();
	AT_confirm_return(pack.source_addr);
	wait_receive();
	AT_receive_read_pack(&pack);

	osDelay(1);

}

void test_receive_pack_once(){
	AT_Init();
	AT_check_addr();

	/*AT_main_schedule()*/
	/*loop_once*/
	start_receive();
	wait_receive();

	AT_Receive_Read_t pack;
	AT_receive_read_pack(&pack);
	/*process_pack*/
	uint8_t type = pack.type;
	uint16_t data = pack.data;

	switch(type){
		case REG_DEVICE:{
			AT_Device_insert(pack.source_addr,pack.data&0xff);
			break;
		}
		case IR_DEVICE:{
			break;
		}
		case SMOKE_DEVICE:{
			break;
		}
		case DOOR_DEVICE:{
			break;
		}
	}

	LOG(rxbuf);
	osDelay(1);

}

void test(){
	test_receive_pack_REG_DEVICE_once();
//	test_connect_24m();
//	test_init();
//	test_setpack();
//	test_register();
//	test_send();
	while(1);
}
