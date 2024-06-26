#include "ADC_Collector.h"

#include "cw32f003_gpio.h"
#include "cw32f003_adc.h"
#include "cw32f003_rcc.h"
#include "tools.h"


uint16_t Volt_ADC_Value;      //�洢ADC����ֵ
uint16_t Volt_ADC_Values[ADC_SAMPLE_SIZE];      //�洢�����ѹADC����ֵ
uint16_t Curr_ADC_Values[ADC_SAMPLE_SIZE];      //�洢�������ADC����ֵ

uint8_t init_complete = 0;
void ADC_Collector_init(void) {

  
  // GPIO ------------------------------------------------------------------------
  GPIO_InitTypeDef    GPIO_Init_Struct;
  __RCC_GPIOB_CLK_ENABLE(); //��ADC��Ӧ����ʱ��

  GPIO_Init_Struct.IT   = GPIO_IT_NONE;
  GPIO_Init_Struct.Mode = GPIO_MODE_ANALOG;            //��GPIO��ģʽ���ó�ģ�⹦��
  GPIO_Init_Struct.Pins = GPIO_PIN_6 | GPIO_PIN_5;      // PB01�ǵ�ѹ�ɼ�����
  GPIO_Init(CW_GPIOB, &GPIO_Init_Struct);
  PB06_ANALOG_ENABLE();                    //ʹ��ģ������
	PB05_ANALOG_ENABLE(); 

  // ADC ------------------------------------------------------------------------
  __RCC_ADC_CLK_ENABLE();   // ��ADCʱ��
  
  ADC_InitTypeDef     ADC_InitStructure;         //ADC���ýṹ��
  ADC_SerialChTypeDef ADC_SerialChStructure;     //ADC����ͨ���ṹ��
  ADC_StructInit(&ADC_InitStructure);      // ADCĬ��ֵ��ʼ��
  
//  ADC_InitStructure.ADC_ClkDiv     = ADC_Clk_Div4; //ADC����ʱ������ PCLK/4 = 6M/4 = 1.5Mhz
  ADC_InitStructure.ADC_ClkDiv     = ADC_Clk_Div128; //ADC����ʱ������ PCLK/128 = 6M/128 = 46.875Khz

  /*�źŵ�ѹ�ϵ�ʱ�����Խ��Ͳο���ѹ����߷ֱ��ʡ� �ı�ο���ѹ��ͬ�������Ʊ�ʾ�ĵ�ѹֵ�ͻ᲻һ����
    ���Ķ����ƣ�ȫ1����ʾ�ľ�����Ĳο���ѹ���ڼ���ʵ�ʵ�ѹʱ������Ҫ���ο���ѹ���ǽ�ȥ��*/
  ADC_InitStructure.ADC_VrefSel    = ADC_Vref_BGR1p5;     //�ο���ѹ����Ϊ1.5V
  ADC_InitStructure.ADC_SampleTime = ADC_SampTime10Clk;   //���ڵ�ѹ�ź�Ϊ�����źţ�ADC����ʱ��Ϊʮ��ADC����������ȷ��׼ȷ

  ADC_SerialChStructure.ADC_Sqr0Chmux  = ADC_SqrCh3;       //����ADC���У�PB06��ADC�ĵ�3ͨ��
  ADC_SerialChStructure.ADC_Sqr1Chmux  = ADC_SqrCh11;      //����ADC���У�PB05��ADC�ĵ�11ͨ��
  ADC_SerialChStructure.ADC_SqrEns     = ADC_SqrEns02;      //SqrΪ�������üĴ���
  ADC_SerialChStructure.ADC_InitStruct = ADC_InitStructure; //ADC��ʼ��

  ADC_SerialChContinuousModeCfg(&ADC_SerialChStructure);   //ADC��������ת��ģʽ����
  ADC_ClearITPendingAll();           //���ADC�����ж�״̬
  ADC_Enable();                      // ADCʹ��
  ADC_SoftwareStartConvCmd(ENABLE);  //ADCת�������������

  init_complete = 1;
}

void ADC_Collector_get(void) {
  static uint8_t index = 0;

  if(!init_complete) return;
  
  // ��ȡ����0��ֵ
  ADC_GetSqr0Result(&Volt_ADC_Values[index]);
  
  // ��ȡ����1��ֵ
  ADC_GetSqr1Result(&Curr_ADC_Values[index]);
  
  index++;
  
  // �����һ��, ��ͷ��ʼ����
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



�� Vrst = Vadc * (R1 + R2) / R1
        = (ADC_Value * Vref / 4095) * (R1 + R2) / R1
        = (ADC_Value * Vref >> 12 ) * (R1 + R2) / R1

�� Vadc���ֵ = 1500mV
�� Vrst���ֵ = 1500mV * (R1 + R2) / R1 
              = 1500mV * 230 / 10 
              = 34500mV 
              = 34.5V

��������Vref��1500mV (1.5V) ���Խ����λҲ��mV
********************************/
float ADC_Collector_get_voltage(void){
//  return (Volt_ADC_Value * ADC_VREF >> 12) * (R1 + R2) / R1;
  
  uint32_t Mean_ADC_Value = Mean_Value_Filter(Volt_ADC_Values, ADC_SAMPLE_SIZE);
  return (Mean_ADC_Value * ADC_VREF >> 12) * (R1 + R2) / R1;
}

/******************************
��ȡ��ֵ�˲���ĵ���
Vrst(mV) = (Mean_ADC_Value * ADC_VREF >> 12)
R = 100m��

Irst(A) = Vrst / R 
        = 1500mV / 100m��
        = 1.5V / 0.1�� 
        = 15A (Max)

Irst(mA) = Vrst * 1000 / R 
******************************/
float ADC_Collector_get_current(void){
//  return (Volt_ADC_Value * ADC_VREF >> 12) * (R1 + R2) / R1;
  
  uint32_t Mean_ADC_Value = Mean_Value_Filter(Curr_ADC_Values, ADC_SAMPLE_SIZE);
  // �����ѹֵ
  uint32_t Vrst = (Mean_ADC_Value * ADC_VREF >> 12);
  // ת�ɵ���ֵ mA
  float Irst = Vrst * 1000.0f / ADC_VREF;
  return Irst;
}


