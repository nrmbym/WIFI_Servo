#ifndef __WIFI_H
#define __WIFI_H
#include "sys.h"


extern u8 sendAT(char *sendStr,char *searchStr,u32 outTime);  //ATָ��ͺ���
extern void Wifi_Init(void);                         //WIFI��ʼ��
extern void ESP8266_Resatart(void);            //�����λ

#endif
