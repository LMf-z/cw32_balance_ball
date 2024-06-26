#include "UART1.h"


#if defined ( __GNUC__ ) && !defined (__clang__) 
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif 
  
/**
 * @brief 配置GPIO
 * 
 */
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  //UART TX RX 复用
  DEBUG_USART_AFTX;                     
  DEBUG_USART_AFRX;                     

  GPIO_InitStructure.Pins = DEBUG_USART_TX_GPIO_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);
    
  GPIO_InitStructure.Pins = DEBUG_USART_RX_GPIO_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT_PULLUP;
  GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @brief 配置UART
 * 
 */
void UART_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;

  USART_InitStructure.USART_BaudRate = DEBUG_USART_BaudRate;
  USART_InitStructure.USART_Over = USART_Over_16;
  USART_InitStructure.USART_Source = USART_Source_PCLK;
  USART_InitStructure.USART_UclkFreq = DEBUG_USART_UclkFreq;
  USART_InitStructure.USART_StartBit = USART_StartBit_FE;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(DEBUG_USARTx, &USART_InitStructure); 	
}

/**
 * @brief 配置NVIC
 * 
 */
static void NVIC_Configuration(void)
{
  //优先级，无优先级分组
  NVIC_SetPriority(DEBUG_USART_IRQ, 0);
  //UARTx中断使能
  NVIC_EnableIRQ(DEBUG_USART_IRQ);
}


void UART1_init(){

  //外设时钟使能
  RCC_AHBPeriphClk_Enable(DEBUG_USART_TX_GPIO_CLK | DEBUG_USART_RX_GPIO_CLK, ENABLE);
  DEBUG_USART_APBClkENx(DEBUG_USART_CLK, ENABLE);
  
  // 配置GPIO
  GPIO_Configuration();
  
  // 配置UART
  UART_Configuration();
  
  // 配置NVIC
  NVIC_Configuration();
    
  // 使能UARTx RC中断
  USART_ITConfig(DEBUG_USARTx, USART_IT_RC, ENABLE);
}

/**
 * @brief 发送字符串
 * 
 * @param USARTx :USARTx外设
 *        参数可以是:
 *           CW_UART1、CW_UART2
 * @param String :待发送的字符串
 */
void UART1_SendString(UART_TypeDef* USARTx, char *String)
{
  while(*String != '\0')
  {
    USART_SendData_8bit(USARTx, *String);
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    String++;
  }
  while(USART_GetFlagStatus(USARTx, USART_FLAG_TXBUSY) == SET);
}

/**
 * @brief 发送字节数组
 * 
 * @param USARTx :USARTx外设
 *        参数可以是:
 *           CW_UART1、CW_UART2
 * @param String :待发送的字符串
 */
void UART1_SendDataArray(UART_TypeDef* USARTx, uint8_t *DataArray, uint32_t len)
{
  while(len--)
  {
    USART_SendData_8bit(USARTx, *DataArray);
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    
    DataArray++;
  }
  while(USART_GetFlagStatus(USARTx, USART_FLAG_TXBUSY) == SET);
}

/**
 * @brief Retargets the C library printf function to the USART.
 * 
 */
PUTCHAR_PROTOTYPE
{
  USART_SendData_8bit(DEBUG_USARTx, (uint8_t)ch);

  while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);

  return ch;
}


