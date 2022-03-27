//2019-06-13  �α����
//

//�α����ͣ�ShapeChange_type=4��  + һ·ADC    +   һ·ADC     ��ص���      RSSI
//      1���ֽ�                    2���ֽ�        2���ֽ�      2���ֽ�      һ���ֽ�
//                                 Buffer[0]      Buffer[2]    Buffer[4]
//                                 Buffer[1]      Buffer[3]    Buffer[5]   Buffer[6]
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"
#include "stm32l4xx_hal.h"
#include "sx1280_app.h"
#include "hw.h"
#include "radio.h"
#include "sx1280.h"
#include "protocol.h"
#include "flash.h"
#include "adc.h"
#include "rtc.h"
#include "iwdg.h"
#include "rng.h"

uint64_t default_sensor_id=0;
uint64_t sensor_id = 0x1000001;//0xB5C8DD94
uint8_t otp_sensor_id_buf[6]= {0};
uint8_t sensor_id_buf[6]= {0x1D,0x9C,0x08,0x2A,0xFF,0xFC}; // {0xA9,0x2E,0x0D,0x00,0x00,0x01};
#define FLASH_OTP_ADDR ((uint32_t)0x1FFF7000)

/*�궨��*/
#define   start_up_time 16 //(����ֵ)ֵԽС������Խ��
#define ALARM_TEMPERATURE 0x0AF8B //75 degree
#define RX_TIMEOUT_TICK_SIZE	RADIO_TICK_SIZE_1000_US
#define MODE_LORA

/*32λ*/
uint32_t BURST_Count_down=0;
volatile uint32_t MainRun_Time=0;     //ƫ��ʱ�� ms��λ
uint32_t sx1280_receive_time=0;//�ڵ���ʱ����ط����и�ֵ�ݼ���
uint32_t RTC_SleepTime =0;

/*16λ*/
__IO uint16_t uhADCxRefValue=0, uhADCxConvertedValue = 0;
float PA0_ADCxRValue=0, PA2_ADCxRValue = 0;
uint16_t Rssi=0;
uint16_t temperatrue=0;

/*8λ*/
uint8_t Buffer[15]= {0x00};
uint8_t RspAckbuff[50]= {0x00};
uint8_t WriteBuffer[10],ReadBuffer[10];
uint8_t	Send_BURST_Count=3;//�ظ�����BURST��������
uint8_t Send_Message_Flag=0;
uint8_t Send_REQ_Flag=0;
uint8_t	Send_BURST_Flag=0;
uint8_t Send_Frame_Type=0;

/*���*/
SPI_HandleTypeDef hspi1;

/*����*/
PacketParams_t packetParams;
PacketStatus_t packetStatus;
HAL_StatusTypeDef hi2c1_status;
int8_t x;
RadioStatus_t s;
float Shift_time=0.0;


/*����*/
void STS31_GetData( void );
void Enter_Shutdown_Mode(void);
uint32_t Handle_RFSendflag(void);
extern uint16_t TxIrqMask;
extern uint32_t Flash_Sensor_ID;
int main( void )
{
    RTC_SleepTime = WakeUp_TimeBasis;
    HwInit( );
    Read_flash_Parameter();//��ȡ����
    memcpy(&sensor_id_buf[2],&Flash_Sensor_ID,4);
    memcpy(&default_sensor_id,sensor_id_buf,6);
    MX_IWDG_Init();
    SX1280_Init(Frequency_list[Frequency_point]);
    RTC_Config();
    Handle_WakeUpSource();
    __HAL_RCC_CLEAR_RESET_FLAGS();
    Battery_Voltage_Measure();	//�α����ݲɼ�
    Handle_RFSendflag();
    HAL_IWDG_Refresh(&hiwdg);//ι��
    Enter_Shutdown_Mode();
}


void Enter_Shutdown_Mode(void)
{
    WRITE_REG( RTC->BKP31R, 0x1 );//��¼RTC�Ĵ��������Ͻ���ػ�ģʽ
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle);
    RTC_SleepTime=((RTC_SleepTime-MainRun_Time)*2.048-start_up_time);//5�ǹ����������ʱ�� 2�Ǽ���ms
    HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, RTC_SleepTime, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    HAL_PWREx_EnterSHUTDOWNMode(); //SHUTDOWN Mode two lines
}

uint32_t Handle_RFSendflag(void)
{
    uint8_t Return_Value=0;
    if( Send_Frame_Type == 1)
    {
        Send_Frame_Type=0; //�������
        SendtoStation_sx1280_frame(MESSAGE,19,ShapeChange_type,Buffer);
        HAL_Delay(35);//���͵���ֵ
        Get_random();
    }
    else if( Send_Frame_Type == 2)
    {
        Radio.SetRfFrequency(Frequency_list[1]);//�л�����Ƶ��
        Send_Frame_Type=0;
        AppState=0;
        sx1280_receive_time=65;//��ʱ����ܳ�֡
        Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);
        SendtoStation_sx1280_frame(REQ, 2, ShapeChange_type, Buffer);
        while(sx1280_receive_time)
        {
            if( IrqState ==true)
            {
                SX1280ProcessIrqs();//ɨ�����״̬�����緢�ͺͽ��գ�����AppStateΪ��Ӧ������Ȼ��ͨ��������봦������
                IrqState =false;
            }
            Return_Value=Process_Appstate_0();
            if(Return_Value==APP_RX)
            {
                Return_Value=Handle_receiveStation_sx1280_frame();
                if(Return_Value==RSP_END)
                {
                    Write_flash_Parameter();//�����ܵ������ò���д��flash��
                    Shift_time=Offset_Delay%WakeUp_TimeBasis;
                    //���ڵ���ʹ��
                    RTC_SleepTime=(Shift_time+RTC_SleepTime-23);
                    HAL_RTCEx_BKUPWrite(&RTCHandle,RTC_BKP_DR1,((Offset_Delay%Message_cycle/WakeUp_TimeBasis)));//��ʱһ��ҵ������5����=30��10s����
                    Read_flash_Parameter();//��ȡ����
                    RspAckbuff[0] = 0x01;
                    SendtoStation_sx1280_frame(ACK,1,RSP_END_ACK_type,RspAckbuff);
                    HAL_Delay(12);//���͵���ֵ
                    break;//������ʱ
                }
            }
        }
    }
    else  if( Send_Frame_Type == 3)
    {
        Radio.SetRfFrequency(Frequency_list[20]);//�л�����Ƶ��
        Send_Frame_Type=0;
        AppState=0;
        sx1280_receive_time=100;//��ʱ����ܳ�֡
        Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);
        SendtoStation_sx1280_frame(RESET_REQ, 2, ShapeChange_type, Buffer);
        while(sx1280_receive_time)
        {
            if( IrqState ==true)
            {
                SX1280ProcessIrqs();//ɨ�����״̬�����緢�ͺͽ��գ�����AppStateΪ��Ӧ������Ȼ��ͨ��������봦������
                IrqState =false;
            }
            Return_Value=Process_Appstate_0();
            if(Return_Value==APP_RX)
            {
                Return_Value=Handle_receiveStation_sx1280_frame();
                if(Return_Value==RESET_ACK)
                {
                    RspAckbuff[0] = 0x01;
                    SendtoStation_sx1280_frame(RESET_SUCCESS_ACK,1,RSP_END_ACK_type,RspAckbuff);
                    HAL_Delay(12);//���͵���ֵ
                    break;//������ʱ
                }
            }
        }
    }
    Radio.SetStandby( STDBY_RC );
    SX1280_Enter_LowPower();
    return 0;
}
