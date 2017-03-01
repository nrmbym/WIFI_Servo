#ifndef __USART2_H
#define __USART2_H

#include "sys.h"
#define Bound_usart_2 9600
#define USART2_RX_BUF_LEN 5000
#define USART2_TX_BUF_LEN 200
#define USART2_REC_LEN    100


extern u8 USART2_RX_BUF[USART2_RX_BUF_LEN];
extern u16 USART2_RX_STA;

extern void USART_2_Init(u32 bound);   //初始化串口2
extern void Data_clean(void);     //清除接收缓存，并且标记USART2_RX_STA为可接收状态
extern char * Data_Analysis(u16 USARTx_RX_STA ,char *buff, char *searchstr);
                                //从串口接收的缓冲数组中查找指定的字符串
extern void u2_printf(char* fmt,...);//串口2打印函数


#endif
