#ifndef __WIFI_H
#define __WIFI_H
#include "sys.h"


extern u8 sendAT(char *sendStr,char *searchStr,u32 outTime);  //AT指令发送函数
extern void Wifi_Init(void);                         //WIFI初始化
extern void ESP8266_Resatart(void);            //软件复位

#endif
