/*
 * main_app.c
 *
 *  Created on: 2023年4月19日
 *      Author: ad565
 */
#include "at_cmd.h"
#include "drv_uart/drv_uart.h"
#include "ssd1306/ssd1306.h"

static	AT_Receive_Read_t received_pack;
static	AT_Request_Set_t request_pack;
extern AT_mode_t g_AT_mode;

void reg_process(){
	int _state = 0;
	LOG("EnterREG\n");
	start_receive();
	LOG("WaitMessage\n");
	_state = wait_receive(100);
		/*update pack*/
	if(_state == 0){
		AT_receive_read_pack(&received_pack);
		AT_Request_Type_t type = received_pack.type;

		if(type == REG_DEVICE){
			AT_process_reg_device(&request_pack,&received_pack);
			LOG("RegFinish\n");
		}
		else{
			LOG("RegError\n");
		}
		g_AT_mode = IDLE;
	}
}

void polling_process(){
	start_receive();
	int _pollstate = wait_receive(1000);
	LOG("PollOnce\n");
	if(_pollstate == 0){
		LOG("GetPack\n");
		AT_receive_read_pack(&received_pack);
		AT_Request_Type_t _type = received_pack.type;
		if(_type == EDGE_ALARM){
			LOG("Alarm\n");
			LOG("%d\n",received_pack.source_addr);
			request_pack.addr = received_pack.source_addr;
			request_pack.data = 0U;
			request_pack.type = MAIN_ACK;
			AT_request_send_pack(&request_pack);
			LOG("SetAlarm");
		}
	}
}

void oledshow(){

}
