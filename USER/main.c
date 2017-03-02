#include "stm32f10x.h"
#include "timer.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "usart2.h"
#include "servo.h"
#include "wifi.h"
#include "lcd.h"

//数字舵机，顺时针最左0.6ms 最右2.7ms，PWM频率50HZ
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
    KEY_Init();
    uart_init(115200);           //初始化串口一
    USART_2_Init(Bound_usart_2);  //初始化串口二
	Servo_Init();
	LCD_Init();      
	Wifi_Init();
    while(1)
    {
		Rotate();
    }
}
