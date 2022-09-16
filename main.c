#include <stdint.h>
#include "stm32f407xx.h"


void button_init(void);
void dma1_init(void);
void uart2_init(void);

void send_some_data(void);
char data_stream[] ="Hello World\r\n";

int main(){
	button_init();
	uart2_init();
	dma1_init();
	
	while(1){
		
	  
	}
	return 0;
}

void button_init(){
	//button is connected to PA10
	GPIO_TypeDef *pGPIOA;
	pGPIOA = GPIOA;
	
	//Enable the pripheral clock for the GPIOA 
	RCC_TypeDef  *pRCC;
	pRCC=RCC;
	pRCC->AHB1ENR |= 0x01;
	//Keep the GPIO pin input mode

	//Enable the Interrupt over that GPIO pin
	EXTI_TypeDef *pEXTI;
	pEXTI=EXTI;
	
	pEXTI->IMR |= 0x01;

	// Enable the clock for SYSCFG
	pRCC->APB2ENR |= (1 << 14);
	
	// Configure the SYSCFG CR1 reg
	SYSCFG_TypeDef* pSYSCFG;
	pSYSCFG=SYSCFG;
	pSYSCFG->EXTICR[0] &= (0x0F << 0);
	
	//Configure the edge detection
	pEXTI->FTSR |= 0x01;
	//Enable the IRQ related to that GPIO pin in NVIC of the processor
	NVIC_EnableIRQ(EXTI0_IRQn);
	
	
}
void dma1_init(){
	RCC_TypeDef *pRCC;
	pRCC = RCC;
	
	DMA_TypeDef *pDMA;
	pDMA=DMA1;
	
	DMA_Stream_TypeDef *pSTREAM6;
	pSTREAM6 = DMA1_Stream6;
	
	USART_TypeDef *pUART2;
	pUART2 = USART2;
	
	
	//1. enable the peripheral clock for the dma1
	pRCC->AHB1ENR |= ( 1 << 21);
	
	//2. identify the stream which is suitable for your peripheral
	     //channel 4 , stream 6
	
	//3. Identify the channel number on which uart2 peripheral sends dma request
	     //channel 4
	pSTREAM6->CR &= ~( 0x7 << 25);
	pSTREAM6->CR |=  ( 0x4 << 25);
	
	//4. Program the source address  (memory)
	pSTREAM6->M0AR = (uint32_t) data_stream;
	
	//5 . Program the destination address (peripheral )
	pSTREAM6->PAR = (uint32_t) &pUART2->DR;
	
	//6. Program number of data items to send 
	uint32_t len = sizeof(data_stream);
	pSTREAM6->NDTR = len;
	
	//7. The direction of data transfer . m2p, p2m , m2m
	pSTREAM6->CR |= (0x1 << 6);
	
	//8. Program the source and destination data width . 
	pSTREAM6->CR &= ~(0x3 << 13); 
	pSTREAM6->CR &= ~(0x3 << 11);
	
	//8a . enable memory auto increment 
	pSTREAM6->CR |= ( 1 << 10);
	
	//9. direct mode or fifo mode 
	pSTREAM6->FCR |= ( 1 << 2);
	
	//10. Select the fifo threshold 
	pSTREAM6->FCR &= ~(0x3 << 0); //clearing 
	pSTREAM6->FCR |=  (0x3 << 0); //setting
	
	//11. enable the circular mode if required 
	
	//12. single transfer or burst transfer . 
	
	//13. Configure the stream priority 
		pSTREAM6->CR |= 0x1;
	
}
void uart2_init(){
	RCC_TypeDef *pRCC;
	pRCC = RCC;
	
	GPIO_TypeDef *pGPIOA;
	pGPIOA = GPIOA;
	
	USART_TypeDef *pUART2;
	pUART2 = USART2;
	
	//enable the peripheral clock for the uart2 peripheral 
	pRCC->APB1ENR |= (0x01 << 17);
	
	//Configure the gpio pins for uart_tx and uart_rx functionality 
	     //PA2 as TX , PA3 as RX
	
	// lets configure PA2 as UART2 TX
	
  //Enable the clock for the GPIOA peripheral 
	pRCC->AHB1ENR |= 0x01;

  // Change the mode of the PA2 to alternate function mode
	pGPIOA->MODER &= ~(0x03 << 4);
	pGPIOA->MODER |= (0x02 << 4);
  pGPIOA->AFR[0] &= ~(0x0F << 8);
	pGPIOA->AFR[0] |= (0x07 << 8);

  //Enable or disable Pull-up resistor of the gpio pin if required. 
	GPIOA->PUPDR |= (0x01 << 4);
	// lets configure PA3 as UART2 RX

  //Change the mode of the PA3 to alternate function mode
	pGPIOA->MODER &= ~(0x03 << 6);
	pGPIOA->MODER |= (0x02 << 6);
  pGPIOA->AFR[0] &= ~(0x0F << 12);
	pGPIOA->AFR[0] |= (0x07 << 12);
  
  // Enable or disable Pull-up resistor for PA3	
	GPIOA->PUPDR |= (0x01 << 6);
	
	
	// Configure the baudrate 
	pUART2->BRR = 0x8B;
	
	// configure the data width, no of stop bits , etc 
	// <no configuration reqd here , we will use default values 
	
	// Enable the TX engine of the uart peripheral 
	pUART2->CR1 |= (0x1 << 3);
	
	
	//Enable the uart peripheral 
	pUART2->CR1 |= (0x1 << 13);
	
}

void send_some_data(void)
{
	char somedata[] ="Hello World\r\n";
	USART_TypeDef *pUART2;
	pUART2 = USART2;
	
	//make sure that in the status register TXE is set (TDR is empty )
	//if TXE is 1, put the byte 
	
	uint32_t len = sizeof(somedata);

	for ( uint32_t i =0 ; i < len ; i++)
	{
		//wer are waiting for TXE to become 1
		while( ! ( pUART2->SR & ( 1 << 7) ) ) ;
		
		pUART2->DR = somedata[i];
	}
	
	
}

