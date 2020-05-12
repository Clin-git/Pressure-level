/****************************************************************************************************************************************************************
** ��Ȩ:          2020-2030,��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:        dac_bsp.c
** ����:          ���
** �汾:          V1.0.0
** ����:          2020-04-27
** ����:          DAC ͨ�����ô�������
** ����:          DAC ͨ������ DACת��
*****************************************************************************************************************************************************************
** �޸���:        No
** �汾:          No
** �޸�����:      No
** ����:          No
****************************************************************************************************************************************************************/

#include "dac_bsp.h"
#include "VariaType.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

static DAC_HandleTypeDef hdac;

/**
* ����       : BSP_DAC_Init()
* ��������   : 2020-04-27
* ����       : ���
* ����       : DAC��ʼ��
* �������   : ��
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : DACͨ��ͨ��DAC_CHANNEL_4_ENABLE��DAC_CHANNEL_5_ENABLE�궨���
* �޸�����   :
*/
void BSP_DAC_Init(void)
{
#if defined(DAC_CHANNEL_1_ENABLE) || defined(DAC_CHANNEL_2_ENABLE)
    
  DAC_ChannelConfTypeDef sConfig = {0};

  /** DAC Initialization 
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }
  
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
#if defined(DAC_CHANNEL_1_ENABLE)
  /** DAC channel OUT1 config 
  */  
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
#endif
  
#if defined(DAC_CHANNEL_2_ENABLE)
  /** DAC channel OUT2 config 
  */
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
#endif
  
#endif
}

/**
* ����       : HAL_DAC_MspInit()
* ��������   : 2020-04-27
* ����       : ���
* ����       : DACʱ�ӡ��ײ�ܽų�ʼ������HAL_DAC_Init�ص����ã�
* �������   : dacHandle��DAC�������
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : DACͨ��ͨ��DAC_CHANNEL_4_ENABLE��DAC_CHANNEL_5_ENABLE�궨���
* �޸�����   :
*/
void HAL_DAC_MspInit(DAC_HandleTypeDef* dacHandle)
{
#if defined(DAC_CHANNEL_1_ENABLE) || defined(DAC_CHANNEL_2_ENABLE)
    
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(dacHandle->Instance==DAC)
  {
  /* USER CODE BEGIN DAC_MspInit 0 */

  /* USER CODE END DAC_MspInit 0 */
    /* DAC clock enable */
    __HAL_RCC_DAC1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**DAC GPIO Configuration    
    PA4     ------> DAC_OUT1
    PA5     ------> DAC_OUT2 
    */
    
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
#if defined(DAC_CHANNEL_1_ENABLE)
    GPIO_InitStruct.Pin |= GPIO_PIN_4;
#endif
#if defined(DAC_CHANNEL_2_ENABLE)
    GPIO_InitStruct.Pin |= GPIO_PIN_5;
#endif
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN DAC_MspInit 1 */

  /* USER CODE END DAC_MspInit 1 */
  }
  
#endif
}

/**
* ����       : HAL_DAC_MspDeInit()
* ��������   : 2020-04-27
* ����       : ���
* ����       : DACʱ�ӡ��ײ�ܽŷ���ʼ������HAL_DAC_DeInit�ص����ã�
* �������   : dacHandle��DAC�������
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : DACͨ��ͨ��DAC_CHANNEL_4_ENABLE��DAC_CHANNEL_5_ENABLE�궨���
* �޸�����   :
*/
void HAL_DAC_MspDeInit(DAC_HandleTypeDef* dacHandle)
{
#if defined(DAC_CHANNEL_1_ENABLE) || defined(DAC_CHANNEL_2_ENABLE)
  uint32_t gpio_pin = 0;
    
  if(dacHandle->Instance==DAC)
  {
  /* USER CODE BEGIN DAC_MspDeInit 0 */

  /* USER CODE END DAC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DAC1_CLK_DISABLE();
  
    /**DAC GPIO Configuration    
    PA4     ------> DAC_OUT1
    PA5     ------> DAC_OUT2 
    */
#if defined(DAC_CHANNEL_1_ENABLE)
    gpio_pin |= GPIO_PIN_4;
#endif
#if defined(DAC_CHANNEL_2_ENABLE)
    gpio_pin |= GPIO_PIN_5;
#endif
    HAL_GPIO_DeInit(GPIOA, gpio_pin);

  /* USER CODE BEGIN DAC_MspDeInit 1 */

  /* USER CODE END DAC_MspDeInit 1 */
  }
  
#endif
} 

/**
* ����       : BSP_DAC_Convert()
* ��������   : 2020-04-27
* ����       : ���
* ����       : ָ��DACͨ��д��DAֵ
* �������   : DA_Value(DAֵ),DA_Channel_Num(DAͨ����)
* �������   : ��
* ���ؽ��   : OP_SUCCESS(�ɹ�),OP_FAILED(ʧ��)
* ע���˵�� : ���ֵ���ܳ���4095,ͨ����ֻ������(DA_CHANNEL_1��DA_CHANNEL_2)
* �޸�����   :
*/
uint8_t BSP_DAC_Convert(uint16_t DA_Value, DA_CHANNEL_NUM DA_Channel_Num)
{
  uint32_t channel;
  //DACֵ������Χ
  if(DA_Value > 4095)
  {
    return OP_FAILED;
  }

  if(DA_Channel_Num != DA_CHANNEL_1 && DA_Channel_Num != DA_CHANNEL_2)
  {
    return OP_FAILED;
  }
  
  if(DA_Channel_Num == DA_CHANNEL_1)
      channel = DAC_CHANNEL_1;
  else if(DA_Channel_Num == DA_CHANNEL_2)
      channel = DAC_CHANNEL_2;
  
  HAL_DAC_SetValue(&hdac, channel, DAC_ALIGN_12B_R, DA_Value);
  HAL_DAC_Start(&hdac, channel);
  
  return OP_SUCCESS;
}

