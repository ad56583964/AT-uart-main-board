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

#define WAIT_TICK 500
uint8_t alarm_remain_tick = 0;
int roll_device_count = 0;
void polling_process(){
	start_receive();

	uint32_t tick_cur = osKernelGetTickCount();

	int _pollstate = wait_receive(WAIT_TICK);
//	LOG("PollOnce\n");
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
			alarm_remain_tick = 5;
		}
		else if(alarm_remain_tick > 0){
			alarm_remain_tick--;
		}
	}
	else if(alarm_remain_tick > 0){
		alarm_remain_tick--;
	}
	uint32_t tick_now = osKernelGetTickCount();
	uint32_t tick_delta = WAIT_TICK - (tick_now - tick_cur);

	if(tick_delta>0 && tick_delta <= WAIT_TICK){
		osDelay(tick_delta);
	}

	LOG("alarm_remain_tick:%d\n",alarm_remain_tick);
	// BEAT EDGE
	if(alarm_remain_tick == 0){
		if(AT_device_table.size != 0){

				request_pack.addr = AT_device_table.Device[roll_device_count].address;
				request_pack.type = BEAT;
				request_pack.data = 0U;
				uint8_t result = AT_request(&request_pack,&received_pack);
				if(result != AT_ERROR && received_pack.type == EDGE_ACK && (request_pack.addr == received_pack.source_addr)){
						LOG("EDGE:%d_ok\n",request_pack.addr);
				}
				else{
					AT_device_table.Device[roll_device_count].beat_lost++;
					if(AT_device_table.Device[roll_device_count].beat_lost > 5){
						delete_device(&AT_device_table,roll_device_count);
						LOG("Delete\n");
					}
				}
				roll_device_count++;
				if(roll_device_count >= AT_device_table.size){
					roll_device_count = 0;
				}
				LOG("roll_device_count:%d",roll_device_count);
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
