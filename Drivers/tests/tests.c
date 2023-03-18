/*
 * tests.c
 *
 *  Created on: Mar 18, 2023
 *      Author: ad565
 */

#include "at_cmd.h"


void test_register(){
//	AT_Send(AT_request_pack.raw,14);
	start_receive();
	wait_receive();

	/*decoding*/
	AT_Receive_pack_t* cur_pack = (AT_Receive_pack_t*)rxbuf;
	if((cur_pack->header_H == AT_receive_pack_template.header_H
	&&	cur_pack->header_L == AT_receive_pack_template.header_L
	)){
		LOG("success");
	}

	decode_AT();
}

void test(){
	test_register();
	while(1);
}
