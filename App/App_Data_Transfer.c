#include "App.h"
#include "Bluetooth.h"

void App_Data_Transfer_init(){

  // 蓝牙初始化
  Bluetooth_init();
  
}

void App_Data_Transfer_task(){
  // 格式化数据, 距离：xxmm, 电流：xxmA, 电压：xxV
  static char data[50];
  // sprintf(data, "Distance: %dmm, Current: %.2fmA, Voltage: %.2fV\n", distance_mm, Irst, Vrst*0.01);
  // sprintf(data, "Distance: %d, Current: %.2f, Voltage: %.2f\n", current_mm, Irst, Vrst * 0.01);
  sprintf(data, "D: %4d, C: %3.2f, V: %3.2f\n", current_mm, Irst, Vrst * 0.01);
  // 蓝牙发送数据
  Bluetooth_send_string(data);
}