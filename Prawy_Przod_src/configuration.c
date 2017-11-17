/*
 * config.c
 *
 *  Created on: 12 lis 2017
 *      Author: wiciu
 */

#include "stm32f10x.h"
#include "configuration.h"
#include "variables.h"

void RCCinit(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

void GPIOinit(){
	GPIO_InitTypeDef gpio;

	GPIO_StructInit(&gpio);

	gpio.GPIO_Pin = GPIO_Pin_0; 			//PC0
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &gpio);
	GPIO_ResetBits(GPIOC, GPIO_Pin_0);

	gpio.GPIO_Pin = GPIO_Pin_10;			//UART3 - TX - PB10
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;		//wyjscie jako funkcja alternatywna w trybie pushpull
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_11;			//UART3 - RX - PB11
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//wejscie plywajace (bez rezystora podciagajacego)
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//PC6, PC7
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;		//piny kierunkowe mostka
	GPIO_Init(GPIOC, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_3;				//PWM - PA3 - wypelnienie podawane na silnik
	gpio.GPIO_Speed = GPIO_Speed_50MHz; 	//zwiêkszamy czêstotliwoœæ pracy z domyœlnej 2MHz na 50Mhz
	gpio.GPIO_Mode = GPIO_Mode_AF_PP; 		//funkcja alternatywna Push-Pull (PWM)
	GPIO_Init(GPIOA, &gpio);
}

void UARTinit()
{
	USART_InitTypeDef uart;

	USART_StructInit(&uart);
	uart.USART_BaudRate = 115200;		//standardowa predkosc transmisji
	uart.USART_Mode = USART_Mode_Rx;	//TYLKO ODBIERAMY - ¿eby DataRegister nie bra³ z wysy³ania
	USART_Init(USART3, &uart);

	USART_Cmd(USART3, ENABLE);
}

void TIMinit()
{
 	TIM_TimeBaseInitTypeDef tim;
 	TIM_OCInitTypeDef  channel;

 	TIM_TimeBaseStructInit(&tim); 				//TIM2 - CH4 - tryb PWM1 - PA3
	tim.TIM_CounterMode = TIM_CounterMode_Up;
	tim.TIM_Prescaler = 64 - 1;
	tim.TIM_Period = 1000 - 1;
	TIM_TimeBaseInit(TIM2, &tim);

	TIM_OCStructInit(&channel);
	channel.TIM_OCMode = TIM_OCMode_PWM1;
	channel.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC4Init(TIM2, &channel);
	TIM_Cmd(TIM2, ENABLE);
}

void DMAinit(){
	DMA_InitTypeDef dma;

	//KONFIGURACJA DMA1 - przekaz sterowania po UART3		DMA1 - CH3
	DMA_StructInit(&dma);
	dma.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR;			//adres rejestru DataRegister UART3 (tam ostatnia konwersja)
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//adres DR sta³y
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//dane 8-bitowe
	dma.DMA_MemoryBaseAddr = (uint32_t)tab_dma;					//adres docelowy - zmienna
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;					//zapis w kolejne miejsce tablicy(nieprzydatne bo wysy³ka 1 bajtu)
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//dane 8-bitowe
	dma.DMA_DIR = DMA_DIR_PeripheralSRC;						//USART3 Ÿród³em
	dma.DMA_BufferSize = DATA_SIZE;								//1 bajt danych
	dma.DMA_M2M = DMA_M2M_Enable;								//przesy³ z pamiêci do pamiêci
	dma.DMA_Mode = DMA_Mode_Circular;							//tryb ciaglego przesy³u
	dma.DMA_Priority = DMA_Priority_High;						//wysoki priorytet
	DMA_Init(DMA1_Channel3, &dma);
	DMA_Cmd(DMA1_Channel3, ENABLE);
}

void NVICinit()
{
	NVIC_InitTypeDef nvic;

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); 	//do przerwania z odbierania danych uart3
	nvic.NVIC_IRQChannel = USART3_IRQn;				//ustawienie param i priorytetu kontrolera przerwan
	nvic.NVIC_IRQChannelPreemptionPriority = 0x00;	//plytka Slave tylko odbiera dane
	nvic.NVIC_IRQChannelSubPriority = 0x00;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
}
