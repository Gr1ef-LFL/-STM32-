#ifndef DLH_H            // Guard against multiple inclusion
#define DLH_H
#include "DIALOG.h"

/* ������UI */
WM_HWIN CreateFramewin(void);

/* �ƹ���ƽ���UI */
WM_HWIN CreateLightFramewin(void);

/* ��ӭ����UI */
WM_HWIN CreatehuanyinFramewin(void);

/* �Ӽ�����UI */
WM_HWIN CreateDataUi(void);

/* ��������UI */
WM_HWIN ElectricalControl(void);

/* ϵͳ����UI */
WM_HWIN CreateConfig(void);

/* ����ϵͳʱ����UI*/
WM_HWIN CreateTimeSetWindow(void);

#endif
