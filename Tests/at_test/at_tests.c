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
	wait_receive(osWaitForever);

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

void test_preload(){

}

void test_receive_pack_REG_DEVICE_once(){
	AT_Init();
	AT_check_addr();

	/*AT_main_schedule()*/
	/*loop_once*/
	start_receive();
	LOG("WAIT MESSAGE");
	wait_receive(osWaitForever);

	AT_Receive_Read_t received_pack;
	AT_receive_read_pack(&received_pack);
	/*process_pack*/
	AT_Request_Type_t type = received_pack.type;

	AT_Request_Set_t request_pack;
	// is REG_DEVICE request
	/*REG_DEVICE*/
	if(type == REG_DEVICE){
		AT_process_reg_device(&request_pack,&received_pack);
	}
}

//void test_receive_pack_REG_DEVICE_once_old(){
//	AT_Init();
//	AT_check_addr();
//
//	/*AT_main_schedule()*/
//	/*loop_once*/
//	start_receive();
//	LOG("WAIT MESSAGE");
//	wait_receive();
//
//	AT_Receive_Read_t pack;
//	AT_receive_read_pack(&pack);
//	/*process_pack*/
//	AT_Request_Type_t type = pack.type;
//	uint16_t data = pack.data;
//
//	// is REG_DEVICE request
//	/*REG_DEVICE*/
//	open_receive();
//	AT_confirm_return(pack.source_addr);
//	clear_semaphore();
//	LOG("MAIN WAIT ACK");
//	wait_receive();
//	AT_receive_read_pack(&pack);
//	if(pack.type == EDGE_ACK){
//		AT_Device_insert(pack.source_addr,pack.data&0xff);
//		LOG("SUCCESS REG DEVICE");
//	}
//	osDelay(1);
//}

void test_AT_confirm_return()
{
	//addr 0x0005
	AT_confirm_return(0x0005);
}

void test_receive_pack_once(){
	AT_Init();
	AT_check_addr();

	/*AT_main_schedule()*/
	/*loop_once*/
	start_receive();
	wait_receive(osWaitForever);

	AT_Receive_Read_t pack;
	AT_receive_read_pack(&pack);
	/*process_pack*/
	uint8_t type = pack.type;
//	uint16_t data = pack.data;

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

void test_AT_request(){
	AT_Request_Set_t pack;
	AT_Receive_Read_t get_pack;
	pack.addr = 0x0005;
	pack.type = MAIN_ACK;
	AT_request(&pack,&get_pack);
	LOG("request success");

}

void test(){
//	test_AT_request(); //SUCCESS
//	test_AT_confirm_return(); //SUCCESS
	test_receive_pack_REG_DEVICE_once();
//	test_connect_24m();
//	test_init();
//	test_setpack();
//	test_register();
//	test_send();
	while(1);
}
