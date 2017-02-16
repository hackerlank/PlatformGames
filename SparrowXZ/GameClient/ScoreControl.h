#ifndef SCORE_CONTROL_HEAD_FILE
#define SCORE_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CardControl.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//������Ϣ
struct tagScoreInfo
{
	//�û���Ϣ
	WORD							wMeChairId;								//��ǰ�û�
	BYTE							cbWinOrder;								//��������
	TCHAR							szUserName[GAME_PLAYER][32];			//�û�����

	//������Ϣ
	LONG							lGameTax;								//��Ϸ˰��
	LONGLONG						lGameScore;								//��Ϸ����
	LONGLONG						lGangScore;								//��ϸ�÷�
	WORD							wProvideUser;							//��Ӧ�û�
	DWORD							dwChiHuRight[MAX_RIGHT_COUNT];			//��������

	
	BYTE							cbGenCount;								//�и���Ŀ
	WORD							wLostFanShu[GAME_PLAYER];				//��������
	LONGLONG						lHuaZhuScore[GAME_PLAYER];				//����÷�
	LONGLONG						lChaJiaoScore[GAME_PLAYER];				//��е÷�
	LONGLONG						lLostHuaZhuScore[GAME_PLAYER];			//����÷�
	LONGLONG						lLostChaJiaoScore[GAME_PLAYER];			//��е÷�
};

//////////////////////////////////////////////////////////////////////////

//���ֿؼ�
class CScoreControl : public CVirtualWindow
{
	//��������
protected:
	tagScoreInfo					m_ScoreInfo;							//������Ϣ
	CChiHuRight						m_ChiHuRight;							//��������
	CD3DFont						m_FontScore;							//��������

	//��Դ����
protected:
	CD3DTexture						m_ImageGameWin;							//������ͼ
	CD3DTexture						m_ImageGameLost;						//������ͼ
	CD3DTexture						m_PngNum;								//������ͼ

	//��������
public:
	//���캯��
	CScoreControl();
	//��������
	virtual ~CScoreControl();

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

	//���ܺ���
public:
	//���ش���
	VOID CloseControl();
	//���û���
	void SetScoreInfo(const tagScoreInfo & ScoreInfo);

	//��������
protected:
	//�滭����
	void DrawNumberString(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/);
};

//////////////////////////////////////////////////////////////////////////

#endif
