#include "servo.h"
#include "usart.h"
#include "usart2.h"
#include "string.h"
#include "timer.h"
//���ֶ����˳ʱ������0.6ms ����2.7ms��PWMƵ��50HZ


//��ʼ������ĽǶȣ�1.6ms������
void Servo_Init(void)
{
    u16 compare_value=165;
    TIM3_PWM_Init(2000,720);  //72M/720=100KHz/50=2000  PWMƵ��Ϊ50Hz,ÿ2000Ϊ20ms
    TIM_SetCompare2(TIM3,compare_value);
}



//��������ָ����ƶ����ת
//compare_value :ռ�ձȱȽ�ֵ
void Rotate(void)
{
    static u16 compare_value=165;
    u8 state=0;
    if(Data_Analysis(USART2_RX_STA,(char *)USART2_RX_BUF,"instruction:")!=0)
    {
        if(strstr((char *)USART2_RX_BUF,"stop")!=0)
            state= 1;
        if(strstr((char *)USART2_RX_BUF,"left")!=0)
            state= 2;
        if(strstr((char *)USART2_RX_BUF,"right")!=0)
            state= 3;
        Data_clean();                  //�������
        switch(state)
        {
        case 1:
            break;
        case 2:
            if(compare_value>=90) compare_value-=30;
			else compare_value=60;
            break;
        case 3:
            if(compare_value<=240 ) compare_value+=30;
			else compare_value=270;
            break;
        default:
            break;
        }
        TIM_SetCompare2(TIM3,compare_value);
		
    }

}



