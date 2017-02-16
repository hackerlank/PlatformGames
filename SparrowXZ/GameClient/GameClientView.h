#pragma once

#include "Stdafx.h"
#include "ControlWnd.h"
#include "CardControl.h"
#include "ScoreControl.h"
#include "HistoryScore.h"
#include "DrawSiceWnd.h"

//////////////////////////////////////////////////////////////////////////

#define FRAME_HEIGHT					38								//���ͷ��
#define FRAME_CAPTION					30								//��ܱ���

//////////////////////////////////////////////////////////////////////////
//��Ϣ����

#define IDM_START					(WM_USER+100)						//��ʼ��Ϣ
#define IDM_CALL_CARD				(WM_USER+101)						//ѡȱ��Ϣ
#define IDM_OUT_CARD				(WM_USER+102)						//������Ϣ
#define IDM_TRUSTEE_CONTROL			(WM_USER+103)						//�йܿ���
#define WM_SET_CAPTION				(WM_USER+229)						//������Ϣ
#define IDM_LBUTTONDOWN_VIEW		(WM_USER+220)						//�����ͼ

#define IDI_DISC_EFFECT				102								//����Ч��

//////////////////////////////////////////////////////////////////////////

//��Ϸ��ͼ
class CGameClientView : public CGameFrameViewD3D
{
	//��־����
protected:
	bool							m_bOutCard;							//���Ʊ�־
	bool							m_bWaitOther;						//�ȴ���־
	bool							m_bTrustee[GAME_PLAYER];			//�Ƿ��й�
	LONGLONG						m_lGangScore[GAME_PLAYER];			//���Ƽ���
	bool							m_bShowSelectTip;					//��ʾ��ȱ
	bool							m_bShowWaitChooseTip;				//�ȴ���ȱ
	BYTE							m_cbVoidShowing[GAME_PLAYER];		//ȱ�ű�־

	//��Ϸ����
protected:
	LONGLONG						m_lCellScore;						//��������
	WORD							m_wBankerUser;						//ׯ���û�
	WORD							m_wCurrentUser;						//��ǰ�û�
	BYTE							m_cbWinOrder[GAME_PLAYER];			//����˳��

	//��ʷ����
public:
	bool							m_bShowScore;						//�ɼ���־
	tagHistoryScore	*				m_pHistoryScore[GAME_PLAYER];		//�ɼ���Ϣ

	//��������
protected:
	bool							m_bBombEffect;						//����Ч��
	BYTE							m_cbBombFrameIndex;					//֡������

	//����Ч��
	WORD							m_wDiscUser;						//�����û�
	BYTE							m_cbDiscFrameIndex;					//֡������

	//���ƶ���
	int								m_nYGangStep;						//��������
	BYTE							m_cbXiaYu;							//�η�����
	WORD							m_wGangChairId;						//������ͼ
	BYTE							m_cbGangIndex;						//��������
	CLapseCount						m_CartoonGangLapseCount;			//���ż���

	//�û�״̬
protected:
	BYTE							m_cbCardData;						//�����˿�
	WORD							m_wOutCardUser;						//�����û�
	BYTE							m_cbUserAction[GAME_PLAYER];		//�û�����

	//λ�ñ���
protected:
	CPoint							m_ptUserFrame[GAME_PLAYER];			//���λ��
	CPoint							m_UserFlagPos[GAME_PLAYER];			//��־λ��
	CPoint							m_PointTrustee[GAME_PLAYER];		//�й�λ��
	CPoint							m_ptWinOrder[GAME_PLAYER];			//˳��λ��
	CPoint							m_ptGangScore[GAME_PLAYER];			//�ܷ�λ��
	CPoint							m_ptVoidShowPos[GAME_PLAYER];		//ȱ��λ��

	//ͼƬ��Դ
protected:
	CD3DTexture						m_ImageBack;						//����ͼ��
	CD3DTexture						m_ImageCenter;						//LOGO  ͼ
	CD3DTexture						m_ImageWait;						//�ȴ���ʾ
	CD3DTexture						m_ImageOutCard;						//������ʾ
	CD3DTexture						m_ImageUserFlag;					//�û���־
	CD3DTexture						m_ImageActionBack;					//��������
	CD3DTexture						m_ImageTrustee;						//�йܱ�־
	CD3DTexture						m_ImageActionAni;					//���ƶ�����Դ
	CD3DTexture						m_ImageDisc;						//����Ч��
	CD3DTexture						m_ImageArrow;						//��ʱ����ͷ	
	CD3DTexture						m_ImageWinOrder;					//����˳��
	CD3DTexture						m_ImageCellScore;					//ͼƬ��Դ
	CD3DTexture						m_PngNumWin;						//ͼƬ��Դ
	CD3DTexture						m_PngNumLost;						//ͼƬ��Դ
	CD3DTexture						m_PngXiaYu;							//ͼƬ��Դ
	CD3DTexture						m_PngGuaFeng;						//ͼƬ��Դ
	CD3DTexture						m_TextureUserFrameH;				//�û����
	CD3DTexture						m_TextureUserFrameV;				//�û����
	CD3DTexture						m_TextureVoidShowing;				//ȱ�ű�־
	CD3DTexture						m_TextureVoidShowingTip;			//ȱ�ű�־
	CD3DTexture						m_TextureWaitChooseTip;				//��ȱ�ȴ�
	CD3DTexture						m_TextureHistoryScore;				//��ʷ�ɼ�

	//ȱ�Ű�ť
public:
	CVirtualButton					m_btCallCard1;						//ȱ�Ű�ť
	CVirtualButton					m_btCallCard2;						//ȱ�Ű�ť
	CVirtualButton					m_btCallCard3;						//ȱ�Ű�ť
	CVirtualButton					m_btOpenHistoryScore;				//���ְ�ť
	CVirtualButton					m_btCloseHistoryScore;				//���ְ�ť

	//�˿˿ؼ�
public:
	CHeapCard						m_HeapCard[GAME_PLAYER];			//�����˿�
	CUserCard						m_UserCard[GAME_PLAYER];			//�û��˿�
	CTableCard						m_TableCard[GAME_PLAYER];			//�����˿�
	CWeaveCard						m_WeaveCard[GAME_PLAYER][4];		//����˿�
	CDiscardCard					m_DiscardCard[GAME_PLAYER];			//�����˿�
	CCardControl					m_HandCardControl;					//�����˿�

	//�ؼ�����
public:
	CVirtualButton					m_btStart;							//��ʼ��ť
	CVirtualButton					m_btStustee;						//�Ϲܿ���
	CVirtualButton					m_btCancelStustee;					//�Ϲܿ���
	CControlWnd						m_ControlWnd;						//���ƴ���
	CScoreControl					m_ScoreControl[GAME_PLAYER];		//���ֿؼ�
	CDrawSiceWnd					m_DrawSiceWnd;						//ɫ�ӿؼ�

protected:
	CString							m_StringCaption;					//����
	CDFontEx						m_FontCaption;						//��������
	CD3DTexture						m_ImageGameFrame[8];				//��Ϸ���
	
	//�ؼ�����
protected:
	CSkinButton						m_btMin;							//��С����ť
	CSkinButton						m_btClose;							//�رհ�ť
	CSkinButton						m_btBank;							//���а�ť

	//��������
public:
	//���캯��
	CGameClientView();
	//��������
	virtual ~CGameClientView();

	//���غ���
private:
	//���ý���
	virtual VOID ResetGameView();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//���溯��
protected:
	//��������
	virtual VOID CartoonMovie();
	//�����豸
	virtual VOID InitGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);
	//�滭����
	virtual VOID DrawGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);
	//�滭���
	virtual VOID DrawGameFrame(CD3DDevice *pD3DDevice, int nWidth, int nHeight);

	//���ܺ���
public:
	//��������
	void SetCellScore(LONGLONG lCellScore);
	//ׯ���û�
	void SetBankerUser(WORD wBankerUser);
	//״̬��־
	void SetStatusFlag(bool bOutCard, bool bWaitOther);
	//������Ϣ
	void SetOutCardInfo(WORD wViewChairID, BYTE cbCardData);
	//������Ϣ
	void SetUserAction(WORD wViewChairID, BYTE bUserAction);
	//���ö���
	bool SetBombEffect(bool bBombEffect);
	//�����û�
	void SetDiscUser(WORD wDiscUser);
	//��ʱ���
	void SetCurrentUser(WORD wCurrentUser);
	//�����й�
	void SetTrustee(WORD wTrusteeUser,bool bTrustee);
	//����˳��
	void SetWinOrder( WORD wChairId, BYTE cbWinOrder );
	//���øܷ�
	void SetGangScore( WORD wChairId, BYTE cbXiaYu, LONGLONG lGangScore[GAME_PLAYER] );
	//����ȱ��
	void SetVoidShowing(WORD wChairID, BYTE cbShowIndex);
	//���û���
	VOID SetHistoryScore(WORD wChairID, tagHistoryScore * pHistoryScore);
	//��ʾ����
	VOID ShowGameScoreInfo();
	//��ʾ��ȱ
	void ShowSelectTip(bool bShow);
	//��ʾ��ȱ
	void ShowWaitChooseTip(bool bShow);

	//��������
protected:
	//�滭����
	void DrawNumberString(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/);
	//�滭С����
	void DrawDecimalString(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/);
	// ���С����
	CString AddDecimal( LONGLONG lScore ,  bool bComma = true, bool bPlus = false );

	//��Ϣӳ��
protected:
	//��ʼ��ť
	afx_msg void OnStart();
	//��ȱ��ť
	afx_msg void OnCallCard1();
	//��ȱ��ť
	afx_msg void OnCallCard2();
	//��ȱ��ť
	afx_msg void OnCallCard3();
	//�Ϲܿ���
	afx_msg void OnStusteeControl();
	//���ְ�ť
	afx_msg void OnBnClickedOpenHistory();
	//���ְ�ť
	afx_msg void OnBnClickedCloseHistory();
	//��������
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//�����Ϣ
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);
	//�����Ϣ
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
	//����
	afx_msg LRESULT OnSetCaption(WPARAM wParam, LPARAM lParam);
	//��С��ť	
	afx_msg void OnButtonMin();
	//�رհ�ť	
	afx_msg void OnButtonClose();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnTimer(UINT nIDEvent);
};

//////////////////////////////////////////////////////////////////////////
