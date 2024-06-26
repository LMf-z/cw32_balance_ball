#include "I2C.h"


static void GPIO_init(uint32_t rcu, GPIO_TypeDef *GPIOx, uint32_t pin){
  CW_SYSCTRL->AHBEN |= rcu;
  GPIO_InitTypeDef init_struct;
  init_struct.Mode  = GPIO_MODE_OUTPUT_OD;
  init_struct.Pins  = pin;
  init_struct.IT    = GPIO_IT_NONE;
  GPIO_Init(GPIOx, &init_struct);
}
#if I2C_SOFT

#if I2C_SPEED >= 400000UL
  #define DELAY()   FirmwareDelay(5)
#else
  #define DELAY()   FirmwareDelay(20)
#endif


// ��ʼ��I2C����������GPIOΪ��©���
void I2C_init(){
  // ��ʵ�֣�CPU����GPIO
  // Ӳʵ�֣�CPU����I2C�ļĴ�����AF���ã���ʼ������
  
  // ��׼ģʽ 100Kbits/s, ����ģʽ 400Kbits/s
  // 100Kbits/s -> 100 000 bit / 1 000 000us -> 1bit/10us -> 5us
  // 400Kbits/s -> 400 000 bit / 1 000 000us -> 4bit/10us -> 1us+
  
  // ����ʱ��1us -> 1bit��Ҫ2us  -> 4bit��Ҫ8us
  
  // SCL
  GPIO_init(I2C_SCL_RCU, I2C_SCL_PORT, I2C_SCL_PIN);
  // SDA
  GPIO_init(I2C_SDA_RCU, I2C_SDA_PORT, I2C_SDA_PIN);
  
}

// ��ʼ�ź�
static void start();
// �豸��ַ���Ĵ�����ַ������
static void send(uint8_t dat);
// ����1���ֽ�
static uint8_t recv();
// ����ack��Ӧ
static void send_ack();
// ����nack��Ӧ
static void send_nack();

// �ȴ���Ӧ������ֵ > 0, �����쳣
static uint8_t wait_ack();
// ֹͣ�ź�
static void stop();


/**********************************************************
 * @brief д���ݵ�I2C�豸ָ���Ĵ���
 * @param  addr �豸��ַ (д��ַ��ǰ7λ)
 * @param  reg  �Ĵ�����ַ
 * @param  dat  �ֽ�����
 * @param  len  ���ݳ���
 * @return 0�ɹ���1 �豸������, 2 �Ĵ���������, 3 ��������Ӧ
 **********************************************************/
uint8_t I2C_write(uint8_t addr, uint8_t reg, uint8_t* dat, uint32_t len){
  
  // ��ʼ
  start();
  
  // �����豸��ַ (д��ַ)
  send((addr << 1) | 0);
  // �ȴ���Ӧ
  if(wait_ack()) return 1;
  
  // ���ͼĴ�����ַ
  send(reg);
  // �ȴ���Ӧ
  if(wait_ack()) return 2;
  
  // ѭ���������е��ֽ�
  for(uint32_t i = 0; i < len; i++){
    // ��������
    send(dat[i]);
    // �ȴ���Ӧ
    if(wait_ack()) return 3;
  }
  
  // ֹͣ
  stop();
  
  return 0;
}

uint8_t I2C_write2(uint8_t addr, uint8_t reg, uint8_t* dat, uint32_t offset, uint32_t len){
  
  // ��ʼ
  start();
  
  // �����豸��ַ (д��ַ)
  send((addr << 1) | 0);
  // �ȴ���Ӧ
  if(wait_ack()) return 1;
  
  // ���ͼĴ�����ַ
  send(reg);
  // �ȴ���Ӧ
  if(wait_ack()) return 2;
  
  // ѭ���������е��ֽ�
//  for(uint32_t i = 0; i < len; i++){
//    // ��������
//    send(dat[i * offset]);
//    // �ȴ���Ӧ
//    if(wait_ack()) return 3;
//  }
  while(len--){
    send(*dat);
    dat += offset;
    // �ȴ���Ӧ
    if(wait_ack()) return 3;
  }
  
  // ֹͣ
  stop();
  
  return 0;
}

/**********************************************************
 * @brief ��I2C�豸ָ���Ĵ�����ȡ����
 * @param  addr �豸��ַ  (д��ַ��ǰ7λ)   0x51
                д��ַ (addr << 1) | 0     0xA2
                ����ַ (addr << 1) | 1     0xA3
 * @param  reg  �Ĵ�����ַ
 * @param  dat  �ֽ�����
 * @param  len  ���ݳ���
 * @return 0�ɹ���1 �豸������, 2 �Ĵ���������, 3 ��������Ӧ
 * @return 
 **********************************************************/
uint8_t I2C_read(uint8_t addr, uint8_t reg, uint8_t* dat, uint32_t len){
  uint8_t addr_write = (addr << 1) | 0;  
  uint8_t addr_read  = (addr << 1) | 1;
  
  // ��ʼ
  start();
  
  // �����豸��ַ (д��ַ)
  send(addr_write);
  // �ȴ���Ӧ
  if(wait_ack()) return 1;
  
  // ���ͼĴ�����ַ
  send(reg);
  // �ȴ���Ӧ
  if(wait_ack()) return 2;
  
  // ��ʼ
  start();
  // �����豸��ַ (����ַ)
  send(addr_read);
  // �ȴ���Ӧ
  if(wait_ack()) return 3;
  
  /******************* ѭ���������� *****************/
  for(uint32_t i = 0; i < len; i++){
    // ����1���ֽ�(8bit)
    dat[i] = recv();
    
    if(i != len - 1){
      // ������ӦACK
      send_ack();
    }else {
      // ���Ϳ���ӦNACK
      send_nack();
    }
  }
  
  /**************************************************/
  
  // ֹͣ
  stop();
  
  return 0;
}


// ��ʼ�ź�
static void start(){
  I2C_SDA_OUT();
  // ����SDA
  SDA(1);
  DELAY();
  
  // ����SCL
  SCL(1);
  DELAY();
  
  // ����SDA
  SDA(0);
  DELAY();
  
  // ����SCL
  SCL(0);
  DELAY();
}

// �豸��ַ���Ĵ�����ַ������
static void send(uint8_t dat){
  I2C_SDA_OUT();
  // 8bit �ȷ���λ
  // 1101 0010
  // 101 0010  ����1λ
  // 01 0010   ����1λ
  for(uint8_t i = 0; i < 8; i++){
    if(dat & 0x80){
      SDA(1); // �ߵ�ƽ
    }else {
      SDA(0); // �͵�ƽ
    }
    DELAY();
    
    SCL(1);
    DELAY();
    SCL(0);
    DELAY();
    
    // ������������1λ
    dat <<= 1;
  }
}

// ����1���ֽ�
static uint8_t recv(){
  // �ͷ�SDA����Ȩ
  I2C_SDA_IN();
  uint8_t cnt = 8; // 1byte = 8bit
  uint8_t byte = 0x00; // ��������������
  
  while(cnt--){   // ����һ��bit�����ո�λ��
    // SCL����
    SCL(0); 
    DELAY(); // �ȴ����豸׼������
    
    SCL(1);  // ����������Ч��
    // 0000 0000 -> 1001 1011
    // 0000 0001    8
    // 0000 0010    7
    // 0000 0100    6
    // 0000 1001    5
    // 0001 0011    4
    // 0010 0110    3
    // 0100 1101    2
    // 1001 1011    1
    
    byte <<= 1;
    
    if(I2C_SDA_STATE()) byte++;
    
    // SCL�ڸߵ�ƽDelayһ�����������һ��ѭ��
    DELAY();
  }
  // SCL�ָ��͵�ƽ, ������
  SCL(0);
  
  return byte;
}

// ����ack��Ӧ
static void send_ack(){
  // ������ȡSDA����Ȩ
  I2C_SDA_OUT();
  // SDA����
  SDA(0);
  DELAY();
  
  // SCL����
  SCL(1);
  DELAY();
  
  // SCL����
  SCL(0);
  DELAY();
  
}
// ����nack��Ӧ
static void send_nack(){
  // ������ȡSDA����Ȩ
  I2C_SDA_OUT();
  // SDA����
  SDA(1);
  DELAY();
  
  // SCL����
  SCL(1);
  DELAY();
  
  // SCL����
  SCL(0);
  DELAY();

}

// �ȴ���Ӧ������ֵ > 0, �����쳣
static uint8_t wait_ack(){
  I2C_SDA_OUT();
  // ����SDA
  SDA(1);
  DELAY();
  
  // ����SCL�� ת������Ȩ��SDAתΪ����ģʽ��
  SCL(1);
  I2C_SDA_IN();
  DELAY();
  
  // �ж�SDA��ƽ״̬
  if(I2C_SDA_STATE() == RESET){
    // ���豸������SDA����Ӧ�ɹ�, ��ȡSDAȨ��
    SCL(0);
    I2C_SDA_OUT();
  } else {
    // ����Ӧ��, ��ȡSDAȨ�ޣ�ֱ�ӽ���
    stop();
    return 1;
  }
  
  return 0;
}
// ֹͣ�ź�
static void stop(){
  I2C_SDA_OUT();
  
  // ����SCL
  SCL(0);
  // ����SDA
  SDA(0);
  DELAY();
  
  // ����SCL
  SCL(1);
  DELAY();
  
  // ����SDA
  SDA(1);
  DELAY();
}


#else

// PB6 - SCL
// PB7 - SDA
void I2C_init(){

  // ��ʵ�֣�CPU����GPIO
  // Ӳʵ�֣�CPU����I2C�ļĴ�����AF���ã���ʼ������
  // ��׼ģʽ 100Kbits/s, ����ģʽ 400Kbits/s
  // 100Kbits/s -> 100 000 bit / 1 000 000us -> 1bit/10us -> 5us
  // 400Kbits/s -> 400 000 bit / 1 000 000us -> 4bit/10us -> 1us+
  
  // GPIO -----------------------------------------
  I2C_SDA_AF();
  I2C_SCL_AF();

  // SCL
  GPIO_init(I2C_SCL_RCU, I2C_SCL_PORT, I2C_SCL_PIN);
  // SDA
  GPIO_init(I2C_SDA_RCU, I2C_SDA_PORT, I2C_SDA_PIN);
  
  // I2C  ------------------------------------------
  RCC_APBPeriphClk_Enable1(RCC_APB1_PERIPH_I2C, ENABLE);
  
	I2C_InitTypeDef I2C_InitStruct;
  /* ���� reset I2C */
  //I2C��ʼ��
  I2C_InitStruct.I2C_BaudEn = ENABLE;
  // 500K=(8000000/(8*(1+1))

  // f_SCL = f_PCLK / 8 / (BRR+1)
  
  // 200K = 6000000 / 8 / (BRR+1)
  // BRR = 6000000 / 8 / 200K - 1
  I2C_InitStruct.I2C_Baud = 0x02;  
  I2C_InitStruct.I2C_FLT = DISABLE;
  I2C_InitStruct.I2C_AA = DISABLE;
  
  I2C_DeInit(); 
  I2C_Master_Init(&I2C_InitStruct);//��ʼ��ģ��
  I2C_Cmd(ENABLE);  //ģ��ʹ��  
  
  /* �յ������Զ��ظ�ACK  whether or not to send an ACK */
//  i2c_ack_config(i2cx, I2C_ACK_ENABLE);

}

/* д���ݵ�I2C�豸ָ���Ĵ��� */
uint8_t I2C_write(uint8_t addr, uint8_t reg, uint8_t* dat, uint32_t len){
  
  uint8_t write_addr = addr << 1;
  
	I2C_MasterWriteEepromData(write_addr, reg, dat, len);//��������len�ֽ�����
  
  return 0;
}


/* ��I2C�豸ָ���Ĵ�����ȡ���� */
uint8_t I2C_read(uint8_t addr, uint8_t reg, uint8_t* dat, uint32_t len){
  uint8_t write_addr = (addr << 1) | 0;
  uint8_t  read_addr = (addr << 1) | 1;
 
	I2C_MasterReadEepomData(read_addr, reg, dat, len);//������ȡ8�ֽ����� 
  return 0;
}

#endif


