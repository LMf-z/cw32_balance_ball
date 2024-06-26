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


// 初始化I2C的两个总线GPIO为开漏输出
void I2C_init(){
  // 软实现：CPU操作GPIO
  // 硬实现：CPU操作I2C的寄存器，AF复用，初始化外设
  
  // 标准模式 100Kbits/s, 快速模式 400Kbits/s
  // 100Kbits/s -> 100 000 bit / 1 000 000us -> 1bit/10us -> 5us
  // 400Kbits/s -> 400 000 bit / 1 000 000us -> 4bit/10us -> 1us+
  
  // 休眠时间1us -> 1bit需要2us  -> 4bit需要8us
  
  // SCL
  GPIO_init(I2C_SCL_RCU, I2C_SCL_PORT, I2C_SCL_PIN);
  // SDA
  GPIO_init(I2C_SDA_RCU, I2C_SDA_PORT, I2C_SDA_PIN);
  
}

// 开始信号
static void start();
// 设备地址，寄存器地址，数据
static void send(uint8_t dat);
// 接收1个字节
static uint8_t recv();
// 发送ack响应
static void send_ack();
// 发送nack响应
static void send_nack();

// 等待响应，返回值 > 0, 出现异常
static uint8_t wait_ack();
// 停止信号
static void stop();


/**********************************************************
 * @brief 写数据到I2C设备指定寄存器
 * @param  addr 设备地址 (写地址的前7位)
 * @param  reg  寄存器地址
 * @param  dat  字节数组
 * @param  len  数据长度
 * @return 0成功，1 设备不存在, 2 寄存器不存在, 3 数据无响应
 **********************************************************/
uint8_t I2C_write(uint8_t addr, uint8_t reg, uint8_t* dat, uint32_t len){
  
  // 开始
  start();
  
  // 发送设备地址 (写地址)
  send((addr << 1) | 0);
  // 等待响应
  if(wait_ack()) return 1;
  
  // 发送寄存器地址
  send(reg);
  // 等待响应
  if(wait_ack()) return 2;
  
  // 循环发送所有的字节
  for(uint32_t i = 0; i < len; i++){
    // 发送数据
    send(dat[i]);
    // 等待响应
    if(wait_ack()) return 3;
  }
  
  // 停止
  stop();
  
  return 0;
}

uint8_t I2C_write2(uint8_t addr, uint8_t reg, uint8_t* dat, uint32_t offset, uint32_t len){
  
  // 开始
  start();
  
  // 发送设备地址 (写地址)
  send((addr << 1) | 0);
  // 等待响应
  if(wait_ack()) return 1;
  
  // 发送寄存器地址
  send(reg);
  // 等待响应
  if(wait_ack()) return 2;
  
  // 循环发送所有的字节
//  for(uint32_t i = 0; i < len; i++){
//    // 发送数据
//    send(dat[i * offset]);
//    // 等待响应
//    if(wait_ack()) return 3;
//  }
  while(len--){
    send(*dat);
    dat += offset;
    // 等待响应
    if(wait_ack()) return 3;
  }
  
  // 停止
  stop();
  
  return 0;
}

/**********************************************************
 * @brief 从I2C设备指定寄存器读取数据
 * @param  addr 设备地址  (写地址的前7位)   0x51
                写地址 (addr << 1) | 0     0xA2
                读地址 (addr << 1) | 1     0xA3
 * @param  reg  寄存器地址
 * @param  dat  字节数组
 * @param  len  数据长度
 * @return 0成功，1 设备不存在, 2 寄存器不存在, 3 数据无响应
 * @return 
 **********************************************************/
uint8_t I2C_read(uint8_t addr, uint8_t reg, uint8_t* dat, uint32_t len){
  uint8_t addr_write = (addr << 1) | 0;  
  uint8_t addr_read  = (addr << 1) | 1;
  
  // 开始
  start();
  
  // 发送设备地址 (写地址)
  send(addr_write);
  // 等待响应
  if(wait_ack()) return 1;
  
  // 发送寄存器地址
  send(reg);
  // 等待响应
  if(wait_ack()) return 2;
  
  // 开始
  start();
  // 发送设备地址 (读地址)
  send(addr_read);
  // 等待响应
  if(wait_ack()) return 3;
  
  /******************* 循环接收数据 *****************/
  for(uint32_t i = 0; i < len; i++){
    // 接收1个字节(8bit)
    dat[i] = recv();
    
    if(i != len - 1){
      // 发送响应ACK
      send_ack();
    }else {
      // 发送空响应NACK
      send_nack();
    }
  }
  
  /**************************************************/
  
  // 停止
  stop();
  
  return 0;
}


// 开始信号
static void start(){
  I2C_SDA_OUT();
  // 拉高SDA
  SDA(1);
  DELAY();
  
  // 拉高SCL
  SCL(1);
  DELAY();
  
  // 拉低SDA
  SDA(0);
  DELAY();
  
  // 拉低SCL
  SCL(0);
  DELAY();
}

// 设备地址，寄存器地址，数据
static void send(uint8_t dat){
  I2C_SDA_OUT();
  // 8bit 先发高位
  // 1101 0010
  // 101 0010  左移1位
  // 01 0010   左移1位
  for(uint8_t i = 0; i < 8; i++){
    if(dat & 0x80){
      SDA(1); // 高电平
    }else {
      SDA(0); // 低电平
    }
    DELAY();
    
    SCL(1);
    DELAY();
    SCL(0);
    DELAY();
    
    // 所有内容左移1位
    dat <<= 1;
  }
}

// 接收1个字节
static uint8_t recv(){
  // 释放SDA控制权
  I2C_SDA_IN();
  uint8_t cnt = 8; // 1byte = 8bit
  uint8_t byte = 0x00; // 空容器接收数据
  
  while(cnt--){   // 接收一个bit（先收高位）
    // SCL拉低
    SCL(0); 
    DELAY(); // 等待从设备准备数据
    
    SCL(1);  // 设置数据有效性
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
    
    // SCL在高电平Delay一会儿，进入下一个循环
    DELAY();
  }
  // SCL恢复低电平, 不能少
  SCL(0);
  
  return byte;
}

// 发送ack响应
static void send_ack(){
  // 主机获取SDA控制权
  I2C_SDA_OUT();
  // SDA拉低
  SDA(0);
  DELAY();
  
  // SCL拉高
  SCL(1);
  DELAY();
  
  // SCL拉低
  SCL(0);
  DELAY();
  
}
// 发送nack响应
static void send_nack(){
  // 主机获取SDA控制权
  I2C_SDA_OUT();
  // SDA拉高
  SDA(1);
  DELAY();
  
  // SCL拉高
  SCL(1);
  DELAY();
  
  // SCL拉低
  SCL(0);
  DELAY();

}

// 等待响应，返回值 > 0, 出现异常
static uint8_t wait_ack(){
  I2C_SDA_OUT();
  // 拉高SDA
  SDA(1);
  DELAY();
  
  // 拉高SCL， 转交控制权（SDA转为输入模式）
  SCL(1);
  I2C_SDA_IN();
  DELAY();
  
  // 判定SDA电平状态
  if(I2C_SDA_STATE() == RESET){
    // 从设备拉低了SDA，响应成功, 获取SDA权限
    SCL(0);
    I2C_SDA_OUT();
  } else {
    // 无人应答, 获取SDA权限，直接结束
    stop();
    return 1;
  }
  
  return 0;
}
// 停止信号
static void stop(){
  I2C_SDA_OUT();
  
  // 拉低SCL
  SCL(0);
  // 拉低SDA
  SDA(0);
  DELAY();
  
  // 拉高SCL
  SCL(1);
  DELAY();
  
  // 拉高SDA
  SDA(1);
  DELAY();
}


#else

// PB6 - SCL
// PB7 - SDA
void I2C_init(){

  // 软实现：CPU操作GPIO
  // 硬实现：CPU操作I2C的寄存器，AF复用，初始化外设
  // 标准模式 100Kbits/s, 快速模式 400Kbits/s
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
  /* 重置 reset I2C */
  //I2C初始化
  I2C_InitStruct.I2C_BaudEn = ENABLE;
  // 500K=(8000000/(8*(1+1))

  // f_SCL = f_PCLK / 8 / (BRR+1)
  
  // 200K = 6000000 / 8 / (BRR+1)
  // BRR = 6000000 / 8 / 200K - 1
  I2C_InitStruct.I2C_Baud = 0x02;  
  I2C_InitStruct.I2C_FLT = DISABLE;
  I2C_InitStruct.I2C_AA = DISABLE;
  
  I2C_DeInit(); 
  I2C_Master_Init(&I2C_InitStruct);//初始化模块
  I2C_Cmd(ENABLE);  //模块使能  
  
  /* 收到数据自动回复ACK  whether or not to send an ACK */
//  i2c_ack_config(i2cx, I2C_ACK_ENABLE);

}

/* 写数据到I2C设备指定寄存器 */
uint8_t I2C_write(uint8_t addr, uint8_t reg, uint8_t* dat, uint32_t len){
  
  uint8_t write_addr = addr << 1;
  
	I2C_MasterWriteEepromData(write_addr, reg, dat, len);//主机发送len字节数据
  
  return 0;
}


/* 从I2C设备指定寄存器读取数据 */
uint8_t I2C_read(uint8_t addr, uint8_t reg, uint8_t* dat, uint32_t len){
  uint8_t write_addr = (addr << 1) | 0;
  uint8_t  read_addr = (addr << 1) | 1;
 
	I2C_MasterReadEepomData(read_addr, reg, dat, len);//主机读取8字节数据 
  return 0;
}

#endif


