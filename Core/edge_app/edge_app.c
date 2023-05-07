/*
 * edge_app.c
 *
 *  Created on: Apr 19, 2023
 *      Author: ad565
 */
#include "at_cmd.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "adc.h"

static AT_Request_Set_t pack;
static AT_Receive_Read_t get_pack;

static uint32_t pv_smoke_adc;

void edge_process_reg(){
	osDelay(1000);
	LOG("Need Pair\n");
	while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) != 1);
	pack.addr = 0x0001;
	pack.data = EDGE_IR;
	pack.type = REG_DEVICE;
	LOG("Send Request\n");
	AT_request(&pack,&get_pack);
	if(get_pack.type == MAIN_ACK){
		pack.addr = 0x0001;
		pack.data = 0U;
		pack.type = EDGE_ACK;
		AT_request_send_pack(&pack);
	}
	LOG("Requested\n");
}

void edge_main_loop(){
	for(;;){
//		sensor_check
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1,1);
		pv_smoke_adc = HAL_ADC_GetValue(&hadc1);
		LOG("%d\n",pv_smoke_adc);
		if( pv_smoke_adc>2000 || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == 1){
			LOG("Alarming");
			pack.addr = 0x0001;
			pack.type = EDGE_ALARM;
			pack.data = 0U;
//			int _state = -1;

			while(1){
				AT_request(&pack,&get_pack);
				if(get_pack.type == MAIN_ACK){
					LOG("StopAlarm");
					break;
				}
			}
		}
		osDelay(100);
	}
}
