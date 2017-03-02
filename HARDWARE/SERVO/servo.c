#include "servo.h"
#include "usart.h"
#include "usart2.h"
#include "string.h"
#include "timer.h"
//���ֶ����˳ʱ������0.6ms ����2.7ms��PWMƵ��50HZ
//����2.05ms   ���� 60ms
u16 compare1_value;
u16 compare2_value;

//��ʼ������ĽǶȣ�1.45ms������
void Servo_Init(void)
{
	compare1_value=125;   //ͨ��1  ��������
    compare2_value=145;   //ͨ��2  ����ˮƽƫ��
    TIM3_PWM_Init(2000,720);  //72M/720=100KHz/50=2000  PWMƵ��Ϊ50Hz,ÿ2000Ϊ20ms
    TIM_SetCompare1(TIM3,compare1_value);
    TIM_SetCompare2(TIM3,compare2_value);
}



//��������ָ����ƶ����ת
//compare_value :ռ�ձȱȽ�ֵ
void Rotate(void)
{

    u8 state=0;
    if(Data_Analysis(USART2_RX_STA,(char *)USART2_RX_BUF,"instruction:")!=0)//������instruction���ַ���
    {
        if(strstr((char *)USART2_RX_BUF,"stop")!=0)
            state= 1;
        if(strstr((char *)USART2_RX_BUF,"left")!=0)
            state= 2;
        if(strstr((char *)USART2_RX_BUF,"right")!=0)
            state= 3;
        if(strstr((char *)USART2_RX_BUF,"up")!=0)
            state= 4;
        if(strstr((char *)USART2_RX_BUF,"down")!=0)
            state= 5;
		Data_clean();                  //�������
        switch(state)
        {
        case 1:
            break;
        case 2:
			compare2_value-=10;
            if(compare2_value<=60) 
            compare2_value=60;
            break;
        case 3:
			compare2_value+=10;
            if(compare2_value>=270 ) 
				compare2_value=270;     //�ı�ͨ�����Ƚ�ֵ�����270  ��С60
            break;
		case 4:
			compare1_value-=10;
            if(compare1_value<=60 ) 
				compare1_value=60;
            break;	
		case 5:
			compare1_value+=10;
            if(compare1_value>=205 )
				compare1_value=205;	   //�ı�ͨ��һ�Ƚ�ֵ�����205  ��С60	
				
            break;			
        default:
            break;
        }
		TIM_SetCompare1(TIM3,compare1_value);
        TIM_SetCompare2(TIM3,compare2_value);

    }
	        

}



