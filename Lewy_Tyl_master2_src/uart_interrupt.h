/*
 * uart_interrupt.h
 *
 *  Created on: 13 lis 2017
 *      Author: wiciu
 */

#ifndef UART_INTERRUPT_H_
#define UART_INTERRUPT_H_

extern void send_char1(char c);
extern void send_char3(char c);
extern void USART1_IRQHandler();

#endif /* UART_INTERRUPT_H_ */
