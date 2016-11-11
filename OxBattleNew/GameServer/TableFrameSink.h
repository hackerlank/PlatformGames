#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "ServerControl.h"

//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//��Ϸ����
protected:
	bool                            m_bSpecialClient[GAME_PLAYER];          //�����ն� 
	WORD							m_wBankerUser;							//ׯ���û�
	WORD							m_wFisrtCallUser;						//ʼ���û�
	WORD							m_wCurrentUser;							//��ǰ�û�
	LONGLONG						m_lExitScore;							//ǿ�˷���
	LONGLONG                        m_lDynamicScore;                        //�ܷ� 
	static bool                     m_bFirstInit;							//��ʼ��

	//�û�����
protected:
	BYTE                            m_cbDynamicJoin[GAME_PLAYER];           //��̬����
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//��Ϸ״̬
	BYTE							m_cbCallStatus[GAME_PLAYER];			//��ׯ״̬
	BYTE							m_cbOxCard[GAME_PLAYER];				//ţţ����
	LONGLONG						m_lTableScore[GAME_PLAYER];				//��ע��Ŀ
	bool							m_bBuckleServiceCharge[GAME_PLAYER];	//�շ����

	LONGLONG                        m_lUserBeginScore[GAME_PLAYER];			//��ʼ����(������)
	LONGLONG                        m_lUserTableScore[GAME_PLAYER];			//��ע����(������)

	//�˿˱���
protected:
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//�����˿�

	//��ע��Ϣ
protected:
	LONGLONG						m_lTurnMaxScore[GAME_PLAYER];			//�����ע

	//�������
protected:
	HINSTANCE						m_hInst;								//���ƾ��
	IServerControl*					m_pServerControl;						//�������

	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	ITableFrame						* m_pITableFrame;						//��ܽӿ�
	CHistoryScore					m_HistoryScore;							//��ʷ�ɼ�
	tagGameServiceOption		    *m_pGameServiceOption;					//���ò���
	tagGameServiceAttrib			*m_pGameServiceAttrib;					//��Ϸ����

	//���Ա���
protected:
	static const WORD				m_wPlayerCount;							//��Ϸ����

	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() {}
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//�����ӿ�
public:
	//��ʼ��
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual void RepositionSink();

	virtual bool IsUserPlaying(WORD wChairID);
	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart();
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE bGameStatus, bool bSendSecret);


	//�¼��ӿ�
public:
	//��ʱ���¼�
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam);
	//��Ϸ��Ϣ����
	virtual bool OnGameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����
	virtual bool OnFrameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����¼�
	virtual bool OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize){return true;}

	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//��ѯ�����
	virtual bool QueryBuckleServiceCharge(WORD wChairID);
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){return false;}
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore){return;}


	//�û��¼�
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; };
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }

	//��Ϸ�¼�
protected:
	//��ׯ�¼�
	bool OnUserCallBanker(WORD wChairID, BYTE bBanker);
	//��ע�¼�
	bool OnUserAddScore(WORD wChairID, LONGLONG lScore);
	//̯���¼�
	bool OnUserOpenCard(WORD wChairID, BYTE bOx);
	//д�ֺ���
	bool TryWriteTableScore(tagScoreInfo ScoreInfoArray[]);

	//���ܺ���
protected:
	//�˿˷���
	void AnalyseCard();
	//����·�
	SCORE GetUserMaxTurnScore(WORD wChairID);
	//�Ƿ�ɼ�
	bool UserCanAddScore(WORD wChairID, LONGLONG lAddScore);
	//�жϿ��
	bool JudgeStock();
	//�Ƿ�˥��
	bool NeedDeductStorage();
	//��ȡ����
	void ReadConfigInformation();
	//���·����û���Ϣ
	void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction);
	//����ͬ���û�����
	void UpdateUserControl(IServerUserItem *pIServerUserItem);
	//�����û�����
	void TravelControlList(ROOMUSERCONTROL keyroomusercontrol);
	//�Ƿ������������
	void IsSatisfyControl(ROOMUSERINFO &userInfo, bool &bEnableControl);
	//���������û�����
	bool AnalyseRoomUserControl(ROOMUSERCONTROL &Keyroomusercontrol, POSITION &ptList);
	//��ȡ��������
	void GetControlTypeString(CONTROL_TYPE &controlType, CString &controlTypestr);
	//д��־�ļ�
	void WriteInfo(LPCTSTR pszString);
};

//////////////////////////////////////////////////////////////////////////

#endif