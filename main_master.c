#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

// Program MASTER LEWY TY£
// Sterowanie W-S-A-D oraz spacja
// Rozszerzone o obs³ugê czujników

#define 	START_SIZE			3
#define		STOP_SIZE			3
#define 	DATA_SIZE			1
#define		BUFFER_SIZE			START_SIZE+STOP_SIZE+DATA_SIZE

// CZUJNIKI ///////////////////////////////////////////////////////////////////////////////////////////////
#define ADC_CHANNELS 6 							//liczba czujnikow analogowych, z ktorych odczytujemy dane
uint16_t adc_value[ADC_CHANNELS];				//bufor do DMA na odczyty z czujnikow
uint32_t vmax=1400; 							//ok. 30cm
uint16_t lbnad=0, lfnad=1, lf=2, rf=3, rfnad=4, rbnad=5;
uint16_t blok_przod=0,blok_tyl=0,blok_prawo=0,blok_lewo=0;
///////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t wypelnienie;
volatile uint32_t timer_ms = 0;
bool FLAG_START=false;
bool FLAG_STOP=false;
bool FLAG_DATA=false;

uint8_t znak_pocz = 0x5B;			// '['
uint8_t znak_kropka = 0x2E;			// '.'
uint8_t znak_kon = 0x5D;			// ']'
uint8_t znak_null = 0x00;			// NULL
volatile uint8_t tab[BUFFER_SIZE];
volatile uint8_t tab_dma[DATA_SIZE];

//CZUJNIKI///////////////////////////////////////////////////////////////////////////////////////////////
void PeriphClock();
void ADCinit();
void EXTIinit();
void Multipleks();
unsigned long int SysTick_Config_Mod(unsigned long int SysTick_CLKSource, unsigned long int Ticks);
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//KONFIGURACJA FUNKCJI STM
void RCCinit();
void GPIOinit();
void UARTinit();
void TIMinit();
void DMAinit();
void NVICinit();

void SysTick_Handler();
void delay_ms(int time);
void send_char3(char c);
void send_char1(char c);

void USART1_IRQHandler();

//FUNKCJE JAZDY
void przod();
void tyl();
void lewo();
void prawo();
void zwrotLewo();
void zwrotPrawo();
void stop();
void nic();

//FUNKCJE TRANSMISJI POLECEN PRZEZ UART1 I STEROWANIE
void czysc_flagi();
void czysc_dane(uint8_t buffer[]);
void ramka(uint8_t buffer[]);
void sprawdz_ramke(uint8_t buffer[]);
void sterowanie(uint8_t buffer[]);

//CZUJNIKI///////////////////////////////////////////////////////////////////////////////////////////////
void EXTI3_IRQHandler()
{
	stop();
	blok_lewo=1;
	blok_przod = 1;
	blok_tyl = 1;
	EXTI_ClearITPendingBit(EXTI_Line3);
}
void EXTI4_IRQHandler()
{
	stop();
	blok_prawo=1;
	blok_przod = 1;
	blok_tyl = 1;
	EXTI_ClearITPendingBit(EXTI_Line4);
}
void EXTI9_5_IRQHandler()
{
	stop();
	blok_tyl=1;
	EXTI_ClearITPendingBit(EXTI_Line8);
	EXTI_ClearITPendingBit(EXTI_Line9);
}
void EXTI15_10_IRQHandler()
{
	stop();
	blok_przod=1;
	EXTI_ClearITPendingBit(EXTI_Line14);
	EXTI_ClearITPendingBit(EXTI_Line15);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(void)
{
	SysTick_Config(SystemCoreClock / 1000);

	PeriphClock();	//CZUJNIKI

	RCCinit();

	ADCinit();  	//CZUJNIKI

	GPIOinit();
	DMAinit();

	EXTIinit();		//CZUJNIKI

	NVICinit();
	UARTinit();
	TIMinit();

	Multipleks();	//CZUJNIKI
	/* SysTick end of count event each 1ms */
	//	RCC_GetClocksFreq(&RCC_Clocks);
	//	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

	//if (SysTick_Config_Mod(SysTick_CLKSource_HCLK_Div8, 9000000ul))
	//	while(1);		// W razie bledu petla nieskonczona
	////////////////////////////////////////////////////////////////////////

	ramka(tab);

	while(1){
		sterowanie(tab_dma);
	}
}

void RCCinit(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//alternatywne funkcje pinow
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	// zegar do uarta1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	// zegar do uarta3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);		//CZUJNIKI
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);							//CZUJNIKI
}
void GPIOinit()
{
	GPIO_InitTypeDef gpio;

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_0; // odpierdalamy
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &gpio);
	GPIO_SetBits(GPIOC, GPIO_Pin_0);

	gpio.GPIO_Pin = GPIO_Pin_9;				// UART1 - TX
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;		//wyjscie jako funkcja alternatywna w trybie pushpull
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_10;			//UART1 - RX
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//wejscie plywajace (bez rezystora podciagajacego)
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_10;			//UART3 - TX
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;		//wyjscie jako funkcja alternatywna w trybie pushpull
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_11;			//UART3 - RX
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//wejscie plywajace (bez rezystora podciagajacego)
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//bity kierunkowe mostka
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;		//konfiguracja pinow portu C mastera
	GPIO_Init(GPIOC, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_3;				// PWM_MOSTEKH
	gpio.GPIO_Speed = GPIO_Speed_50MHz; 	// zwiêkszamy czêstotliwoœæ pracy pinów z domyœlnej 2MHz na 50Mhz
	gpio.GPIO_Mode = GPIO_Mode_AF_PP; 		//funkcja alternatywna Push-Pull (PWM)
	GPIO_Init(GPIOA, &gpio);

	//CZUJNIKI////////////////////////////////////////////////////////////////////////////////////////////////
	//analogowe
	gpio.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_4|GPIO_Pin_5;		//lewy tyl, lewy przod nadkole, lewy przod, prawy przod, prawy przod nadkole
	gpio.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &gpio);
	gpio.GPIO_Pin = GPIO_Pin_1;							//prawy tyl
	gpio.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &gpio);
	//cyfrowe
	gpio.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_8|GPIO_Pin_9;	//lewy srodek, prawy srodek, tyl
	gpio.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &gpio);
	gpio.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;			//przod
	gpio.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &gpio);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
}
void UARTinit()
{
	USART_InitTypeDef uart;

	USART_StructInit(&uart);
	uart.USART_BaudRate = 115200;			//standardowa predkosc transmisji
//	uart.USART_Mode = USART_Mode_Rx;		//TYLKO ODBIERAMY - ¿eby DataRegister nie bra³ z wysy³ania, dla UART1
	USART_Init(USART1, &uart);
	uart.USART_Mode = USART_Mode_Tx;		//UART3 tylko wysy³a
	USART_Init(USART3, &uart);

	USART_Cmd(USART1, ENABLE);
	USART_Cmd(USART3, ENABLE);
}
void TIMinit()
{
	TIM_TimeBaseInitTypeDef tim;
	TIM_OCInitTypeDef  channel;

	TIM_TimeBaseStructInit(&tim); 					//konfigurujemy TIM2
	tim.TIM_CounterMode = TIM_CounterMode_Up;
	tim.TIM_Prescaler = 64 - 1;
	tim.TIM_Period = 1000 - 1;
	TIM_TimeBaseInit(TIM2, &tim);

	TIM_OCStructInit(&channel);
	channel.TIM_OCMode = TIM_OCMode_PWM1; 			//kana³y maj¹ pracowaæ w trybie PWM
	channel.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC4Init(TIM2, &channel);
	TIM_Cmd(TIM2, ENABLE);
}
void DMAinit(){
	DMA_InitTypeDef dma;

	//KONFIGURACJA DMA1 do odczytu wyniku konwersji
	DMA_StructInit(&dma);
	dma.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;			//adres rejestru DataRegister UART1 (tam ostatnia konwersja)
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//adres DR sta³y
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//dane 8-bitowe
	dma.DMA_MemoryBaseAddr = (uint32_t)tab_dma;					//adres docelowy - zmienna
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;					//zapis w kolejne miejsce tablicy(nieprzydatne bo wysy³ka 1 bajtu)
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//dane 8-bitowe
	dma.DMA_DIR = DMA_DIR_PeripheralSRC;						//USART2 Ÿród³em
	dma.DMA_BufferSize = DATA_SIZE;								//1 bajt danych
	dma.DMA_M2M = DMA_M2M_Enable;								//przesy³ z pamiêci do pamiêci
	dma.DMA_Mode = DMA_Mode_Circular;							//tryb ci¹g³ego przesy³u
	dma.DMA_Priority = DMA_Priority_High;						//wysoki priorytet
	DMA_Init(DMA1_Channel5, &dma);
	DMA_Cmd(DMA1_Channel5, ENABLE);

	//CZUJNIKI////////////////////////////////////////////////////////////////////////////////////////////////
	dma.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma.DMA_MemoryBaseAddr = (uint32_t)adc_value;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma.DMA_DIR = DMA_DIR_PeripheralSRC;
	dma.DMA_BufferSize = ADC_CHANNELS;
	dma.DMA_M2M = DMA_M2M_Enable;
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_Priority = DMA_Priority_VeryHigh;						//wysoki priorytet
	DMA_Init(DMA1_Channel1, &dma);
	DMA_Cmd(DMA1_Channel1, ENABLE);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
}
void NVICinit()
{
	NVIC_InitTypeDef nvic;

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 	//do przerwania z odbierania danych
	nvic.NVIC_IRQChannel = USART1_IRQn;				//ustawienie param i priorytetu kontrolera przerwan
	nvic.NVIC_IRQChannelPreemptionPriority = 0x00;	// dla UART1 - przerwanie od odbierania danych
	nvic.NVIC_IRQChannelSubPriority = 0x00;			// z terminala
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	//CZUJNIKI//////////////////////////////////////////////////////////////////////////////////////////////////
	nvic.NVIC_IRQChannel = EXTI15_10_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0x00;
	nvic.NVIC_IRQChannelSubPriority = 0x00;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	nvic.NVIC_IRQChannel = EXTI9_5_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0x00;
	nvic.NVIC_IRQChannelSubPriority = 0x00;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	nvic.NVIC_IRQChannel = EXTI3_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0x00;
	nvic.NVIC_IRQChannelSubPriority = 0x00;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	nvic.NVIC_IRQChannel = EXTI4_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0x00;
	nvic.NVIC_IRQChannelSubPriority = 0x00;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

//CZUJNIKI///////////////////////////////////////////////////////////////////////////////////////////////////////
void PeriphClock(){
	RCC_ClocksTypeDef RCC_Clocks;
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

	  /*Tu nalezy umiescic kod zwiazany z konfiguracja sygnalow zegarowych potrzebnych w programie peryferiow*/
	}
}
void ADCinit()
{
	ADC_InitTypeDef adc;
	ADC_StructInit(&adc);
	adc.ADC_ScanConvMode = ENABLE;
	adc.ADC_ContinuousConvMode = ENABLE;
	adc.ADC_NbrOfChannel = ADC_CHANNELS;
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

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource8);
	EXTI_StructInit(&exti);
	exti.EXTI_Line = EXTI_Line8;
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
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);		//czujnik lewy tyl
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);		//czujnik lewy przod nadkole
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);		//czujnik lewy przod
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 4, ADC_SampleTime_55Cycles5);		//czujnik prawy przod
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 5, ADC_SampleTime_55Cycles5);		//czujnik prawy przod nadkole
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 6, ADC_SampleTime_55Cycles5);		//czujnik prawy tyl

	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));

	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);											// start pomiaru
}
unsigned long int SysTick_Config_Mod(unsigned long int SysTick_CLKSource, unsigned long int Ticks)
{
	//inicjalizacja licznika SysTick
	//zastepuje funkcje z bibliotek STM w zwiazku z bledem w funcji SysTick_Config
	unsigned long int Settings;

	assert_param(IS_SYSTICK_CLK_SOURCE(SysTick_CLKSource));

	if (Ticks > SysTick_LOAD_RELOAD_Msk)  return (1);             //Kontrola, czy wartosc poczatkowa nie przekracza max

	SysTick->LOAD = (Ticks & SysTick_LOAD_RELOAD_Msk) - 1;        //Ustaw wartosc poczatkowa licznika
	NVIC_SetPriority (SysTick_IRQn, 0);                           //Ustaw priorytet przerwania
	SysTick->VAL  = 0;                                            //Ustaw wartosc aktualna licznika
	Settings=SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;  //Ustaw flagi wlaczaenia SysTick IRQ  i samego licznika
	if (SysTick_CLKSource == SysTick_CLKSource_HCLK){             //Wybierz flage ustawien zrodla sygnalu zegarowego
		Settings |= SysTick_CLKSource_HCLK;
	} 	else {
		Settings &= SysTick_CLKSource_HCLK_Div8;
	}
	SysTick->CTRL = Settings;                                     //Zapisz ustawienia do rejestru sterujacego SysTick (i wlacz licznik)
	return (0);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SysTick_Handler()
{
	if (timer_ms) {
		timer_ms--;
	}

	//CZUJNIKI/////////////////////////////////////////////////////////////////////////////////////////////////
	/*if ((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 1)&&(adc_value[lbnad]<vmax)&&(adc_value[lfnad]<vmax))
		//TUTAJ WYRZUCA CI¥GLE BLOK_LEWO=1; DO SPRAWDZENIA dwa czujniki
		blok_lewo=0;
	else
		blok_lewo=1;
	if ((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 1)&&(adc_value[rbnad]<vmax)&&(adc_value[rfnad]<vmax))
		blok_prawo=0;
	else
		blok_prawo=1;*/
	if ((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8) == 1)&&(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == 1))
		blok_tyl=0;
	else
		blok_tyl=1;
	if ((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14) == 1)&&(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15) == 1)/*&&(adc_value[rf]<vmax)&&(adc_value[lf]<vmax)*/)
		blok_przod=0;
	else
		blok_przod=1;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
void delay_ms(int time)
{
	timer_ms = time;
	while (timer_ms) {};
}
void send_char3(char c)				//procedura wyslania znaku przez UART3 z Mastera do Slave'ów
{
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	//czekamy az bufor nadawczy uart3 bedzie wolny
    	USART_SendData(USART3, c);
}

////////////////////////////////////////////////////
void send_char1(char c)				//procedura wyslania znaku przez UART1 TEST CZUJNIKOW ODPOWIEDZI
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	//czekamy az bufor nadawczy uarT1 bedzie wolny
    	USART_SendData(USART1, c);
}
////////////////////////////////////////////////////////////////////

void USART1_IRQHandler()			//przerwanie od odbierania danych przez UART1 - bluetooth
{
	 char c = USART_ReceiveData(USART1);
	 tab[START_SIZE]=c;
	 sprawdz_ramke(tab);
	 czysc_dane(tab);
}

void przod()
{
	send_char3('w');		//przesyla chara do Slave przez uart3
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_7);
	wypelnienie = 1000;
	TIM_SetCompare4(TIM2, wypelnienie);
}
void tyl()
{
	send_char3('s');		//przesyla chara do Slave przez uart3
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_6);
	wypelnienie = 1000;
	TIM_SetCompare4(TIM2, wypelnienie);
}
void lewo()
{
	send_char3('a');		//przesyla chara do Slave przez uart3
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_6);
	wypelnienie = 1000;
	TIM_SetCompare4(TIM2, wypelnienie);
}
void prawo()
{
	send_char3('d');		//przesyla chara do Slave przez uart3
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_7);
	wypelnienie = 1000;
	TIM_SetCompare4(TIM2, wypelnienie);
}
void zwrotLewo()
{
	send_char3('q');		//przesyla chara do Slave przez uart3
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_6);
	wypelnienie = 1000;
	TIM_SetCompare4(TIM2, wypelnienie);
}
void zwrotPrawo()
{
	send_char3('e');		//przesyla chara do Slave przez uart3
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_7);
	wypelnienie = 1000;
	TIM_SetCompare4(TIM2, wypelnienie);
}
void stop()
{
	send_char3(' ');
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
}
void nic()
{
	switch(wypelnienie){
	default:
		TIM_SetCompare4(TIM2, wypelnienie);
		break;
	}
}

void czysc_flagi(){
	FLAG_START=false;
	FLAG_STOP=false;
	FLAG_DATA=false;
}
void czysc_dane(uint8_t buffer[]){
	for(int i=START_SIZE;i<START_SIZE+DATA_SIZE;i++){
		buffer[i]=znak_null;
	}
}
void ramka(uint8_t buffer[]){			//ramka [..x..]
	buffer[0]=znak_pocz;
	for(int i=1;i<START_SIZE;i++){
		buffer[i]=znak_kropka;
	}

	//miejsce na znak z USART2

	for(int i=START_SIZE+DATA_SIZE;i<BUFFER_SIZE-1;i++){
		buffer[i]=znak_kropka;
	}
	buffer[BUFFER_SIZE-1]=znak_kon;
}
void sprawdz_ramke(uint8_t buffer[]){
	if(buffer[0]=='[' && buffer[1]=='.' && buffer[2]=='.'){
		FLAG_START=true;

		if(buffer[START_SIZE]!=znak_null){
			FLAG_DATA=true;

			if(buffer[START_SIZE+DATA_SIZE]=='.' && buffer[START_SIZE+DATA_SIZE+1]=='.' && buffer[BUFFER_SIZE-1]==']'){
				FLAG_STOP=true;
			}
			else{czysc_flagi();}
		}
		else{czysc_flagi();}
	}
	else{czysc_flagi();}
}
void sterowanie(uint8_t buffer[]){

	//FUNKCJA JAZDY Z BLOKOWANIEM

	if(blok_lewo==1){
	/*	switch (buffer[0])
		{
			case 'd':
				prawo();
				break;
			 case ' ':
				stop();
				break;
			 default:
				nic();
				break;
		}*/

		send_char1('L');
	}

	if(blok_prawo==1){
		/*switch (buffer[0])
		{
			case 'a':
				lewo();
				break;
			 case ' ':
				stop();
				break;
			 default:
				nic();
				break;
		}*/
		send_char1('P');send_char1('r');
	}

	if(blok_tyl==1){
		/*switch (buffer[0])
		{
			case 'w':
				przod();
				break;
			 case ' ':
				stop();
				break;
			 default:
				nic();
				break;
		}*/
		send_char1('T');
	}

	if(blok_przod==1){
		/*switch (buffer[0])
		{
			 case 's':
				tyl();
				break;
			 case ' ':
				stop();
				break;
			 default:
				nic();
				break;
		}*/
		send_char1('P');send_char1('z');
	}


	//WLASCIWA FUNKCJA JAZDY BEZ BLOKOWANIA
		if(blok_tyl==0 && blok_lewo==0 && blok_prawo==0 && blok_przod==0 && FLAG_START==true && FLAG_DATA==true && FLAG_STOP==true){

			 switch (buffer[0])
			 {
				 case 'w':
					przod();
					break;
				 case 's':
					tyl();
					break;
				 case 'a':
					lewo();
					break;
				 case 'd':
					prawo();
					break;
				 case 'q':
					zwrotLewo();
					break;
				 case 'e':
					zwrotPrawo();
					break;
				 case ' ':
					stop();
					break;
				 default:
					nic();
					break;
			 }
		}
		delay_ms(50);
}
