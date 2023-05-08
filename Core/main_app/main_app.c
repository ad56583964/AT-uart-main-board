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

void open_alarm(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, RESET);
}

void close_alarm(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, SET);
}

void reg_process(){
	int _state = 0;
	LOG("EnterREG\n");
	start_receive();
	LOG("WaitMessage\n");
	_state = wait_receive(3000);
		/*update pack*/
	if(_state == AT_OK){
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

void set_Alarm(const uint16_t addr){
	for(int i = 0; i < AT_device_table.size; i++){
		if(AT_device_table.Device[i].address == addr){
			AT_device_table.Device[i].state = DEVICE_ALARMING;
		}
	}
	open_alarm();
}

void polling_process(){
	start_receive();

	uint32_t tick_cur = osKernelGetTickCount();

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
			set_Alarm(received_pack.source_addr);
		}
	}
	uint32_t tick_now = osKernelGetTickCount();
	uint32_t tick_delta = 1000 - (tick_now - tick_cur);

	if(tick_delta>0 && tick_delta <= 1000){
		osDelay(tick_delta);
	}

	request_pack.addr = AT_device_table.Device[0].address;
	request_pack.type = BEAT;
	request_pack.data = 0U;
	uint8_t result = AT_request(&request_pack,&received_pack);
	if(result != AT_ERROR){
		if(received_pack.type == EDGE_ACK){
			LOG("EDGE:%d_ok",request_pack.addr);
		}
	}
	else{

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


	AT_device_table.size = 2;
	AT_device_table.Device[0].address = 0x0005;
	AT_device_table.Device[1].address = 0x0010;
	AT_device_table.Device[0].type = EDGE_IR;
	AT_device_table.Device[1].type = EDGE_DOOR;
	AT_device_table.Device[1].state = DEVICE_IDLE;
	AT_device_table.Device[0].state = DEVICE_ALARMING;
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
	if(AT_device_table.size != 0){
		for(int i = AT_device_table.size-1; i >= 0; i--){
			ssd1306_SetCursor(0*FONTW,i*FONTH);
			ssd1306_WriteString(type_name[AT_device_table.Device[i].type],Font_7x10,Black);
			ssd1306_SetCursor(5*FONTW,i*FONTH);
			ssd1306_WriteHex(AT_device_table.Device[i].address);
			if(AT_device_table.Device[i].state == DEVICE_ALARMING){
				ssd1306_SetCursor(4*FONTW,i*FONTH);
				ssd1306_WriteChar('X',Font_7x10,Black);
			}
		}
	}
	else{
		ssd1306_SetCursor(0*FONTH,0*FONTW);
		ssd1306_WriteString("No Device",Font_7x10,Black);
	}
}
