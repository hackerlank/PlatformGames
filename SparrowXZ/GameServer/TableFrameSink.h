#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "HistoryScore.h"

//////////////////////////////////////////////////////////////////////////
//ö�ٶ���

//Ч������
enum enEstimatKind
{
	EstimatKind_OutCard,			//����Ч��
	EstimatKind_GangCard,			//����Ч��
};

//���Ƶ÷�
struct tagGangScore
{
	BYTE		cbGangCount;							//�ܸ���
	LONGLONG	lScore[MAX_WEAVE][GAME_PLAYER];			//ÿ���ܵ÷�
};

//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//��Ϸ����
protected:
	LONG							m_lSiceCount;							//���ӵ���
	WORD							m_wBankerUser;							//ׯ���û�
	WORD							m_wFirstWinUser;						//��Ӯ�û�
	LONGLONG						m_lGameScore[GAME_PLAYER];				//��Ϸ�÷�
	BYTE							m_cbCardIndex[GAME_PLAYER][MAX_INDEX];	//�û��˿�
	bool							m_bTrustee[GAME_PLAYER];				//�Ƿ��й�
	BYTE							m_cbGenCount[GAME_PLAYER];				//�и���Ŀ
	bool							m_bPlayStatus[GAME_PLAYER];				//���״̬
	BYTE							m_cbWinCount;							//Ӯ�ı���
	WORD							m_wWinOrder[GAME_PLAYER];				//����˳��
	LONG							m_lGameTax[GAME_PLAYER];				//��Ϸ˰��
	tagGangScore					m_GangScore[GAME_PLAYER];				//���Ʒ���
	WORD							m_wLostFanShu[GAME_PLAYER][GAME_PLAYER];//��������

	//ȱ����Ϣ
protected:
	bool							m_bCallCard[GAME_PLAYER];				//�û�״̬
	BYTE							m_cbCallCard[GAME_PLAYER];				//�û�ȱ��

	//������Ϣ
protected:
	WORD							m_wOutCardUser;							//�����û�
	BYTE							m_cbOutCardData;						//�����˿�
	BYTE							m_cbOutCardCount;						//������Ŀ
	BYTE							m_cbDiscardCount[GAME_PLAYER];			//������Ŀ
	BYTE							m_cbDiscardCard[GAME_PLAYER][55];		//������¼

	//������Ϣ
protected:
	BYTE							m_cbSendCardData;						//�����˿�
	BYTE							m_cbSendCardCount;						//������Ŀ
	BYTE							m_cbLeftCardCount;						//ʣ����Ŀ
	BYTE							m_cbRepertoryCard[MAX_REPERTORY];		//����˿�

	//��������
protected:
	WORD							m_wHeapHand;							//����ͷ��
	WORD							m_wHeapTail;							//����β��
	BYTE							m_cbHeapCardInfo[4][2];					//������Ϣ

	//���б���
protected:
	WORD							m_wResumeUser;							//��ԭ�û�
	WORD							m_wCurrentUser;							//��ǰ�û�
	WORD							m_wProvideUser;							//��Ӧ�û�
	BYTE							m_cbProvideCard;						//��Ӧ�˿�

	//״̬����
protected:
	bool							m_bSendStatus;							//����״̬
	bool							m_bGangStatus;							//����״̬
	WORD							m_wProvideGang;							//�����û�
	bool							m_bGangOutStatus;						//����״̬
	bool							m_bEnjoinChiHu[GAME_PLAYER];			//��ֹ�Ժ�
	bool							m_bEnjoinChiPeng[GAME_PLAYER];			//��ֹ����

	//�û�״̬
public:
	bool							m_bResponse[GAME_PLAYER];				//��Ӧ��־
	BYTE							m_cbUserAction[GAME_PLAYER];			//�û�����
	BYTE							m_cbOperateCard[GAME_PLAYER];			//�����˿�
	BYTE							m_cbPerformAction[GAME_PLAYER];			//ִ�ж���

	//����˿�
protected:
	BYTE							m_cbWeaveItemCount[GAME_PLAYER];		//�����Ŀ
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][MAX_WEAVE];//����˿�

	//������Ϣ
protected:
	BYTE							m_cbChiHuCard;							//�Ժ��˿�
	DWORD							m_dwChiHuKind[GAME_PLAYER];				//�Ժ����
	CChiHuRight						m_ChiHuRight[GAME_PLAYER];				//�Ժ�Ȩλ
	WORD							m_wProvider[GAME_PLAYER];				//�����û�

	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	CHistoryScore					m_HistoryScore;							//��ʷ�ɼ�
	ITableFrame						* m_pITableFrame;						//��ܽӿ�
	const tagGameServiceOption		* m_pGameServiceOption;					//���ò���


	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { }
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��ʼ��
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual VOID RepositionSink();

	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem){  return 0; };
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}

	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore){};

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart();
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//�¼��ӿ�
public:
	//��ʱ���¼�
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize) { return false; }
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason) { return false; }

	//����ӿ�
public:
	//��Ϸ��Ϣ����
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//�û��¼�
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize) { return true; }


	//��Ϸ�¼�
protected:
	//�û�ѡȱ
	bool OnUserCallCard(WORD wChairID, BYTE cbCallCard);
	//�û�����
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData);
	//�û�����
	bool OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard);

	//��������
protected:
	//���Ͳ���
	bool SendOperateNotify();
	//�ɷ��˿�
	bool DispatchCardData(WORD wCurrentUser,bool bTail=false);
	//��Ӧ�ж�
	bool EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind);
	//�Ժ��û�
	void ProcessChiHuUser( WORD wChairId, bool bGiveUp );
	//����Ȩλ
	void FiltrateRight( WORD wChairId,CChiHuRight &chr );
};

//////////////////////////////////////////////////////////////////////////

#endif
