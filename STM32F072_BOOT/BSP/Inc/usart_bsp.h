/**@file        usart_bsp.h
* @details      usart_bsp.c��ͷ�ļ�,�����˴���������API����
* @author       ���
* @date         2020-04-28
* @version      V1.0.0
* @copyright    2020-2030,��������Ϊ�Ƽ���չ���޹�˾
**********************************************************************************
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/04/28  <td>1.0.0    <td>���    <td>������ʼ�汾
* </table>
*
**********************************************************************************
*/

#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "VariaType.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

#define USER_USART              USART1
#define USER_USART_ALTERNATE    GPIO_AF1_USART1

/* USER CODE BEGIN Private defines */
#define USER_USART_TX_PIN_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define USER_USART_RX_PIN_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define USER_USART_TX_PIN               GPIO_PIN_9
#define USER_USART_RX_PIN               GPIO_PIN_10
#define USER_USART_TX_PIN_GPIOX         GPIOA
#define USER_USART_RX_PIN_GPIOX         GPIOA

#define USER_USART_DMA_IRQHANDLER       DMA1_Channel2_3_IRQHandler
#define USER_USART_IRQHANDLER           USART1_IRQHandler

#define USER_USART_IRQN                 USART1_IRQn
#define USER_USART_DMA_CHANNEL_IRQN     DMA1_Channel2_3_IRQn


#ifndef OP_SUCCESS
#define OP_SUCCESS
#endif

#ifndef OP_FAILED
#define OP_FAILED
#endif


/**@brief       ���ڳ�ʼ��
* @param[in]    baudrate : ������; 
* @param[in]    parity : ��żУ��;
* @return       ����ִ�н��
* - None
*/
void BSP_USART_UART_Init(uint32_t baudrate, uint32_t parity);

/**@brief       ���ڷ���ʼ��
* @return       ����ִ�н��
* - None
*/
void BSP_USART_UART_DeInit(void);
    
/**@brief       ����DMA����
* @param[in]    pData : DMA���ͻ���ָ��;
* @param[in]    Size : �����С
* @return       ����ִ�н��
* - OP_SUCCESS(�ɹ�)
* - OP_FAILED(ʧ��)
*/
uint8_t BSP_UART_Transmit_DMA(uint8_t *pData, uint16_t Size);

/**@brief       ����DMA����
* @param[in]    pData : DMA���ջ���ָ��;
* @param[in]    Size : �����С
* @return       ����ִ�н��
* - OP_SUCCESS(�ɹ�)
* - OP_FAILED(ʧ��)
*/
uint8_t BSP_UART_Receive_DMA(uint8_t *pData, uint16_t Size);

/**@brief       �������ý��ճ�ʱ�ж�
* @param[in]    TimeoutValue : ��ʱʱ�䣬��λ������λ��;
* @return       ����ִ�н��
* - None
* @note         ʾ����1����ʼλ��8λ���ݣ�1��ֹͣλ�Ĵ������ã�
* ��10λΪ1�������ֽڣ�������4�������ֽڵĳ�ʱʱ�䣬
* TimeoutValue = 40
*/
void BSP_UART_ReceiverTimeout_Config(uint32_t TimeoutValue);

/**@brief       ���������ַ�ƥ���ж�
* @param[in]    Value : ƥ����ַ�;
* @return       ����ִ�н��
* - None
*/
void BSP_UART_CharMatch_Config(uint8_t Value);

/**@brief       ���ڽ��ճ�ʱ�жϻص�����
* @param[in]    huart : ���ڴ������;
* @return       ����ִ�н��
* - None
* @note         �ú������������ļ����ض��壬�����Զ����һ����������HAL���в����ڵ�
*/
void HAL_UART_RxTimoCallback(UART_HandleTypeDef *huart);

/**@brief       �����ַ�ƥ���жϻص�����
* @param[in]    huart : ���ڴ������;
* @return       ����ִ�н��
* - None
* @note         �ú������������ļ����ض��壬�����Զ����һ����������HAL���в����ڵ�
*/
void HAL_UART_CMatchCallback(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif
#endif 
