#include "MQ135_ADC.h"

__IO uint16_t ADC_ConvertedValue = 0;	//���ڱ���ADC�ɼ�ת������������
float AirQuality = 0;	 				//���ڱ�����������ת���ɵĿ���ppmֵ
float ADC_ConvertedValueLocal = 0;  		//���ڱ���ת�������ĵ�ѹֵ    
//static ��ֹ�ⲿ��������
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// �� ADC IO�˿�ʱ��
	ADC_GPIO_APBxClock_FUN ( ADC_GPIO_CLK, ENABLE );
	
	// ���� ADC IO ����ģʽ
	// ����Ϊģ������
	GPIO_InitStructure.GPIO_Pin = ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	// ��ʼ�� ADC IO
	GPIO_Init(ADC_PORT, &GPIO_InitStructure);	
}
// ANSI C��׼��C89 ��׼
// C99
// C11 2011

 void ADCx_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStruct;	
	//ʱ��
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK, ENABLE);
	
	DMA_DeInit(ADC_DMA_CHANNEL);
	//�����ַΪ��ADC���ݼĴ�����ַ(ADC_DR)
	DMA_InitStructure.DMA_PeripheralBaseAddr = ( uint32_t ) ( & ( ADC_x->DR ) );
	//�洢����ַ��ʵ�������ڲ�SRAM�ı���
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;
	//����Դ����������
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	//��������Сδ1,�������Ĵ�СӦ�õ��ڴ洢���Ĵ�С
	DMA_InitStructure.DMA_BufferSize = 1;
	//����Ĵ���ֻ��һ������ַ���õ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//�洢����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable; 
	//�������ݴ�С  ���ּ������ֽ�
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	//�洢�����ݴ�СҲΪ���ֽڣ����������ݴ�С��ͬ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	//ѭ������ģʽ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
	//DMA����ͨ�����ȼ�  �� 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	//��ֹ�洢�����洢��ģʽ����Ϊ��ʱʹ�����赽�洢��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	//��ʼ��DMA
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	//ʹ��DMAͨ��
	DMA_Cmd(ADC_DMA_CHANNEL , ENABLE);
	//----------------------------------------------------------------------
	ADC_APBxClock_FUN ( ADC_CLK, ENABLE );
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//����
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;	   //��ͨ��,һ��ADCͨ������ɨ��
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//����ת��
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//�ⲿ�����ر�
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���
	ADC_InitStruct.ADC_NbrOfChannel = 1;			//�ɼ�ͨ����Ŀ1��ͨ��
	ADC_Init(ADC_x, &ADC_InitStruct);	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);				//����Ϊ8��Ƶ
	//����ADCͨ��CHANNEL�Ĳ�������Ϊ��55.5��
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);	
	
 	// ʹ��ADC DMA ����
	ADC_DMACmd(ADC_x, ENABLE);

	ADC_Cmd(ADC_x, ENABLE);	
	//��λУ׼�Ĵ���
	ADC_ResetCalibration(ADC_x);
	// �ȴ���λ���
	while(ADC_GetResetCalibrationStatus(ADC_x));
	// ADC��ʼУ׼
	ADC_StartCalibration(ADC_x);
	// �ȴ�У׼���
	while(ADC_GetCalibrationStatus(ADC_x));
	//δ�����ⲿ����������ʹ���������ADCת��
	ADC_SoftwareStartConvCmd(ADC_x, ENABLE);
}
void MQ135_GetValue(int* AirQuality_t)
{		
	ADC_ConvertedValueLocal =(float) ADC_ConvertedValue/4096*3.3; //12λAD 2^12 ADC�ڲ��ο���ѹ3.3V 
	AirQuality = ADC_ConvertedValueLocal*990/3.3;
	*AirQuality_t = (int)AirQuality;
}
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
}
