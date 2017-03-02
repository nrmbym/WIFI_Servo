#include "stm32f10x.h"
#include "timer.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "usart2.h"
#include "servo.h"
#include "wifi.h"
#include "lcd.h"

//���ֶ����˳ʱ������0.6ms ����2.7ms��PWMƵ��50HZ
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
    KEY_Init();
    uart_init(115200);           //��ʼ������һ
    USART_2_Init(Bound_usart_2);  //��ʼ�����ڶ�
	Servo_Init();
	LCD_Init();      
	Wifi_Init();
    while(1)
    {
		Rotate();
    }
}
