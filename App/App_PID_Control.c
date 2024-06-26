#include "App.h"
#include "Servo.h"
#include "VL53L0X.h"
#include "tools.h"

// pid参数 （较好）
// float kp = 0.1;
// float kd = 0.15;
// float ki = 0.002;

// 待修正
float kp = 0.122;  // 0.135 error
float kd = 0.231;
float ki = 0.004;

// 当前舵机角度
float init_angle = 130;
// 当前距离
int current_mm = 150;
// 目标距离
int target_mm = 150;  // mm

void App_PID_Control_init() {
  // 舵机初始化
  Servo_init();

#if LASER_PID_SWITCH
  // 激光测距
  while (!VL53L0X_init(1)) {
    SysTickDelay(200);
    printf("init error\n");
  }
  startContinuous(20);
#endif

  // printf("Btim2   \t\t init complete!\n");
  // printf("ADC     \t\t init complete!\n");
  // printf("Servo   \t\t init complete!\n");
  // printf("VL53L0X \t\t init complete!\n");
}

#define SAMPLE_SIZE 5
uint16_t Distance_Values[SAMPLE_SIZE];  // 存储多个距离值
uint16_t Distance_Values_Index = 0;

void App_PID_Control_task() {
  
  static int cte = 0, last_cte = 0, integral_cte = 0;  // Cross Track Error
  static float p = 0, i = 0, d = 0;

#if LASER_PID_SWITCH
  if (menu_level != NODE_LASER && menu_level != NODE_PID){
    return;
  }
  
  uint16_t distance_mm = readRangeContinuousMillimeters();
  distance_mm = CLIP_VALUE(distance_mm, 0, 2000);
  // 数据存到Distance_Values数组中
  Distance_Values[Distance_Values_Index] = distance_mm;
  Distance_Values_Index = (Distance_Values_Index + 1) % SAMPLE_SIZE;
  // 均值滤波
  current_mm = Mean_Value_Filter(Distance_Values, SAMPLE_SIZE);

  printf("current: %d mm\n", current_mm);
  

#if 1
  
  if (menu_level != NODE_PID){
    return;
  }
  
  // Kp, Kd, Ki
  // 计算本次误差（当前位置和目标的差值）Proportional
  cte = current_mm - target_mm;  // 负值：小球太近，正值：小球太远 100mm -> 150mm , -50mm
  p = kp * cte;

  // 计算前两次误差的变化 Derivative
  d = kd * (cte - last_cte);
  last_cte = cte;

  // 计算历史误差的和 （消除稳态误差） Integral
  integral_cte += cte;
  i = ki * integral_cte;

  // 不能让累积误差太多
  integral_cte = CLIP_VALUE(integral_cte, -10000, 10000);

  float pid = p + d + i;
  // 将运动角度限定到[60, 165]
  float angle = init_angle + pid;
  angle = CLIP_VALUE(angle, 60, 165);

  Servo_set_angle(angle);
#endif

#endif
}