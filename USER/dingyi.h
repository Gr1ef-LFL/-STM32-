#ifndef __DINGYI_H
#define __DINGYI_H
//�������ȼ�
#define START_TASK_PRIO				3
//�����ջ��С	
#define START_STK_SIZE 				128
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//TOUCH����
//�����������ȼ�
#define TOUCH_TASK_PRIO				4
//�����ջ��С
#define TOUCH_STK_SIZE				128
//������ƿ�
OS_TCB TouchTaskTCB;
//�����ջ
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//touch����
void touch_task(void *p_arg);

//LED0������
//�����������ȼ�
#define LED0_TASK_PRIO 				5
//�����ջ��С
#define LED0_STK_SIZE				128
//������ƿ�
OS_TCB Led0TaskTCB;
//�����ջ
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
//led0����
void led0_task(void *p_arg);



//��⶯̬�ڴ�����
//�����������ȼ�
#define CHECKMEMORY_TASK_PRIO 				6
//�����ջ��С
#define CHECKMEMORY_STK_SIZE				128
//������ƿ�
OS_TCB CHECKMEMORYTaskTCB;
//�����ջ
CPU_STK CHECKMEMORY_TASK_STK[CHECKMEMORY_STK_SIZE];
//led0����
void CheckMemory_task(void *p_arg);

//EMWINDEMO����
//�����������ȼ�
#define EMWINDEMO_TASK_PRIO			7
//�����ջ��С
#define EMWINDEMO_STK_SIZE			2048
//������ƿ�
OS_TCB EmwindemoTaskTCB;
//�����ջ
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
//emwindemo_task����
	void emwindemo_task(void *p_arg);

//OneNet�������
//�����������ȼ�
#define TASK8_PRIO			8
//�����ջ��С
#define TASK8_STK_SIZE			256
//������ƿ�
OS_TCB TASK8_TCB;
//�����ջ
CPU_STK Task8_Stk[TASK8_STK_SIZE];
//emwindemo_task����
	void Task8(void *p_arg);

//OneNet������������
//�����������ȼ�
#define TASK9_PRIO			9
//�����ջ��С
#define TASK9_STK_SIZE			256
//������ƿ�
OS_TCB TASK9_TCB;
//�����ջ
CPU_STK Task9_Stk[TASK9_STK_SIZE];
//emwindemo_task����
	void Task9(void *p_arg);

#define TASK10_PRIO			10
//�����ջ��С
#define TASK10_STK_SIZE			128
//������ƿ�
OS_TCB TASK10_TCB;
//�����ջ
CPU_STK Task10_Stk[TASK10_STK_SIZE];
//����Onenet��������
void Task10(void *p_arg);

#define TASK11_PRIO			11
//�����ջ��С
#define TASK11_STK_SIZE			128
//������ƿ�
OS_TCB TASK11_TCB;
//�����ջ
CPU_STK Task11_Stk[TASK10_STK_SIZE];
//����Onenet��������
void Task11(void *p_arg);

#endif
