#include "UART1.h"


#if defined ( __GNUC__ ) && !defined (__clang__) 
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif 
  
/**
 * @brief ����GPIO
 * 
 */
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  //UART TX RX ����
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
 * @brief ����UART
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
 * @brief ����NVIC
 * 
 */
static void NVIC_Configuration(void)
{
  //���ȼ��������ȼ�����
  NVIC_SetPriority(DEBUG_USART_IRQ, 0);
  //UARTx�ж�ʹ��
  NVIC_EnableIRQ(DEBUG_USART_IRQ);
}


void UART1_init(){

  //����ʱ��ʹ��
  RCC_AHBPeriphClk_Enable(DEBUG_USART_TX_GPIO_CLK | DEBUG_USART_RX_GPIO_CLK, ENABLE);
  DEBUG_USART_APBClkENx(DEBUG_USART_CLK, ENABLE);
  
  // ����GPIO
  GPIO_Configuration();
  
  // ����UART
  UART_Configuration();
  
  // ����NVIC
  NVIC_Configuration();
    
  // ʹ��UARTx RC�ж�
  USART_ITConfig(DEBUG_USARTx, USART_IT_RC, ENABLE);
}

/**
 * @brief �����ַ���
 * 
 * @param USARTx :USARTx����
 *        ����������:
 *           CW_UART1��CW_UART2
 * @param String :�����͵��ַ���
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
 * @brief �����ֽ�����
 * 
 * @param USARTx :USARTx����
 *        ����������:
 *           CW_UART1��CW_UART2
 * @param String :�����͵��ַ���
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


