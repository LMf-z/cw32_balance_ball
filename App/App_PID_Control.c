#include "App.h"
#include "Servo.h"
#include "VL53L0X.h"
#include "tools.h"

// pid���� ���Ϻã�
// float kp = 0.1;
// float kd = 0.15;
// float ki = 0.002;

// ������
float kp = 0.122;  // 0.135 error
float kd = 0.231;
float ki = 0.004;

// ��ǰ����Ƕ�
float init_angle = 130;
// ��ǰ����
int current_mm = 150;
// Ŀ�����
int target_mm = 150;  // mm

void App_PID_Control_init() {
  // �����ʼ��
  Servo_init();

#if LASER_PID_SWITCH
  // ������
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
uint16_t Distance_Values[SAMPLE_SIZE];  // �洢�������ֵ
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
  // ���ݴ浽Distance_Values������
  Distance_Values[Distance_Values_Index] = distance_mm;
  Distance_Values_Index = (Distance_Values_Index + 1) % SAMPLE_SIZE;
  // ��ֵ�˲�
  current_mm = Mean_Value_Filter(Distance_Values, SAMPLE_SIZE);

  printf("current: %d mm\n", current_mm);
  

#if 1
  
  if (menu_level != NODE_PID){
    return;
  }
  
  // Kp, Kd, Ki
  // ���㱾������ǰλ�ú�Ŀ��Ĳ�ֵ��Proportional
  cte = current_mm - target_mm;  // ��ֵ��С��̫������ֵ��С��̫Զ 100mm -> 150mm , -50mm
  p = kp * cte;

  // ����ǰ�������ı仯 Derivative
  d = kd * (cte - last_cte);
  last_cte = cte;

  // ������ʷ���ĺ� ��������̬�� Integral
  integral_cte += cte;
  i = ki * integral_cte;

  // �������ۻ����̫��
  integral_cte = CLIP_VALUE(integral_cte, -10000, 10000);

  float pid = p + d + i;
  // ���˶��Ƕ��޶���[60, 165]
  float angle = init_angle + pid;
  angle = CLIP_VALUE(angle, 60, 165);

  Servo_set_angle(angle);
#endif

#endif
}