#include "stm32f407xx.h"


void clear_pending_bit(void){
	EXTI_TypeDef* pEXTI;
	pEXTI=EXTI;
	if((pEXTI->PR & 0x01)) pEXTI->PR |= 0x01;
	
	
}

void EXTI0_IRQHandler(void){
	USART_TypeDef *pUART2;
	pUART2 = USART2;
	
	//here we will send UART2_TX DMA request to DMA1 controller
	pUART2->CR3 |= ( 1 << 7);
	
	clear_pending_bit();
	 //send_some_data();
	
}