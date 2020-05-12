#ifndef __USART_APP_H
#define __USART_APP_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "usart_bsp.h"
#include "string.h"

#define USART_BAUDRATE_4800_CODE        0
#define USART_BAUDRATE_9600_CODE        1
#define USART_BAUDRATE_19200_CODE       2
#define USART_BAUDRATE_38400_CODE       3
#define USART_BAUDRATE_57600_CODE       4
#define USART_BAUDRATE_115200_CODE      5

#define USART_PARITY_NONE_CODE          0
#define USART_PARITY_ODD_CODE           1
#define USART_PARITY_EVEN_CODE          2

/* Ĭ�ϲ����� 9600 bit/s */
#define DEFAULT_UART_BAUD               9600    

/* Ĭ�ϳ�ʱʱ�䣬35��ʾ3.5���ֽ�ʱ�� */
#define DEFAULT_UART_TIMEOUT            35

#define USART_USING_485

#ifdef  USART_USING_485
#define _485_RE_DE_PIN_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define _485_RE_DE_PIN                  GPIO_PIN_8
#define _485_RE_DE_GPIO_PORT            GPIOA

#define TX_ON   HAL_GPIO_WritePin(_485_RE_DE_GPIO_PORT, _485_RE_DE_PIN, GPIO_PIN_SET)
#define RX_ON   HAL_GPIO_WritePin(_485_RE_DE_GPIO_PORT, _485_RE_DE_PIN, GPIO_PIN_RESET)
#endif

/**
* ����       : Sensor_USART_Init()
* ��������   : 2020-04-29
* ����       : ���
* ����       : ���ݲ����ʺ���żУ�������ʼ������������
* �������   : baudrate_code,�����ʴ���; parity_code,��żУ�����ʹ���;
*              Receive_Data,���ջ���ָ��; Receive_Data_Size,���ջ����С;
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   :
*/
void Sensor_USART_Init(uint8_t baudrate_code, uint8_t parity_code, 
                    uint8_t *Receive_Data, uint16_t Receive_Data_Size);

/**
* ����       : Sensor_USART_Get_RX_Len()
* ��������   : 2020-04-29
* ����       : ���
* ����       : ��ȡ����������һ�ν��յ������ݳ���
* �������   : ��
* �������   : ��
* ���ؽ��   : һ�ν��յ������ݳ���
* ע���˵�� : 
* �޸�����   :
*/
uint16_t Sensor_USART_Get_RX_Len(void);

/**
* ����       : Sensor_USART_Get_RXBuf_Used_Len()
* ��������   : 2020-04-29
* ����       : ���
* ����       : ��ȡ���������ڽ��յ����ݺ󣬴ӽ��ջ���ͷ��������β���ĳ���
* �������   : ��
* �������   : ��
* ���ؽ��   : �ӽ��ջ���ͷ��������β���ĳ���
* ע���˵�� : 
* �޸�����   :
*/
uint16_t Sensor_USART_Get_RXBuf_Used_Len(void);

/**
* ����       : Sensor_USART_Get_BaudRate_Code()
* ��������   : 2020-04-29
* ����       : ���
* ����       : ��ȡ���������ڵĲ����ʴ���
* �������   : ��
* �������   : ��
* ���ؽ��   : ���ڵĲ����ʴ���
* ע���˵�� : 
* �޸�����   :
*/
uint8_t Sensor_USART_Get_BaudRate_Code(void);

/**
* ����       : Sensor_USART_Get_Parity_Code()
* ��������   : 2020-04-29
* ����       : ���
* ����       : ��ȡ���������ڵ���żУ�����
* �������   : ��
* �������   : ��
* ���ؽ��   : ���ڵ���żУ�����
* ע���˵�� : 
* �޸�����   :
*/
uint8_t Sensor_USART_Get_Parity_Code(void);

/**
* ����       : Sensor_USART_Get_RX_Updata_Flag()
* ��������   : 2020-04-29
* ����       : ���
* ����       : ��ȡ���ո��±�־
* �������   : ��
* �������   : ��
* ���ؽ��   : ���ո��±�־
* ע���˵�� : 
* �޸�����   :
*/
uint8_t Sensor_USART_Get_RX_Updata_Flag(void);

/**
* ����       : Sensor_USART_Get_TX_Cplt_Flag()
* ��������   : 2020-04-29
* ����       : ���
* ����       : ��ȡ������ɱ�־
* �������   : ��
* �������   : ��
* ���ؽ��   : ������ɱ�־
* ע���˵�� : 
* �޸�����   :
*/
uint8_t Sensor_USART_Get_TX_Cplt_Flag(void);

/**
* ����       : Sensor_USART_Clear_RX_Updata_Flag()
* ��������   : 2020-04-29
* ����       : ���
* ����       : ������ո��±�־
* �������   : ��
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   :
*/
void Sensor_USART_Clear_RX_Updata_Flag(void);

/**
* ����       : Sensor_USART_Clear_TX_Cplt_Flag()
* ��������   : 2020-04-29
* ����       : ���
* ����       : ���������ɱ�־
* �������   : ��
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   :
*/
void Sensor_USART_Clear_TX_Cplt_Flag(void);

#ifdef __cplusplus
}
#endif
#endif
