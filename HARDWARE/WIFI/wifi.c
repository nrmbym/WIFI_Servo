#include "wifi.h"
#include "usart.h"
#include "usart2.h"
#include "delay.h"
#include "lcd.h"

char WIFI_Name[20]={"liuxin"};            //WIFI����
char WIFI_password[20]={"xin12345678"};    //WIFI����
char IP_port[50]={"\"192.168.191.1\",10000"};  //IP�Ͷ˿ں�
//char WIFI_Name[20]={"M5 Note"};            //WIFI����
//char WIFI_password[20]={""};    //WIFI����
//char IP_port[50]={"\"10.157.134.222\",10000"};  //IP�Ͷ˿ں�

u8 esp8266_status;//ESP8266״̬���

//��ʼ��ESP8266����
//ģ�鿪��������WIFI������WiFi
//esp8266_status:ģ��״ָ̬ʾ̬
void Wifi_Init()
 {
	u8 i=0;
	char connection_str[50];  //���ڴ����ʽ����ATָ��
	esp8266_status=0;  //����̬
	
	ESP8266_Resatart();  //�����λ
	delay_ms(1000);
	while(sendAT("AT","OK",500) != 0)
    {
		i++;
		LCD_ShowString(20,34,200,12,12,"AT_INIT FAIL...");
        printf("���ڳ�ʼ��ATָ�```%d\r\n",i);
        if(i >= 50)
        {
            printf("\r\n��ʼ��ATָ�ʧ�ܣ�����������Ƭ��\r\n");
            while(1);	//�ȴ����Ź���λ
        }
    }
	LCD_ShowString(20,34,200,12,12,"AT INIT OK!");
	printf("ATָ���ʼ�ųɹ�\r\n");
	esp8266_status=1;   //��ʼ��ATָ��̬
	sendAT("ATE0","OK",2000);
	i=0;
	
	//����ΪAP/STATIONģʽ
	while(sendAT("AT+CWMODE=3","no change",5000)!=0)
	{
		i++;
		LCD_ShowString(20,48,200,12,12,"AP/STATION MODE FAIL...");
		printf("����AP/STATIONģʽʧ��\r\n");
		if(i>20)
		{
			while(1);
		}
	}
	LCD_ShowString(20,48,200,12,12,"AP/STATION MODE OK");
	printf("����AP/STATIONģʽ�ɹ�\r\n");
	//����Ϊ��������
	if(sendAT("AT+CIPMUX=0","OK",5000)!=0)
	{
		LCD_ShowString(20,62,200,12,12,"SET SINGLE CONNECT FAIL");
		printf("����Ϊ��������ʧ��");
	}
	else 
	{
		LCD_ShowString(20,62,200,12,12,"SET SINGLE CONNECT OK");
		printf("����Ϊ�������ӳɹ�\r\n");
	}
	//������������WIFI
	sprintf(connection_str,"AT+CWJAP=\"%s\",\"%s\"",WIFI_Name,WIFI_password);
	if(sendAT(connection_str,"OK",15000)!=0)
	{
		sprintf(connection_str,"CONNECT TO %s FAIL",WIFI_Name);
		LCD_ShowString(20,74,200,12,12,(char *)connection_str);
		printf("����WIFIʧ��\r\n");
	}
	else
	{
		sprintf(connection_str,"CONNECT TO %s OK",WIFI_Name);
		LCD_ShowString(20,74,200,12,12,(char *)connection_str);
		printf("����WIFI�ɹ�\r\n���ӵ�%s\r\n",WIFI_Name);
	}
	esp8266_status=2;  //����WIFI̬
	sprintf(connection_str,"AT+CIPSTART=\"TCP\",%s",IP_port);
	if(sendAT(connection_str,"OK",5000)!=0)
	{
		LCD_ShowString(20,88,200,12,12,"TCP CONNECT FAIL");
		printf("����TCPʧ��\r\n");
	}
	else
	{
		LCD_ShowString(20,88,200,12,12,"TCP CONNECT OK");
		printf("����TCP�ɹ�\r\n");
	}
	esp8266_status=3;  //TCP����̬
	Data_clean();
}



//����һ��ATָ�������Ƿ��յ�ָ����Ӧ��
//sendStr:���͵������ַ���,��sendStr<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//searchStr:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//outTime:�ȴ�ʱ��(��λ:1ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       -1,����ʧ��
u8 sendAT(char *sendStr,char *searchStr,u32 outTime)
{
    u8 ret = 0;
    char * res ;
    delay_ms(300);
	Data_clean();
    if((u32)sendStr < 0xFF)
    {
        while((USART2->SR&0X40)==0);//�ȴ���һ�����ݷ������
        USART2->DR=(u32)sendStr;
    }
    else
    {
        u2_printf(sendStr);//����ATָ��
        u2_printf("\r\n");//���ͻس�����
    }
    if(searchStr && outTime)//��searchStr��outTime��Ϊ0ʱ�ŵȴ�Ӧ��
    {
        while((--outTime)&&(res == 0))//�ȴ�ָ����Ӧ���ʱʱ�䵽��
        {
            res = Data_Analysis(USART2_RX_STA,(char *)USART2_RX_BUF,searchStr);
            delay_ms(1);
        }
        if(outTime == 0) {
            ret = 1;    //��ʱ
        }
        if(res != 0)//res��Ϊ0֤���յ�ָ��Ӧ��
        {
            ret = 0;
        }
    }
    return ret;
}
void ESP8266_Resatart(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//�������ģ�鹩��
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

GPIO_SetBits(GPIOA,GPIO_Pin_5);	
//	PAout(5)=1;
//	delay_ms(5000);
//	PAout(5)=0;	
//	u8 i=0;
//	while(sendAT("AT+RST\r\n","OK",5000)!=0)
//	{
//		i++;
//		printf("ESP8266��λʧ��\r\n");
//	}
//	LCD_ShowString(20,20,200,12,12,"ESP8266 RESET OK!");
//	printf("ESP8266��λ�ɹ�");
	
}
