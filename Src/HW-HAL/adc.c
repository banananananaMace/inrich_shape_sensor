#include "adc.h"
#include "protocol.h"
#include "string.h"
ADC_HandleTypeDef hadc1;

float a;
uint16_t b;
uint16_t Shape_Data_1=0;
uint16_t Shape_Data_2=0;
uint8_t Burst_Status=0;
float Shape_Change_Data=0;
float First_Shape_Change_Data=0;
void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig;
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc1.Init.LowPowerAutoWait = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    hadc1.Init.OversamplingMode = DISABLE;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }
    sConfig.Channel = ADC_CHANNEL_VREFINT;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

}

void ADC1toChanel_Ref(void)
{
    ADC_ChannelConfTypeDef sConfig;

    sConfig.Channel = ADC_CHANNEL_VREFINT;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

void ADC1toChanel_8(void)
{
    ADC_ChannelConfTypeDef sConfig;
    sConfig.Channel = ADC_CHANNEL_8;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}


void ADC1toChanel_5(void)
{
    ADC_ChannelConfTypeDef sConfig;
    sConfig.Channel = ADC_CHANNEL_5;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}
void ADC1toChanel_7(void)
{
    ADC_ChannelConfTypeDef sConfig;
    sConfig.Channel = ADC_CHANNEL_5;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}


void Battery_Voltage_Measure(void)
{

    MX_ADC1_Init();
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
    {
        /* Calibration Error */
        Error_Handler();
    }

    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        /* Start Conversation Error */
        Error_Handler();
    }


    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
    {
        /* End Of Conversion flag not set on time */
        Error_Handler();
    }
    else
    {
        uhADCxRefValue = HAL_ADC_GetValue(&hadc1);
    }


    ADC1toChanel_8();

    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
    {
        /* Calibration Error */
        Error_Handler();
    }


    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        /* Start Conversation Error */
        Error_Handler();
    }


    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
    {
        /* End Of Conversion flag not set on time */
        Error_Handler();
    }
    else
    {
        uhADCxConvertedValue = HAL_ADC_GetValue(&hadc1);
    }

    a = 1.34*uhADCxConvertedValue/uhADCxRefValue*2;
    memcpy(&Buffer[4],&a,4);

    //�α����� PA0
    ADC1toChanel_5();
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
    {
        /* Calibration Error */
        Error_Handler();
    }
    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        /* Start Conversation Error */
        Error_Handler();
    }

    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
    {
        /* End Of Conversion flag not set on time */
        Error_Handler();
    }
    else
    {
        PA0_ADCxRValue = HAL_ADC_GetValue(&hadc1);
    }
    //�α����� PA2
    ADC1toChanel_7();
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
    {
        /* Calibration Error */
        Error_Handler();
    }
    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        /* Start Conversation Error */
        Error_Handler();
    }
    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
    {
        /* End Of Conversion flag not set on time */
        Error_Handler();
    }
    else
    {
        PA2_ADCxRValue = HAL_ADC_GetValue(&hadc1);
    }


    if (PA2_ADCxRValue > PA0_ADCxRValue)
    {
        Shape_Change_Data = PA2_ADCxRValue;
    }
    else
    {
        Shape_Change_Data = PA0_ADCxRValue;
    }
    if (Shape_Change_Data>2048)
    {
        Shape_Change_Data=Shape_Change_Data-2048;
    }
    else
    {
        Shape_Change_Data=2048-Shape_Change_Data;
    }
    Shape_Change_Data=Shape_Change_Data*4.65;
    First_Shape_Change_Data = HAL_RTCEx_BKUPRead(&RTCHandle,RTC_BKP_DR18);
    Shape_Change_Data = (Shape_Change_Data-First_Shape_Change_Data);
    memcpy(Buffer,&Shape_Change_Data,4);
}


void ShapeChange_Measure(void)
{
    MX_ADC1_Init();
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
    {
        /* Calibration Error */
        Error_Handler();
    }

    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        /* Start Conversation Error */
        Error_Handler();
    }


    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
    {
        /* End Of Conversion flag not set on time */
        Error_Handler();
    }
    else
    {
        uhADCxRefValue = HAL_ADC_GetValue(&hadc1);
    }
    //�α����� PA0
    ADC1toChanel_5();
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
    {
        /* Calibration Error */
        Error_Handler();
    }
    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        /* Start Conversation Error */
        Error_Handler();
    }

    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
    {
        /* End Of Conversion flag not set on time */
        Error_Handler();
    }
    else
    {
        PA0_ADCxRValue = HAL_ADC_GetValue(&hadc1);
    }
    //�α����� PA2
    ADC1toChanel_7();
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
    {
        /* Calibration Error */
        Error_Handler();
    }
    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        /* Start Conversation Error */
        Error_Handler();
    }
    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
    {
        /* End Of Conversion flag not set on time */
        Error_Handler();
    }
    else
    {
        PA2_ADCxRValue = HAL_ADC_GetValue(&hadc1);
    }


    if (PA2_ADCxRValue > PA0_ADCxRValue)
    {
        First_Shape_Change_Data = PA2_ADCxRValue;
    }
    else
    {
        First_Shape_Change_Data = PA0_ADCxRValue;
    }
    if (First_Shape_Change_Data>2048)
    {
        First_Shape_Change_Data=First_Shape_Change_Data-2048;
    }
    else
    {
        First_Shape_Change_Data=2048-First_Shape_Change_Data;
    }
    First_Shape_Change_Data=First_Shape_Change_Data*4.65;
}
