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
	IS_DOWN = 0,
	IS_UP = 1,
	IS_PUSHED
};

enum button_state{
	BUTTON_UP = 0U,
	BUTTON_DOWN = 1U
};

char state = 0;

typedef struct _button_t {
	uint8_t id;
	uint8_t state;
	uint8_t event_state;
	uint8_t cur_state;
	uint8_t ack;
} Button_t;

Button_t button[2];

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

void pushed01_callback(){
	AT_mode = REG;
	LOG("REG\n");
}

void pushed02_callback(){
	AT_mode = POLLING;
	LOG("POLLING\n");
}


void ButtonCallback(){
	char B11 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11);
	char B10 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10);

	if(B11 == BUTTON_DOWN){
		button[0].state = IS_DOWN;
		if(button[0].cur_state == IS_DOWN){
			button[0].event_state = IS_PUSHED;
		}
	}
	else if(B11 == BUTTON_UP){
		button[0].state = IS_UP;
		if(button[0].event_state == IS_PUSHED){
			pushed01_callback(); //不行，放在这里会阻塞的吧
			button[0].event_state = IS_UP;
//			使用 消息通知替代
//			看一下耗时暂时先使用callback
		}
	}

	if(B10 == BUTTON_DOWN){
		button[1].state = IS_DOWN;
		if(button[1].cur_state == IS_DOWN){
			button[1].event_state = IS_PUSHED;
		}
	}
	else if(B10 == BUTTON_UP){
		button[1].state = IS_UP;
		if(button[1].event_state == IS_PUSHED){
			pushed02_callback();
			button[0].event_state = IS_UP;
		}
	}

	button[0].cur_state = button[0].state;
	button[1].cur_state = button[1].state;
}

char PUSHED_ACK = 0;
void ButtonCallback_old(){
	char B11 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11);

	if(B11 == BUTTON_DOWN){
		state = IS_DOWN;
		if(cur_state == IS_DOWN){
			state = IS_PUSHED;
			PUSHED_ACK++;
		}
	}

	else if(B11 == BUTTON_UP){
		state = IS_UP;
		PUSHED_ACK = 0;
		if(cur_state == IS_PUSHED){
//			pushed_callback(); //不行，放在这里会阻塞的吧
//			使用 消息通知替代
//			看一下耗时暂时先使用callback
		}
	}

	cur_state = state;
}


