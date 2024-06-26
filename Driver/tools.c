#include "tools.h"

uint32_t Mean_Value_Filter(uint16_t *values, uint32_t size)     //均值滤波
{
    uint32_t sum = 0; // u32防止求和后的数字溢出
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
    
    // 减掉最大最小值
    sum -= max + min;
    // 求剩余的均值
    sum  = sum / (size - 2);  
    
    return sum;
}
