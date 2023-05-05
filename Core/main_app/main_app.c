/*
 * main_app.c
 *
 *  Created on: 2023年4月19日
 *      Author: ad565
 */
#include "at_cmd.h"
#include "drv_uart/drv_uart.h"
#include "ssd1306/ssd1306.h"
#include <stdio.h>

static	AT_Receive_Read_t received_pack;
static	AT_Request_Set_t request_pack;
extern AT_mode_t g_AT_mode;
extern AT_Device_Table_t AT_device_table;

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



//const char *type_name[3];
//type_name[0] = "DOOR";
//type_name[1] = "SMOKE";
//type_name[2] = "IR";
const char *type_name[] = {"DOOR", "SMOKE", "IR"};
const char *state_name[] = {
    "IDLE",
    "REG",
    "POLLING",
    "ALARM"
};

void init_edge_name(){

}

void device_show_test(){


	AT_device_table.Size = 2;
	AT_device_table.Device[0].address = 0x0005;
	AT_device_table.Device[1].address = 0x0010;
	AT_device_table.Device[0].type = EDGE_IR;
	AT_device_table.Device[1].type = EDGE_DOOR;
}

//string Edge_name =
#define FONTH 12
#define FONTW 9

void ssd1306_WriteHex(uint16_t value) {
    char hex_string[7]; // 6个字符（0x0000）加1个空字符

    snprintf(hex_string, sizeof(hex_string), "0x%04X", value);
    ssd1306_WriteString(hex_string,Font_7x10,Black);
}

void oledshow(){

	//State Show
	ssd1306_SetCursor(11*FONTW,5);
	ssd1306_WriteString(state_name[g_AT_mode],Font_7x10,Black);

	//Device Show
	if(AT_device_table.Size != 0){
		for(int i = AT_device_table.Size-1; i >= 0; i--){
			ssd1306_SetCursor(0*FONTW,i*FONTH);
			ssd1306_WriteString(type_name[AT_device_table.Device[i].type],Font_7x10,Black);
			ssd1306_SetCursor(5*FONTW,i*FONTH);
			ssd1306_WriteHex(AT_device_table.Device[i].address);
		}
	}
	else{
		ssd1306_SetCursor(0*FONTH,0*FONTW);
		ssd1306_WriteString("No Device",Font_7x10,Black);
	}
}
