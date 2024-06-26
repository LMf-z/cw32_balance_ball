#include "ADC_Collector.h"

#include "cw32f003_gpio.h"
#include "cw32f003_adc.h"
#include "cw32f003_rcc.h"
#include "tools.h"


uint16_t Volt_ADC_Value;      //存储ADC采样值
uint16_t Volt_ADC_Values[ADC_SAMPLE_SIZE];      //存储多个电压ADC采样值
uint16_t Curr_ADC_Values[ADC_SAMPLE_SIZE];      //存储多个电流ADC采样值

uint8_t init_complete = 0;
void ADC_Collector_init(void) {

  
  // GPIO ------------------------------------------------------------------------
  GPIO_InitTypeDef    GPIO_Init_Struct;
  __RCC_GPIOB_CLK_ENABLE(); //打开ADC对应引脚时钟

  GPIO_Init_Struct.IT   = GPIO_IT_NONE;
  GPIO_Init_Struct.Mode = GPIO_MODE_ANALOG;            //将GPIO的模式配置成模拟功能
  GPIO_Init_Struct.Pins = GPIO_PIN_6 | GPIO_PIN_5;      // PB01是电压采集引脚
  GPIO_Init(CW_GPIOB, &GPIO_Init_Struct);
  PB06_ANALOG_ENABLE();                    //使能模拟引脚
	PB05_ANALOG_ENABLE(); 

  // ADC ------------------------------------------------------------------------
  __RCC_ADC_CLK_ENABLE();   // 打开ADC时钟
  
  ADC_InitTypeDef     ADC_InitStructure;         //ADC配置结构体
  ADC_SerialChTypeDef ADC_SerialChStructure;     //ADC序列通道结构体
  ADC_StructInit(&ADC_InitStructure);      // ADC默认值初始化
  
//  ADC_InitStructure.ADC_ClkDiv     = ADC_Clk_Div4; //ADC工作时钟配置 PCLK/4 = 6M/4 = 1.5Mhz
  ADC_InitStructure.ADC_ClkDiv     = ADC_Clk_Div128; //ADC工作时钟配置 PCLK/128 = 6M/128 = 46.875Khz

  /*信号电压较低时，可以降低参考电压来提高分辨率。 改变参考电压后，同样二进制表示的电压值就会不一样，
    最大的二进制（全1）表示的就是你的参考电压，在计算实际电压时，就需要将参考电压考虑进去。*/
  ADC_InitStructure.ADC_VrefSel    = ADC_Vref_BGR1p5;     //参考电压设置为1.5V
  ADC_InitStructure.ADC_SampleTime = ADC_SampTime10Clk;   //由于电压信号为慢速信号，ADC采样时间为十个ADC采样周期以确保准确

  ADC_SerialChStructure.ADC_Sqr0Chmux  = ADC_SqrCh3;       //配置ADC序列，PB06是ADC的第3通道
  ADC_SerialChStructure.ADC_Sqr1Chmux  = ADC_SqrCh11;      //配置ADC序列，PB05是ADC的第11通道
  ADC_SerialChStructure.ADC_SqrEns     = ADC_SqrEns02;      //Sqr为序列配置寄存器
  ADC_SerialChStructure.ADC_InitStruct = ADC_InitStructure; //ADC初始化

  ADC_SerialChContinuousModeCfg(&ADC_SerialChStructure);   //ADC序列连续转换模式配置
  ADC_ClearITPendingAll();           //清除ADC所有中断状态
  ADC_Enable();                      // ADC使能
  ADC_SoftwareStartConvCmd(ENABLE);  //ADC转换软件启动命令

  init_complete = 1;
}

void ADC_Collector_get(void) {
  static uint8_t index = 0;

  if(!init_complete) return;
  
  // 读取序列0数值
  ADC_GetSqr0Result(&Volt_ADC_Values[index]);
  
  // 读取序列1数值
  ADC_GetSqr1Result(&Curr_ADC_Values[index]);
  
  index++;
  
  // 到最后一个, 从头开始覆盖
  if (index >= ADC_SAMPLE_SIZE) index = 0;
}



/*******************************
ADC_Value        Vadc 
----------  =  --------
  4095           Vref

R1 = 10K, R2 = 220K

     Vadc       Vrst
I = ------ = ---------
      R1       R1 + R2 



∴ Vrst = Vadc * (R1 + R2) / R1
        = (ADC_Value * Vref / 4095) * (R1 + R2) / R1
        = (ADC_Value * Vref >> 12 ) * (R1 + R2) / R1

∵ Vadc最大值 = 1500mV
∴ Vrst最大值 = 1500mV * (R1 + R2) / R1 
              = 1500mV * 230 / 10 
              = 34500mV 
              = 34.5V

这里由于Vref是1500mV (1.5V) 所以结果单位也是mV
********************************/
float ADC_Collector_get_voltage(void){
//  return (Volt_ADC_Value * ADC_VREF >> 12) * (R1 + R2) / R1;
  
  uint32_t Mean_ADC_Value = Mean_Value_Filter(Volt_ADC_Values, ADC_SAMPLE_SIZE);
  return (Mean_ADC_Value * ADC_VREF >> 12) * (R1 + R2) / R1;
}

/******************************
读取均值滤波后的电流
Vrst(mV) = (Mean_ADC_Value * ADC_VREF >> 12)
R = 100mΩ

Irst(A) = Vrst / R 
        = 1500mV / 100mΩ
        = 1.5V / 0.1Ω 
        = 15A (Max)

Irst(mA) = Vrst * 1000 / R 
******************************/
float ADC_Collector_get_current(void){
//  return (Volt_ADC_Value * ADC_VREF >> 12) * (R1 + R2) / R1;
  
  uint32_t Mean_ADC_Value = Mean_Value_Filter(Curr_ADC_Values, ADC_SAMPLE_SIZE);
  // 计算电压值
  uint32_t Vrst = (Mean_ADC_Value * ADC_VREF >> 12);
  // 转成电流值 mA
  float Irst = Vrst * 1000.0f / ADC_VREF;
  return Irst;
}


