#include "App.h"
#include "EC12.h"
#include "Keys.h"
#include "oled.h"
#include "Servo.h"
#include <math.h>
/*****************************************
Menu
--------
1.舵机角度控制
2.激光测距模块
3.上位机数据协议
4.PID联调
--------
*****************************************/

// 菜单列表
char *menu_list[] = {
  "Servo Control",
  "Laser Ranging",
  "PID Tuning   ",
  "ADC Collector",
};
uint8_t menu_list_len = sizeof(menu_list) / sizeof(menu_list[0]);
// #define MENU_MAIN_LEN 4

#define KEY_OK   2
#define KEY_BACK 1

volatile int16_t menu_pos = 2;
volatile int16_t menu_level = 0;
volatile uint8_t node_level_current = 0;

void EC12_on_changed(u8 dir) {

  // 节点处理
  if (menu_level == NODE_SERVO || menu_level == NODE_ADC) {
    float cur_angle = Servo_get_angle();
    if (dir == 1) {
      cur_angle += 5;
    } else {
      cur_angle -= 5;
    }
    Servo_set_angle(cur_angle);
    printf("cur_angle: %.1f\n", cur_angle);
    return;
  }

  printf("menu_level: %d\n", menu_level);
  // 主菜单
  if (dir == 1) {
    menu_pos++;
  } else {
    menu_pos--;
  }

  // menu_pos range: [1, menu_list_len]
  if (menu_pos < 1) {
    menu_pos = menu_list_len;
  } else if (menu_pos > menu_list_len) {
    menu_pos = 1;
  }
  printf("menu_pos: %d\n", menu_pos);
}


void handle_menu_key_down(uint8_t btn) {
  if (btn == KEY_OK) {
    if (menu_level < 10) {  // 如果是主菜单
      printf("Enter pos[%d]\n", menu_pos);

      menu_level = menu_pos * 10;

      switch(menu_level){
        case NODE_SERVO:
          node_level_current = 1;
          break;
        case NODE_LASER:
          node_level_current = 1;
          break;
        case NODE_ADC:
          node_level_current = 1;
          break;
        case NODE_PID:
          node_level_current = 1;
          break;
        default:
          node_level_current = 0;
          break;
      }
      
      printf("Enter to [%d]\n", menu_level);
    }
  } else if (btn == KEY_BACK) {
    menu_level /= 10;
    node_level_current = 0;

    printf("Exti to [%d]\n", menu_level);
  }
}

void Keys_on_key_down(uint8_t btn) {
  printf("Key[%d] down!\n", btn);

  handle_menu_key_down(btn);

}
void Keys_on_key_up(uint8_t btn) {
  //printf("Key[%d] up!\n", btn);
}

void App_OLED_Menu_init() {
  Keys_init();
  // 旋钮编码器
  EC12_init();

  OLED_Init();  // 初始化OLED

#if 0
  OLED_Clear(0);
  OLED_ShowString(10, 0, "Good job!", 8, 1);  // 6*12
  OLED_Refresh();
#endif
}

void App_OLED_Menu_keys() {
  //  printf("scan\n");
  Keys_scan();
}

char OLED_String_Buffer[20];
#define radius 48

void handle_node_level(void) {
  if (menu_level == NODE_SERVO) {
    OLED_Clear(0);
    float cur_angle = Servo_get_angle();
    // Servo Control
    OLED_ShowString(0, 0, "Servo Control", 8, 1);
    // 绘制半圆形在底部
    // OLED_DrawCircle(64, 63, radius);
    float x_offset = radius * cos(PI * cur_angle / 180);
    float y_offset = radius * sin(PI * cur_angle / 180);
    // 绘制舵机旋转指针
//    OLED_DrawLine(64, 64, 0, 3, 1);
    OLED_DrawLine(63 - x_offset, 63 - y_offset, 63, 63, 1);
    // 绘制舵机角度
    sprintf(OLED_String_Buffer, "%.1f", cur_angle);
    OLED_ShowString(52, 63-10, OLED_String_Buffer, 8, 1);
    // OLED_ShowString(128-22, 0, OLED_String_Buffer, 12, 1);
    OLED_Refresh();
  } else if(menu_level == NODE_LASER) {
    OLED_Clear(0);
    // Laser Ranging
    OLED_ShowString(0, 0, "Laser Ranging", 8, 1);

    // Distance
    sprintf(OLED_String_Buffer, "Distance: %dmm", current_mm);
    OLED_ShowString(0, 16, OLED_String_Buffer, 8, 1);

#if 0
    // Draw a line, distance_mm range [0, 300] -> [0, 127], y == 32
    uint8_t offset_y = 28;
    uint16_t current = current_mm;
    uint8_t line_count = current / 300 + 1;
    for (uint8_t i = 0; i < line_count; i++) {
      if (i == line_count - 1) {
        OLED_DrawLine(0, offset_y + i * 6,  current * 127.0 / 300, offset_y + i * 6, 1);
      } else {
        OLED_DrawLine(0, offset_y + i * 6,  127, offset_y + i * 6, 1);
        current -= 300;
      }
    }
#else
    OLED_DrawLine(0, 32, current_mm * 127.0f / 300, 32, 1);
#endif
    OLED_Refresh();

  } else if(menu_level == NODE_ADC) {
    OLED_Clear(0);
    // Adc Collector
    OLED_ShowString(0, 0, "ADC Collector", 8, 1);
    // Voltage
    sprintf(OLED_String_Buffer, "Voltage: %5.2fV", Vrst * 0.0001);
    OLED_ShowString(0, 16, OLED_String_Buffer, 8, 1);
    // Current
    sprintf(OLED_String_Buffer, "Current: %5.2fmA", Irst);
    OLED_ShowString(0, 32, OLED_String_Buffer, 8, 1);
    OLED_Refresh();

  } else if(menu_level == NODE_PID) {
    OLED_Clear(0);
    // PID Tuning
    OLED_ShowString(0, 0, "PID Tuning", 8, 1);
    // 施工中
//    OLED_ShowString(0, 16, "Under Construction", 8, 1);
    // kp
    sprintf(OLED_String_Buffer, "Kp: %5.3f", kp);
    OLED_ShowString(0, 16, OLED_String_Buffer, 8, 1);
    // ki
    sprintf(OLED_String_Buffer, "Ki: %5.3f", ki);
    OLED_ShowString(0, 28, OLED_String_Buffer, 8, 1);
    // kd
    sprintf(OLED_String_Buffer, "Kd: %5.3f", kd);
    OLED_ShowString(0, 40, OLED_String_Buffer, 8, 1);
    // current_mm
    sprintf(OLED_String_Buffer, "Distance: %dmm", current_mm);
    OLED_ShowString(0, 52, OLED_String_Buffer, 8, 1);

    OLED_Refresh();
  }
}


// 默认显示主菜单
static int16_t pre_pos = -1;
static int16_t pre_menu_level = -1;


void App_OLED_Menu_screen() {
  // printf("App_OLED_Menu_screen\n");
  //    Segment_Loop();

  // 末端节点
  if (node_level_current == 1) {
    handle_node_level();
    return;
  }

  // 菜单节点
  //  printf("1_menu_pos: %d, menu_level: %d\n", menu_pos, menu_level);
  if ((menu_level == pre_menu_level) && (menu_pos == pre_pos)) {
    return;
  }

  pre_pos = menu_pos;
  pre_menu_level = menu_level;

//  printf("new_menu pos: %d, level: %d\n", menu_pos, menu_level);

  if (menu_level < 10) {
    OLED_Clear(0);
    // Main Menu
    OLED_ShowString(0, 0, "      Main Menu      ", 8, 1);
    for (uint8_t i = 0; i < menu_list_len; i++) {
      sprintf(OLED_String_Buffer, "%d.%s", i + 1, menu_list[i]);
      #if 0
      // 绘制矩形背景
      if (i == (menu_pos - 1)){
         OLED_DrawRectangle(0, 16 + 12 * i, 127, 11, TRUE, 1);
        // 显示菜单项(反色)
         OLED_ShowString(0, 16 + 12 * i + 2, OLED_String_Buffer, 8, 0);
      }else {
         OLED_ShowString(0, 16 + 12 * i + 2, OLED_String_Buffer, 8, 1);
      }
      #else
      OLED_ShowString(0, 16 + 12 * i + 2, OLED_String_Buffer, 8, i != (menu_pos - 1));
      #endif
    }
    OLED_Refresh();
  }
}