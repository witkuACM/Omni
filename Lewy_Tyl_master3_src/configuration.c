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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);		//CZUJNIKI
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
}

void GPIOinit(){
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);

	gpio.GPIO_Pin = GPIO_Pin_0; 			//PC0
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &gpio);
	GPIO_SetBits(GPIOC, GPIO_Pin_0);

	gpio.GPIO_Pin = GPIO_Pin_9;				// UART1 - TX - PA9
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;		//wyjscie jako funkcja alternatywna w trybie pushpull
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_10;			//UART1 - RX - PA10
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//wejscie plywajace (bez rezystora podciagajacego)
	GPIO_Init(GPIOA, &gpio);

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

	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //ENKODERY - PA6 PA7
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 // TIM3		 CH1 CH2
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &gpio);

	//CZUJNIKI ANALOGOWE
	gpio.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_4|GPIO_Pin_5;		//lewy tyl, lewy przod nadkole, lewy przod, prawy przod, prawy przod nadkole
	gpio.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &gpio);
	gpio.GPIO_Pin = GPIO_Pin_1;							//prawy tyl
	gpio.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &gpio);

	//CZUJNIKI CYFROWE
	gpio.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_9;	//lewy srodek, prawy srodek, lewy tyl
	gpio.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &gpio);
	gpio.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_13;			//lewy przod, prawy przod, prawy tyl
	gpio.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &gpio);
}

void UARTinit()
{
	USART_InitTypeDef uart;

	USART_StructInit(&uart);
	uart.USART_BaudRate = 115200;		//standardowa predkosc transmisji
	uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;;
	USART_Init(USART1, &uart);
	uart.USART_Mode = USART_Mode_Tx;	//UART3 tylko wysy³a
	USART_Init(USART3, &uart);

	USART_Cmd(USART1, ENABLE);
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

	tim.TIM_Period = 1200 - 1; 						//do enkodera
	tim.TIM_Prescaler = 0;
	tim.TIM_ClockDivision = 0;
	tim.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &tim);

	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_Cmd(TIM3, ENABLE);

	tim.TIM_CounterMode = TIM_CounterMode_Up;		//TIM4 - przerwanie po 1ms
	tim.TIM_Prescaler = 6400 - 1;					//podstawa czasu 0,1ms
	tim.TIM_Period = 10 - 1;						//odlicza 1ms
	TIM_TimeBaseInit(TIM4, &tim);

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);		//zdarzenie Update informuje o przepelnieniu
	TIM_Cmd(TIM4, ENABLE);
}

void DMAinit(){
	DMA_InitTypeDef dma;

	//KONFIGURACJA DMA1 - przekaz sterowania po UART1		DMA1 - CH5
	DMA_StructInit(&dma);
	dma.DMA_PeripheralBaseAddr = tab_instrukcje;				//adres rejestru DataRegister UART3 (tam ostatnia konwersja)
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//adres DR sta³y
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//dane 8-bitowe
	dma.DMA_MemoryBaseAddr = tab_dma;							//adres docelowy - zmienna
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;					//zapis w kolejne miejsce tablicy(nieprzydatne bo wysy³ka 1 bajtu)
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//dane 8-bitowe
	dma.DMA_DIR = DMA_DIR_PeripheralSRC;						//USART3 Ÿród³em
	dma.DMA_BufferSize = INSTR_SIZE;							//1 bajt danych
	dma.DMA_M2M = DMA_M2M_Enable;								//przesy³ z pamiêci do pamiêci
	dma.DMA_Mode = DMA_Mode_Circular;							//tryb ciaglego przesy³u
	dma.DMA_Priority = DMA_Priority_High;						//wysoki priorytet
	DMA_Init(DMA1_Channel5, &dma);
	DMA_Cmd(DMA1_Channel5, ENABLE);

	//CZUJNIKI 												DMA1 - CH1
	dma.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma.DMA_MemoryBaseAddr = (uint32_t)adc_value;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma.DMA_DIR = DMA_DIR_PeripheralSRC;
	dma.DMA_BufferSize = ADC_CHANNELS;
	dma.DMA_M2M = DMA_M2M_Disable;
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_Priority = DMA_Priority_VeryHigh;	//bardzo wysoki priorytet

	DMA_Init(DMA1_Channel1, &dma);
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

void NVICinit()
{
	NVIC_InitTypeDef nvic;

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 	//do przerwania z odbierania danych
	nvic.NVIC_IRQChannel = USART1_IRQn;				//ustawienie param i priorytetu kontrolera przerwan
	nvic.NVIC_IRQChannelPreemptionPriority = 0x02;	// dla UART1 - przerwanie od odbierania danych
	nvic.NVIC_IRQChannelSubPriority = 0x02;			// z terminala
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	nvic.NVIC_IRQChannel = TIM4_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0x00;
	nvic.NVIC_IRQChannelSubPriority = 0x00;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	//CZUJNIKI
	nvic.NVIC_IRQChannel = EXTI15_10_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0x01;
	nvic.NVIC_IRQChannelSubPriority = 0x01;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	nvic.NVIC_IRQChannel = EXTI9_5_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0x01;
	nvic.NVIC_IRQChannelSubPriority = 0x01;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	nvic.NVIC_IRQChannel = EXTI3_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0x01;
	nvic.NVIC_IRQChannelSubPriority = 0x01;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	nvic.NVIC_IRQChannel = EXTI4_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0x01;
	nvic.NVIC_IRQChannelSubPriority = 0x01;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
}

//CZUJNIKI
void PeriphClock(){
	ErrorStatus HSEStartUpStatus;  							//zmienna opisujaca rezultat uruchomienia HSE
	RCC_DeInit();	                                        //Reset ustawien RCC
	RCC_HSEConfig(RCC_HSE_ON);                             	//Wlaczenie HSE
	HSEStartUpStatus = RCC_WaitForHSEStartUp();		        //Odczekaj az HSE bedzie gotowy
	if(HSEStartUpStatus == SUCCESS)
	{
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		FLASH_SetLatency(FLASH_Latency_2);                   //ustaw zwloke dla pamieci Flash; zaleznie od taktowania rdzenia
		                                                       //0:<24MHz; 1:24~48MHz; 2:>48MHz
	    RCC_HCLKConfig(RCC_SYSCLK_Div1);                     //ustaw HCLK=SYSCLK
	    RCC_PCLK2Config(RCC_HCLK_Div1);                      //ustaw PCLK2=HCLK
	    RCC_PCLK1Config(RCC_HCLK_Div2);                      //ustaw PCLK1=HCLK/2
	    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); //ustaw PLLCLK = HSE*9 czyli 8MHz * 9 = 72 MHz
	    RCC_PLLCmd(ENABLE);                                  //wlacz PLL
	    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);  //odczekaj na poprawne uruchomienie PLL
	    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);           //ustaw PLL jako zrodlo sygnalu zegarowego
	    while(RCC_GetSYSCLKSource() != 0x08);                //odczekaj az PLL bedzie sygnalem zegarowym systemu
	}
}

void ADCinit()
{
	ADC_InitTypeDef adc;
	ADC_StructInit(&adc);
	adc.ADC_Mode = ADC_Mode_Independent;
	adc.ADC_ScanConvMode = ENABLE;
	adc.ADC_ContinuousConvMode = ENABLE;
	adc.ADC_NbrOfChannel = ADC_CHANNELS;
	adc.ADC_DataAlign = ADC_DataAlign_Right;
	adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_Init(ADC1, &adc);
}

void EXTIinit()
{
	EXTI_InitTypeDef exti;

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource15);
	EXTI_StructInit(&exti);
	exti.EXTI_Line = EXTI_Line15;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource14);
	EXTI_StructInit(&exti);
	exti.EXTI_Line = EXTI_Line14;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);
	EXTI_StructInit(&exti);
	exti.EXTI_Line = EXTI_Line7;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource9);
	EXTI_StructInit(&exti);
	exti.EXTI_Line = EXTI_Line9;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource3);
	EXTI_StructInit(&exti);
	exti.EXTI_Line = EXTI_Line3;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);
	EXTI_StructInit(&exti);
	exti.EXTI_Line = EXTI_Line4;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);
}

void Multipleks()
{
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_28Cycles5);		//czujnik lewy tyl
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_28Cycles5);		//czujnik lewy przod nadkole
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_28Cycles5);		//czujnik lewy przod
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 4, ADC_SampleTime_28Cycles5);		//czujnik prawy przod
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 5, ADC_SampleTime_28Cycles5);		//czujnik prawy przod nadkole
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 6, ADC_SampleTime_28Cycles5);		//czujnik prawy tyl

	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));

	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);											// start pomiaru
}
