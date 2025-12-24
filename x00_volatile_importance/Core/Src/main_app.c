
#include "main_app.h"

volatile uint8_t status = 0;
GPIO_InitTypeDef hgpio;


int main(void){

	HAL_Init();
	SystemClockConfig();

	LED_Init();
	Button_Init();

		while(!status){


		}

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);

	while(TRUE);

	return 0;
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	if(GPIO_Pin == GPIO_PIN_13){

		if(status){

			status = 0;
		}
		else{

			status = 1;
		}
	}

}

void EXTI15_10_IRQHandler(void){

	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);

}

void LED_Init(void){

	__HAL_RCC_GPIOA_CLK_ENABLE();

	hgpio.Mode = GPIO_MODE_OUTPUT_PP;
	hgpio.Speed = GPIO_SPEED_HIGH;
	hgpio.Pull = GPIO_NOPULL;
	hgpio.Pin = GPIO_PIN_5;

	HAL_GPIO_Init(GPIOA, &hgpio);

}
void Button_Init(void){

	__HAL_RCC_GPIOC_CLK_ENABLE();

	hgpio.Mode = GPIO_MODE_IT_FALLING;
	hgpio.Pull = GPIO_NOPULL;
	hgpio.Speed = GPIO_SPEED_HIGH;
	hgpio.Pin = GPIO_PIN_13;

	HAL_GPIO_Init(GPIOC, &hgpio);

	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);


}


void SystemClockConfig(void){

	//TODO

}

void Error_Handler(void){

	while(TRUE);

}

void SysTick_Handler(void){

	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();

}



