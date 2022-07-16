#ifndef __MQ135_ADC_H__
#define	__MQ135_ADC_H__


#include "stm32f10x.h"
/**---ADCѡ��-----ADC1 PC5
 *
 **DMAͨ��ѡ��----DMA1 CHANNEL1
 *
 **/
// ADC GPIO�궨��
// ע�⣺����ADC�ɼ���IO����û�и��ã�����ɼ���ѹ����Ӱ��
#define    ADC_GPIO_APBxClock_FUN        RCC_APB2PeriphClockCmd
#define    ADC_GPIO_CLK                  RCC_APB2Periph_GPIOC  
#define    ADC_PORT                      GPIOC
#define    ADC_PIN                       GPIO_Pin_5


// ADC ���ѡ��
// ������ ADC1/2�����ʹ��ADC3���ж���ص�Ҫ�ĳ�ADC3��
#define    ADC_APBxClock_FUN             RCC_APB2PeriphClockCmd
#define    ADC_x                         ADC1
#define    ADC_CLK                       RCC_APB2Periph_ADC1

// ADC ͨ���궨��
#define    ADC_CHANNEL                   ADC_Channel_15

#define    ADC_DMA_CLK                   RCC_AHBPeriph_DMA1
#define    ADC_DMA_CHANNEL               DMA1_Channel1

/*****һЩ����������(����)****/
//__IO uint16_t ADC_ConvertedValue;	//���ڱ���ADC�ɼ�ת������������
//float AirQuality0;	 				//���ڱ�����������ת���ɵĿ���ppmֵ
//float ADC_ConvertedValueLocal0;  		//���ڱ���ת�������ĵ�ѹֵ    

/**
  * @��Ҫ 	MQ135_ADC��ʼ������  ����ADC��ͨ��DMA��ȡ
  *         ADC���ݴ洢��      ADC_ConvertedValue 
  *			����ADC����ʱ��Ҫ  extern __IO uint16_t ADC_ConvertedValue;  * 
  * @������ ��
  * @����ֵ ��

  **/
void ADCx_Init(void);
/**
  * @��Ҫ 	MQ135ADת�����ݻ�ȡ����
  *			�洢�� ��ADC_ConvertedValue		AirQuality		ADC_ConvertedValueLocal
  *			��Ҫ������������
  *			
  * @������ ��
  * @����ֵ ��
  **/
void MQ135_GetValue(int* AirQuality_t);

#endif /* __MQ135_ADC_H__ */
