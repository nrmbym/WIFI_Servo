#ifndef __USART2_H
#define __USART2_H

#include "sys.h"
#define Bound_usart_2 9600
#define USART2_RX_BUF_LEN 5000
#define USART2_TX_BUF_LEN 200
#define USART2_REC_LEN    100


extern u8 USART2_RX_BUF[USART2_RX_BUF_LEN];
extern u16 USART2_RX_STA;

extern void USART_2_Init(u32 bound);   //��ʼ������2
extern void Data_clean(void);     //������ջ��棬���ұ��USART2_RX_STAΪ�ɽ���״̬
extern char * Data_Analysis(u16 USARTx_RX_STA ,char *buff, char *searchstr);
                                //�Ӵ��ڽ��յĻ��������в���ָ�����ַ���
extern void u2_printf(char* fmt,...);//����2��ӡ����


#endif
