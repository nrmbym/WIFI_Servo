#include "wifi.h"
#include "usart.h"
#include "usart2.h"
#include "delay.h"
#include "lcd.h"

char WIFI_Name[20]={"liuxin"};            //WIFI名字
char WIFI_password[20]={"xin12345678"};    //WIFI密码
char IP_port[50]={"\"192.168.191.1\",10000"};  //IP和端口号
//char WIFI_Name[20]={"M5 Note"};            //WIFI名字
//char WIFI_password[20]={""};    //WIFI密码
//char IP_port[50]={"\"10.157.134.222\",10000"};  //IP和端口号

u8 esp8266_status;//ESP8266状态标记

//初始化ESP8266函数
//模块开机，配置WIFI，连接WiFi
//esp8266_status:模块状态指示态
void Wifi_Init()
 {
	u8 i=0;
	char connection_str[50];  //用于储存格式化的AT指令
	esp8266_status=0;  //开机态
	
	ESP8266_Resatart();  //软件复位
	delay_ms(1000);
	while(sendAT("AT","OK",500) != 0)
    {
		i++;
		LCD_ShowString(20,34,200,12,12,"AT_INIT FAIL...");
        printf("正在初始化AT指令集```%d\r\n",i);
        if(i >= 50)
        {
            printf("\r\n初始化AT指令集失败，正在重启单片机\r\n");
            while(1);	//等待看门狗复位
        }
    }
	LCD_ShowString(20,34,200,12,12,"AT INIT OK!");
	printf("AT指令初始号成功\r\n");
	esp8266_status=1;   //初始化AT指令态
	sendAT("ATE0","OK",2000);
	i=0;
	
	//设置为AP/STATION模式
	while(sendAT("AT+CWMODE=3","no change",5000)!=0)
	{
		i++;
		LCD_ShowString(20,48,200,12,12,"AP/STATION MODE FAIL...");
		printf("配置AP/STATION模式失败\r\n");
		if(i>20)
		{
			while(1);
		}
	}
	LCD_ShowString(20,48,200,12,12,"AP/STATION MODE OK");
	printf("配置AP/STATION模式成功\r\n");
	//设置为单线连接
	if(sendAT("AT+CIPMUX=0","OK",5000)!=0)
	{
		LCD_ShowString(20,62,200,12,12,"SET SINGLE CONNECT FAIL");
		printf("设置为单线连接失败");
	}
	else 
	{
		LCD_ShowString(20,62,200,12,12,"SET SINGLE CONNECT OK");
		printf("设置为单线连接成功\r\n");
	}
	//输入密码连接WIFI
	sprintf(connection_str,"AT+CWJAP=\"%s\",\"%s\"",WIFI_Name,WIFI_password);
	if(sendAT(connection_str,"OK",15000)!=0)
	{
		sprintf(connection_str,"CONNECT TO %s FAIL",WIFI_Name);
		LCD_ShowString(20,74,200,12,12,(char *)connection_str);
		printf("连接WIFI失败\r\n");
	}
	else
	{
		sprintf(connection_str,"CONNECT TO %s OK",WIFI_Name);
		LCD_ShowString(20,74,200,12,12,(char *)connection_str);
		printf("连接WIFI成功\r\n连接到%s\r\n",WIFI_Name);
	}
	esp8266_status=2;  //连接WIFI态
	sprintf(connection_str,"AT+CIPSTART=\"TCP\",%s",IP_port);
	if(sendAT(connection_str,"OK",5000)!=0)
	{
		LCD_ShowString(20,88,200,12,12,"TCP CONNECT FAIL");
		printf("连接TCP失败\r\n");
	}
	else
	{
		LCD_ShowString(20,88,200,12,12,"TCP CONNECT OK");
		printf("连接TCP成功\r\n");
	}
	esp8266_status=3;  //TCP连接态
	Data_clean();
}



//发送一条AT指令，并检测是否收到指定的应答
//sendStr:发送的命令字符串,当sendStr<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//searchStr:期待的应答结果,如果为空,则表示不需要等待应答
//outTime:等待时间(单位:1ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       -1,发送失败
u8 sendAT(char *sendStr,char *searchStr,u32 outTime)
{
    u8 ret = 0;
    char * res ;
    delay_ms(300);
	Data_clean();
    if((u32)sendStr < 0xFF)
    {
        while((USART2->SR&0X40)==0);//等待上一次数据发送完成
        USART2->DR=(u32)sendStr;
    }
    else
    {
        u2_printf(sendStr);//发送AT指令
        u2_printf("\r\n");//发送回车换行
    }
    if(searchStr && outTime)//当searchStr和outTime不为0时才等待应答
    {
        while((--outTime)&&(res == 0))//等待指定的应答或超时时间到来
        {
            res = Data_Analysis(USART2_RX_STA,(char *)USART2_RX_BUF,searchStr);
            delay_ms(1);
        }
        if(outTime == 0) {
            ret = 1;    //超时
        }
        if(res != 0)//res不为0证明收到指定应答
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
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//推挽输出模块供电
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
//		printf("ESP8266复位失败\r\n");
//	}
//	LCD_ShowString(20,20,200,12,12,"ESP8266 RESET OK!");
//	printf("ESP8266复位成功");
	
}
