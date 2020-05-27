/****************************************************************************************************************************************************************
** ��Ȩ:          2020-2030,��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:        Picocap_app.c
** ����:          ���
** �汾:          V1.0.0
** ����:          2020-04-30
** ����:          PCapоƬӦ�ü������ݴ���
** ����:          ��ȡPCapоƬ,�Զ��������ݽ����˲�,����ת��
*****************************************************************************************************************************************************************
** �޸���:        No
** �汾:          No
** �޸�����:      No
** ����:          No
****************************************************************************************************************************************************************/

#include "Picocap_app.h"
#include "VariaType.h"
#include "TypeConvert.h"


/**
* ����       : SwitchCurFilter()
* ��������   : 2020-04-30
* ����       : ���
* ����       : �л��˲�ˮƽ����
* �������   : FiltFactor(�˲�ϵ��)
* �������   : FilterParam,ָ���˲������ṹ��;
* ���ؽ��   : ��
* ע���˵�� : �˲�ˮƽ��9�������ִӵ͵��߶�Ӧ�˲���ȴӵ͵���
* �޸�����   :
*/
void SwitchCurFilter(uint8_t FiltFactor, DataFilterParam *FilterParam)
{  
    switch(FiltFactor)
    {
    case 0:
        FilterParam->FilterBufMax   = 1;
        FilterParam->FilterCycle    = 1;
    break;

    case 1:
        FilterParam->FilterBufMax   = 1;
        FilterParam->FilterCycle    = 12;
    break;

    case 2:
        FilterParam->FilterBufMax   = 2;
        FilterParam->FilterCycle    = 12;
    break;

    case 3:
        FilterParam->FilterBufMax   = 3;
        FilterParam->FilterCycle    = 12;
    break;

    case 4:
        FilterParam->FilterBufMax   = 4;
        FilterParam->FilterCycle    = 15;
    break;

    case 5:
        FilterParam->FilterBufMax   = 5;
        FilterParam->FilterCycle    = 24;
    break;

    case 6:
        FilterParam->FilterBufMax   = 6;
        FilterParam->FilterCycle    = 30;
    break;

    case 7:
        FilterParam->FilterBufMax   = 8;
        FilterParam->FilterCycle    = 30;
    break;

    case 8:
        FilterParam->FilterBufMax   = 8;
        FilterParam->FilterCycle    = 60;
    break;

    case 9:
        FilterParam->FilterBufMax   = 10;
        FilterParam->FilterCycle    = 96;
    break;

    default:
        FilterParam->FilterBufMax   = 1;
        FilterParam->FilterCycle    = 10;
    break;
    }

    FilterParam->FilterStart = 0;
}

#ifdef __IN_FLASH_APP_H
/**
* ����       : DataFilterParam_Init()
* ��������   : 2020-05-05
* ����       : ���
* ����       : ��ʼ�������˲���Ҫ�Ĳ����ṹ
* �������   : Filterfactor_CountMax, �˲�ϵ�����ۼ���������ֵ,�˲�ϵ���ڸ�8λ,
*              �ۼ���������ֵ�ڵ�8λ;
* �������   : FilterParam,�����˲���Ҫ�Ĳ����ṹָ��;
* ���ؽ��   : ��
* ע���˵�� : Ҫʹ�ñ�����,Ҫ����In_Flash_app.c��In_Flash_app.h��
*              In_Flash.ch��In_Flash.h���ļ�
* �޸�����   :
*/
void DataFilterParam_Init(DataFilterParam *FilterParam, uint16_t Filterfactor_CountMax)
{
    uint8_t Read_Data[4];
    
    InFlash_Read_MultiBytes(FILTER, Read_Data, 1);
    SwitchCurFilter(Read_Data[0], FilterParam);    
    FilterParam->InputCountMax = Filterfactor_CountMax;
    InFlash_Read_MultiBytes(CAPMAX, Read_Data, 4);
    FilterParam->InputRangeMax = HexToUlong(Read_Data);
    InFlash_Read_MultiBytes(CAPMIN, Read_Data, 4);
    FilterParam->InputRangeMin = HexToUlong(Read_Data);
}
#else
/**
* ����       : DataFilterParam_Init()
* ��������   : 2020-05-05
* ����       : ���
* ����       : ��ʼ�������˲���Ҫ�Ĳ����ṹ
* �������   : Filterfactor_CountMax, �˲�ϵ�����ۼ���������ֵ,�˲�ϵ���ڸ�8λ,
*              �ۼ���������ֵ�ڵ�8λ;
*              InputRangeMax, ���뷶Χ���ֵ;
*              InputRangeMin, ���뷶Χ��Сֵ;
* �������   : FilterParam,�����˲���Ҫ�Ĳ����ṹָ��;
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   : 
*/
void DataFilterParam_Init(DataFilterParam *FilterParam, uint16_t Filterfactor_CountMax,
                            uint32_t InputRangeMax, uint32_t InputRangeMin)
{
    SwitchCurFilter(Filterfactor_CountMax >> 8, FilterParam);
    FilterParam->InputCountMax = Filterfactor_CountMax & 0xFF;
    FilterParam->InputRangeMax = InputRangeMax;
    FilterParam->InputRangeMin = InputRangeMin;
}
#endif

#ifdef __IN_FLASH_APP_H
/**
* ����       : DataConvertParam_Init()
* ��������   : 2020-05-05
* ����       : ���
* ����       : ��ʼ������ת����Ҫ�Ĳ����ṹ
* �������   : ��
* �������   : DataConvert_Param,����ת����Ҫ�Ĳ����ṹָ��;
* ���ؽ��   : ��
* ע���˵�� : Ҫʹ�ñ�����,Ҫ����In_Flash_app.c��In_Flash_app.h��
*              In_Flash.ch��In_Flash.h���ļ�
* �޸�����   :
*/
void DataConvertParam_Init(PCap_DataConvert_Param *DataConvert_Param)
{
    uint8_t Read_Data[4];
    
    InFlash_Read_MultiBytes(COMPENSATE, Read_Data, 1);
    DataConvert_Param->CompenEn = Read_Data[0];
    
    InFlash_Read_MultiBytes(HEIGHTRANGE, Read_Data, 2);
    DataConvert_Param->HeightRange = (uint16_t)(Read_Data[0] << 8) | Read_Data[1];
    
    InFlash_Read_MultiBytes(CAPMIN, Read_Data, 4);
    DataConvert_Param->CapMin = HexToUlong(Read_Data);
    
    InFlash_Read_MultiBytes(CAPMAX, Read_Data, 4);
    DataConvert_Param->CapMax = HexToUlong(Read_Data);
    
    InFlash_Read_MultiBytes(CAPMINBAK, Read_Data, 4);
    DataConvert_Param->CapMinBak = HexToUlong(Read_Data);
    
    InFlash_Read_MultiBytes(CAPMAXBAK, Read_Data, 4);
    DataConvert_Param->CapMaxBak = HexToUlong(Read_Data);
    
    InFlash_Read_MultiBytes(CAPADMIN, Read_Data, 2);
    DataConvert_Param->CapADMin = (uint16_t)(Read_Data[0] << 8) | Read_Data[1];
    
    InFlash_Read_MultiBytes(CAPADLOW, Read_Data, 2);
    DataConvert_Param->CapADLow = (uint16_t)(Read_Data[0] << 8) | Read_Data[1];
    
    InFlash_Read_MultiBytes(CAPADHIGH, Read_Data, 2);
    DataConvert_Param->CapADHigh = (uint16_t)(Read_Data[0] << 8) | Read_Data[1];
    
    InFlash_Read_MultiBytes(CAPADMAX, Read_Data, 2);
    DataConvert_Param->CapADMax = (uint16_t)(Read_Data[0] << 8) | Read_Data[1];
    
    InFlash_Read_MultiBytes(CAPDAMIN, Read_Data, 2);
    DataConvert_Param->CapDAMin = (uint16_t)(Read_Data[0] << 8) | Read_Data[1];
    
    InFlash_Read_MultiBytes(CAPDALOW, Read_Data, 2);
    DataConvert_Param->CapDALow = (uint16_t)(Read_Data[0] << 8) | Read_Data[1];
    
    InFlash_Read_MultiBytes(CAPDAHIGH, Read_Data, 2);
    DataConvert_Param->CapDAHigh = (uint16_t)(Read_Data[0] << 8) | Read_Data[1];
    
    InFlash_Read_MultiBytes(CAPDAMAX, Read_Data, 2);
    DataConvert_Param->CapDAMax = (uint16_t)(Read_Data[0] << 8) | Read_Data[1];
    
    InFlash_Read_MultiBytes(CORRECT_K, Read_Data, 2);
    DataConvert_Param->Correct_K = (float)((Read_Data[0] << 8) | Read_Data[1]) / 100.0;
    
    InFlash_Read_MultiBytes(CORRECT_B, Read_Data, 2);
    DataConvert_Param->Correct_B = (float)((Read_Data[0] << 8) | Read_Data[1]) / 100.0;
}
#else
/**
* ����       : DataConvertParam_Init()
* ��������   : 2020-05-05
* ����       : ���
* ����       : ��ʼ������ת����Ҫ�Ĳ����ṹ
* �������   : CompenEn, ����ʹ��;
*              Param, Ҫд��DataConvert_Param�Ĳ�������ָ��;
*              Param_Size, Param�Ĵ�С;
* �������   : DataConvert_Param,����ת����Ҫ�Ĳ����ṹָ��;
* ���ؽ��   : OP_SUCCESS(�ɹ�)��OP_FAILED(ʧ��)
* ע���˵�� : Param�����ݷֲ�(���ֽڷֲ�): 0~1,λ�Ƹ߶�; 2~5,�����������;
*              6~9,������������; 10~13,�����������; 14~17,������������; 
*              18~19,����ADֵ���; 20~21,����ADֵ�¿̶�; 22~23,����ADֵ�Ͽ̶�; 
*              24~25,����ADֵ������; 26~27,����DAֵ���; 28~29,����DAֵ�¿̶�; 
*              30~31,����DAֵ�Ͽ̶�; 32~33,����DAֵ������; 34~35,��������ϵ��K; 
*              36~37,��������ϵ��B;
* �޸�����   :
*/
uint8_t DataConvertParam_Init(PCap_DataConvert_Param *DataConvert_Param, uint8_t CompenEn,
                            uint8_t *Param, uint16_t Param_Size)
{
    uint8_t cnt = 0;
    
    DataConvert_Param->CompenEn = CompenEn;
    if(Param != NULL && Param_Size != 0)
    {
        DataConvert_Param->HeightRange = (Param[cnt] << 8) | Param[cnt + 1];
        cnt += 2;    
        DataConvert_Param->CapMin = HexToUlong(&Param[cnt]);
        cnt += 4;
        DataConvert_Param->CapMax = HexToUlong(&Param[cnt]);
        cnt += 4;
        DataConvert_Param->CapADMin = (Param[cnt] << 8) | Param[cnt + 1];
        cnt += 2;
        DataConvert_Param->CapADLow = (Param[cnt] << 8) | Param[cnt + 1];
        cnt += 2;
        DataConvert_Param->CapADHigh = (Param[cnt] << 8) | Param[cnt + 1];
        cnt += 2;
        DataConvert_Param->CapADMax = (Param[cnt] << 8) | Param[cnt + 1];
        cnt += 2;
        DataConvert_Param->CapDAMin = (Param[cnt] << 8) | Param[cnt + 1];
        cnt += 2;
        DataConvert_Param->CapDALow = (Param[cnt] << 8) | Param[cnt + 1];
        cnt += 2;
        DataConvert_Param->CapDAHigh = (Param[cnt] << 8) | Param[cnt + 1];
        cnt += 2;
        DataConvert_Param->CapDAMax = (Param[cnt] << 8) | Param[cnt + 1];
        cnt += 2;    
        DataConvert_Param->Correct_K = (float)((Param[cnt] << 8) | Param[cnt + 1]) / 100.0;
        cnt += 2;
        DataConvert_Param->Correct_B = (float)((Param[cnt] << 8) | Param[cnt + 1]) / 100.0;
        
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
* ����       : Sensor_PCap_GetResult()
* ��������   : 2020-04-29
* ����       : ���
* ����       : ��ȡPCapԭʼ�ɼ�ֵ
* �������   : reg_addr,����Ĵ����ĵ�ַ;
* �������   : PCap_Result,����PCap��������
* ���ؽ��   : �����ɹ�/ʧ��
* ע���˵�� : 
* �޸�����   : 
*/
uint8_t Sensor_PCap_GetResult(uint8_t reg_addr, uint32_t *PCap_Result)
{
    uint32_t PCap_Status;
    uint8_t Result = OP_FAILED;
    static uint32_t Tick_Old = 0;

    //��ʱʱ�䵽�ɼ�����ֵ
    if(HAL_GetTick() - Tick_Old > PCAP_COLLECT_CYCLE)
    {
        //��ȡPCap״̬
        PCap_Status = PCap_Res_Sta();
        if(PCap_Status & PCAP_RUNBIT)
        {
            *PCap_Result = PCap_Res_Capacitance(reg_addr);
            Result = OP_SUCCESS;
        }
        PCap_Measure();
        Tick_Old = HAL_GetTick();
    }
    
    return Result;
}

/**
* ����       : Sensor_DataFilter()
* ��������   : 2020-04-29
* ����       : ���
* ����       : ���ݰ�ָ���˲����������˲�
* �������   : FilterParam,ָ���˲������ṹ��;InputValue,���������
* �������   : OutputValue,�˲����ֵ
* ���ؽ��   : �����ɹ�/ʧ��
* ע���˵�� : ʹ�ñ�����ǰ,�Ƚ�FilterParam��ʼ��(��������ÿһ����Ա�����ʵ�ֵ),����������ʹ��
* �޸�����   : 
*/
uint8_t Sensor_DataFilter(DataFilterParam *FilterParam, uint32_t InputValue, uint32_t *OutputValue)
{
    static uint32_t Data_1st_FilterBuf[DATA_1ST_FILTER_BUF_MAX];        //һ���˲�����
    static uint32_t Data_1st_FilterBufBak[DATA_1ST_FILTER_BUF_MAX];     //һ���˲����汸��
    static uint32_t Data_2nd_FilterBuf[DATA_2ND_FILTER_BUF_MAX];        //�����˲�����
    static uint32_t Data_2nd_FilterBufBak[DATA_2ND_FILTER_BUF_MAX];     //�����˲����汸��
    static uint32_t DataResBuf[DATA_BUF_MAX];                           //����ԭʼ����ֵ�Ļ���
    static uint8_t DataResBuf_Count;
    uint8_t i;
    uint32_t Data_Avg1;
    uint32_t Data_Avg2;
    uint32_t Data_Avg3;
    uint64_t Data_FilterSum;                                            //�˲������ۼƺ�
    
    if(InputValue > FilterParam->InputRangeMax)
    {
        InputValue = FilterParam->InputRangeMax;
    }
    if(InputValue < FilterParam->InputRangeMin)
    {
        InputValue = FilterParam->InputRangeMin;
    }
    
    if(FilterParam->FilterStart == 0)
    {
        DataResBuf_Count = 0;
        for(i = 0; i < DATA_1ST_FILTER_BUF_MAX; i++)
        {
            Data_1st_FilterBuf[i] = InputValue;
        }
        for(i = 0; i < DATA_2ND_FILTER_BUF_MAX; i++)
        {
            Data_2nd_FilterBuf[i] = InputValue;
        }
        FilterParam->FilterStart = 1;
        
        return OP_FAILED;
    }
    
    DataResBuf[DataResBuf_Count++] = InputValue;
    if(DataResBuf_Count < DATA_BUF_MAX)
    {
        return OP_FAILED;
    }
    else
    {
        DataResBuf_Count = 0;
    }
    
    Data_Avg1 = GetDelExtremeAndAverage(DataResBuf, DATA_BUF_MAX, 2, 5);
    
    if(FilterParam->FilterCycle < 2)
    {
        *OutputValue = Data_Avg1;
        return OP_SUCCESS;
    }
    else
    {
        for(i = 0; i < FilterParam->FilterCycle - 1; i++)
        {
            Data_1st_FilterBuf[i] = Data_1st_FilterBuf[i + 1];
        }
        Data_1st_FilterBuf[i] = Data_Avg1;
        
        if(FilterParam->FilterCycle >= 3)
        {
            for(i = 0; i < FilterParam->FilterCycle; i++)
            {
                Data_1st_FilterBufBak[i] = Data_1st_FilterBuf[i];
            }
            Data_Avg2 = GetDelExtremeAndAverage(Data_1st_FilterBufBak, 
                                            FilterParam->FilterCycle, 
                                            FilterParam->FilterCycle / 3, 
                                            FilterParam->FilterCycle / 3);
        }
        else
        {
            Data_FilterSum = 0;                                                               
            for(i = 0; i < FilterParam->FilterCycle; i++)
            {
                Data_FilterSum += Data_1st_FilterBuf[i];
            }           
            Data_Avg2 = Data_FilterSum / FilterParam->FilterCycle;        /* �����˲������ƽ��ֵ */
        }
        
        if(FilterParam->FilterBufMax < 2)            
        {
            *OutputValue = Data_Avg2;
            return OP_SUCCESS;
        }
        else
        {
            for(i = 0; i < FilterParam->FilterBufMax - 1; i++)
            {
                Data_2nd_FilterBuf[i] = Data_2nd_FilterBuf[i + 1];
            }
            Data_2nd_FilterBuf[i] = Data_Avg2;
            
            if(FilterParam->FilterBufMax >= 3)
            {
                for(i = 0; i < FilterParam->FilterBufMax; i++)
                {
                    Data_2nd_FilterBufBak[i] = Data_2nd_FilterBuf[i];
                }
                Data_Avg3 = GetDelExtremeAndAverage(Data_2nd_FilterBufBak, 
                                                FilterParam->FilterBufMax, 
                                                FilterParam->FilterBufMax / 3, 
                                                FilterParam->FilterBufMax / 3);
            }
            else
            {
                Data_FilterSum = 0;                                                               
                for(i = 0; i < FilterParam->FilterBufMax; i++)
                {
                    Data_FilterSum += Data_2nd_FilterBuf[i];
                }           
                Data_Avg3 = Data_FilterSum / FilterParam->FilterBufMax;        /* �����˲������ƽ��ֵ */
            }
            
            *OutputValue = Data_Avg3;
            return OP_SUCCESS;
        }       
    }    
}

/**
* ����       : Sensor_PCap_DataConvert()
* ��������   : 2020-04-29
* ����       : ���
* ����       : ���ݰ�ָ�����Ʋ�����������ת��
* �������   : DataConvert_Param,ָ�����Ʋ����ṹ��;InputValue,���������
* �������   : DataConvert_Out,������ݵĲ����ṹ��
* ���ؽ��   : ��
* ע���˵�� : ʹ�ñ�����ǰ,�Ƚ�DataConvert_Param��ʼ��(��������ÿһ����Ա�����ʵ�ֵ),����������ʹ��
* �޸�����   : 
*/
void Sensor_PCap_DataConvert(PCap_DataConvert_Param *DataConvert_Param, 
                                uint32_t InputValue, 
                                PCap_DataConvert_Out_Param *DataConvert_Out)
{
    uint16_t LiquidHeightAD;
    uint32_t PCap_Result;
    float LiquidHeightRate;
    float Rate;
    //����ʹ��,����K,Bֵ
    if(DataConvert_Param->CompenEn == COMPENSATE_ENABLE)        
    {
        PCap_Result = (uint32_t)(InputValue * DataConvert_Param->Correct_K 
                                + (DataConvert_Param->Correct_B - 100));
    }
    else
    {
        PCap_Result = InputValue;
    }
    //������ݱ���
    LiquidHeightRate = (float)(PCap_Result - DataConvert_Param->CapMin) 
                                / (float)(DataConvert_Param->CapMax - DataConvert_Param->CapMin);
    
    if(1.0 < LiquidHeightRate)
    {
        LiquidHeightRate = 1.0;
    }
    else if (0.0 > LiquidHeightRate)
    {
        LiquidHeightRate = 0.0;
    }
    //ת����0--65535��Χ�ڵ�ADֵ
    LiquidHeightAD = (uint16_t)(LiquidHeightRate * PCAP_ADC_MAX_VALUE);
    //ת����Һλ�߶�
    DataConvert_Out->LiquidHeight = (float)(LiquidHeightRate 
                                            * DataConvert_Param->HeightRange);
    DataConvert_Out->PCapDA_ResultValue = (uint16_t)(LiquidHeightRate * (PCAP_DAC_MAX_VALUE 
                                                    - PCAP_DAC_MIN_VALUE)) 
                                                    + PCAP_DAC_MIN_VALUE;
    //���û��4���ֵ����ֱ�Ӽ���DA���ֵ
    if((0x0000 == DataConvert_Param->CapADLow) || (0x0000 == DataConvert_Param->CapDAHigh))
    {
        DataConvert_Out->PCapDA_OutValue = (uint16_t)(LiquidHeightRate * (DataConvert_Param->CapDAMax 
                                                    - DataConvert_Param->CapDAMin)) 
                                                    + DataConvert_Param->CapDAMin;
    }
    else
    {                 
        //ADֵ�����AD����
        if(LiquidHeightAD < DataConvert_Param->CapADMin)  
        {
            DataConvert_Out->PCapDA_OutValue = DataConvert_Param->CapDAMin;
        }
        //ADֵ�����AD���¿̶�AD֮��
        else if((LiquidHeightAD < DataConvert_Param->CapADLow) 
            && (LiquidHeightAD) >= DataConvert_Param->CapADMin)
        {
            Rate = (float)(LiquidHeightAD - DataConvert_Param->CapADMin)
                / (float)(DataConvert_Param->CapADLow - DataConvert_Param->CapADMin);
            DataConvert_Out->PCapDA_OutValue = DataConvert_Param->CapDALow - DataConvert_Param->CapDAMin;
            DataConvert_Out->PCapDA_OutValue = (uint16_t)(Rate * DataConvert_Out->PCapDA_OutValue) 
                                            + DataConvert_Param->CapDAMin;
        }
        //ADֵ���¿̶�ADֵ���Ͽ̶�ADֵ֮��
        else if((LiquidHeightAD < DataConvert_Param->CapADHigh) 
            && (LiquidHeightAD >= DataConvert_Param->CapADLow))
        {
            Rate = (float)(LiquidHeightAD - DataConvert_Param->CapADLow) 
                / (float)(DataConvert_Param->CapADHigh - DataConvert_Param->CapADLow);
            DataConvert_Out->PCapDA_OutValue = DataConvert_Param->CapDAHigh - DataConvert_Param->CapDALow;
            DataConvert_Out->PCapDA_OutValue = (uint16_t)(Rate * DataConvert_Out->PCapDA_OutValue) 
                                            + DataConvert_Param->CapDALow;
        }
        //ADֵ���Ͽ̶�ADֵ��������ADֵ֮��
        else if((LiquidHeightAD < DataConvert_Param->CapADMax) 
            && (LiquidHeightAD >= DataConvert_Param->CapADHigh))
        {
            Rate = (float)(LiquidHeightAD - DataConvert_Param->CapADHigh) 
                / (float)(DataConvert_Param->CapADMax - DataConvert_Param->CapADHigh);
            DataConvert_Out->PCapDA_OutValue = DataConvert_Param->CapDAMax - DataConvert_Param->CapDAHigh;
            DataConvert_Out->PCapDA_OutValue = (uint16_t)(Rate * DataConvert_Out->PCapDA_OutValue) 
                                                    + DataConvert_Param->CapDAHigh;
        }                                            
        //ADֵ������������
        else                                                       
        {
            DataConvert_Out->PCapDA_OutValue = DataConvert_Param->CapDAMax;
        }
    } 
    DataConvert_Out->LiquidHeightAD = LiquidHeightAD;
}

