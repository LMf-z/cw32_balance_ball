#ifndef __UART1_H__
#define __UART1_H__

#include "cw32f003.h"
#include "cw32f003_rcc.h"
#include "cw32f003_gpio.h"
#include "cw32f003_uart.h"
//UARTx
#define  DEBUG_USARTx                   CW_UART1
#define  DEBUG_USART_CLK                RCC_APB2_PERIPH_UART1
#define  DEBUG_USART_APBClkENx          RCC_APBPeriphClk_Enable2
#define  DEBUG_USART_BaudRate           115200
#define  DEBUG_USART_UclkFreq           6000000

//UARTx GPIO 
//TX: PB01, RX: PC04
#define  DEBUG_USART_TX_GPIO_CLK        RCC_AHB_PERIPH_GPIOB
#define  DEBUG_USART_RX_GPIO_CLK        RCC_AHB_PERIPH_GPIOC
#define  DEBUG_USART_TX_GPIO_PORT       CW_GPIOB   
#define  DEBUG_USART_TX_GPIO_PIN        GPIO_PIN_1
#define  DEBUG_USART_RX_GPIO_PORT       CW_GPIOC
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_PIN_4

//GPIO AF
#define  DEBUG_USART_AFTX               PB01_AFx_UART1TXD()
#define  DEBUG_USART_AFRX               PC04_AFx_UART1RXD()

//ÖÐ¶Ï
#define  DEBUG_USART_IRQ                UART1_IRQn


  
void UART1_init();

void UART1_SendString(UART_TypeDef* USARTx, char *String);

void UART1_SendDataArray(UART_TypeDef* USARTx, uint8_t *DataArray, uint32_t len);

#endif