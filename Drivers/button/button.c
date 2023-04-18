/*
 * button.c
 *
 *  Created on: 2023年4月12日
 *      Author: ad565
 */

//struct button{
//	char id;
//	char state;
//	(void*) callback;
//};

//void create_button(GPIOA,GPIO_PIN){
//
//}
#include "stm32f1xx.h"
#include "cmsis_os.h"
#include "at_cmd.h"

extern osTimerId_t ButtonProcessTimerHandle;

enum state{
	IS_DOWN = 0U,
	IS_UP = 1U,
	IS_PUSHED
};

enum button_state{
	BUTTON_UP = 0U,
	BUTTON_DOWN = 1U
};

char state = 0;

//the registered callback in HAL
//取消使用 EXTI 触发
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//	if(GPIO_Pin == GPIO_PIN_11){
//		state = IS_PUSHED;
//
//		return;
//	}
//}

char cur_state = IS_UP;

extern AT_mode_t AT_mode;

void pushed_callback(){
	AT_mode = REG;
}

void ButtonCallback(){
	char B11 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11);

	if(B11 == BUTTON_DOWN){
		state = IS_DOWN;
		if(cur_state == IS_DOWN){
			state = IS_PUSHED;
			pushed_callback(); //不行，放在这里会阻塞的吧
//			使用 消息通知替代
//			看一下耗时暂时先使用callback
		}
	}
	else if(B11 == BUTTON_UP){
		state = IS_UP;
	}

	cur_state = state;
}


