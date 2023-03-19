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
	AT_request_pack pack;
	AT_receive_read_pack(pack);
//	/*decoding*/
//	AT_Receive_pack_t* cur_pack = (AT_Receive_pack_t*)rxbuf;
//	if((cur_pack->header_H == AT_receive_pack_template.header_H
//	&&	cur_pack->header_L == AT_receive_pack_template.header_L
//	)){
//		LOG("success");
//	}
	decode_AT();
}

void test(){
	test_setpack();
//	test_send();
	while(1);
}
