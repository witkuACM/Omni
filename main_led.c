/* Na TIM1 jakby w innej kolejnoœci kolory przypisuje, sprawdz przed oddaniem programu*/

#define true 1
#define false 0
#define buffor_size 434

#include <math.h>
#include "stm32f10x.h"

typedef int bool;

volatile uint16_t stad[buffor_size];
int a = 0;
int pom = 0;

volatile int petla = 0;
volatile int roznica[3] = { 0, 0, 0 };
volatile int wartosc[3] = { 0, 0, 0 };
volatile int umiesc[3];
bool przejscia1[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
bool poszlo = 0;
int dec = 0;
int strona = 0;
int wszystkie = 0;
int wszystkie_pasek = 0;
int numer_diody = 0;
int wyswietlone[5] = { 0, 1, 1, 1, 1 };
char stany[21] = { '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
		'0', '0', '0', '0', '0', '0', '0', '0', '0' };
int diodyplynnie[21] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0 };
int wybrane_3[5] = { 0, 0, 0, 0, 0 };
int wybor_3 = 0;
int wybor_pasek = 0;
int wybrane_pasek[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int zera[3] = { 0, 0, 0 };
int zielony[3] = { 0, 80, 0 };
int czerwony[3] = { 80, 0, 0 };
int ciemnyniebieski[3] = { 0, 0, 80 };
int zolty[3] = { 70, 55, 15 };
int fioletowy[3] = { 56, 4, 64 };
int habrowy[3] = { 4, 8, 80 };
int rozowy[3] = { 80, 4, 40 };
int pomaranczowy[3] = { 80, 8, 0 };
int jasnyniebieski[3] = { 0, 80, 80 };
volatile int plynnie_pasek[3] = { 0, 0, 0 };

void plynnie();
void zmien(int _param1[3], int _param2[3], int _zer, int _ust);
void SysTick_Handler();
void send_char(char c);
int __io_putchar(int c);
void plynnie();
int natab(char _c);
void wlacz();
void u1(int _param[3]);
void u2(int _param[3]);
void u3(int _param[3]);
void u4(int _param[3]);
void u5(int _param[3]);

void pasek(int diod, int _kolor[3]);
void policz_pasek();
int natab_pasek(char _c);

void zmien(int _param1[3], int _param2[3], int _zer, int _ust) {
	przejscia1[_zer] = 0;
	przejscia1[_ust] = 1;
	wartosc[0] = _param2[0];
	wartosc[1] = _param2[1];
	wartosc[2] = _param2[2];
	roznica[0] = _param2[0] - _param1[0];
	roznica[1] = _param2[1] - _param1[1];
	roznica[2] = _param2[2] - _param1[2];
	petla = 1000;
}

void policz_pasek() {
	plynnie_pasek[0] = wartosc[0] - roznica[0] * petla / 1000;
	plynnie_pasek[1] = wartosc[1] - roznica[1] * petla / 1000;
	plynnie_pasek[2] = wartosc[2] - roznica[2] * petla / 1000;
}

void SysTick_Handler() {
	if (petla) {
		if (petla % 50 == 0) {
			if (diodyplynnie[0] == 1) {
				TIM_SetCompare1(TIM4, wartosc[0] - roznica[0] * petla / 1000);
				TIM_SetCompare2(TIM4, wartosc[1] - roznica[1] * petla / 1000);
				TIM_SetCompare3(TIM4, wartosc[2] - roznica[2] * petla / 1000);
			}

			if (diodyplynnie[1] == 1) {
				TIM_SetCompare1(TIM2, wartosc[0] - roznica[0] * petla / 1000);
				TIM_SetCompare2(TIM2, wartosc[1] - roznica[1] * petla / 1000);
				TIM_SetCompare3(TIM2, wartosc[2] - roznica[2] * petla / 1000);
			}

			if (diodyplynnie[2] == 1) {
				TIM_SetCompare1(TIM3, wartosc[0] - roznica[0] * petla / 1000);
				TIM_SetCompare2(TIM3, wartosc[1] - roznica[1] * petla / 1000);
				TIM_SetCompare3(TIM3, wartosc[2] - roznica[2] * petla / 1000);
			}

			if (diodyplynnie[3] == 1) {
				TIM_SetCompare1(TIM1, wartosc[0] - roznica[0] * petla / 1000);
				TIM_SetCompare2(TIM1, wartosc[1] - roznica[1] * petla / 1000);
				TIM_SetCompare3(TIM1, wartosc[2] - roznica[2] * petla / 1000);
			}
			if (diodyplynnie[4] == 1) {
				TIM_SetCompare4(TIM1, wartosc[0] - roznica[0] * petla / 1000);
				TIM_SetCompare4(TIM2, wartosc[1] - roznica[1] * petla / 1000);
				TIM_SetCompare4(TIM4, wartosc[2] - roznica[2] * petla / 1000);
			}
			policz_pasek();
			for (int i = 5; i < 13; i++)
				if (diodyplynnie[i] == 1)
					pasek(i, plynnie_pasek);
		}
		petla--;
	} else if (petla == 0 && poszlo == 1)
		plynnie();
}

void send_char(char c) {
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
		;
	USART_SendData(USART2, c);
}

int __io_putchar(int c) {
	if (c == '\n')
		send_char('\r');
	send_char(c);
	return c;
}

void plynnie() {
	if (poszlo == 0) {
		for (int i = 0; i < 9; i++)
			przejscia1[i] = 0;
		poszlo = 1;
		przejscia1[0] = 1;
	}
	if (przejscia1[0] == 1 && petla == 0) {
		zmien(zielony, jasnyniebieski, 0, 1);
	} else if (przejscia1[1] == 1 && petla == 0) {
		zmien(jasnyniebieski, habrowy, 1, 2);
	} else if (przejscia1[2] == 1 && petla == 0) {
		zmien(habrowy, ciemnyniebieski, 2, 3);
	} else if (przejscia1[3] == 1 && petla == 0) {
		zmien(ciemnyniebieski, fioletowy, 3, 4);
	} else if (przejscia1[4] == 1 && petla == 0) {
		zmien(fioletowy, rozowy, 4, 5);
	} else if (przejscia1[5] == 1 && petla == 0) {
		zmien(rozowy, czerwony, 5, 6);
	} else if (przejscia1[6] == 1 && petla == 0) {
		zmien(czerwony, pomaranczowy, 6, 7);
	} else if (przejscia1[7] == 1 && petla == 0) {
		zmien(pomaranczowy, zolty, 7, 8);
	} else if (przejscia1[8] == 1 && petla == 0) {
		zmien(zolty, zielony, 8, 0);
	}
}

int natab(char _c) {
	if (_c > 48 && _c < 54) {
		return _c - 49;
	}
	return -1;
}

int natab_pasek(char _c) {
	if (_c > 48 && _c < 57) {
		return _c - 49;
	}
	return -1;
}

void pasek(int diod, int _kolor[3]) {
	//DMA_Cmd(DMA1_Channel3, DISABLE);
	diod = diod - 5;
	int pom = _kolor[1] * 3.1875;
	if (pom == 0)
		for (int i = 0; i < 8; i++)
			stad[diod * 24 + i] = 22;
	for (int i = 0; i < 8; i++) {
		if (pom >= pow(2, 7 - i)) {
			pom = pom - pow(2, 7 - i);
			stad[diod * 24 + i] = 52;
		} else if (pom == 0) {
			for (i; i < 8; i++)
				stad[diod * 24 + i] = 22;
			break;
		} else
			stad[diod * 24 + i] = 22;
	}

	pom = _kolor[0] * 3.1875;
	if (pom == 0)
		for (int i = 0; i < 8; i++)
			stad[diod * 24 + 8 + i] = 22;
	for (int i = 0; i < 8; i++) {
		if (pom >= pow(2, 7 - i)) {
			pom = pom - pow(2, 7 - i);
			stad[diod * 24 + 8 + i] = 52;
		} else if (pom == 0) {
			for (i; i < 8; i++)
				stad[diod * 24 + 8 + i] = 22;
			break;
		} else
			stad[diod * 24 + 8 + i] = 22;
	}

	pom = _kolor[2] * 3.1875;
	if (pom == 0)
		for (int i = 0; i < 8; i++)
			stad[diod * 24 + 16 + i] = 22;
	for (int i = 0; i < 8; i++) {
		if (pom >= pow(2, 7 - i)) {
			pom = pom - pow(2, 7 - i);
			stad[diod * 24 + 16 + i] = 52;
		} else if (pom == 0) {
			for (i; i < 8; i++)
				stad[diod * 24 + 16 + i] = 22;
			break;
		} else
			stad[diod * 24 + 16 + i] = 22;
	}
	//DMA_Cmd(DMA1_Channel3, ENABLE);
}

void wlacz() {
	for (int i = 0; i < 5; i++) {
		if (stany[i] == '0') {
			if (i == 0) {
				u1(zera);
				diodyplynnie[0] = 0;
			} else if (i == 1) {
				u2(zera);
				diodyplynnie[1] = 0;
			} else if (i == 2) {
				u3(zera);
				diodyplynnie[2] = 0;
			} else if (i == 3) {
				u4(zera);
				diodyplynnie[3] = 0;
			} else if (i == 4) {
				u5(zera);
				diodyplynnie[4] = 0;
			}
		} else if (stany[i] == '1') {
			if (i == 0)
				u1(zielony);
			else if (i == 1)
				u2(zielony);
			else if (i == 2)
				u3(zielony);
			else if (i == 3)
				u4(zielony);
			else if (i == 4)
				u5(zielony);
		} else if (stany[i] == '2') {
			if (i == 0)
				u1(jasnyniebieski);
			else if (i == 1)
				u2(jasnyniebieski);
			else if (i == 2)
				u3(jasnyniebieski);
			else if (i == 3)
				u4(jasnyniebieski);
			else if (i == 4)
				u5(jasnyniebieski);
		} else if (stany[i] == '3') {
			if (i == 0)
				u1(habrowy);
			else if (i == 1)
				u2(habrowy);
			else if (i == 2)
				u3(habrowy);
			else if (i == 3)
				u4(habrowy);
			else if (i == 4)
				u5(habrowy);
		} else if (stany[i] == '4') {
			if (i == 0)
				u1(ciemnyniebieski);
			else if (i == 1)
				u2(ciemnyniebieski);
			else if (i == 2)
				u3(ciemnyniebieski);
			else if (i == 3)
				u4(ciemnyniebieski);
			else if (i == 4)
				u5(ciemnyniebieski);
		} else if (stany[i] == '5') {
			if (i == 0)
				u1(fioletowy);
			else if (i == 1)
				u2(fioletowy);
			else if (i == 2)
				u3(fioletowy);
			else if (i == 3)
				u4(fioletowy);
			else if (i == 4)
				u5(fioletowy);
		} else if (stany[i] == '6') {
			if (i == 0)
				u1(rozowy);
			else if (i == 1)
				u2(rozowy);
			else if (i == 2)
				u3(rozowy);
			else if (i == 3)
				u4(rozowy);
			else if (i == 4)
				u5(rozowy);
		} else if (stany[i] == '7') {
			if (i == 0)
				u1(czerwony);
			else if (i == 1)
				u2(czerwony);
			else if (i == 2)
				u3(czerwony);
			else if (i == 3)
				u4(czerwony);
			else if (i == 4)
				u5(czerwony);
		} else if (stany[i] == '8') {
			if (i == 0)
				u1(pomaranczowy);
			else if (i == 1)
				u2(pomaranczowy);
			else if (i == 2)
				u3(pomaranczowy);
			else if (i == 3)
				u4(pomaranczowy);
			else if (i == 4)
				u5(pomaranczowy);
		} else if (stany[i] == '9') {
			if (i == 0)
				u1(zolty);
			else if (i == 1)
				u2(zolty);
			else if (i == 2)
				u3(zolty);
			else if (i == 3)
				u4(zolty);
			else if (i == 4)
				u5(zolty);
		} else if (stany[i] == 'k') {
			diodyplynnie[i] = 1;
			poszlo = 0;
			plynnie();
		}
	}
	for (int i = 5; i < 21; i++) {
		if (stany[i] == '0') {
			diodyplynnie[i] = 0;
			pasek(i, zera);
		} else if (stany[i] == '1')
			pasek(i, zielony);
		else if (stany[i] == '2')
			pasek(i, jasnyniebieski);
		else if (stany[i] == '3')
			pasek(i, habrowy);
		else if (stany[i] == '4')
			pasek(i, ciemnyniebieski);
		else if (stany[i] == '5')
			pasek(i, fioletowy);
		else if (stany[i] == '6')
			pasek(i, rozowy);
		else if (stany[i] == '7')
			pasek(i, czerwony);
		else if (stany[i] == '8')
			pasek(i, pomaranczowy);
		else if (stany[i] == '9')
			pasek(i, zolty);
		else if (stany[i] == 'k') {
			diodyplynnie[i] = 1;
			poszlo = 0;
			plynnie();
		}
	}
}

void u1(int _param[3]) {
	TIM_SetCompare1(TIM4, _param[0]);
	TIM_SetCompare2(TIM4, _param[1]);
	TIM_SetCompare3(TIM4, _param[2]);
}

void u2(int _param[3]) {
	TIM_SetCompare1(TIM2, _param[0]);
	TIM_SetCompare2(TIM2, _param[1]);
	TIM_SetCompare3(TIM2, _param[2]);
}

void u3(int _param[3]) {
	TIM_SetCompare1(TIM3, _param[0]);
	TIM_SetCompare2(TIM3, _param[1]);
	TIM_SetCompare3(TIM3, _param[2]);
}

void u4(int _param[3]) {
	TIM_SetCompare1(TIM1, _param[0]);
	TIM_SetCompare2(TIM1, _param[1]);
	TIM_SetCompare3(TIM1, _param[2]);
}

void u5(int _param[3]) {
	TIM_SetCompare4(TIM1, _param[0]);
	TIM_SetCompare4(TIM2, _param[1]);
	TIM_SetCompare4(TIM4, _param[2]);
}

int main(void) {
	GPIO_InitTypeDef gpio;
	TIM_TimeBaseInitTypeDef tim;
	TIM_OCInitTypeDef channel;
	USART_InitTypeDef uart;
	DMA_InitTypeDef dma;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	SysTick_Config(SystemCoreClock / 1000);
	GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2, ENABLE);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_2;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_3;
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_5;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &gpio);
	GPIO_SetBits(GPIOA, GPIO_Pin_5);

	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_10;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_0;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_11;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_9;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio);

	USART_StructInit(&uart);
	uart.USART_BaudRate = 115200;
	USART_Init(USART2, &uart);

	USART_Cmd(USART2, ENABLE);
	TIM_DMACmd(TIM3, TIM_DMA_Update, ENABLE);
	DMA_DeInit(DMA1_Channel3);

	dma.DMA_PeripheralBaseAddr = (uint32_t) &TIM3->CCR4;
	dma.DMA_MemoryBaseAddr = (uint32_t) &stad;
	dma.DMA_DIR = DMA_DIR_PeripheralDST;
	dma.DMA_BufferSize = buffor_size;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_Priority = DMA_Priority_VeryHigh;
	dma.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel3, &dma);
	DMA_Cmd(DMA1_Channel3, ENABLE);

	TIM_TimeBaseStructInit(&tim);
	tim.TIM_CounterMode = TIM_CounterMode_Up;
	tim.TIM_Prescaler = 0;
	tim.TIM_Period = 80 - 1;
	TIM_TimeBaseInit(TIM4, &tim);

	tim.TIM_CounterMode = TIM_CounterMode_Up;
	tim.TIM_Prescaler = 0;
	tim.TIM_Period = 80 - 1;
	TIM_TimeBaseInit(TIM2, &tim);

	tim.TIM_CounterMode = TIM_CounterMode_Up;
	tim.TIM_Prescaler = 0;
	tim.TIM_Period = 80 - 1;
	TIM_TimeBaseInit(TIM3, &tim);

	TIM_CtrlPWMOutputs(TIM1, ENABLE);

	tim.TIM_CounterMode = TIM_CounterMode_Up;
	tim.TIM_Prescaler = 0;
	tim.TIM_Period = 80 - 1;
	TIM_TimeBaseInit(TIM1, &tim);

	TIM_OCStructInit(&channel);
	channel.TIM_OCMode = TIM_OCMode_PWM1;
	channel.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM4, &channel);
	TIM_OC2Init(TIM4, &channel);
	TIM_OC3Init(TIM4, &channel);
	TIM_OC4Init(TIM4, &channel);

	TIM_Cmd(TIM4, ENABLE);

	channel.TIM_OCMode = TIM_OCMode_PWM1;
	channel.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM2, &channel);
	TIM_OC2Init(TIM2, &channel);
	TIM_OC3Init(TIM2, &channel);
	TIM_OC4Init(TIM2, &channel);

	TIM_Cmd(TIM2, ENABLE);

	channel.TIM_OCMode = TIM_OCMode_PWM1;
	channel.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM3, &channel);
	TIM_OC2Init(TIM3, &channel);
	TIM_OC3Init(TIM3, &channel);
	TIM_OC4Init(TIM3, &channel);

	TIM_DMACmd(TIM3, TIM_DMA_Update, ENABLE);
	TIM_Cmd(TIM3, ENABLE);

	channel.TIM_OCMode = TIM_OCMode_PWM1;
	channel.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM1, &channel);
	TIM_OC2Init(TIM1, &channel);
	TIM_OC3Init(TIM1, &channel);
	TIM_OC4Init(TIM1, &channel);

	TIM_Cmd(TIM1, ENABLE);

	while (1) {
		if (wyswietlone[0] == 0) {
			printf("\n\n\nMENU\n"
					"--------------------------\n"
					"1. Ustaw kolor\n");
			wyswietlone[0] = 1;
			while (1) {
				if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE)) {
					char c = USART_ReceiveData(USART2);
					if (c == '1') {
						wyswietlone[1] = 0;
						break;
					}
				}
			}
		} else if (wyswietlone[1] == 0) {
			if (wyswietlone[1] == 0) {
				printf(
						"\n\n\nNumer diody\n"
								"--------------------------\n"
								"1. 1\n2. 2\n3. 3 i 4\n4. 5\n5. 6\n6. Paski led\n7. Wszystkie\ni. Kilka\np. Powrot\n");
				wyswietlone[1] = 1;
			}
			while (1) {
				if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE)) {
					char c = USART_ReceiveData(USART2);
					dec = natab(c);
					if (c == '7') {
						wyswietlone[2] = 0;
						wszystkie = 1;
						break;
					}
					if (c == '6') {
						wyswietlone[3] = 0;
						wszystkie = 0;
						break;
					}
					if (c == 'i') {
						printf(
								"\n--------------------------\nWybierz i potwierdz wciskajac ENTER\n");
						wszystkie = 0;
						wybor_3 = 1;
						while (1) {
							if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE)) {
								char c = USART_ReceiveData(USART2);
								dec = natab(c);
								if (dec >= 0)
									wybrane_3[dec] = 1;
								else if (c == 13)
									break;
							}
						}
						wyswietlone[2] = 0;
						break;
					} else if (dec >= 0) {
						wyswietlone[2] = 0;
						wszystkie = 0;
						break;
					} else if (c == 'p') {
						wszystkie = 0;
						wyswietlone[0] = 0;
						break;
					}
				}
			}
		} else if (wyswietlone[2] == 0) {
			printf(
					"\n\n\nWybierz kolor\n"
							"--------------------------\n"
							"1. Zielony\n2. Jasny niebieski\n3. Habrowy\n4. Ciemny niebieski\n5. Fioletowy\n"
							"6. Rozowy\n7. Czerwony\n8. Pomaranczowy\n9. Zolty\n0. Wylacz\nk. Plynnie\np. Powrot\n");
			wyswietlone[2] = 1;
			while (1) {
				if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE)) {
					char c = USART_ReceiveData(USART2);
					if ((c > 47 && c < 58) || c == 'k') {
						if (wszystkie == 1) {
							for (int i = 0; i < 5; i++)
								stany[i] = c;
						} else if (wybor_3 == 1) {
							for (int j = 0; j < 5; j++)
								if (wybrane_3[j] == 1)
									stany[j] = c;
						} else if (wybor_pasek == 1) {
							for (int j = 5; j < 21; j++)
								if (wybrane_pasek[j - 5] == 1)
									stany[j] = c;
						} else if (wszystkie_pasek == 1 && strona == 0) {
							for (int i = 5; i < 13; i++)
								stany[i] = c;
						} else if (wszystkie_pasek == 1 && strona == 1) {
							for (int i = 13; i < 21; i++)
								stany[i] = c;
						} else if (dec >= 0) {
							stany[dec] = c;
						} else if (numer_diody >= 0)
							stany[5 + strona * 8 + numer_diody] = c;
						wlacz();
					} else if (c == 'p') {
						wybor_3 = 0;
						for (int j = 0; j < 5; j++)
							wybrane_3[j] = 0;
						wybor_pasek = 0;
						for (int j = 0; j < 16; j++)
							wybrane_pasek[j] = 0;
						wyswietlone[1] = 0;
						break;
					}
				}
			}
		} else if (wyswietlone[3] == 0) {
			printf("\n\n\nWybierz pasek\n"
					"--------------------------\n"
					"1. Prawy\n2. Lewy\np. Powrot\n");
			wyswietlone[3] = 1;
			while (1) {
				if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE)) {
					char c = USART_ReceiveData(USART2);
					if (c == '1') {
						strona = 0;
						wyswietlone[4] = 0;
						break;
					}
					if (c == '2') {
						strona = 1;
						wyswietlone[4] = 0;
						break;
					}
					if (c == 'p') {
						wyswietlone[1] = 0;
						break;
					}
				}
			}
		} else if (wyswietlone[4] == 0) {
			printf(
					"\n\n\nNumer diody\n"
							"--------------------------\n"
							"1. 1\n2. 2\n3. 3\n4. 4\n5. 5\n6. 6\n7. 7\n8. 8\n9. Wszystkie\ni. Kilka\np. Powrot\n");
			wyswietlone[4] = 1;
			while (1) {
				if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE)) {
					char c = USART_ReceiveData(USART2);
					numer_diody = natab_pasek(c);
					wszystkie_pasek = 0;
					if (c == '9') {
						wyswietlone[2] = 0;
						wszystkie_pasek = 1;
						break;
					}
					if (c == 'i') {
						printf(
								"\n--------------------------\nWybierz i potwierdz wciskajac ENTER\n");
						wszystkie_pasek = 0;
						wybor_pasek = 1;
						while (1) {
							if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE)) {
								char c = USART_ReceiveData(USART2);
								numer_diody = natab_pasek(c);
								if (numer_diody >= 0)
									wybrane_pasek[numer_diody + strona * 8] = 1;
								else if (c == 13)
									break;
							}
						}
						wyswietlone[2] = 0;
						break;
					}
					if (numer_diody >= 0) {
						wyswietlone[2] = 0;
						break;
					} else if (c == 'p') {
						wyswietlone[3] = 0;
						break;
					}
				}
			}
		}
	}
}
