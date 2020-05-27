/****************************************************************************************************************************************************************
** ��Ȩ:          2020-2030,��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:        ModbusRtu_Conf.c
** ����:          ���
** �汾:          V1.0.0
** ����:          2020-04-29
** ����:          Modbus �����봦������
** ����:          ����λ�����͵�ָ����н�������Ӧ,���й�������Զ��ϴ��ĳ�����붼��д�ڱ��ļ�
*****************************************************************************************************************************************************************
** �޸���:        No
** �汾:          No
** �޸�����:      No
** ����:          No
****************************************************************************************************************************************************************/

#include "ModBusRtu.h"


#ifdef __PICOCAP_APP_H

static uint32_t Calib_CapMin;                                                          //�궨�������ֵ
static uint32_t Calib_CapMax;  
//static uint8_t CapDA_ClibEn = DAOUTCLIB_DISABLE;
static uint16_t Calib_CapDAMin;                                                        //�궨����DA���ֵ
static uint16_t Calib_CapDALow;                                                        //�궨����DA�¿̶�
static uint16_t Calib_CapDAHigh;                                                       //�궨����DA�Ͽ̶�
static uint16_t Calib_CapDAMax;                                                        //�궨����DA����ֵ
static uint16_t Calib_TempDAMin;                                                       //�궨�¶�DA���ֵ
static uint16_t Calib_TempDAMax;                                                       //�궨�¶�DA������
static uint16_t Calib_ADMin;                                                           //�궨����AD���ֵ
static uint16_t Calib_ADLow;                                                           //�궨����AD�¿̶�
static uint16_t Calib_ADHigh;                                                          //�궨����AD�Ͽ̶�
static uint16_t Calib_ADMax;                                                           //�궨����AD������
//static float RecvVol[4];

const uint8_t SensorSoftVersion[8] = {0x07, 'S', 'V', '1', '.', '0', '.', '5'};//����汾�� ��ʽ:�汾�ų��ȣ��汾������


//Modbus �������³�ʼ���ص�����
static int MB_USART_ReInit(ModBusBaseParam_TypeDef *ModBusBaseParam);

/**
* ����       : ModBus_ReadPreHandle()
* ��������   : 2020-05-07
* ����       : ���
* ����       : Modbus ��Ԥ����,�ӽ��յ�ԭʼ��������ȡ����ϢID�ͼĴ�������,���豸��ַ�͹�������뷢�ͻ�����
* �������   : ModBusBaseParam: ModBus����Ļ��������ṹ��;        
* �������   : ReadAddr: ��ȡ��ַ(��ϢID)
*              RegNum: �Ĵ�������
* ���ؽ��   : ��
* ע���˵�� : �Ĵ�����ַ����Խ��
* �޸�����   :
*/
static void ModBus_ReadPreHandle(ModBusBaseParam_TypeDef *ModBusBaseParam, uint16_t *ReadAddr, uint16_t *RegNum)
{
    //�Ĵ�����ַ���ֽ�
    *ReadAddr = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[2];
    *ReadAddr <<= 8;
    //�Ĵ�����ַ���ֽ�
    *ReadAddr |= ModBusBaseParam->ModBus_TX_RX.Receive_Buf[3];

    //�Ĵ������ݸ��ֽ�
    *RegNum = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[4];
    *RegNum <<= 8;
    //�Ĵ����������ֽ�
    *RegNum |= ModBusBaseParam->ModBus_TX_RX.Receive_Buf[5];

    ModBusBaseParam->ModBus_TX_RX.Send_Len = 0;
    //�豸��ַ
    ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
        = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[0];
    //������
    ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
        = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[1];
    //�����ֽ���
    ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
        = *RegNum * 2;
}

/**
* ����       : ModBus_ModBus_ReadPreHandlePreHandle()
* ��������   : 2020-05-07
* ����       : ���
* ����       : Modbus дԤ����,�ӽ��յ�ԭʼ��������ȡ����ϢID,���豸��ַ��
*              �����롢��ϢID����Ϣ���ȴ��뷢�ͻ�����
* �������   : ModBusBaseParam: ModBus����Ļ��������ṹ��;
*              arg:�û��Զ���Ĳ���,����Ϊ�豸����
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : �Ĵ�����ַ����Խ��
* �޸�����   :
*/
static void ModBus_WritePreHandle(ModBusBaseParam_TypeDef *ModBusBaseParam, uint16_t *WriteAddr)
{
    uint8_t Nr;
    //�Ĵ�����ַ���ֽ�
    *WriteAddr = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[2];
    *WriteAddr <<= 8;
    //�Ĵ�����ַ���ֽ�
    *WriteAddr |= ModBusBaseParam->ModBus_TX_RX.Receive_Buf[3];

    //����Ӧ���ݴ��뷢�ͻ���
    for(Nr = 0; Nr < 6; Nr++)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[Nr] 
            = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[Nr];
    }
    ModBusBaseParam->ModBus_TX_RX.Send_Len = 6;
}

/**
* ����       : ModbusFunc03()
* ��������   : 2020-04-29
* ����       : ���
* ����       : Modbus 03��������Ϣ֡����
* �������   : ModBusBaseParam: ModBus����Ļ��������ṹ��;
*              arg:�û��Զ���Ĳ���,�����豸����
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   :
*/
void ModbusFunc03(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    uint16_t Nr;
    //�Ĵ�������
    uint16_t DataBuf;
    //�Ĵ�����ַ
    uint16_t ReadAddr;
    //�Ĵ�������
    uint16_t RegNum;
    //�豸����
    ModBus_Device_Param *Device_Param;
        
    Device_Param = (ModBus_Device_Param *)arg;
    //��Ԥ����
    ModBus_ReadPreHandle(ModBusBaseParam, &ReadAddr, &RegNum);
    //���ʵ�ַ������Ч��Χ��
    if(!((ReadAddr >= HOLDING_REG_REGION1_BGEIN) && ((ReadAddr + RegNum) <= (HOLDING_REG_REGION1_END + 1))))
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        //���ʵ�ַ��Ч
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
        return;
    }

    for(Nr = 0; Nr < RegNum; Nr++)
    {
        switch(ReadAddr)
        {
        case 0x0030:
            //�豸��ַ
            DataBuf = ModBusBaseParam->Device_Addr;
        break;
                
        case 0x0031:
            //������
            DataBuf = ModBusBaseParam->BaudRate;
        break;
                
        case 0x0032:
            //��żУ��
            DataBuf = ModBusBaseParam->Parity;
        break;

        case 0x0033:
            //��
            DataBuf = 0;
        break;

        case 0x0034:
            //����ʹ��
            DataBuf = Device_Param->PCap_DataConvert->CompenEn;
        break;

        case 0x0035:
            //�˲�ϵ��
            DataBuf = Device_Param->DataFilter->FilterFactor;
        break;

        case 0x0036:
            //�Զ��ϴ�����
            DataBuf = ModBusBaseParam->AutoUpload;
        break;

        case 0x0037:
            //��������ϵ��K
            DataBuf = (uint16_t)(Device_Param->PCap_DataConvert->Correct_K * 100);
        break;

        case 0x0038:
            //��������ϵ��B
            DataBuf = (uint16_t)Device_Param->PCap_DataConvert->Correct_B + 100;
        break;

        case 0x0039:
            //�����ʽ ����
            DataBuf = ModBusBaseParam->Output_Mode;
        break;

        case 0x003F:
            //���� ����
            DataBuf = Device_Param->PCap_DataConvert->HeightRange;
        break;

        case 0x0040:
            //��
            DataBuf = 0;
        break;

        case 0x0041:
            //��
            DataBuf = 0;
        break;

        default:
            DataBuf = 0;
        break;
        }
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
            = (unsigned char)(DataBuf >> 8);
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
            = (unsigned char)DataBuf;
        ReadAddr++;
    }
}

/**
* ����       : ModbusFunc04()
* ��������   : 2020-04-29
* ����       : ���
* ����       : Modbus 04��������Ϣ֡����
* �������   : ModBusBaseParam: ModBus����Ļ��������ṹ��;
*              arg:�û��Զ���Ĳ���,�����豸����
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   :
*/
void ModbusFunc04(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    //��������
    float fbuf;
    uint16_t Nr;
    //�Ĵ�����ַ
    uint16_t ReadAddr;
    //�Ĵ�������
    uint16_t RegNum;
    //�Ĵ�������
    uint32_t DataBuf;
    //�豸����
    ModBus_Device_Param *Device_Param;
        
    Device_Param = (ModBus_Device_Param *)arg;
    //��Ԥ����
    ModBus_ReadPreHandle(ModBusBaseParam, &ReadAddr, &RegNum);
    //�Ĵ�����ַ��Ч
    if((!(((ReadAddr + RegNum) <= (INPUT_REG_REGION1_END + 2)) 
        || ((ReadAddr == INPUT_REG_REGION2_BEGIN) 
        && ((ReadAddr + RegNum) <= (INPUT_REG_REGION2_END + 2))))
        || (0 != (ReadAddr&0x0001)))&&(0 != (RegNum&0x0001)))
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
        return;
    }

    for(Nr = 0; Nr < RegNum; Nr += 2)
    {
        switch(ReadAddr)
        {
        case 0x0000:
            //Һλ�߶�ADֵ
            fbuf = (float)Device_Param->PCap_DataConvert_Out->LiquidHeightAD;
            DataBuf = *(unsigned long *)&fbuf;
        break;
            
        case 0x0002:
            //�����¶�
            DataBuf = *(unsigned long *)&Device_Param->ADC_TemperOut->TemperInAir;
        break;
            
        case 0x0004:
            //Һ���¶� ����
            DataBuf = *(unsigned long *)&Device_Param->ADC_TemperOut->TemperInLiquid;
        break;

        case 0x0080:
            //PCapԭʼֵ
            fbuf = (float)Device_Param->PCap_DataConvert_Out->PCap_ResultValue;
            DataBuf = *(unsigned long *)&fbuf;
        break;

        default:
            DataBuf = 0;
        break;
        }

        ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
            = (DataBuf >> 24);
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
            = (DataBuf >> 16);
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
            = (DataBuf >> 8);
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
            = (unsigned char)DataBuf;
        ReadAddr += 2;
    }
}

/**
* ����       : ModbusFunc05()
* ��������   : 2020-04-29
* ����       : ���
* ����       : Modbus 05��������Ϣ֡����
* �������   : ModBusBaseParam: ModBus����Ļ��������ṹ��;
*              arg:�û��Զ���Ĳ���,�����豸����
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   :
*/
void ModbusFunc05(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    //�Ĵ�����ַ
    uint16_t ReadAddr;
    //�Ĵ�������
    uint16_t DataBuf;
    static uint8_t CalibFlag = CALIB_CLEAR;
    //�豸����
    ModBus_Device_Param *Device_Param;
        
    Device_Param = (ModBus_Device_Param *)arg;
    //дԤ����
    ModBus_WritePreHandle(ModBusBaseParam, &ReadAddr);    
    //��ַ��Ч���ڲ�Flashʹ���޴�
    if(!(((ReadAddr >= SINGLE_COIL_REGION1_BEGIN)&&(ReadAddr <= SINGLE_COIL_REGION1_END))
       /*&&(FLASH_WR_ENABLE == device_obj->ProductParam->InRomWrEn) */|| (0x004A == ReadAddr)))
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }

    //�������ݸ��ֽ�
    DataBuf = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[4];
    DataBuf <<= 8;
    //�������ݵ��ֽ�
    DataBuf |= ModBusBaseParam->ModBus_TX_RX.Receive_Buf[5];

    //����������Ч
    if((0x0000 != DataBuf)&&(0xFF00 != DataBuf))
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_VALU_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }
     
    switch(ReadAddr)
    {
    case 0x004A:                                //Ϊ�˸���Э�����
        //�ڲ��洢��ʹ�ܻ��ֹ
        if(0xFF00 == DataBuf)
        {
            ModBusBaseParam->InRomWrEn = FLASH_WR_ENABLE;
        }
        else
        {
            ModBusBaseParam->InRomWrEn = FLASH_WR_DISABLE;
        }
    break;      
      
    case 0x0050:
        //���ݱ궨
        if(0xFF00 == DataBuf)       //�궨������
        {
            Calib_CapMax = Device_Param->PCap_DataConvert_Out->PCap_ResultValue;
            CalibFlag |= CALIB_CAPMAX_FLAG;
        }
        else                        //�궨���
        {
            Calib_CapMin = Device_Param->PCap_DataConvert_Out->PCap_ResultValue;
            CalibFlag |= CALIB_CAPMIN_FLAG;
        }
        //���ݱ궨��־λ��Ч��д��궨����  
        if(CALIB_CAPEOC_FLAG == CalibFlag)
        {
            MB_Cap_Calibration(arg);
            CalibFlag = CALIB_CLEAR;
        }
    break;

    case 0x0051:
        //�ָ����ݱ궨����ֵ
        if(0xFF00 == DataBuf)
        {
            Calib_CapMin = Device_Param->PCap_DataConvert->CapMinBak;
            Calib_CapMax = Device_Param->PCap_DataConvert->CapMaxBak;
            MB_Cap_Calibration(arg);
        }
    break;

    case 0x0052:
        //�豸�����ⶳ
        if(0xFF00 == DataBuf)
        {
            ModBusBaseParam->Freeze = FREEZE_ENABLE;
        }
        else
        {
            ModBusBaseParam->Freeze = FREEZE_DISABLE;
        }
    break;

    case 0x0053:
        //�ڲ�Flashʹ�ܻ��ֹ
        if(0xFF00 == DataBuf)
        {
            ModBusBaseParam->InRomWrEn = FLASH_WR_ENABLE;
        }
        else
        {
            ModBusBaseParam->InRomWrEn = FLASH_WR_DISABLE;
        }
    break;
            
    default:
    break;
    }	
}

/**
* ����       : ModbusFunc10()
* ��������   : 2020-04-29
* ����       : ���
* ����       : Modbus 10��������Ϣ֡����
* �������   : ModBusBaseParam: ModBus����Ļ��������ṹ��;
*              arg:�û��Զ���Ĳ���,�����豸����
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   :
*/
void ModbusFunc10(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    uint16_t Nr;
    //��������
    uint16_t Index;
    //�Ĵ�����ַ
    uint16_t ReadAddr;
    //�Ĵ�������
    uint16_t RegNum;
    //16λ�����ݴ�
    uint16_t u16temp;
    //8λ��������
    uint8_t u8array[2];
    //�豸����
    ModBus_Device_Param *Device_Param;
        
    Device_Param = (ModBus_Device_Param *)arg;
    
    //�Ĵ����������ֽ�
    RegNum = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[4];
    RegNum <<= 8;
    //�Ĵ����������ֽ�
    RegNum |= ModBusBaseParam->ModBus_TX_RX.Receive_Buf[5];
    //дԤ����
    ModBus_WritePreHandle(ModBusBaseParam, &ReadAddr);    
    //�Ĵ�����ַ��Ч
    if(!((ReadAddr >= MUL_REG_REGION1_BEGIN)&&((ReadAddr + RegNum) <= (MUL_REG_REGION1_END + 1))
       &&(ModBusBaseParam->ModBus_TX_RX.Receive_Buf[6] == (RegNum * 2))))
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }

    //�ڲ�ROM���ʽ�ֹ
    if(FLASH_WR_ENABLE != ModBusBaseParam->InRomWrEn)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_DEVC_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }

    Index = 0;

    for(Nr = 0; Nr < RegNum; Nr++)
    {
        u8array[0] = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + Index];
        u8array[1] = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[8 + Index];
        u16temp = (uint16_t)(ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + Index] << 8) 
            | ModBusBaseParam->ModBus_TX_RX.Receive_Buf[8 + Index];

        switch(ReadAddr)
        {
        case 0x0030:
            //�豸��ַ
            if((0 < ModBusBaseParam->ModBus_TX_RX.Receive_Buf[8 + Index]) 
                && (0xF8 > ModBusBaseParam->ModBus_TX_RX.Receive_Buf[8 + Index]))
            {
                ModBusBaseParam->Device_Addr = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[8 + Index];
                InFlash_Write3T_MultiBytes(DEVICE_ADDR, &ModBusBaseParam->Device_Addr, 1);
            }
        break;
                
        case 0x0031:
            //������
            if(0x06 > ModBusBaseParam->ModBus_TX_RX.Receive_Buf[8 + Index])
            {
                ModBusBaseParam->BaudRate = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[8 + Index];
                InFlash_Write3T_MultiBytes(BAUDRATE, &ModBusBaseParam->BaudRate, 1);
                ModBusBaseParam->ModBus_CallBack = MB_USART_ReInit;
            }
        break;
                
        case 0x0032:
            //��żУ��
            if(0x03 > ModBusBaseParam->ModBus_TX_RX.Receive_Buf[8 + Index])
            {
                ModBusBaseParam->Parity = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[8 + Index];
                InFlash_Write3T_MultiBytes(PARITY, &ModBusBaseParam->Parity, 1);
                ModBusBaseParam->ModBus_CallBack = MB_USART_ReInit;
            }
        break;
          
        case 0x0033:
            //��
        break;
          
        case 0x0034:
            //����ʹ��
            if(2 > ModBusBaseParam->ModBus_TX_RX.Receive_Buf[8 + Index])
            {
                Device_Param->PCap_DataConvert->CompenEn = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[8 + Index];
                InFlash_Write3T_MultiBytes(COMPENSATE, &Device_Param->PCap_DataConvert->CompenEn, 1);
            }
        break;
          
        case 0x0035:
            //�˲�ϵ��
            if(0x0A > ModBusBaseParam->ModBus_TX_RX.Receive_Buf[8 + Index])
            {
                Device_Param->DataFilter->FilterFactor = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[8 + Index];
                InFlash_Write3T_MultiBytes(FILTER, &Device_Param->DataFilter->FilterFactor, 1);
                SwitchCurFilter(Device_Param->DataFilter->FilterFactor, Device_Param->DataFilter);
            }
        break;
          
        case 0x0036:
            //�Զ��ϴ�����
            ModBusBaseParam->AutoUpload = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[8 + Index];
            InFlash_Write3T_MultiBytes(AUTO_UPLOAD, &ModBusBaseParam->AutoUpload, 1);
        break;
          
        case 0x0037:
            //��������ϵ��K
            if(0 < u16temp)
            {
                Device_Param->PCap_DataConvert->Correct_K = (float)u16temp / 100.0;
                InFlash_Write3T_MultiBytes(CORRECT_K, (uint8_t const *)u8array, 2);
            }
        break;
          
        case 0x0038:
            //��������ϵ��B
            Device_Param->PCap_DataConvert->Correct_B = (float)u16temp;
            InFlash_Write3T_MultiBytes(CORRECT_B, (uint8_t const *)u8array, 2);
        break;
          
        case 0x0039:
            //�����ʽ
            if(2 > ModBusBaseParam->ModBus_TX_RX.Receive_Buf[8 + Index])
            {
                ModBusBaseParam->Output_Mode = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[8 + Index];
                InFlash_Write3T_MultiBytes(OUTPUTMODE, &ModBusBaseParam->Output_Mode, 1);
            }
        break;
          
        case 0x003F:
            //����
            if(u16temp > 0)
            {
                Device_Param->PCap_DataConvert->HeightRange = u16temp;
                InFlash_Write3T_MultiBytes(HEIGHTRANGE, (uint8_t const *)u8array, 2);
            }
        break;
          
        case 0x0040:
            //��
        break;
          
        case 0x0041:
            //��
        break;
          
        default:
            
        break;
        }     
            
        Index += 2;
        ReadAddr++;
    }
}

/**
* ����       : ModbusFunc25()
* ��������   : 2020-04-29
* ����       : ���
* ����       : Modbus 25��������Ϣ֡����
* �������   : ModBusBaseParam: ModBus����Ļ��������ṹ��;
*              arg:�û��Զ���Ĳ���,�����豸����
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   :
*/
/*void ModbusFunc25(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    //�Ĵ�����ַ
    uint16_t ReadAddr;
    //��������
    uint16_t DataBuf;
    //�豸����
    ModBus_Device_Param *Device_Param;
    static uint8_t CapDA_ClibFlag = CALIB_CLEAR;    
    static uint8_t CapAD_ClibFlag = CALIB_CLEAR;
    static uint8_t CapAD_ClibEn = ADCLIB_DISABLE;
    static uint8_t TempDA_ClibFlag = CALIB_CLEAR;
    static uint8_t TempDA_ClibEn = DAOUTCLIB_DISABLE;
        
    Device_Param = (ModBus_Device_Param *)arg;

    //дԤ����
    ModBus_WritePreHandle(ModBusBaseParam, &ReadAddr);
    //�Ĵ�����ַ��Ч
    if(!((ReadAddr < 0x000A)&&(FLASH_WR_ENABLE == ModBusBaseParam->InRomWrEn)))
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }
    //�Ĵ������ݸ��ֽ�
    DataBuf = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[4];
    DataBuf <<= 8;
    //�Ĵ������ݵ��ֽ�
    DataBuf |= ModBusBaseParam->ModBus_TX_RX.Receive_Buf[5];

    //�Ĵ�������Ч
    if((0x0000 != DataBuf)&&(0xFF00 != DataBuf))
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_VALU_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }

    switch(ReadAddr)
    {
    case 0x0000:
    //��
    break;

    case 0x0001:
        //����DA�궨���¿̶���
        if(DAOUTCLIB_ENABLE == CapDA_ClibEn)
        {
            if(0xFF00 == DataBuf)
            {
                CapDA_ClibFlag |= CALIB_CAPDAHIH_FLAG;
                Calib_CapDAHigh = Device_Param->PCap_DataConvert_Out->PCapDA_ResultValue;
            }
            else
            {
                CapDA_ClibFlag |= CALIB_CAPDALOW_FLAG;
                Calib_CapDALow = Device_Param->PCap_DataConvert_Out->PCapDA_ResultValue;
            }
        }
    break;

    case 0x0002:
        //����DA�궨ʹ��
        if(0xFF00 == DataBuf)
        {
            CapDA_ClibEn = DAOUTCLIB_ENABLE;
        }
        else if(0x0000 == DataBuf)
        {
            CapDA_ClibEn = DAOUTCLIB_DISABLE;
        }

        if(DAOUTCLIB_DISABLE == CapDA_ClibEn && CapDA_ClibFlag == CALIB_CAPDAEOC_FLAG)
        {
            MB_CapDAOut_Calibration(arg);
            CapDA_ClibFlag = CALIB_CLEAR;
        }
    break;

    case 0x0003:
        //����DA�궨���������
        if(DAOUTCLIB_ENABLE == CapDA_ClibEn)
        {
            if(0xFF00 == DataBuf)
            {
                CapDA_ClibFlag |= CALIB_CAPDAMAX_FLAG;
                Calib_CapDAMax = Device_Param->PCap_DataConvert_Out->PCapDA_ResultValue;
            }
            else
            {
                CapDA_ClibFlag |= CALIB_CAPDAMIN_FLAG;
                Calib_CapDAMin = Device_Param->PCap_DataConvert_Out->PCapDA_ResultValue;
            }
        }
    break;

    case 0x0004:
        //�¶ȱ궨ʹ��
        if(0xFF00 == DataBuf)
        {
            TempDA_ClibEn = DAOUTCLIB_ENABLE;
        }
        else
        {
            TempDA_ClibEn = DAOUTCLIB_DISABLE;
        }

        if((DAOUTCLIB_DISABLE == TempDA_ClibEn) && (CALIB_TEMPDAEOC_FLAG == TempDA_ClibFlag))
        {
            MB_TempDAOut_Calibration(arg);
            TempDA_ClibFlag = CALIB_CLEAR;
        }
    break;

    case 0x0005:
        //�¶ȱ궨
        if(DAOUTCLIB_ENABLE == TempDA_ClibEn)
        {
            if(0xFF00 == DataBuf)
            {
                TempDA_ClibFlag |= CALIB_TEMPDAMAX_FLAG;
                Calib_TempDAMax = Device_Param->ADC_TemperOut->TemperInAirAD;
            }
            else
            {
                TempDA_ClibFlag |= CALIB_TEMPDAMIN_FLAG;
                Calib_TempDAMin = Device_Param->ADC_TemperOut->TemperInAirAD;
            }
        }
    break;

    case 0x0007:
        //����AD�궨ʹ��
        if(0xFF00 == DataBuf)
        {
            CapAD_ClibFlag = CALIB_CLEAR;
            CapAD_ClibEn = ADCLIB_ENABLE;
        }
        else
        {
            CapAD_ClibEn = ADCLIB_DISABLE;
        }

        if((ADCLIB_DISABLE == CapAD_ClibEn) && (CALIB_CAPADEOC_FLAG == CapAD_ClibFlag))
        {
            MB_CapAD_Calibration(arg);
            CapAD_ClibFlag = CALIB_CLEAR;
        }
    break;

    case 0x0008:
        //����AD�궨���¿̶���
        if(ADCLIB_ENABLE == CapAD_ClibEn)
        {
            if(0xFF00 == DataBuf)
            {
                Calib_ADHigh = Device_Param->PCap_DataConvert_Out->LiquidHeightAD;
                CapAD_ClibFlag |= CALIB_CAPADHIH_FLAG;
            }
            else
            {
                Calib_ADLow = Device_Param->PCap_DataConvert_Out->LiquidHeightAD;
                CapAD_ClibFlag |= CALIB_CAPADLOW_FLAG;
            }
        }
    break;

    case 0x0009:
        //����AD�궨���������
        if(ADCLIB_ENABLE == CapAD_ClibEn)
        {
            if(0xFF00 == DataBuf)
            {
                Calib_ADMax = Device_Param->PCap_DataConvert_Out->LiquidHeightAD;
                CapAD_ClibFlag |= CALIB_CAPADMAX_FLAG;
            }
            else
            {
                Calib_ADMin = Device_Param->PCap_DataConvert_Out->LiquidHeightAD;
                CapAD_ClibFlag |= CALIB_CAPADMIN_FLAG;
            }
        }
    break;

    default:
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_VALU_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
    break;
    }
}*/

/**
* ����       : ModbusFunc26()
* ��������   : 2020-04-29
* ����       : ���
* ����       : Modbus 26��������Ϣ֡����
* �������   : ModBusBaseParam: ModBus����Ļ��������ṹ��;
*              arg:�û��Զ���Ĳ���,�����豸����
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   :
*/
void ModbusFunc26(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    uint8_t i;
    //��������
    float fBuf;
    uint16_t Nr;
    //�Ĵ�����ַ
    uint16_t ReadAddr;
    //�Ĵ�������
    uint16_t RegNum;
    //�Ĵ�����������
    uint32_t DataBuf;
    //�豸����
    ModBus_Device_Param *Device_Param;
        
    Device_Param = (ModBus_Device_Param *)arg;
    //��Ԥ����
    ModBus_ReadPreHandle(ModBusBaseParam, &ReadAddr, &RegNum);
    //�Ĵ������ʵ�ַ��Ч
    if(!((((DEF_MUL_REG_REGION1_BEGIN <= ReadAddr) && (DEF_MUL_REG_REGION1_END >= ReadAddr) 
        && ((ReadAddr + RegNum) <= (DEF_MUL_REG_REGION1_END + 2))))
     && (0x00 != RegNum) && (0 == (RegNum & 0x01)) && (0 == (ReadAddr & 0x01))))
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }

    for(Nr = 0; Nr < RegNum; Nr += 2, ReadAddr += 2)
    {
        switch(ReadAddr)
        {
        case 0x0080:
            fBuf = (float)Device_Param->PCap_DataConvert->CapMax;                                       //����������
            DataBuf = *(uint32_t *)&fBuf;
        break;

        case 0x0082:
            fBuf = (float)Device_Param->PCap_DataConvert->CapMin;                                       //�������
            DataBuf = *(uint32_t *)&fBuf;
        break;

        case 0x0084:     
            fBuf = (float)Device_Param->PCap_DataConvert->CapDAMax;                                     //DA����
            DataBuf = *(uint32_t *)&fBuf;
        break;
          
        case 0x0086:
            fBuf = (float)Device_Param->PCap_DataConvert->CapDAHigh;                                     //DA�Ͽ̶�
            DataBuf = *(uint32_t *)&fBuf;
        break;
          
        case 0x0088:
            fBuf = (float)Device_Param->PCap_DataConvert->CapDALow;                                     //DA�¿̶�
            DataBuf = *(uint32_t *)&fBuf;
        break;  
          
        case 0x008A:
            fBuf = (float)Device_Param->PCap_DataConvert->CapDAMin;                                     //DA���
            DataBuf = *(uint32_t *)&fBuf;
        break; 
          
        case 0x008C: 
            fBuf = (float)Device_Param->PCap_DataConvert->CapADMax;                                     //AD����
            DataBuf = *(uint32_t *)&fBuf;
        break;

        case 0x008E:
            fBuf = (float)Device_Param->PCap_DataConvert->CapADHigh;                                     //AD�Ͽ̶�
            DataBuf = *(uint32_t *)&fBuf;
        break;       
            
        case 0x0090:
            fBuf = (float)Device_Param->PCap_DataConvert->CapADLow;                                     //AD�¿̶�
            DataBuf = *(uint32_t *)&fBuf;
        break;  
          
        case 0x0092:
            fBuf = (float)Device_Param->PCap_DataConvert->CapADMin;                                      //AD���
            DataBuf = *(uint32_t *)&fBuf;
        break;       
          
        case 0x0094:  
            
        break;
          
        case 0x0096:  
            
        break;

        case 0x0098:  
            
        break;

        case 0x009A:  
            
        break;      
          
        case 0x009C:                                                                
            fBuf = Device_Param->ADC_TemperParam->Temper_K1;                                               //�����¶�����ϵ��K1   
            DataBuf = *(uint32_t *)&fBuf;
        break;
          
        case 0x009E:
            fBuf = Device_Param->ADC_TemperParam->Temper_B1;                                               //�����¶�����ϵ��B1
            DataBuf = *(uint32_t *)&fBuf;
        break;
          
        case 0x00A0:
            fBuf = Device_Param->ADC_TemperParam->Temper_K2;                                               //Һ���¶�����ϵ��K2
            DataBuf = *(uint32_t *)&fBuf;
        break;
          
        case 0x00A2:
            fBuf = Device_Param->ADC_TemperParam->Temper_B2;                                               //Һ���¶�����ϵ��B2
            DataBuf = *(uint32_t *)&fBuf;
        break;
          
        default:
            
        break;
        }

        for(i = 4; i > 0; i--)
        {
            ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
                = (uint8_t)(DataBuf >> ((i - 1)*8));
        }
    }
}

/**
* ����       : ModbusFunc27()
* ��������   : 2020-04-29
* ����       : ���
* ����       : Modbus 27��������Ϣ֡����
* �������   : ModBusBaseParam: ModBus����Ļ��������ṹ��;
*              arg:�û��Զ���Ĳ���,�����豸����
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   :
*/
void ModbusFunc27(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    //��������
    float fbuf;
    uint16_t Nr;
    //����
    uint16_t index;
    //�Ĵ�����ַ
    uint16_t ReadAddr;
    //�Ĵ�������
    uint16_t RegNum;
    //�豸����
    ModBus_Device_Param *Device_Param;
    uint8_t Cur_Param[4];
            
    //uint16_t DA_BakHih, DA_BakLow;
          
    Device_Param = (ModBus_Device_Param *)arg;    
    //дԤ����
    ModBus_WritePreHandle(ModBusBaseParam, &ReadAddr);  
    //�ڲ�ROMδʹ�ܷ���ʧ��
    if(FLASH_WR_ENABLE != ModBusBaseParam->InRomWrEn)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_DEVC_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }
    //�Ĵ����������ֽ�
    RegNum = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[4];
    RegNum <<= 8;
    //�Ĵ����������ֽ�
    RegNum |= ModBusBaseParam->ModBus_TX_RX.Receive_Buf[5];
    //�Ĵ������ʵ�ַ��Ч
    if(!((((DEF_MUL_REG_REGION1_BEGIN <= ReadAddr) && (DEF_MUL_REG_REGION1_END >= ReadAddr) 
        && ((ReadAddr + RegNum) <= (DEF_MUL_REG_REGION1_END + 2))))
        && (0x00 != RegNum) && (0 == (RegNum & 0x01)) && (0 == (ReadAddr & 0x01))))
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }
    index = 0;
    for(Nr = 0; Nr < RegNum; Nr += 2, ReadAddr += 2)
    {
        switch(ReadAddr)
        {
        case 0x0080:
            fbuf = HexToFloat(&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + index]);
            Device_Param->PCap_DataConvert->CapMax = (uint32_t)fbuf;
            Device_Param->DataFilter->InputRangeMax = (uint32_t)fbuf;
            Cur_Param[0] = (uint8_t)(Device_Param->PCap_DataConvert->CapMax >> 24);
            Cur_Param[1] = (uint8_t)(Device_Param->PCap_DataConvert->CapMax >> 16);;       
            Cur_Param[2] = (uint8_t)(Device_Param->PCap_DataConvert->CapMax >> 8);
            Cur_Param[3] = (uint8_t)Device_Param->PCap_DataConvert->CapMax;   
            InFlash_Write3T_MultiBytes(CAPMAX, Cur_Param, 4);
        break;
          
        case 0x0082:
            fbuf = HexToFloat(&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + index]);
            Device_Param->PCap_DataConvert->CapMin = (uint32_t)fbuf;
            Device_Param->DataFilter->InputRangeMin = (uint32_t)fbuf;
            Cur_Param[0] = (uint8_t)(Device_Param->PCap_DataConvert->CapMin >> 24);
            Cur_Param[1] = (uint8_t)(Device_Param->PCap_DataConvert->CapMin >> 16);;       
            Cur_Param[2] = (uint8_t)(Device_Param->PCap_DataConvert->CapMin >> 8);
            Cur_Param[3] = (uint8_t)Device_Param->PCap_DataConvert->CapMin;  
            InFlash_Write3T_MultiBytes(CAPMIN, Cur_Param, 4);
        break;

        case 0x0084:
            fbuf = HexToFloat(&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + index]);
            if(fbuf <= PCAP_DAC_MAX_VALUE && fbuf >= PCAP_DAC_MIN_VALUE)
            {
                Device_Param->PCap_DataConvert->CapDAMax = (uint16_t)fbuf;
                Cur_Param[0] = (uint8_t)(Device_Param->PCap_DataConvert->CapDAMax >> 8);
                Cur_Param[1] = (uint8_t)Device_Param->PCap_DataConvert->CapDAMax;               
                InFlash_Write3T_MultiBytes(CAPDAMAX, Cur_Param, 2);
            }            
        break;
               
        case 0x0086:
            fbuf = HexToFloat(&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + index]);
            if(fbuf <= PCAP_DAC_MAX_VALUE && fbuf >= PCAP_DAC_MIN_VALUE)
            {
                Device_Param->PCap_DataConvert->CapDAHigh = (uint16_t)fbuf;
                Cur_Param[0] = (uint8_t)(Device_Param->PCap_DataConvert->CapDAHigh >> 8);
                Cur_Param[1] = (uint8_t)Device_Param->PCap_DataConvert->CapDAHigh;               
                InFlash_Write3T_MultiBytes(CAPDAHIGH, Cur_Param, 2); 
            }
        break;
          
        case 0x0088:
            fbuf = HexToFloat(&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + index]);
            if(fbuf <= PCAP_DAC_MAX_VALUE && fbuf >= PCAP_DAC_MIN_VALUE)
            {
                Device_Param->PCap_DataConvert->CapDALow = (uint16_t)fbuf;
                Cur_Param[0] = (uint8_t)(Device_Param->PCap_DataConvert->CapDALow >> 8);
                Cur_Param[1] = (uint8_t)Device_Param->PCap_DataConvert->CapDALow;               
                InFlash_Write3T_MultiBytes(CAPDALOW, Cur_Param, 2); 
            }      
        break;
              
        case 0x008A:
            fbuf = HexToFloat(&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + index]);
            if(fbuf <= PCAP_DAC_MAX_VALUE && fbuf >= PCAP_DAC_MIN_VALUE)
            {
                Device_Param->PCap_DataConvert->CapDAMin = (uint16_t)fbuf;
                Cur_Param[0] = (uint8_t)(Device_Param->PCap_DataConvert->CapDAMin >> 8);
                Cur_Param[1] = (uint8_t)Device_Param->PCap_DataConvert->CapDAMin;               
                InFlash_Write3T_MultiBytes(CAPDAMIN, Cur_Param, 2);
            }     
        break; 
          
        case 0x008C:
            fbuf = HexToFloat(&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + index]);
            if(fbuf <= PCAP_ADC_MAX_VALUE && fbuf >= PCAP_ADC_MIN_VALUE)
            {
                Device_Param->PCap_DataConvert->CapADMax = (uint16_t)fbuf;
                Cur_Param[0] = (uint8_t)(Device_Param->PCap_DataConvert->CapADMax >> 8);
                Cur_Param[1] = (uint8_t)Device_Param->PCap_DataConvert->CapADMax;               
                InFlash_Write3T_MultiBytes(CAPADMAX, Cur_Param, 2);      
            }            
        break; 
               
        case 0x008E:
            fbuf = HexToFloat(&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + index]);
            if(fbuf <= PCAP_ADC_MAX_VALUE && fbuf >= PCAP_ADC_MIN_VALUE)
            {
                Device_Param->PCap_DataConvert->CapADHigh = (uint16_t)fbuf;
                Cur_Param[0] = (uint8_t)(Device_Param->PCap_DataConvert->CapADHigh >> 8);
                Cur_Param[1] = (uint8_t)Device_Param->PCap_DataConvert->CapADHigh;               
                InFlash_Write3T_MultiBytes(CAPADHIGH, Cur_Param, 2);       
            }                
        break; 
          
        case 0x0090:
            fbuf = HexToFloat(&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + index]);
            if(fbuf <= PCAP_ADC_MAX_VALUE && fbuf >= PCAP_ADC_MIN_VALUE)
            {
                Device_Param->PCap_DataConvert->CapADLow = (uint16_t)fbuf;
                Cur_Param[0] = (uint8_t)(Device_Param->PCap_DataConvert->CapADLow >> 8);
                Cur_Param[1] = (uint8_t)Device_Param->PCap_DataConvert->CapADLow;               
                InFlash_Write3T_MultiBytes(CAPADLOW, Cur_Param, 2);           
            }
        break; 
               
        case 0x0092:
            fbuf = HexToFloat(&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + index]);
            if(fbuf <= PCAP_ADC_MAX_VALUE && fbuf >= PCAP_ADC_MIN_VALUE)
            {
                Device_Param->PCap_DataConvert->CapADMin = (uint16_t)fbuf;
                Cur_Param[0] = (uint8_t)(Device_Param->PCap_DataConvert->CapADMin >> 8);
                Cur_Param[1] = (uint8_t)Device_Param->PCap_DataConvert->CapADMin;               
                InFlash_Write3T_MultiBytes(CAPADMIN, Cur_Param, 2);   
            }            
        break;       

        default:
            
        break;
        }
        index += 4;
    }
}

/**
* ����       : ModbusFunc2A()
* ��������   : 2020-04-29
* ����       : ���
* ����       : Modbus 2A��������Ϣ֡����
* �������   : ModBusBaseParam: ModBus����Ļ��������ṹ��;
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   :
*/
void ModbusFunc2A(ModBusBaseParam_TypeDef *ModBusBaseParam)
{
    uint8_t i;
    uint8_t j;
    uint8_t objlen;
    uint8_t RegNum;
    uint16_t ReadAddr;

    RegNum = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[5];
    //дԤ����
    ModBus_WritePreHandle(ModBusBaseParam, &ReadAddr);    
    if(!((ReadAddr >= MUL_VERSION_INF_BEGIN)
        &&(ReadAddr <= MUL_VERSION_INF_END)
        &&((ReadAddr + RegNum) <= (MUL_VERSION_INF_END + 1))
        &&(0 != RegNum)&&(0x00 == ModBusBaseParam->ModBus_TX_RX.Receive_Buf[4])))
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }

    j = 6;
    for(i = 0; i < RegNum; i++)
    {
        switch(ReadAddr)
        {
        //��������
        case 0x00E0:
            objlen = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[j] + 1;
            if(0x2F < objlen)
            {
                j += objlen;
                break;
            }
            InFlash_Write_MultiBytes(ORGANIZATION, 
                (uint8_t const *)&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[j], objlen);                                    
            j += objlen;
        break;
        //��Ʒ����    
        case 0x00E1:
            objlen = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[j] + 1;
            if(0x2F < objlen)
            {
                j += objlen;
                break;
            }
            InFlash_Write_MultiBytes(PRODUCTION, 
                (uint8_t const *)&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[j], objlen);                               
            j += objlen;
        break;
        //Ӳ���汾    
        case 0x00E2:
            objlen = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[j] + 1;
            if(0x2F < objlen)
            {
                j += objlen;
                break;
            }
            InFlash_Write_MultiBytes(HARDWAREVER, 
                (uint8_t const *)&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[j], objlen);                                  
            j += objlen;
        break;
        //����汾    
        case 0x00E3:
            objlen = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[j] + 1;
            //                                                                                                    
            j += objlen;
        break;
        //�豸ID    
        case 0x00E4:
            objlen = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[j] + 1;
            if(0x2F < objlen)
            {
                j += objlen;
                break;
            }
            InFlash_Write_MultiBytes(DEVICENUM, 
                (uint8_t const *)&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[j], objlen);                                     
            j += objlen;
        break;
        //�ͻ�����    
        case 0x00E5:
            objlen = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[j] + 1;
            if(0x2F < objlen)
            {
                j += objlen;
                break;
            }
            InFlash_Write_MultiBytes(CUSTOMER, 
                (uint8_t const *)&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[j], objlen);                               
            j += objlen;
        break;
            
        default:
            
        break;
        }
        ReadAddr++;
    }
}

/**
* ����       : ModbusFunc2B()
* ��������   : 2020-04-29
* ����       : ���
* ����       : Modbus 2B��������Ϣ֡����
* �������   : ModBusBaseParam: ModBus����Ļ��������ṹ��;
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   :
*/
void ModbusFunc2B(ModBusBaseParam_TypeDef *ModBusBaseParam)
{
    uint8_t i;
    uint8_t objlen;
    uint16_t RegNum;
    uint16_t ReadAddr;

    //��Ԥ����
    ModBus_ReadPreHandle(ModBusBaseParam, &ReadAddr, &RegNum);
    ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = RegNum >> 8;
    ModBusBaseParam->ModBus_TX_RX.Send_Buf[3] = RegNum & 0xFF;
    
    if(!((ReadAddr >= MUL_VERSION_INF_BEGIN)
        &&(ReadAddr <= MUL_VERSION_INF_END)
        &&((ReadAddr + RegNum) <= (MUL_VERSION_INF_END + 1))
        &&(0 != RegNum)&&(0x00 == ModBusBaseParam->ModBus_TX_RX.Receive_Buf[4])))
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }
    memcpy(&ModBusBaseParam->ModBus_TX_RX.Send_Buf[4], &ModBusBaseParam->ModBus_TX_RX.Receive_Buf[2], 2);
    ModBusBaseParam->ModBus_TX_RX.Send_Len = 6;
    for(i = 0; i < RegNum; i++)
    {
        switch(ReadAddr)
        {
        //��������  
        case 0x00E0:
            objlen = InFlash_Read_OneByte(ORGANIZATION);                                                       
            ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = objlen;
            if((objlen > (SEND_SIZE / 3))||(0 == objlen))
            {
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len - 1] = 1;
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = 0;
                break;
            }
            InFlash_Read_MultiBytes((ORGANIZATION + 1), 
                &ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len], objlen);
            ModBusBaseParam->ModBus_TX_RX.Send_Len += objlen;
        break;
        //��Ʒ����  
        case 0x00E1:
            objlen = InFlash_Read_OneByte(PRODUCTION);                                                  
            ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = objlen;
            if((objlen > (SEND_SIZE / 3))||(0 == objlen))
            {
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len - 1] = 1;
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = 0;
                break;
            }
            InFlash_Read_MultiBytes((PRODUCTION + 1), 
                &ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len], objlen);
            ModBusBaseParam->ModBus_TX_RX.Send_Len += objlen;
        break;
        //Ӳ���汾  
        case 0x00E2:
            objlen = InFlash_Read_OneByte(HARDWAREVER);                                                     
            ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = objlen;
            if((objlen > (SEND_SIZE / 3))||(0 == objlen))
            {
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len - 1] = 1;
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = 0;
                break;
            }
            InFlash_Read_MultiBytes((HARDWAREVER + 1), 
                &ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len], objlen);
            ModBusBaseParam->ModBus_TX_RX.Send_Len += objlen;
        break;
        //����汾 
        case 0x00E3:                                                    
            ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = SensorSoftVersion[0];

            for(objlen = 0; objlen < 7; objlen++)                                                        
            {
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
                    = SensorSoftVersion[objlen + 1];
            }
        break;
        //�豸ID  
        case 0x00E4:
            objlen = InFlash_Read_OneByte(DEVICENUM);                                                        
            ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = objlen;
            if((objlen > (SEND_SIZE / 3))||(0 == objlen))
            {
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len - 1] = 1;
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = 0;
                break;
            }
            InFlash_Read_MultiBytes((DEVICENUM + 1), 
                &ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len], objlen);
            ModBusBaseParam->ModBus_TX_RX.Send_Len += objlen;
        break;
        //�ͻ�����  
        case 0x00E5:
            objlen = InFlash_Read_OneByte(CUSTOMER);                                                  
            ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = objlen;
            if((objlen > (SEND_SIZE / 3))||(0 == objlen))
            {
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len - 1] = 1;
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = 0;
                break;
            }
            InFlash_Read_MultiBytes((CUSTOMER + 1), 
                &ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len], objlen);
            ModBusBaseParam->ModBus_TX_RX.Send_Len += objlen;
        break;

        default:
            ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
            ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
            ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        break;
        }
        ReadAddr++;
    }
}

/**
* ����       : ModbusFunc41()
* ��������   : 2020-04-29
* ����       : ���
* ����       : Modbus 41��������Ϣ֡����
* �������   : ModBusBaseParam: ModBus����Ļ��������ṹ��;
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   :
*/
void ModbusFunc41(ModBusBaseParam_TypeDef *ModBusBaseParam)
{
    uint16_t temp[2];
    
    temp[0] = ((uint16_t)ModBusBaseParam->ModBus_TX_RX.Receive_Buf[2] << 8) 
                | ModBusBaseParam->ModBus_TX_RX.Receive_Buf[3];
    temp[1] = ((uint16_t)ModBusBaseParam->ModBus_TX_RX.Receive_Buf[4] << 8) 
                | ModBusBaseParam->ModBus_TX_RX.Receive_Buf[5];
    if(temp[0] != 0x0001 || temp[1] != 0x0000)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[0] = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[0];
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] = 0x41 | MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_VALU_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
    }
    else
    {
        memcpy(ModBusBaseParam->ModBus_TX_RX.Send_Buf, ModBusBaseParam->ModBus_TX_RX.Receive_Buf, 5);
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[5] = 0x01;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[6] = 0x00;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 7;
        MDB_Snd_Msg_RTU(ModBusBaseParam, 
            ModBusBaseParam->ModBus_TX_RX.Send_Buf, ModBusBaseParam->ModBus_TX_RX.Send_Len);
        HAL_Delay(10);
        HAL_NVIC_SystemReset();
    }
}

/**
* ����       : ModbusAutoUpload()
* ��������   : 2020-04-29
* ����       : ���
* ����       : Modbus ��Ϣ֡�Զ��ϴ�����
* �������   : ModBusBaseParam: ModBus����Ļ��������ṹ��;
*              arg:�û��Զ���Ĳ���,�����豸����
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : 
* �޸�����   :
*/
void ModbusAutoUpload(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    float fbuf;
    uint8_t Nr;
    uint32_t DataBuf;
    //�豸����
    ModBus_Device_Param *Device_Param;
        
    Device_Param = (ModBus_Device_Param *)arg;

    ModBusBaseParam->ModBus_TX_RX.Send_Buf[0] = ModBusBaseParam->Device_Addr;
    ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] = 0x04;
    ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = 0x08;

    ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
    fbuf = (float)Device_Param->PCap_DataConvert_Out->LiquidHeightAD;
    DataBuf = *(unsigned long *)&fbuf;
    for(Nr = 4; Nr > 0; Nr--)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
            = (uint8_t)(DataBuf >> ((Nr - 1)*8));
    }

    ModBusBaseParam->ModBus_TX_RX.Send_Len = 7;
    DataBuf = *(unsigned long *)&Device_Param->ADC_TemperOut->TemperInAir;
    for(Nr = 4; Nr > 0; Nr--)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
            = (uint8_t)(DataBuf >> ((Nr - 1)*8));
    }
    MDB_Snd_Msg_RTU_NoLimit(ModBusBaseParam, 
        ModBusBaseParam->ModBus_TX_RX.Send_Buf, ModBusBaseParam->ModBus_TX_RX.Send_Len);
}


/**
* ����       : MB_Cap_Calibration()
* ��������   : 2020-05-06
* ����       : ׯ��Ⱥ
* ����       : ���ݱ궨����
* �������   : arg:�û��Զ���Ĳ���,����Ϊ�豸����
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� :
* �޸�����   :
*/
void MB_Cap_Calibration(void *arg)
{
    uint8_t Cur_Param[8];
    //�豸����
    ModBus_Device_Param *Device_Param;
        
    Device_Param = (ModBus_Device_Param *)arg;
    
    if(Calib_CapMin < Calib_CapMax)
    {
        //���������뻺��
        Cur_Param[0] = (uint8_t)(Calib_CapMin >> 24);
        Cur_Param[1] = (uint8_t)(Calib_CapMin >> 16);
        Cur_Param[2] = (uint8_t)(Calib_CapMin >> 8);
        Cur_Param[3] = (uint8_t)Calib_CapMin;
        //����������뻺��
        Cur_Param[4] = (uint8_t)(Calib_CapMax >> 24);
        Cur_Param[5] = (uint8_t)(Calib_CapMax >> 16);
        Cur_Param[6] = (uint8_t)(Calib_CapMax >> 8);
        Cur_Param[7] = (uint8_t)Calib_CapMax;
        //�������
        Device_Param->PCap_DataConvert->CapMin = Calib_CapMin;
        //��������
        Device_Param->PCap_DataConvert->CapMax = Calib_CapMax;
        
        Device_Param->DataFilter->InputRangeMin = Calib_CapMin;
        Device_Param->DataFilter->InputRangeMax = Calib_CapMax;
        //���ݱ궨����д���ڲ�Flash
        InFlash_Write3T_MultiBytes(CAPMIN, Cur_Param, 8);
        //����������㱸��ֵ��ȱʡֵ��д��궨ֵ
        if(0xFFFFFFFF == Device_Param->PCap_DataConvert->CapMaxBak)
        {
            //���ݱ�������д���ڲ�Flash
            InFlash_Write3T_MultiBytes(CAPMINBAK, Cur_Param, 8);
            //������㱸��
            Device_Param->PCap_DataConvert->CapMinBak = Calib_CapMin;
            //�������㱸��
            Device_Param->PCap_DataConvert->CapMaxBak = Calib_CapMax;
        }
    }
    //Device_Param->PCap_DataConvert->CapRange = Device_Param->PCap_DataConvert->CapMax - Device_Param->PCap_DataConvert->CapMin;
    //����Һλ������ֵΪ���̵İٷ�֮��
    //Device_Param->PCap_DataConvert->CapWaveLimit = (uint32_t)(Device_Param->PCap_DataConvert->CapRange * 0.03);
}

/**
* ����       : MB_CapAD_Calibration()
* ��������   : 2020-05-06
* ����       : ���
* ����       : ����ADֵ�궨����
* �������   : arg:�û��Զ���Ĳ���,����Ϊ�豸����
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : �궨ֵ���ܳ���65535
* �޸�����   :
*/
void MB_CapAD_Calibration(void *arg)
{
    uint8_t Cur_Param[8];
    //�豸����
    ModBus_Device_Param *Device_Param;
        
    Device_Param = (ModBus_Device_Param *)arg;
    //�������AD�궨ֵ��Ч��д���ڲ�Flash
    if((Calib_ADMin < Calib_ADLow) && (Calib_ADLow < Calib_ADHigh) && (Calib_ADHigh < Calib_ADMax))
    {
        Cur_Param[0] = (uint8_t)(Calib_ADMin >> 8);
        Cur_Param[1] = (uint8_t)Calib_ADMin;
        Cur_Param[2] = (uint8_t)(Calib_ADLow >> 8);
        Cur_Param[3] = (uint8_t)Calib_ADLow;
        Cur_Param[4] = (uint8_t)(Calib_ADHigh >> 8);
        Cur_Param[5] = (uint8_t)Calib_ADHigh;
        Cur_Param[6] = (uint8_t)(Calib_ADMax >> 8);
        Cur_Param[7] = (uint8_t)Calib_ADMax;

        Device_Param->PCap_DataConvert->CapADMin = Calib_ADMin;
        Device_Param->PCap_DataConvert->CapADLow = Calib_ADLow;
        Device_Param->PCap_DataConvert->CapADHigh = Calib_ADHigh;
        Device_Param->PCap_DataConvert->CapADMax = Calib_ADMax;
        InFlash_Write3T_MultiBytes(CAPADMIN, Cur_Param, 8);
    }
}

/**
* ����       : MB_CapDAOut_Calibration()
* ��������   : 2020-05-06
* ����       : ���
* ����       : ����DAֵ�궨����
* �������   : arg:�û��Զ���Ĳ���,����Ϊ�豸����
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : �궨ֵ���ܳ���4095
* �޸�����   :
*/
void MB_CapDAOut_Calibration(void *arg)
{
    uint8_t Cur_Param[8];
    //�豸����
    ModBus_Device_Param *Device_Param;
    
    Device_Param = (ModBus_Device_Param *)arg;
    if((Calib_CapDAMax > Calib_CapDAHigh) && (Calib_CapDAHigh > Calib_CapDALow) 
        && (Calib_CapDALow > Calib_CapDAMin))
    {                        
        Cur_Param[0] = (uint8_t)(Calib_CapDAMin >> 8);
        Cur_Param[1] = (uint8_t)Calib_CapDAMin;
        Cur_Param[2] = (uint8_t)(Calib_CapDALow >> 8);
        Cur_Param[3] = (uint8_t)Calib_CapDALow;
        Cur_Param[4] = (uint8_t)(Calib_CapDAHigh >> 8);
        Cur_Param[5] = (uint8_t)Calib_CapDAHigh;
        Cur_Param[6] = (uint8_t)(Calib_CapDAMax >> 8);
        Cur_Param[7] = (uint8_t)Calib_CapDAMax;
        InFlash_Write3T_MultiBytes(CAPDAMIN, Cur_Param, 8);
        
        Device_Param->PCap_DataConvert->CapDAMin = Calib_CapDAMin;
        Device_Param->PCap_DataConvert->CapDALow = Calib_CapDALow;
        Device_Param->PCap_DataConvert->CapDAHigh = Calib_CapDAHigh;
        Device_Param->PCap_DataConvert->CapDAMax = Calib_CapDAMax;
    }
}

/**
* ����       : MB_TempDAOut_Calibration()
* ��������   : 2020-05-06
* ����       : ���
* ����       : �¶�DAֵ�궨����
* �������   : arg:�û��Զ���Ĳ���,����Ϊ�豸����
* �������   : ��
* ���ؽ��   : ��
* ע���˵�� : �궨ֵ���ܳ���4095
* �޸�����   :
*/
void MB_TempDAOut_Calibration(void *arg)
{
    uint8_t Cur_Param[4];
    //�豸����
    ModBus_Device_Param *Device_Param;
    
    Device_Param = (ModBus_Device_Param *)arg;
    //����¶�DA�궨ֵ��Ч��д���ڲ�Flash
    if((Calib_TempDAMax > Calib_TempDAMin) && (DAC_VALUE_MAX > Calib_TempDAMax) && (DAC_VALUE_MAX > Calib_TempDAMin))
    {
        Cur_Param[0] = (unsigned char)(Calib_TempDAMin >> 8);
        Cur_Param[1] = (unsigned char)Calib_TempDAMin;
        Cur_Param[2] = (unsigned char)(Calib_TempDAMax >> 8);
        Cur_Param[3] = (unsigned char)Calib_TempDAMax;
        InFlash_Write3T_MultiBytes(TEMPDAMIN, Cur_Param, 4);

        Device_Param->ADC_TemperParam->TempDAMin = Calib_TempDAMin;
        Device_Param->ADC_TemperParam->TempDAMax = Calib_TempDAMax;
        Device_Param->ADC_TemperParam->TempDARange = Device_Param->ADC_TemperParam->TempDAMax 
                                                - Device_Param->ADC_TemperParam->TempDAMin;
    }
}

/**
* ����       : MB_USART_ReInit()
* ��������   : 2020-05-08
* ����       : ���
* ����       : Modbus �������³�ʼ��
* �������   : ModBusBaseParam: ModBus����Ļ��������ṹ��;  
* �������   : ��
* ���ؽ��   : int����ֵ
* ע���˵�� : ��������ModBus��Ӧ��λ����,����
* �޸�����   :
*/
static int MB_USART_ReInit(ModBusBaseParam_TypeDef *ModBusBaseParam)
{        
    Sensor_USART_Init(  ModBusBaseParam->BaudRate, 
                        ModBusBaseParam->Parity, 
                        ModBusBaseParam->ModBus_TX_RX.USART_Receive_Buf, 
                        ModBusBaseParam->ModBus_TX_RX.USART_Receive_Buf_Len);
    
    return OP_SUCCESS;
}

#endif
