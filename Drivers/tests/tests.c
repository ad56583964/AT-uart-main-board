/*
 * tests.c
 *
 *  Created on: Mar 18, 2023
 *      Author: ad565
 */

#include "at_cmd.h"

//	不会传出去的字节序也是乱的吧
void test_send(){
//	AT_request_setdata();
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

	if(!(AT_Device_insert(pack.target_addr) == ADD_SUCCESS)){
		LOG("ALREADY EXIST");
	}
}

void test(){
//	test_setpack();
	test_register();
//	test_send();
	while(1);
}
