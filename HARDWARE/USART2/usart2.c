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
������ɱ�־ | ���յ�0XOD  |���ݸ���           *
***********************************************/
void USART_2_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART2_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStrcture;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 ,ENABLE);//ʹ�ܴ���ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);  //GPIOAʱ��ʹ��
	
	USART_DeInit(USART2);                                 //��λ����2
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;            //
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;    //TX�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;            //
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;    //RX��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	USART2_InitStructure.USART_BaudRate=bound;    //����������
	USART2_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;      //Ӳ��������
	USART2_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;                        //����Ϊ�������
	USART2_InitStructure.USART_Parity=USART_Parity_No;        //����żУ��λ
	USART2_InitStructure.USART_StopBits=USART_StopBits_1;       //1ֹͣλ
	USART2_InitStructure.USART_WordLength=USART_WordLength_8b;     //�ֳ�Ϊ8λ���ݸ�ʽ
	
	USART_Init(USART2,&USART2_InitStructure);
	
	
	NVIC_InitStrcture.NVIC_IRQChannel= USART2_IRQn;
	NVIC_InitStrcture.NVIC_IRQChannelPreemptionPriority= 3;//��ռ���ȼ�
	NVIC_InitStrcture.NVIC_IRQChannelSubPriority=3 ; //�����ȼ�
	NVIC_InitStrcture.NVIC_IRQChannelCmd= ENABLE;
	NVIC_Init(&NVIC_InitStrcture);
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);   //�����ж�,�ж�����Ϊ���Ĵ����ǿ�
	USART_Cmd(USART2,ENABLE);                       //����ʹ��
	
	TIM7_Int_Init(99,7199);		//10ms�ж�
	USART2_RX_STA=0;		//����
	TIM_Cmd(TIM7,DISABLE);			//�رն�ʱ��7
}


//����2�����жϷ�����
void USART2_IRQHandler(void)
{
	u8 Res;     //�������շ�������һ���ֽ�
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		Res=USART2->DR;//��DR���ݼĴ�����ֵ����Res
		if((USART2_RX_STA&0X8000)==0)  //����δ���  
		{
			if(USART2_RX_STA<USART2_RX_BUF_LEN)	//�����Խ�������
			{
				TIM_SetCounter(TIM7,0);//���������          				//���������
				if(USART2_RX_STA==0) 				//ʹ�ܶ�ʱ��7���ж� 
				{
					TIM_Cmd(TIM7,ENABLE);//ʹ�ܶ�ʱ��7
				}
				USART2_RX_BUF[USART2_RX_STA++]=Res;	//��¼���յ���ֵ	 
			}else 
			{
				USART2_RX_STA|=1<<15;				//ǿ�Ʊ�ǽ������
			} 
		}
	}	
}


//����2��printf����
void u2_printf(char* fmt,...)
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(USART2,USART2_TX_BUF[j]); 
	} 
}


//�Ӵ��ڽ��յĻ��������в���ָ�����ַ���

char * Data_Analysis(u16 USARTx_RX_STA ,char *buff, char *searchstr)
{
    char * ret=	NULL;
    if((USARTx_RX_STA&(1<<15))!=0)//
    {
        ret=strstr(buff,searchstr);
    }
    return ret;
}

//������ջ��棬���ұ��USART2_RX_STAΪ�ɽ���״̬
void Data_clean(void)
{
    u8 i=0;
    for(i=0; i<=USART2_REC_LEN; i++)
    {
        USART2_RX_BUF[i]=0;
    }
    USART2_RX_STA=0;
}
