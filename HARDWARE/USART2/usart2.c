#include "usart2.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "timer.h"


u8 USART2_RX_BUF[USART2_RX_BUF_LEN];
u8 USART2_TX_BUF[USART2_TX_BUF_LEN];
u16 USART2_RX_STA=0;
/***********************************************
15           |  14         | 13~0              *
接收完成标志 | 接收到0XOD  |数据个数           *
***********************************************/
void USART_2_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART2_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStrcture;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 ,ENABLE);//使能串口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);  //GPIOA时钟使能
	
	USART_DeInit(USART2);                                 //复位串口2
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;            //
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;    //TX推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;            //
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;    //RX浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	USART2_InitStructure.USART_BaudRate=bound;    //波特率设置
	USART2_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;      //硬件流控制
	USART2_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;                        //设置为输入输出
	USART2_InitStructure.USART_Parity=USART_Parity_No;        //无奇偶校验位
	USART2_InitStructure.USART_StopBits=USART_StopBits_1;       //1停止位
	USART2_InitStructure.USART_WordLength=USART_WordLength_8b;     //字长为8位数据格式
	
	USART_Init(USART2,&USART2_InitStructure);
	
	
	NVIC_InitStrcture.NVIC_IRQChannel= USART2_IRQn;
	NVIC_InitStrcture.NVIC_IRQChannelPreemptionPriority= 3;//抢占优先级
	NVIC_InitStrcture.NVIC_IRQChannelSubPriority=3 ; //子优先级
	NVIC_InitStrcture.NVIC_IRQChannelCmd= ENABLE;
	NVIC_Init(&NVIC_InitStrcture);
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);   //开启中断,中断类型为读寄存器非空
	USART_Cmd(USART2,ENABLE);                       //串口使能
	
	TIM7_Int_Init(99,7199);		//10ms中断
	USART2_RX_STA=0;		//清零
	TIM_Cmd(TIM7,DISABLE);			//关闭定时器7
}


//串口2接收中断服务函数
void USART2_IRQHandler(void)
{
	u8 Res;     //用来接收返回来的一个字节
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		Res=USART2->DR;//将DR数据寄存器的值赋给Res
		if((USART2_RX_STA&0X8000)==0)  //接收未完成  
		{
			if(USART2_RX_STA<USART2_RX_BUF_LEN)	//还可以接收数据
			{
				TIM_SetCounter(TIM7,0);//计数器清空          				//计数器清空
				if(USART2_RX_STA==0) 				//使能定时器7的中断 
				{
					TIM_Cmd(TIM7,ENABLE);//使能定时器7
				}
				USART2_RX_BUF[USART2_RX_STA++]=Res;	//记录接收到的值	 
			}else 
			{
				USART2_RX_STA|=1<<15;				//强制标记接收完成
			} 
		}
	}	
}


//串口2，printf函数
void u2_printf(char* fmt,...)
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(USART2,USART2_TX_BUF[j]); 
	} 
}


//从串口接收的缓冲数组中查找指定的字符串

char * Data_Analysis(u16 USARTx_RX_STA ,char *buff, char *searchstr)
{
    char * ret=	NULL;
    if((USARTx_RX_STA&(1<<15))!=0)//
    {
        ret=strstr(buff,searchstr);
    }
    return ret;
}

//清除接收缓存，并且标记USART2_RX_STA为可接收状态
void Data_clean(void)
{
    u8 i=0;
    for(i=0; i<=USART2_REC_LEN; i++)
    {
        USART2_RX_BUF[i]=0;
    }
    USART2_RX_STA=0;
}
