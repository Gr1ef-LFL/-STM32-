#include "bsp.h"


void BSP_Init(void)
{
	delay_init();	    	//��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	LED_Init();			    //LED�˿ڳ�ʼ��
	TFTLCD_Init();			//LCD��ʼ��	
	KEY_Init();	 			//������ʼ��
	BEEP_Init();			//��ʼ��������
	FSMC_SRAM_Init();		//��ʼ��SRAM
	my_mem_init(SRAMIN); 	//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);  	//��ʼ���ⲿ�ڴ��
  RTC_Init();         //��ʼ��RTC
	exfuns_init();			//Ϊfatfs�ļ�ϵͳ�����ڴ�
	f_mount(fs[0],"0:",1);	//����SD��
	f_mount(fs[1],"1:",1);	//����FLASH
	TP_Init();				//��������ʼ��
	font_init();
	Usart2_Init(115200);
	TIM4_Init(300,7200); 
	ADCx_Init();
	MotorInit();
	TIM2_PWM_ENABLE();
	TIM_SetCompare2(TIM2,0);
	TIM8_PWM_Init(199,7199);
	TIM1_PWM_Init(199,7199);
	TIM_SetCompare1(TIM8,195); // 0��
	LightControlInit();
	control_flag = 1;
}