/****************************************************************************************************************************************************************
** ��Ȩ:          2020-2030,��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:        adc_app.c
** ����:          ���
** �汾:          V1.0.0
** ����:          2020-05-06
** ����:          ADCģ���Ӧ��
** ����:          ADC DMA �������� ADC�¶����ݻ�ȡ�봦��
*****************************************************************************************************************************************************************
** �޸���:        No
** �汾:          No
** �޸�����:      No
** ����:          No
****************************************************************************************************************************************************************/

#include "adc_app.h"

static uint16_t Temp30_Vref_Adc;                                //30���϶�ADC��׼ֵ
static uint16_t Temp110_Vref_Adc;                               //110���϶�ADC��׼ֵ
static float TempVarySlope;                                     //�¶ȱ仯б��
static uint16_t gADC_GetData[AD_GROUP_MAX][AD_CHANNEL_MAX];     //ADC��DMA���ݻ�����
static uint16_t uADC_GetResult[AD_GROUP_MAX][AD_CHANNEL_MAX];   //ADC�������,�������ݴ���

static uint8_t ADC_Cplt_Flag = 0;                               //ADC��ɱ�־
static uint8_t ADC_Updata_Flag = 0;                             //ADC���ݸ��±�־


#ifdef __IN_FLASH_APP_H
/**
* ����       : Sensor_ADC_TemperParam_Init()
* ��������   : 2020-05-06
* ����       : ���
* ����       : ��ʼ���¶�ת����Ҫ�Ĳ����ṹ
* �������   : ��
* �������   : ADC_TemperParam,�¶�ת����Ҫ�Ĳ����ṹָ��;
* ���ؽ��   : ��
* ע���˵�� : Ҫʹ�ñ�����,Ҫ����In_Flash_app.c��In_Flash_app.h��
*              In_Flash.ch��In_Flash.h���ļ�
* �޸�����   :
*/
void Sensor_ADC_TemperParam_Init(ADC_TemperParam_TypeDef *ADC_TemperParam)
{
    uint8_t Read_Data[4];
    
    InFlash_Read_MultiBytes(TEMPER_K1, Read_Data, 2);
    ADC_TemperParam->Temper_K1 = (uint16_t)(Read_Data[0] << 8) | Read_Data[1];
    
    InFlash_Read_MultiBytes(TEMPER_B1, Read_Data, 2);
    ADC_TemperParam->Temper_B1 = (uint16_t)(Read_Data[0] << 8) | Read_Data[1];
    
    InFlash_Read_MultiBytes(TEMPER_K2, Read_Data, 2);
    ADC_TemperParam->Temper_K2 = (uint16_t)(Read_Data[0] << 8) | Read_Data[1];
    
    InFlash_Read_MultiBytes(TEMPER_B2, Read_Data, 2);
    ADC_TemperParam->Temper_B2 = (uint16_t)(Read_Data[0] << 8) | Read_Data[1];
    
    InFlash_Read_MultiBytes(TEMPDAMIN, Read_Data, 2);
    ADC_TemperParam->TempDAMin = (uint16_t)(Read_Data[0] << 8) | Read_Data[1];
    
    InFlash_Read_MultiBytes(TEMPDAMAX, Read_Data, 2);
    ADC_TemperParam->TempDAMax = (uint16_t)(Read_Data[0] << 8) | Read_Data[1];   
}

#else
/*
* ����       : Sensor_ADC_TemperParam_Init()
* ��������   : 2020-05-06
* ����       : ���
* ����       : ��ʼ���¶�ת����Ҫ�Ĳ����ṹ
* �������   : Param, Ҫд��ADC_TemperParam�Ĳ�������ָ��;
*              Param_Size, Param�Ĵ�С;
* �������   : ADC_TemperParam,�¶�ת����Ҫ�Ĳ����ṹָ��;
* ���ؽ��   : OP_SUCCESS(�ɹ�)��OP_FAILED(ʧ��)
* ע���˵�� : Param����������(���ֽ�˳������): 0~1,�¶�1����ϵ��K1; 
*              2~3,�¶�1����ϵ��B1; 4~5,�¶�2����ϵ��K2; 6~7,�¶�2����ϵ��B2;
*              8~9,�¶�DAת�����ֵ; 10~11,�¶�DAת������ֵ;
* �޸�����   :
*/
uint8_t Sensor_ADC_TemperParam_Init(ADC_TemperParam_TypeDef *ADC_TemperParam, uint8_t *Param, 
                            uint16_t Param_Size)
{
    uint8_t cnt = 0;

    if(Param != NULL && Param_Size != 0)
    {
        ADC_TemperParam->Temper_K1 = (uint16_t)(Param[cnt] << 8) | Param[cnt + 1];   
        cnt += 2;
        ADC_TemperParam->Temper_B1 = (uint16_t)(Param[cnt] << 8) | Param[cnt + 1];
        cnt += 2;
        ADC_TemperParam->Temper_K2 = (uint16_t)(Param[cnt] << 8) | Param[cnt + 1];
        cnt += 2;
        ADC_TemperParam->Temper_B2 = (uint16_t)(Param[cnt] << 8) | Param[cnt + 1];
        cnt += 2;
        ADC_TemperParam->TempDAMin = (uint16_t)(Param[cnt] << 8) | Param[cnt + 1];
        cnt += 2;
        ADC_TemperParam->TempDAMax = (uint16_t)(Param[cnt] << 8) | Param[cnt + 1]; 
        
        if(cnt > Param_Size)
        {
            return OP_FAILED;
        }
        return OP_SUCCESS;
    }
    return OP_FAILED;
}
#endif

/**
* ����       : Sensor_ADC_Init()
* ��������   : 2020-04-26
* ����       : ���
* ����       : ��ʼ��ADC�����stm32оƬ�ڲ��¶ȴ��������¶ȱ仯б�ʣ�����ADC��DMA����
* �������   : ��
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   :
*/
void Sensor_ADC_Init(void)
{    
    //ADC������ʼ��
    BSP_ADC_Init();
    //��ȡ30���϶�ADC��׼ֵ
    Temp30_Vref_Adc = *(__IO uint16_t *)VREF30ADDR;
    //��ȡ110���϶�ADC��׼ֵ
    Temp110_Vref_Adc = *(__IO uint16_t *)VREF110ADDR;
    //�¶ȱ仯б�ʼ���
    TempVarySlope = 80.0 / (float)(Temp110_Vref_Adc - Temp30_Vref_Adc);
    //��ʼADC DMA����
    BSP_ADC_Start_DMA((uint32_t *)&gADC_GetData[0][0],  AD_SEQBUFF_MAX);
}

/**
* ����       : Sensor_ADC_Data_Handler()
* ��������   : 2020-04-26
* ����       : ���
* ����       : ADC���ݴ���,��ADC�Ļ�������ȫ�����浽��һ��������,���������λADC���ݸ��±�־
* �������   : ��
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   :
*/
void Sensor_ADC_Data_Handler(void)
{
    uint8_t Ad_group;
    uint8_t Ad_Channel;
    static uint16_t DMA_TC_Cnt = 0;
    //ADC������ɲŻᴦ������
    if(ADC_Cplt_Flag)
    {
        ADC_Cplt_Flag = 0;
        DMA_TC_Cnt++;

        if(AD_SEQBUFF_MAX <= DMA_TC_Cnt)
        {
            //��ȡȫ��ADCͨ��������
            for(Ad_Channel = 0; Ad_Channel < AD_CHANNEL_MAX; Ad_Channel++)
            {
                for(Ad_group = 0; Ad_group < AD_GROUP_MAX; Ad_group++)
                {
                    uADC_GetResult[Ad_group][Ad_Channel] = gADC_GetData[Ad_group][Ad_Channel];
                }
            }
            ADC_Updata_Flag = UPDATA_OK;
            DMA_TC_Cnt = 0;
        }
    }
}

/**
* ����       : Sensor_ADC_GetChn_Value()
* ��������   : 2020-04-26
* ����       : ���
* ����       : �û���ȡADCͨ����ƽ���˲��������
* �������   : Channel_Num:ͨ����,ָ����ȡ��ͨ��
* �������   : ��
* ���ؽ��   : ƽ���˲���Ľ��
* ע���˵�� : 
* �޸�����   :
*/
uint16_t Sensor_ADC_GetChn_Value(AD_CHANNEL_NUM AD_Channel_Num)
{
    uint8_t cn;
    uint32_t AD_Sum = 0;

    if(AD_Channel_Num >= AD_CHANNEL_MAX)
        return 0;
    
    for(cn = 0; cn < AD_GROUP_MAX; cn++)
    {
        //��ͨ�������������ۼ�
        AD_Sum += uADC_GetResult[cn][AD_Channel_Num];
    }
    //����ƽ��ֵ
    return AD_Sum / AD_GROUP_MAX;
}

/**
* ����       : Sensor_ADC_Get_TemperData()
* ��������   : 2020-04-26
* ����       : ���
* ����       : ��ȡADC�¶�����
* �������   : ��
* �������   : ��
* ���ؽ��   : �¶�ֵ
* ע���˵�� : �¶ȼ���ɲο�STM32F072�����ֲ�
* �޸�����   :
*/
float Sensor_ADC_Get_TemperData(void)
{
    uint8_t cn;
    float Result;
    uint32_t AD_Sum = 0;
    uint16_t AD_Average;

    for(cn = 0; cn < AD_GROUP_MAX; cn++)
    {
        //���¶�ͨ�������������ۼ�
        AD_Sum += uADC_GetResult[cn][AD_CHANNEL_TEMPSENSOR];
    }
    //����ƽ��ֵ
    AD_Average = AD_Sum / AD_GROUP_MAX;
    //�����¶ȱ仯б�ʵõ�ʵ���¶�
    Result = TempVarySlope * (AD_Average - Temp30_Vref_Adc) + 30;
    
    return Result;
}

/**
* ����       : Sensor_ADC_Get_Updata_Flag()
* ��������   : 2020-04-26
* ����       : ���
* ����       : ��ȡADC���ݸ��±�־
* �������   : ��
* �������   : ��
* ���ؽ��   : ADC���±�־
* ע���˵�� : 
* �޸�����   :
*/
uint8_t Sensor_ADC_Get_Updata_Flag(void)
{
    return ADC_Updata_Flag;
}

/**
* ����       : Sensor_ADC_Clean_Updata_Flag()
* ��������   : 2020-04-26
* ����       : ���
* ����       : ���ADC���±�־
* �������   : ��
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   :
*/
void Sensor_ADC_Clean_Updata_Flag(void)
{
    ADC_Updata_Flag = UPDATA_NONE;
}

/**
* ����       : HAL_ADC_ConvCpltCallback()
* ��������   : 2020-04-26
* ����       : ���
* ����       : ADCת����ɻص����������λADC��ɱ�־
* �������   : ADC�������ָ��
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   :
*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    ADC_Cplt_Flag = 1;
}
