#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Program SLAVE LEWY PRZÓD
// Sterowanie W-S-A-D oraz spacja

#define 	START_SIZE			3
#define		STOP_SIZE			3
#define 	DATA_SIZE			1
#define		BUFFER_SIZE			START_SIZE+STOP_SIZE+DATA_SIZE

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

void RCCinit();
void GPIOinit();
void UARTinit();
void TIMinit();
void DMAinit();
void NVICinit();

void SysTick_Handler();
void delay_ms(int time);
void send_char(char c);

void USART3_IRQHandler();

void przod();
void tyl();
void lewo();
void prawo();
void zwrotLewo();
void zwrotPrawo();
void stop();
void nic();

void czysc_flagi();
void czysc_dane(uint8_t buffer[]);
void ramka(uint8_t buffer[]);
void sprawdz_ramke(uint8_t buffer[]);
void sterowanie(uint8_t buffer[]);

int main(void)
{
	SysTick_Config(SystemCoreClock / 1000);	//ustawienie przerwania od timera co 1ms; delay na przyszlosc

	RCCinit();
	GPIOinit();
	DMAinit();
	NVICinit();
	UARTinit();
	TIMinit();

	ramka(tab);

	while(1){
		sterowanie(tab_dma);
	}
}

void RCCinit(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//alternatywne funkcje pinow
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	// zegar do uarta3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}
void GPIOinit()
{
	GPIO_InitTypeDef gpio;

	GPIO_StructInit(&gpio);

	gpio.GPIO_Pin = GPIO_Pin_0; // odpierdalamy
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &gpio);
	GPIO_ResetBits(GPIOC, GPIO_Pin_0);

	gpio.GPIO_Pin = GPIO_Pin_10;			//UART3 - TX
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;		//wyjscie jako funkcja alternatywna w trybie pushpull
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_11;			//UART3 - RX
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//wejscie plywajace (bez rezystora podciagajacego)
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;		//konfiguracja pinow portu C - 6,7 piny kierunkowe mostka
	GPIO_Init(GPIOC, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_3;
	gpio.GPIO_Speed = GPIO_Speed_50MHz; // zwiêkszamy czêstotliwoœæ pracy pinów z domyœlnej 2MHz na 50Mhz
	gpio.GPIO_Mode = GPIO_Mode_AF_PP; //funkcja alternatywna Push-Pull (PWM)
	GPIO_Init(GPIOA, &gpio);

}
void UARTinit()
{
	USART_InitTypeDef uart;

	USART_StructInit(&uart);
	uart.USART_BaudRate = 115200;		//standardowa predkosc transmisji
	uart.USART_Mode = USART_Mode_Rx;	//TYLKO ODBIERAMY - ¿eby DataRegister nie bra³ z wysy³ania, dla UART3
	USART_Init(USART3, &uart);

	USART_Cmd(USART3, ENABLE);
}
void TIMinit()
{
 	TIM_TimeBaseInitTypeDef tim;
 	TIM_OCInitTypeDef  channel;

 	TIM_TimeBaseStructInit(&tim); 				//konfigurujemy TIM4
	tim.TIM_CounterMode = TIM_CounterMode_Up;
	tim.TIM_Prescaler = 64 - 1;
	tim.TIM_Period = 1000 - 1;
	TIM_TimeBaseInit(TIM2, &tim);

	TIM_OCStructInit(&channel);
	channel.TIM_OCMode = TIM_OCMode_PWM1; 		//kana³y maj¹ pracowaæ w trybie PWM
	channel.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC4Init(TIM2, &channel);
	TIM_Cmd(TIM2, ENABLE);
}
void DMAinit(){
	DMA_InitTypeDef dma;

	//KONFIGURACJA DMA1 do odczytu wyniku konwersji
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
	dma.DMA_Mode = DMA_Mode_Circular;								//tryb jednorazowego przesy³u
	dma.DMA_Priority = DMA_Priority_High;						//wysoki priorytet
	DMA_Init(DMA1_Channel3, &dma);
	DMA_Cmd(DMA1_Channel3, ENABLE);
}
void NVICinit()
{
	NVIC_InitTypeDef nvic;

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); 	//do przerwania z odbierania danych uart3
	nvic.NVIC_IRQChannel = USART3_IRQn;				//ustawienie param i priorytetu kontrolera przerwan
	nvic.NVIC_IRQChannelPreemptionPriority = 0x00;	// plytka Slave tylko odbiera dane (poki co)
	nvic.NVIC_IRQChannelSubPriority = 0x00;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
}

void SysTick_Handler()
{
 if (timer_ms) {
 timer_ms--;
 }
}
void delay_ms(int time) //procedura opóŸniaj¹ca
{
 timer_ms = time;
 while (timer_ms) {};
}
void send_char(char c)	//procedura wyslania znaku przez UART3 - ze Slave do Mastera; aktualnie nie uzywana
{
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	//czekamy az bufor nadawczy bedzie wolny
    USART_SendData(USART3, c);
}

void USART3_IRQHandler()		//przerwanie od odbierania danych przez UART3
{
	//PODCZAS CIRCULARA DMA FLAGA OD ODEBRANIA DANYCH WYZEROWANA - rejestr DR ciagle sprawdzany
		 char c = USART_ReceiveData(USART3);
		 tab[START_SIZE]=c;
		 sprawdz_ramke(tab);
		 czysc_dane(tab);
}

void przod()
{
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_7); //ok teraz bez testu nie wiem, który kierunek silnika ustawic, zale¿y od pod³¹czenia do sterownika
	wypelnienie = 1000;				//dlatego kierunki silnika bêdzie trzeba ustawic dopiero w poniedzia³ek ustawiaj¹c 6 lub 7 to samo we wszystkich casach wszystkich sterowników
	TIM_SetCompare4(TIM2, wypelnienie);
}
void tyl()
{
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
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_7);
	wypelnienie = 1000;
	TIM_SetCompare4(TIM2, wypelnienie);
}
void prawo()
{
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_6);
	wypelnienie = 1000;
	TIM_SetCompare4(TIM2, wypelnienie);
}
void zwrotLewo()
{
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
	wypelnienie = 0;
	TIM_SetCompare4(TIM2, wypelnienie);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
}
void nic()
{
	switch(wypelnienie){
	default:
		TIM_SetCompare2(TIM4, wypelnienie);
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
		if(FLAG_START==1 && FLAG_DATA==1 && FLAG_STOP==1){

			// ROZNE POLECENIA 1-znakowe
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
