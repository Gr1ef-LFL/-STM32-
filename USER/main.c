#include "bsp.h"
#include "CMD.h"
/****************************************************************
	                       ȫ�ֱ���
****************************************************************/

extern WM_HWIN hWin_temp_Edit;
extern WM_HWIN hWin_humi_Edit;
extern WM_HWIN hWin_lux_Edit;
extern WM_HWIN hWin_ppm_Edit;
extern u8 windos_flag;
extern u8 control_flag;

/****************************************************************
	                       ��������
****************************************************************/

/* ��������ʱ�� */
OS_TMR BeepTmr;
/* ��Ϣ����ָ�� */
OS_Q SensorDataMsg;


/****************************************************************
	                       �����������ȼ�
****************************************************************/
#define START_TASK_PRIO							3
#define TOUCH_TASK_PRIO							4
#define LED0_TASK_PRIO 							5
#define DATACOLLECTION_TASK_PRIO 		6
#define UI_TASK_PRIO				      	7
#define CONNECTONENET_PRIO					8
#define DATAUPLOAD_PRIO							9
#define CMD_PRIO									  10
#define AUTOCONTROL_PRIO						11
/****************************************************************
	                       ���������ջ��С	
****************************************************************/
#define START_STK_SIZE 						128
#define TOUCH_STK_SIZE						128
#define LED0_STK_SIZE							128
#define DATACOLLECTION_STK_SIZE		128
#define UI_TASK_SIZE				      2048
#define CONNECTONENET_STK_SIZE		256
#define DATAUPLOAD_STK_SIZE			  256
#define CMD_STK_SIZE						  128
#define AUTOCONTROL_STK_SIZE			128
/****************************************************************
	                       ����������ƿ�	
****************************************************************/
OS_TCB StartTaskTCB;
OS_TCB TouchTaskTCB;
OS_TCB Led0TaskTCB;
OS_TCB DATACOLLECTIONTaskTCB;
OS_TCB UI_TASK_TCB;
OS_TCB CONNECTONENET_TCB;
OS_TCB DATAUPLOAD_TCB;
OS_TCB CMD_TCB;
OS_TCB AUTOCONTROL_TCB;
/****************************************************************
	                       ���������ջ	
****************************************************************/
CPU_STK START_TASK_STK[START_STK_SIZE];
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
CPU_STK DATACOLLECTION_TASK_STK[DATACOLLECTION_STK_SIZE];
CPU_STK UI_TASK_STK[UI_TASK_SIZE];
CPU_STK CONNECTONENET_STK[CONNECTONENET_STK_SIZE];
CPU_STK DATAUPLOAD_STK[DATAUPLOAD_STK_SIZE];
CPU_STK CMD_STK[CMD_STK_SIZE];
CPU_STK AUTOCONTROL_STK[AUTOCONTROL_STK_SIZE];
/****************************************************************
	                       ����ִ�к�������	
****************************************************************/
void start_task(void *p_arg);
void touch_task(void *p_arg);
void SystemLED_task(void *p_arg);
void datacollection_task(void *p_arg);
void UI_task(void *p_arg);
void connectOneNet_task (void *p_arg);
void dataupload_task(void *p_arg);
void cmd_task(void *p_arg);
void Autocontrol_task(void *p_arg);
void Beep_task(void *p_tmr, void *p_arg);


int main(void)
{	
	OS_ERR err;
	CPU_SR_ALLOC();
	BSP_Init();
	control_flag = 1;
	OSInit(&err);			//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();	//�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
	while(1);
}

//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);//����CRCʱ��
	GUI_Init();  			//STemWin��ʼ��
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	
	//����������������Ϣ����
	OSQCreate ((OS_Q         *)&SensorDataMsg,            //ָ����Ϣ���е�ָ��
               (CPU_CHAR     *)"DATA of Sensor",  //���е�����
               (OS_MSG_QTY    )30,                //���ɴ����Ϣ����Ŀ
               (OS_ERR       *)&err); 
	//�������������ж�ʱ��
	OSTmrCreate((OS_TMR		*)&BeepTmr,		//��ʱ��1
                (CPU_CHAR	*)"BeepTmr",		//��ʱ������
                (OS_TICK	 )0,			//���ӳ�
                (OS_TICK	 )50,          //50*10=500ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //����ģʽ
                (OS_TMR_CALLBACK_PTR)Beep_task,//��ʱ��1�ص�����
                (void	    *)0,			//����Ϊ0
                (OS_ERR	    *)&err);		//���صĴ�����
	
	
	//UI_Task����	
	OSTaskCreate((OS_TCB*     )&UI_TASK_TCB,		
				 (CPU_CHAR*   )"UI_Task����	", 		
                 (OS_TASK_PTR )UI_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )UI_TASK_PRIO,     
                 (CPU_STK*    )&UI_TASK_STK[0],	
                 (CPU_STK_SIZE)UI_TASK_SIZE/10,	
                 (CPU_STK_SIZE)UI_TASK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	//��������
	OSTaskCreate((OS_TCB*     )&TouchTaskTCB,		
				 (CPU_CHAR*   )"Touch task", 		
                 (OS_TASK_PTR )touch_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )TOUCH_TASK_PRIO,     
                 (CPU_STK*    )&TOUCH_TASK_STK[0],	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE/10,	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);			 
	//�ɼ���������
	OSTaskCreate((OS_TCB*     )&DATACOLLECTIONTaskTCB,		
				 (CPU_CHAR*   )"�ɼ���������", 		
                 (OS_TASK_PTR )datacollection_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )DATACOLLECTION_TASK_PRIO,     
                 (CPU_STK*    )&DATACOLLECTION_TASK_STK[0],	
                 (CPU_STK_SIZE)DATACOLLECTION_STK_SIZE/10,	
                 (CPU_STK_SIZE)DATACOLLECTION_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);						 
	//LED����������
	OSTaskCreate((OS_TCB*     )&Led0TaskTCB,		
				 (CPU_CHAR*   )"LED����������", 		
                 (OS_TASK_PTR )SystemLED_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )LED0_TASK_PRIO,     
                 (CPU_STK*    )&LED0_TASK_STK[0],	
                 (CPU_STK_SIZE)LED0_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED0_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	/* OneNet�������� */
    OSTaskCreate((OS_TCB     *)&CONNECTONENET_TCB,                
                 (CPU_CHAR   *)"OneNet��������",
                 (OS_TASK_PTR ) connectOneNet_task ,
                 (void       *) 0,
                 (OS_PRIO     ) CONNECTONENET_PRIO,
                 (CPU_STK    *)	&CONNECTONENET_STK[0],
                 (CPU_STK_SIZE) CONNECTONENET_STK_SIZE / 10,
                 (CPU_STK_SIZE) CONNECTONENET_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
			
	/* OneNet�ϴ��������� */
	    OSTaskCreate((OS_TCB     *)&DATAUPLOAD_TCB,                
                 (CPU_CHAR   *)"OneNet�ϴ���������",
                 (OS_TASK_PTR ) dataupload_task,
                 (void       *) 0,
                 (OS_PRIO     ) DATAUPLOAD_PRIO,
                 (CPU_STK    *)	&DATAUPLOAD_STK[0],
                 (CPU_STK_SIZE) DATAUPLOAD_STK_SIZE / 10,
                 (CPU_STK_SIZE) DATAUPLOAD_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
			OS_TaskSuspend((OS_TCB*)&DATAUPLOAD_TCB,&err);
	/* OneNet�·��������� */
	    OSTaskCreate((OS_TCB     *)&CMD_TCB,                
                 (CPU_CHAR   *)"OneNet�·���������",
                 (OS_TASK_PTR ) cmd_task,
                 (void       *) 0,
                 (OS_PRIO     ) CMD_PRIO,
                 (CPU_STK    *)	&CMD_STK[0],
                 (CPU_STK_SIZE) CMD_STK_SIZE / 10,
                 (CPU_STK_SIZE) CMD_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
	/* ������������ */
	    OSTaskCreate((OS_TCB     *)&AUTOCONTROL_TCB,                
                 (CPU_CHAR   *)"������������",
                 (OS_TASK_PTR ) Autocontrol_task,
                 (void       *) 0,
                 (OS_PRIO     ) AUTOCONTROL_PRIO,
                 (CPU_STK    *)	&AUTOCONTROL_STK[0],
                 (CPU_STK_SIZE) AUTOCONTROL_STK_SIZE / 10,
                 (CPU_STK_SIZE) AUTOCONTROL_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
	
	OS_TaskSuspend((OS_TCB*)&CMD_TCB,&err);							 
	OS_TaskSuspend((OS_TCB*)&AUTOCONTROL_TCB,&err);							 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
}

/* ���������� */
void Beep_task(void *p_tmr, void *p_arg)
{
	BEEP = ~BEEP;
}


//UI��ʾ����
void UI_task(void *p_arg)
{
	while(1)
	{
		UI_DISPLAY();
	}
	
}


//TOUCH����
void touch_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		GUI_TOUCH_Exec();	
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//��ʱ5ms
	}
}

//�������������
void datacollection_task(void *p_arg)
{
	int SensorData[5] = {0};
	char temp_data[7];
	char humi_data[7];
	char lux_data[25];
	char ppm_data[10];
	OS_ERR err;
	DHT11_Init();
	iic_by30_init();
	while(1)
	{
		OSIntEnter();	
		DHT11_Read_Data(&SensorData[0],&SensorData[1]);
		get_sunlight_value(&SensorData[2]);
		OSIntExit();
		MQ135_GetValue(&SensorData[3]);	
		if(windos_flag){
			sprintf(temp_data,"%d C",SensorData[0]);
			sprintf(humi_data,"%d%%",SensorData[1]);
			sprintf(lux_data,"%d Lux",SensorData[2]);
			sprintf(ppm_data,"%d PPM",SensorData[3]);
			TEXT_SetText(hWin_temp_Edit,temp_data);
			TEXT_SetText(hWin_humi_Edit,humi_data);
			TEXT_SetText(hWin_lux_Edit,lux_data);
			TEXT_SetText(hWin_ppm_Edit,ppm_data);
		}
		
		if(SensorData[2] >= 500)
			SensorData[2] = 500;
		/* ������Ϣ���и�OneEnt�ϴ����� */
		OSQPost((OS_Q        *)&SensorDataMsg,                             //��Ϣ����ָ��
					 (void        *)SensorData,                //Ҫ���͵����ݵ�ָ�룬���ڴ���׵�ַͨ�����С����ͳ�ȥ��
					 (OS_MSG_SIZE  )30,     //�����ֽڴ�С
					 (OS_OPT       )OS_OPT_POST_FIFO | OS_OPT_POST_ALL, //�Ƚ��ȳ��ͷ�����ȫ���������ʽ
					 (OS_ERR      *)&err);	                            //���ش�������		
		//printf("�Ѿ������ź�������������\r\n");
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err);//��ʱ1ms
	}
}

//LED������
void SystemLED_task(void *p_arg)
{
	OS_ERR err;
	CPU_STK_SIZE free,used;  
	while(1)
	{
		LED0 = !LED0;
		OSTaskStkChk (&UI_TASK_TCB,&free,&used,&err);  
		//printf("control_flag: %d\r\n",control_flag);
    //printf("EmwindemoTaskTCB      used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);//��ʱ500ms
	}
}

//���Ƽ������
void connectOneNet_task(void *p_arg)
{
	OS_ERR err;
	WiFi_ResetIO_Init();            //��ʼ��WiFi�ĸ�λIO
  MQTT_Buff_Init();               //��ʼ������,����,�������ݵ� ������ �Լ���״̬����
	OneNetIoT_Parameter_Init();	    //��ʼ������OneNetƽ̨MQTT�������Ĳ���	
	while(1)                        //��ѭ��
	{		
		/*--------------------------------------------------------------------*/
		/*   Connect_flag=1ͬ����������������,���ǿ��Է������ݺͽ���������    */
		/*--------------------------------------------------------------------*/
		if(Connect_flag==1)
		{     
			/*-------------------------------------------------------------*/
			/*                     �����ͻ���������                      */
			/*-------------------------------------------------------------*/
				if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr)
			{                //if�����Ļ���˵�����ͻ�������������
				//3������ɽ���if
				//��1�֣�0x10 ���ӱ���
				//��2�֣�0x82 ���ı��ģ���ConnectPack_flag��λ����ʾ���ӱ��ĳɹ�
				//��3�֣�SubcribePack_flag��λ��˵�����ӺͶ��ľ��ɹ����������Ŀɷ�
				if((MQTT_TxDataOutPtr[2]==0x10)||((MQTT_TxDataOutPtr[2]==0x82)&&(ConnectPack_flag==1))||(SubcribePack_flag==1))
				{    
					printf("��������:0x%x\r\n",MQTT_TxDataOutPtr[2]);  //������ʾ��Ϣ
					MQTT_TxData(MQTT_TxDataOutPtr);                       //��������
					MQTT_TxDataOutPtr += BUFF_UNIT;                       //ָ������
					if(MQTT_TxDataOutPtr==MQTT_TxDataEndPtr)              //���ָ�뵽������β����
						MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];            //ָ���λ����������ͷ
				} 				
			}//�����ͻ��������ݵ�else if��֧��β
			
			/*-------------------------------------------------------------*/
			/*                     ������ջ���������                      */
			/*-------------------------------------------------------------*/
			if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr){  //if�����Ļ���˵�����ջ�������������														
				printf("���յ�����:");
				/*-----------------------------------------------------*/
				/*                    ����CONNACK����                  */
				/*-----------------------------------------------------*/				
				//if�жϣ������һ���ֽ���0x20����ʾ�յ�����CONNACK����
				//��������Ҫ�жϵ�4���ֽڣ�����CONNECT�����Ƿ�ɹ�
				if(MQTT_RxDataOutPtr[2]==0x20){             			
				    switch(MQTT_RxDataOutPtr[5]){					
						case 0x00 : printf("CONNECT���ĳɹ�\r\n");                            //���������Ϣ	
								    ConnectPack_flag = 1;                                        //CONNECT���ĳɹ������ı��Ŀɷ�
									break;                                                       //������֧case 0x00                                              
						case 0x01 : printf("�����Ѿܾ�����֧�ֵ�Э��汾��׼������\r\n");     //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x01   
						case 0x02 : printf("�����Ѿܾ������ϸ�Ŀͻ��˱�ʶ����׼������\r\n"); //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x02 
						case 0x03 : printf("�����Ѿܾ�������˲����ã�׼������\r\n");         //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x03
						case 0x04 : printf("�����Ѿܾ�����Ч���û��������룬׼������\r\n");   //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������						
									break;                                                       //������֧case 0x04
						case 0x05 : printf("�����Ѿܾ���δ��Ȩ��׼������\r\n");               //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������						
									break;                                                       //������֧case 0x05 		
						default   : printf("�����Ѿܾ���δ֪״̬��׼������\r\n");             //���������Ϣ 
									Connect_flag = 0;                                            //Connect_flag���㣬��������					
									break;                                                       //������֧case default 								
					}
				}			
				//if�жϣ���һ���ֽ���0x90����ʾ�յ�����SUBACK����
				//��������Ҫ�ж϶��Ļظ��������ǲ��ǳɹ�
				else if(MQTT_RxDataOutPtr[2]==0x90){ 
						switch(MQTT_RxDataOutPtr[6]){					
						case 0x00 :
						case 0x01 : printf("���ĳɹ�\r\n");            //���������Ϣ
												SubcribePack_flag = 1;                //SubcribePack_flag��1����ʾ���ı��ĳɹ����������Ŀɷ���
												Ping_flag = 0;                        //Ping_flag����
												LED1 = 0;
												TIM3_ENABLE_30S();                    //����30s��PING��ʱ��
												OS_TaskResume(&DATAUPLOAD_TCB,&err);
												OS_TaskResume(&CMD_TCB,&err);
												OS_TaskResume(&AUTOCONTROL_TCB,&err);
												break;                                //������֧                                             
						default   : printf("����ʧ�ܣ�׼������\r\n");  //���������Ϣ 
												Connect_flag = 0;                     //Connect_flag���㣬��������
												break;                                //������֧ 								
					}					
				}
				//if�жϣ���һ���ֽ���0xD0����ʾ�յ�����PINGRESP����
				else if(MQTT_RxDataOutPtr[2]==0xD0)
			{ 
					printf("PING���Ļظ�\r\n"); 		  //���������Ϣ 
					if(Ping_flag==1){                     //���Ping_flag=1����ʾ��һ�η���
						 Ping_flag = 0;    				  //Ҫ���Ping_flag��־
					}else if(Ping_flag>1){ 				  //���Ping_flag>1����ʾ�Ƕ�η����ˣ�������2s����Ŀ��ٷ���
						Ping_flag = 0;     				  //Ҫ���Ping_flag��־
						TIM3_ENABLE_30S(); 				  //PING��ʱ���ػ�30s��ʱ��
					}				
				}	
				//if�жϣ������һ���ֽ���0x30����ʾ�յ����Ƿ�������������������
				//����Ҫ��ȡ��������
				else if((MQTT_RxDataOutPtr[2]==0x30))
				{ 
					printf("�������ȼ�0����\r\n"); 		   //���������Ϣ 
					MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);  //����ȼ�0��������
				}				
								
				MQTT_RxDataOutPtr += BUFF_UNIT;                     //ָ������
				if(MQTT_RxDataOutPtr==MQTT_RxDataEndPtr)            //���ָ�뵽������β����
					MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];          //ָ���λ����������ͷ                        
			}//������ջ��������ݵ�else if��֧��β		
		}//Connect_flag=1��if��֧�Ľ�β
		
		/*--------------------------------------------------------------------*/
		/*      Connect_flag=0ͬ�������Ͽ�������,����Ҫ�������ӷ�����         */
		/*--------------------------------------------------------------------*/
		else
		{ 
			printf("��Ҫ���ӷ�����\r\n");                 //���������Ϣ
			TIM_Cmd(TIM4,DISABLE);                           //�ر�TIM4 
			TIM_Cmd(TIM3,DISABLE);                           //�ر�TIM3  
			WiFi_RxCounter=0;                                //WiFi������������������                        
			memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);          //���WiFi���ջ����� 
			if(WiFi_Connect_IoTServer()==0)
			{   			     //���WiFi�����Ʒ�������������0����ʾ��ȷ������if
				printf("����TCP���ӳɹ�\r\n");            //���������Ϣ
				Connect_flag = 1;                            //Connect_flag��1����ʾ���ӳɹ�	
				WiFi_RxCounter=0;                            //WiFi������������������                        
				memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);      //���WiFi���ջ����� 
				MQTT_Buff_ReInit();                          //���³�ʼ�����ͻ�����                    
			}				
		}
		
		delay_ms(100);
	}
}

void dataupload_task(void *p_arg)
{
	
	OS_ERR err;
	OS_MSG_SIZE MsgSize;
	while(1)
	{		
		char head1[3];
		char temp[50];				//����һ����ʱ������1,��������ͷ
		char tempAll[150];			//����һ����ʱ������2��������������
		int	dataLen = 0;			//���ĳ���
		int *TEXT_Buffer;	
		
		TEXT_Buffer = OSQPend ((OS_Q         *)&SensorDataMsg,               
									(OS_TICK       )0,                   
									(OS_OPT        )OS_OPT_PEND_BLOCKING,  //���û�л�ȡ���ź����͵ȴ�
									(OS_MSG_SIZE  *)&MsgSize,          
									(CPU_TS       *)0,                     
									(OS_ERR       *)&err); 
		//printf("�Ѿ����ܵ��ź�����\r\n");
		//printf("���ܵ�����:%d��ʪ�ȣ�%d����:%d,��:%d\r\n",TEXT_Buffer[0],TEXT_Buffer[1],TEXT_Buffer[2],TEXT_Buffer[3]);
		memset(temp,    0, 50);				    //��ջ�����1
		memset(tempAll, 0, 100);				//��ջ�����2
		memset(head1,   0, 3);					//���MQTTͷ
		sprintf(temp,"{\"temperature\":\"%d\",\"humidity\":\"%d\",\"Lux\":\"%d\",\"ppm\":\"%d\"}",
		TEXT_Buffer[0], TEXT_Buffer[1],TEXT_Buffer[2],TEXT_Buffer[3]);//��������
		head1[0] = 0x03; 						//�̶���ͷ
		head1[1] = 0x00; 						//�̶���ͷ
		head1[2] = strlen(temp);  				//ʣ�೤��	
		sprintf(tempAll, "%c%c%c%s", head1[0], head1[1], head1[2], temp);
		dataLen = strlen(temp) + 3;
		//printf("%s",tempAll);
		MQTT_PublishQs0(P_TOPIC_NAME,tempAll, dataLen);//������ݣ�������������
		//printf("Seed Data OK!\r\n ");
		OSTimeDlyHMSM(0,0,5,0,OS_OPT_TIME_PERIODIC,&err);//��ʱ1ms
	}
}
/* �ƶ���������� */
void cmd_task(void *p_arg)
{
	OS_ERR err;
	
	char online_cmd[20];
	char *cmd;
	unsigned char OnlinePwmControl;
	while(1)
	{
		
		/* �ƶ�ָ��� */
		if(MQTT_CMDOutPtr != MQTT_CMDInPtr)							  //if�����Ļ���˵�����������������	
			{                             		       
					printf("����:%s\r\n",&MQTT_CMDOutPtr[2]);              //���������Ϣ
				
				/* ���յ��ƿ����ź�֮��ر����ܵ���ģʽ */
				control_flag = 0;
				
				
				/**************************  
				
					���ܵ����ƵƵ�ָ��
				
				**************************/
				if(!memcmp(&MQTT_CMDOutPtr[2],LED1_ON,strlen(LED1_ON)))//�ж�ָ������LED1_ON
				{                                            
					Light1 =  1;
					printf("�Ѵ�LED1��\r\n");
				}
				if(!memcmp(&MQTT_CMDOutPtr[2],LED1_OFF,strlen(LED1_OFF)))//�ж�ָ������LED1_ON
				{                                            
					Light1 =  0;
					printf("�ѹر�LED1��\r\n");
				}
				
					if(!memcmp(&MQTT_CMDOutPtr[2],LED2_ON,strlen(LED2_ON)))//�ж�ָ������LED1_ON
				{                                            
					Light2 =  1;
					printf("�Ѵ�LED2��\r\n");
				}
				if(!memcmp(&MQTT_CMDOutPtr[2],LED2_OFF,strlen(LED2_OFF)))//�ж�ָ������LED1_ON
				{                                            
					Light2 =  0;
					printf("�ѹر�LED2��\r\n");
				}
				
					if(!memcmp(&MQTT_CMDOutPtr[2],LED3_ON,strlen(LED3_ON)))//�ж�ָ������LED1_ON
				{                                            
					Light3 =  1;
					printf("�Ѵ�LED3��\r\n");
				}
				if(!memcmp(&MQTT_CMDOutPtr[2],LED3_OFF,strlen(LED3_OFF)))//�ж�ָ������LED1_ON
				{                                            
					Light3 =  0;
					printf("�ѹر�LED3��\r\n");
				}
				
					if(!memcmp(&MQTT_CMDOutPtr[2],LED4_ON,strlen(LED4_ON)))//�ж�ָ������LED1_ON
				{                                            
					Light4 =  1;
					printf("�Ѵ�LED4��\r\n");
				}
				if(!memcmp(&MQTT_CMDOutPtr[2],LED4_OFF,strlen(LED4_OFF)))//�ж�ָ������LED1_ON
				{                                            
					Light4 =  0;
					printf("�ѹر�LED4��\r\n");
				}
				
				
				/************************** 
				
				���ܵ������������ȵ�ָ�� 
				
				**************************/
				
				if(!memcmp(&MQTT_CMDOutPtr[2],Paiqishan_ON,strlen(Paiqishan_ON)))//�ж�ָ������LED1_ON
				{                                            
					MotorPWM = 1;
					printf("�Ѵ������ȣ�\r\n");
				}
				if(!memcmp(&MQTT_CMDOutPtr[2],Paiqishan_OFF,strlen(Paiqishan_OFF)))//�ж�ָ������LED1_ON
				{                                            
					MotorPWM = 0;
					printf("�ѹر������ȣ�\r\n");
				}
				
				if(!memcmp(&MQTT_CMDOutPtr[2],Windos_ON,strlen(Windos_ON)))//�ж�ָ������LED1_ON
				{                                            
					TIM_SetCompare1(TIM8,175); // 180��;
					printf("�Ѵ򿪴�����\r\n");
				}
				if(!memcmp(&MQTT_CMDOutPtr[2],Windos_OFF,strlen(Windos_OFF)))//�ж�ָ������LED1_ON
				{                                            
					TIM_SetCompare1(TIM8,195);
					printf("�ѹرմ�����\r\n");
				}
				
				
				/**************************  
				
					���ܵ����Ƶ����ָ��
				
				**************************/
				if(!memcmp(&MQTT_CMDOutPtr[2],dianji,strlen(dianji)))
				{
					/* 
						OneNet��������Ϊ��fangshan:������ֵ 
						������ֵ��ΧΪ:0 - 100
						ȡ��ָ���еĿ�����ֵ�ַ�����ת��Ϊu8����
					*/
					strcpy(online_cmd,(char*)&MQTT_CMDOutPtr[2]);
					cmd = &online_cmd[9];
					OnlinePwmControl = atoi(cmd);
					printf("%d",OnlinePwmControl);
					TIM_SetCompare2(TIM2,OnlinePwmControl);
					
				}					
				//������������ᷢ��״̬
				else printf("δָ֪��\r\n");				//���������Ϣ	
				MQTT_CMDOutPtr += BUFF_UNIT;                             	 //ָ������
				if(MQTT_CMDOutPtr==MQTT_CMDEndPtr)           	             //���ָ�뵽������β����
					MQTT_CMDOutPtr = MQTT_CMDBuf[0];          	             //ָ���λ����������ͷ		
			}
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err);//��ʱ1ms
	}
}

/* ������������ */
void Autocontrol_task(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE MsgSize;
	while(1)
	{
		int *TEXT_Buffer;	
		TEXT_Buffer = OSQPend ((OS_Q         *)&SensorDataMsg,               
									(OS_TICK       )0,                   
									(OS_OPT        )OS_OPT_PEND_BLOCKING,  //���û�л�ȡ���ź����͵ȴ�
									(OS_MSG_SIZE  *)&MsgSize,          
									(CPU_TS       *)0,                     
									(OS_ERR       *)&err);
		
		/* 
			 TEXT_Buffer[0] = �¶� 
			 TEXT_Buffer[1] = ʪ��
			 TEXT_Buffer[2] = ��ǿ
		   TEXT_Buffer[3] = ú���������
		*/
		
		/**********************************
		
			����1������ú��й©����� 
		
		**********************************/
		
		/* ���ܳ���ú��й© */
		if(TEXT_Buffer[3] >= 300){
			printf("����1\r\n");
			/* ���������� */
			OSTmrStart(&BeepTmr,&err);
			/* ����ͨ�� */
			TIM_SetCompare1(TIM8,175);
			/* �������� */
			//MotorPWM = 1;
		}else if(TEXT_Buffer[3] <= 300){
			/* �رշ��������� */
			BEEP = 0;
			OSTmrStop(&BeepTmr,OS_OPT_TMR_NONE,0,&err);
			/* �رմ��� */
			TIM_SetCompare1(TIM8,195);
			/* �ر������� */
			//MotorPWM = 0;			
		}
		
		/**********************************
		
			����2�����ݹ���ǿ�ȵ��ڴ����Ŀ��� 
		
		**********************************/
		
			if(TEXT_Buffer[2] <= 10 && control_flag){
				/* ������ҹ */
				TIM_SetCompare1(TIM1,195);
			}else if(TEXT_Buffer[2] <= 30 && TEXT_Buffer[2] >= 10 && control_flag){
				/* ��һ��� */
				TIM_SetCompare1(TIM1,190); // 45��
		}else if(TEXT_Buffer[2] <= 120 && TEXT_Buffer[2] >= 30 && control_flag){
			 /* �й���  */
			TIM_SetCompare1(TIM1,185); // 90��
		}else if (TEXT_Buffer[2] <= 400 && TEXT_Buffer[2] >= 120 && control_flag){
			/* �ճ� */
			TIM_SetCompare1(TIM1,175); // 180��
		}
		
		/**********************************
		
			����3�������¶Ⱥ�ʪ�ȿ��ط��Ȼ򿪹ش���
		
		**********************************/
		
		if(TEXT_Buffer[1] >= 30 && control_flag){
			TIM_SetCompare2(TIM2,50);
		}
		else if(TEXT_Buffer[1] >= 35 && control_flag){
			TIM_SetCompare2(TIM2,100);
			TIM_SetCompare1(TIM8,175);
		}else if (TEXT_Buffer[1] <= 30 && control_flag) 
		{
			TIM_SetCompare2(TIM2,0);
			TIM_SetCompare1(TIM8,195);
		}
	}
}
