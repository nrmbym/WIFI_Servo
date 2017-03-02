#include "servo.h"
#include "usart.h"
#include "usart2.h"
#include "string.h"
#include "timer.h"
//数字舵机，顺时针最左0.6ms 最右2.7ms，PWM频率50HZ
//最上2.05ms   最下 60ms
u16 compare1_value;
u16 compare2_value;

//初始化舵机的角度，1.45ms的脉宽
void Servo_Init(void)
{
	compare1_value=125;   //通道1  控制仰角
    compare2_value=145;   //通道2  控制水平偏移
    TIM3_PWM_Init(2000,720);  //72M/720=100KHz/50=2000  PWM频率为50Hz,每2000为20ms
    TIM_SetCompare1(TIM3,compare1_value);
    TIM_SetCompare2(TIM3,compare2_value);
}



//解析串口指令，控制舵机旋转
//compare_value :占空比比较值
void Rotate(void)
{

    u8 state=0;
    if(Data_Analysis(USART2_RX_STA,(char *)USART2_RX_BUF,"instruction:")!=0)//解析“instruction”字符串
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
		Data_clean();                  //清除数据
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
				compare2_value=270;     //改变通道二比较值，最大270  最小60
            break;
		case 4:
			compare1_value-=10;
            if(compare1_value<=60 ) 
				compare1_value=60;
            break;	
		case 5:
			compare1_value+=10;
            if(compare1_value>=205 )
				compare1_value=205;	   //改变通道一比较值，最大205  最小60	
				
            break;			
        default:
            break;
        }
		TIM_SetCompare1(TIM3,compare1_value);
        TIM_SetCompare2(TIM3,compare2_value);

    }
	        

}



