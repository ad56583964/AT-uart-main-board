

#include "sensor_test.h"

#include "main.h"
#include "drv_uart/drv_uart.h"
#include "at_cmd.h"
#include "main.h"


void test_IR(){
	#define GPIO_PB11_INITED

	while(1){
		int result = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11);
		if(result == 1){
			LOG("IR TRIGGED");
		}
		osDelay(100);
	}
}

void test_IR_counter(){
	#define GPIO_PB11_INITED

	int event_counter = 0;
	const int time_threshold = 10; // counter * 100ms
	while(1){
		int result = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11);
		if(result == 0){
			//clear counter
			event_counter = 0;
		}
		if(result == 1){
			event_counter += 1;
		}
		if(event_counter == time_threshold){
			LOG("IR TRIGGED");
		}
		osDelay(100);
	}
}

void sensor_test(){
	LOG("sensor test\n");
//	test_IR(); //success
	test_IR_counter();
}
