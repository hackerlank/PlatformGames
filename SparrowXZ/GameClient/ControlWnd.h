#ifndef CONTROL_WND_HEAD_FILE
#define CONTROL_WND_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////////////

//��Ϣ����
//��Ϣ����
#define IDM_LISTEN_CARD				(WM_USER+500)						//�˿˲���
#define IDM_CARD_OPERATE			(WM_USER+501)						//�˿˲���

//////////////////////////////////////////////////////////////////////////////////

//���ֿؼ�
class CControlWnd : public CVirtualWindow
{
	//�Ҷȶ���
protected:
	BYTE							m_cbAlphaIndex;						//͸������
	CLapseCount						m_AlphaLapseCount;					//���ż���

	//���ñ���
protected:
	BYTE							m_cbActionMask;						//��������
	BYTE							m_cbCenterCard;						//�����˿�
	BYTE							m_cbGangCard[16];					//��������
	CPoint							m_PointBenchmark;					//��׼λ��
	CSize							m_ControlSize;

	//״̬����
protected:
	BYTE							m_cbItemCount;						//������Ŀ
	BYTE							m_cbCurrentItem;					//��ǰ����

	//�ؼ�����
protected:
	CVirtualButton					m_btChiHu;							//�Ժ���ť
	CVirtualButton					m_btListen;							//���ư�ť
	CVirtualButton					m_btGiveUp;							//������ť

	//��Դ����
protected:
	CD3DTexture						m_TextureControlTop;				//��ԴͼƬ
	CD3DTexture						m_TextureControlMid;				//��ԴͼƬ
	CD3DTexture						m_TextureControlButtom;				//��ԴͼƬ
	CD3DTexture						m_TextureActionExplain;				//��������
	CD3DTexture						m_TextureItemFrame;					//ѡ�б߿�

	//��������
public:
	//���캯��
	CControlWnd();
	//��������
	virtual ~CControlWnd();

	//�ؼ�����
public:
	//��׼λ��
	void SetBenchmarkPos(int nXPos, int nYPos);
	//�����˿�
	void SetControlInfo(BYTE cbCenterCard, BYTE cbActionMask, tagGangCardResult & GangCardResult);
	//���ش���
	VOID CloseControl();

	//�ڲ�����
protected:
	//�����ؼ�
	void RectifyControl();

	//ϵͳ�¼�
protected:
	//������Ϣ
	virtual VOID OnWindowMovie();
	//������Ϣ
	virtual VOID OnWindowCreate(CD3DDevice * pD3DDevice);
	//������Ϣ
	virtual VOID OnWindowDestory(CD3DDevice * pD3DDevice);

	//���غ���
protected:
	//����¼�
	virtual VOID OnEventMouse(UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos);
	//��ť�¼�
	virtual VOID OnEventButton(UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos);
	//�滭����
	virtual VOID OnEventDrawWindow(CD3DDevice * pD3DDevice, INT nXOriginPos, INT nYOriginPos);
};

//////////////////////////////////////////////////////////////////////////////////

#endif