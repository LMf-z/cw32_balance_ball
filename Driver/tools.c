#include "tools.h"

uint32_t Mean_Value_Filter(uint16_t *values, uint32_t size)     //��ֵ�˲�
{
    uint32_t sum = 0; // u32��ֹ��ͺ���������
    uint16_t max = 0;
    uint16_t min = 0xffff;
    int      i;

    for(i = 0; i < size; i++)
    {
        sum += values[i];
        if(values[i] > max)
        {
            max = values[i];
        }
        if(values[i] < min)
        {
            min = values[i];
        }
    }
    
    // ���������Сֵ
    sum -= max + min;
    // ��ʣ��ľ�ֵ
    sum  = sum / (size - 2);  
    
    return sum;
}
