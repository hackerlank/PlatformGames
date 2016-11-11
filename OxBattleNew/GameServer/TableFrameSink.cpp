#include "StdAfx.h"
#include "TableFrameSink.h"
#include "DlgCustomRule.h"
#include <conio.h>
#include <locale>

//////////////////////////////////////////////////////////////////////////

//��̬����
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//��Ϸ����
bool				CTableFrameSink::m_bFirstInit=true;

//���������Ϣ
CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO> g_MapRoomUserInfo;	//���USERIDӳ�������Ϣ
//�����û�����
CList<ROOMUSERCONTROL, ROOMUSERCONTROL&> g_ListRoomUserControl;		//�����û���������
//�������Ƽ�¼
CList<CString, CString&> g_ListOperationRecord;						//�������Ƽ�¼

ROOMUSERINFO	g_CurrentQueryUserInfo;								//��ǰ��ѯ�û���Ϣ

//ȫ�ֱ���
LONGLONG						g_lRoomStorageStart = 0LL;								//������ʼ���
LONGLONG						g_lRoomStorageCurrent = 0LL;							//����Ӯ��
LONGLONG						g_lStorageDeductRoom = 0LL;								//�ؿ۱���
LONGLONG						g_lStorageMax1Room = 0LL;								//���ⶥ
LONGLONG						g_lStorageMul1Room = 0LL;								//ϵͳ��Ǯ����
LONGLONG						g_lStorageMax2Room = 0LL;								//���ⶥ
LONGLONG						g_lStorageMul2Room = 0LL;								//ϵͳ��Ǯ����
//////////////////////////////////////////////////////////////////////////

#define	IDI_SO_OPERATE							2							//����ʱ��
#define	TIME_SO_OPERATE							80000						//����ʱ��

//������ʶ
#define IDI_DELAY_ENDGAME			10				//������ʶ
#define IDI_DELAY_TIME				3000			//��ʱʱ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrameSink::CTableFrameSink()
{
	//��Ϸ����	
	m_lExitScore=0;	
	m_lDynamicScore=0;
	m_wBankerUser=INVALID_CHAIR;
	m_wFisrtCallUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	//�û�״̬
	ZeroMemory(m_cbDynamicJoin,sizeof(m_cbDynamicJoin));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbCallStatus,sizeof(m_cbCallStatus));
	ZeroMemory(m_bSpecialClient,sizeof(m_bSpecialClient));
	for(BYTE i=0;i<m_wPlayerCount;i++)m_cbOxCard[i]=0xff;

	//�˿˱���
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//��ע��Ϣ
	ZeroMemory(m_lTurnMaxScore,sizeof(m_lTurnMaxScore));

	ZeroMemory(m_bBuckleServiceCharge,sizeof(m_bBuckleServiceCharge));

	//�������
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	//�������
	g_ListRoomUserControl.RemoveAll();
	g_ListOperationRecord.RemoveAll();
	ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));

	//�������
	m_hInst = NULL;
	m_pServerControl = NULL;
	m_hInst = LoadLibrary(TEXT("OxNewServerControl.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerControl = (CREATE)GetProcAddress(m_hInst,"CreateServerControl"); 
		if ( ServerControl )
		{
			m_pServerControl = static_cast<IServerControl*>(ServerControl());
		}
	}


	if(m_bFirstInit)
	{
		CString strName = GetFileDialogPath()+	"\\OxNewBattle.log";
		CFileFind findLogFile;
		if(findLogFile.FindFile(strName))
		{
			::DeleteFile(strName);
		}

		m_bFirstInit=false;
	}
	return;
}

//��������
CTableFrameSink::~CTableFrameSink(void)
{
	if( m_pServerControl )
	{
		delete m_pServerControl;
		m_pServerControl = NULL;
	}

	if( m_hInst )
	{
		FreeLibrary(m_hInst);
		m_hInst = NULL;
	}
}

//�ӿڲ�ѯ--��������Ϣ�汾
void * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//��ʼ��
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

	//��Ϸ����
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	
	//��ȡ����
	ReadConfigInformation();


	return true;
}

//��λ����
void CTableFrameSink::RepositionSink()
{
	//��Ϸ����
	m_lExitScore=0;	
	m_wCurrentUser=INVALID_CHAIR;
	m_lDynamicScore=0;
	//�û�״̬
	ZeroMemory(m_cbDynamicJoin,sizeof(m_cbDynamicJoin));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbCallStatus,sizeof(m_cbCallStatus));
	ZeroMemory(m_bSpecialClient,sizeof(m_bSpecialClient));
	for(BYTE i=0;i<m_wPlayerCount;i++)m_cbOxCard[i]=0xff;

	//�˿˱���
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//��ע��Ϣ
	ZeroMemory(m_lTurnMaxScore,sizeof(m_lTurnMaxScore));

	return;
}

//��Ϸ״̬
bool CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	ASSERT(wChairID<m_wPlayerCount && m_cbPlayStatus[wChairID]==TRUE);
	if(wChairID<m_wPlayerCount && m_cbPlayStatus[wChairID]==TRUE)return true;
	return false;
}

//�û�����
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//���·����û���Ϣ
	UpdateRoomUserInfo(pIServerUserItem, USER_OFFLINE);

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��ʷ����
	if (bLookonUser==false) m_HistoryScore.OnEventUserEnter(pIServerUserItem->GetChairID());

	if(m_pITableFrame->GetGameStatus()!=GS_TK_FREE)
		m_cbDynamicJoin[pIServerUserItem->GetChairID()]=true;

	//���·����û���Ϣ
	UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
	
	//����ͬ���û�����
	UpdateUserControl(pIServerUserItem);

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��ʷ����
	if (bLookonUser==false) 
	{
		m_HistoryScore.OnEventUserLeave(pIServerUserItem->GetChairID());
		m_cbDynamicJoin[pIServerUserItem->GetChairID()]=false;
		m_bSpecialClient[pIServerUserItem->GetChairID()]=false;
	}
	
	//���·����û���Ϣ
	UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

	return true;
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	//���
	if(g_lRoomStorageCurrent>0 && NeedDeductStorage())g_lRoomStorageCurrent=g_lRoomStorageCurrent-g_lRoomStorageCurrent*g_lStorageDeductRoom/1000;

	CString strInfo;
	strInfo.Format(TEXT("��ǰ��棺%I64d"), g_lRoomStorageCurrent);
	NcaTextOut(strInfo, m_pGameServiceOption->szServerName);
	
	//����״̬
	m_pITableFrame->SetGameStatus(GS_TK_CALL);

	//�û�״̬
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//��ȡ�û�
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

		if(pIServerUserItem==NULL)
		{
			m_cbPlayStatus[i]=FALSE;
		}
		else
		{
			m_cbPlayStatus[i]=TRUE;

			//���·����û���Ϣ
			UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
		}
	}

	ZeroMemory(m_bBuckleServiceCharge,sizeof(m_bBuckleServiceCharge));

	//�׾����ʼ��
	if(m_wFisrtCallUser==INVALID_CHAIR)
	{
		m_wFisrtCallUser=rand()%m_wPlayerCount;
	}
	else
	{
		m_wFisrtCallUser=(m_wFisrtCallUser+1)%m_wPlayerCount;
	}

	//ʼ���û�
	while(m_cbPlayStatus[m_wFisrtCallUser]!=TRUE)
	{
		m_wFisrtCallUser=(m_wFisrtCallUser+1)%m_wPlayerCount;
	}

	//��ǰ�û�
	m_wCurrentUser=m_wFisrtCallUser;

	//���ñ���
	CMD_S_CallBanker CallBanker;
	CallBanker.wCallBanker=m_wCurrentUser;
	CallBanker.bFirstTimes=true;
	CopyMemory(CallBanker.cbPlayerStatus,m_cbPlayStatus,sizeof(m_cbPlayStatus));
	//��������
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbPlayStatus[i]!=TRUE) continue;
		m_pITableFrame->SendTableData(i,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));
	}
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));
	m_pITableFrame->SetGameTimer(IDI_SO_OPERATE,TIME_SO_OPERATE,1,0);

	//����˿�
	CMD_S_AllCard AllCard={0};
	BYTE bTempArray[GAME_PLAYER*MAX_COUNT];
	m_GameLogic.RandCardList(bTempArray,sizeof(bTempArray));
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);	
		if(pIServerUser==NULL)continue;
		if(pIServerUser->IsAndroidUser())AllCard.bAICount[i] =true;

		//�ɷ��˿�
		CopyMemory(m_cbHandCardData[i],&bTempArray[i*MAX_COUNT],MAX_COUNT);
		CopyMemory(AllCard.cbCardData[i],&bTempArray[i*MAX_COUNT],MAX_COUNT);
	}

//	//��������
//	for (WORD i=0;i<m_wPlayerCount;i++)
//	{
//		IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);	
//		if(pIServerUser==NULL)continue;
//#ifndef _DEBUG
//		if(CUserRight::IsGameCheatUser(pIServerUser->GetUserRight())==false || m_bSpecialClient[i]==false)continue;
//#endif 
//		m_pITableFrame->SendTableData(i,SUB_S_ALL_CARD,&AllCard,sizeof(AllCard));
//	}
	return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_DISMISS:		//��Ϸ��ɢ
		{
			////Ч�����
			//ASSERT(pIServerUserItem!=NULL);
			//ASSERT(wChairID<GAME_PLAYER);

			//��������
			CMD_S_GameEnd GameEnd = {0};

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			//���·����û���Ϣ
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				//��ȡ�û�
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

				if (!pIServerUserItem)
				{
					continue;
				}

				UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);
			}

			return true;
		}
	case GER_NORMAL:		//�������
		{
			//ɾ��ʱ��
			m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

			//�������
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			WORD wWinTimes[GAME_PLAYER],wWinCount[GAME_PLAYER];
			ZeroMemory(wWinCount,sizeof(wWinCount));
			ZeroMemory(wWinTimes,sizeof(wWinTimes));

			//�����˿�
			BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
			CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

			//ׯ�ұ���
			ASSERT(m_cbOxCard[m_wBankerUser]<2);
			if(m_cbOxCard[m_wBankerUser]==TRUE)
				wWinTimes[m_wBankerUser]=m_GameLogic.GetTimes(cbUserCardData[m_wBankerUser],MAX_COUNT);
			else wWinTimes[m_wBankerUser]=1;

			//�Ա����
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(i==m_wBankerUser || m_cbPlayStatus[i]==FALSE)continue;

				ASSERT(m_cbOxCard[i]<2);

				//�Ա��˿�
				if (m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,m_cbOxCard[i],m_cbOxCard[m_wBankerUser])) 
				{
					wWinCount[i]++;
					//��ȡ����
					if(m_cbOxCard[i]==TRUE)
						wWinTimes[i]=m_GameLogic.GetTimes(cbUserCardData[i],MAX_COUNT);
					else wWinTimes[i]=1;
				}
				else
				{
					wWinCount[m_wBankerUser]++;
				}
			}

			//ͳ�Ƶ÷�
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(i==m_wBankerUser || m_cbPlayStatus[i]==FALSE)continue;

				if(wWinCount[i]>0)	//�м�ʤ��
				{
					GameEnd.lGameScore[i]=m_lTableScore[i]*wWinTimes[i];
					GameEnd.lGameScore[m_wBankerUser]-=GameEnd.lGameScore[i];
					m_lTableScore[i]=0;
				}
				else					//ׯ��ʤ��
				{
					GameEnd.lGameScore[i]=(-1)*m_lTableScore[i]*wWinTimes[m_wBankerUser];
					GameEnd.lGameScore[m_wBankerUser]+=(-1)*GameEnd.lGameScore[i];
					m_lTableScore[i]=0;
				}
			}

			//�м�ǿ�˷���	
			GameEnd.lGameScore[m_wBankerUser]+=m_lExitScore;

			//�뿪�û�
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_lTableScore[i]>0)GameEnd.lGameScore[i]=-m_lTableScore[i];
			}

			//�޸Ļ���
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));

			bool bDelayOverGame=false;

			//����˰��
			for(WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)continue;

				if(GameEnd.lGameScore[i]>0L)
				{
					GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i,GameEnd.lGameScore[i]);
					if(GameEnd.lGameTax[i]>0)
						GameEnd.lGameScore[i] -= GameEnd.lGameTax[i];
				}

				//��ʷ����
				m_HistoryScore.OnEventUserScore(i,GameEnd.lGameScore[i]);

				//�������
				ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];
				ScoreInfoArray[i].lRevenue = GameEnd.lGameTax[i];
				ScoreInfoArray[i].cbType = (GameEnd.lGameScore[i]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;

				if(ScoreInfoArray[i].cbType ==SCORE_TYPE_LOSE && bDelayOverGame==false)
				{
					IServerUserItem *pUserItem=m_pITableFrame->GetTableUserItem(i);
					if(pUserItem!=NULL && (pUserItem->GetUserScore()+GameEnd.lGameScore[i] )<m_pGameServiceOption->lMinTableScore)
					{
	    				bDelayOverGame=true;
					}
				}
			}

#ifdef _DEBUG

			bDelayOverGame=true;

#endif // _DEBUG

			if(bDelayOverGame)
			{
				GameEnd.cbDelayOverGame = 3;
			}

			//������Ϣ
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE) continue;
				m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			}

			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			
			TryWriteTableScore(ScoreInfoArray);
			
			//���ͳ��
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(i);
				if (pIServerUserIte==NULL) continue;

				//����ۼ�
				if(!pIServerUserIte->IsAndroidUser())
					g_lRoomStorageCurrent-=GameEnd.lGameScore[i];

			}

			//������Ϸ
			if(bDelayOverGame)
			{
				ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
			   m_pITableFrame->SetGameTimer(IDI_DELAY_ENDGAME,IDI_DELAY_TIME,1,0L);
			}
			else
			{
				m_pITableFrame->ConcludeGame(GS_TK_FREE);

				//���·����û���Ϣ
				for (WORD i=0; i<GAME_PLAYER; i++)
				{
					//��ȡ�û�
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

					if (!pIServerUserItem)
					{
						continue;
					}

					UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
				}
			}

			//���Ϳ��
			CString strInfo;
			strInfo.Format(TEXT("��ǰ��棺%I64d"), g_lRoomStorageCurrent);
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if (!pIServerUserItem)
				{
					continue;
				}
				if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
				{
					m_pITableFrame->SendGameMessage(pIServerUserItem, strInfo, SMT_CHAT);

					CMD_S_ADMIN_STORAGE_INFO StorageInfo;
					ZeroMemory(&StorageInfo, sizeof(StorageInfo));
					StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
					StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
					StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
					StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
					StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
					StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
					StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
					m_pITableFrame->SendTableData(i, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
					m_pITableFrame->SendLookonData(i, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				}
			}
			
			return true;
		}
	case GER_USER_LEAVE:		//�û�ǿ��
	case GER_NETWORK_ERROR:
		{
			//Ч�����
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount && (m_cbPlayStatus[wChairID]==TRUE||m_cbDynamicJoin[wChairID]==FALSE));

			if(m_cbPlayStatus[wChairID]==FALSE) return true;
			//����״̬
			m_cbPlayStatus[wChairID]=FALSE;
			m_cbDynamicJoin[wChairID]=FALSE;

			//�������
			CMD_S_PlayerExit PlayerExit;
			PlayerExit.wPlayerID=wChairID;

			//������Ϣ
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(i==wChairID || (m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE))continue;
				m_pITableFrame->SendTableData(i,SUB_S_PLAYER_EXIT,&PlayerExit,sizeof(PlayerExit));
			}
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLAYER_EXIT,&PlayerExit,sizeof(PlayerExit));


			WORD wWinTimes[GAME_PLAYER];
			
			ZeroMemory(wWinTimes,sizeof(wWinTimes));

			if (m_pITableFrame->GetGameStatus()>GS_TK_CALL)
			{
				if (wChairID==m_wBankerUser)	//ׯ��ǿ��
				{
					//�������
					CMD_S_GameEnd GameEnd;
					ZeroMemory(&GameEnd,sizeof(GameEnd));
					ZeroMemory(wWinTimes,sizeof(wWinTimes));

					BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
					CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

					//�÷ֱ���
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(i==m_wBankerUser || m_cbPlayStatus[i]==FALSE)continue;
						wWinTimes[i]=(m_pITableFrame->GetGameStatus()!=GS_TK_PLAYING)?(1):(m_GameLogic.GetTimes(cbUserCardData[i],MAX_COUNT));
					}

					//ͳ�Ƶ÷� ���»�û��
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(i==m_wBankerUser || m_cbPlayStatus[i]==FALSE)continue;
						GameEnd.lGameScore[i]=m_lTableScore[i]*wWinTimes[i];
						GameEnd.lGameScore[m_wBankerUser]-=GameEnd.lGameScore[i];
						m_lTableScore[i]=0;
					}

					//�м�ǿ�˷��� 
					GameEnd.lGameScore[m_wBankerUser]+=m_lExitScore;

					//�뿪�û�
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(m_lTableScore[i]>0)GameEnd.lGameScore[i]=-m_lTableScore[i];
					}

					//�޸Ļ���
					tagScoreInfo ScoreInfoArray[GAME_PLAYER];
					ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

					//����˰��
					for(WORD i=0;i<m_wPlayerCount;i++)
					{
						if(m_cbPlayStatus[i]==FALSE && i!=m_wBankerUser)continue;
						
						if(GameEnd.lGameScore[i]>0L)
						{
							GameEnd.lGameTax[i]=m_pITableFrame->CalculateRevenue(i,GameEnd.lGameScore[i]);
							if(GameEnd.lGameTax[i]>0)
								GameEnd.lGameScore[i]-=GameEnd.lGameTax[i];
						}

						//�������
						ScoreInfoArray[i].lRevenue = GameEnd.lGameTax[i];
						ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];

						if(i==m_wBankerUser)
							ScoreInfoArray[i].cbType =SCORE_TYPE_FLEE;
						else if(m_cbPlayStatus[i]==TRUE)
							ScoreInfoArray[i].cbType =(GameEnd.lGameScore[i]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
					}
					
					TryWriteTableScore(ScoreInfoArray);

					//������Ϣ
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(i==m_wBankerUser || (m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE))continue;
						m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
					}
					m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
						
					//д����
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(m_cbPlayStatus[i]==FALSE && i!=m_wBankerUser)continue;

						//��ȡ�û�
						IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(i);

						//����ۼ�
						if ((pIServerUserIte!=NULL)&&(!pIServerUserIte->IsAndroidUser())) 
							g_lRoomStorageCurrent-=GameEnd.lGameScore[i];

					}
					//������Ϸ
					m_pITableFrame->ConcludeGame(GS_TK_FREE);
				}
				else						//�м�ǿ��
				{
					//�Ѿ���ע
					if (m_lTableScore[wChairID]>0L)
					{
						ZeroMemory(wWinTimes,sizeof(wWinTimes));

						//�û��˿�
						BYTE cbUserCardData[MAX_COUNT];
						CopyMemory(cbUserCardData,m_cbHandCardData[m_wBankerUser],MAX_COUNT);

						//�û�����
						wWinTimes[m_wBankerUser]=(m_pITableFrame->GetGameStatus()==GS_TK_SCORE)?(1):(m_GameLogic.GetTimes(cbUserCardData,MAX_COUNT));

						//�޸Ļ���
						LONGLONG lScore=-m_lTableScore[wChairID]*wWinTimes[m_wBankerUser];
						m_lExitScore+=(-1*lScore);
						m_lTableScore[wChairID]=(-1*lScore);
						
						tagScoreInfo ScoreInfoArray[GAME_PLAYER];
						ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));
						ScoreInfoArray[wChairID].lScore = lScore;
						ScoreInfoArray[wChairID].cbType = SCORE_TYPE_FLEE;
						
						TryWriteTableScore(ScoreInfoArray);

						//��ȡ�û�
						IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(wChairID);
						
						//����ۼ�
						if ((pIServerUserIte!=NULL)&&(!pIServerUserIte->IsAndroidUser())) 
							g_lRoomStorageCurrent-=lScore;
					}

					//�������
					WORD wUserCount=0;
					for (WORD i=0;i<m_wPlayerCount;i++)if(m_cbPlayStatus[i]==TRUE)wUserCount++;

					//������Ϸ
					if(wUserCount==1)
					{
						//�������
						CMD_S_GameEnd GameEnd;
						ZeroMemory(&GameEnd,sizeof(GameEnd));
						ASSERT(m_lExitScore>=0L); 

						//ͳ�Ƶ÷�
						GameEnd.lGameScore[m_wBankerUser]+=m_lExitScore;
						GameEnd.lGameTax[m_wBankerUser]=m_pITableFrame->CalculateRevenue(m_wBankerUser,GameEnd.lGameScore[m_wBankerUser]);
						GameEnd.lGameScore[m_wBankerUser]-=GameEnd.lGameTax[m_wBankerUser];

						//�뿪�û�
						for (WORD i=0;i<m_wPlayerCount;i++)
						{
							if(m_lTableScore[i]>0)GameEnd.lGameScore[i]=-m_lTableScore[i];
						}

						//������Ϣ
						m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
						m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

						WORD Zero = 0;
						for (Zero=0;Zero<m_wPlayerCount;Zero++)if(m_lTableScore[Zero]!=0)
							break;

						if(Zero!=m_wPlayerCount)
						{
							//�޸Ļ���
							tagScoreInfo ScoreInfoArray[GAME_PLAYER];
							ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));
							ScoreInfoArray[m_wBankerUser].lScore=GameEnd.lGameScore[m_wBankerUser];
							ScoreInfoArray[m_wBankerUser].lRevenue = GameEnd.lGameTax[m_wBankerUser];
							ScoreInfoArray[m_wBankerUser].cbType = SCORE_TYPE_WIN;

							TryWriteTableScore(ScoreInfoArray);

							//��ȡ�û�
							IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(wChairID);
							
							//����ۼ�
							if ((pIServerUserIte!=NULL)&&(!pIServerUserIte->IsAndroidUser())) 
								g_lRoomStorageCurrent-=GameEnd.lGameScore[m_wBankerUser];

						}

						//������Ϸ
						m_pITableFrame->ConcludeGame(GS_TK_FREE);		
					}
					else if	(m_pITableFrame->GetGameStatus()==GS_TK_SCORE && m_lTableScore[wChairID]==0L)
					{
						OnUserAddScore(wChairID,0);
					}
					else if (m_pITableFrame->GetGameStatus()==GS_TK_PLAYING && m_cbOxCard[wChairID]==0xff)
					{
						OnUserOpenCard(wChairID,0);
					}
				}
			}
			else 
			{
				//�������
				WORD wUserCount=0;
				for (WORD i=0;i<m_wPlayerCount;i++)if(m_cbPlayStatus[i]==TRUE)wUserCount++;

				//������Ϸ
				if(wUserCount==1)
				{
					//�������
					CMD_S_GameEnd GameEnd;
					ZeroMemory(&GameEnd,sizeof(GameEnd));

					//������Ϣ
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)continue;
						m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
					}
					m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

					//������Ϸ
					m_pITableFrame->ConcludeGame(GS_TK_FREE);			
				}
				else if(m_wCurrentUser==wChairID)OnUserCallBanker(wChairID,0);
			}

			//���Ϳ��
			CString strInfo;
			strInfo.Format(TEXT("��ǰ��棺%I64d"), g_lRoomStorageCurrent);
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if (!pIServerUserItem)
				{
					continue;
				}
				if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
				{
					m_pITableFrame->SendGameMessage(pIServerUserItem, strInfo, SMT_CHAT);

					CMD_S_ADMIN_STORAGE_INFO StorageInfo;
					ZeroMemory(&StorageInfo, sizeof(StorageInfo));
					StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
					StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
					StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
					StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
					StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
					StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
					StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
					m_pITableFrame->SendTableData(i, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
					m_pITableFrame->SendLookonData(i, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				}
			}
			
			UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

			//���·����û���Ϣ
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				if (i == wChairID)
				{
					continue;
				}

				//��ȡ�û�
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

				if (!pIServerUserItem)
				{
					continue;
				}

				UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
			}

			return true;
		}
	}

	return false;
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//����״̬
		{
			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//���ñ���
			StatusFree.lCellScore=0L;
			StatusFree.lRoomStorageStart = g_lRoomStorageStart;
			StatusFree.lRoomStorageCurrent = g_lRoomStorageCurrent;

			//��ʷ����
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				StatusFree.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusFree.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			
			//��ȡ�Զ�������
			tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
			ASSERT(pCustomRule);
			tagCustomAndroid CustomAndroid;
			ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
			CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
			CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
			CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
			CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
			CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;
			CopyMemory(&StatusFree.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));
			
			//Ȩ���ж�
			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
			{
				CMD_S_ADMIN_STORAGE_INFO StorageInfo;
				ZeroMemory(&StorageInfo, sizeof(StorageInfo));
				StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
				StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
				StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
				StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
				StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
				StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
				StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
				
				m_pITableFrame->SendTableData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				m_pITableFrame->SendLookonData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
			}

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_TK_CALL:	//��ׯ״̬
		{
			//��������
			CMD_S_StatusCall StatusCall;
			ZeroMemory(&StatusCall,sizeof(StatusCall));

			//��ʷ����
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				StatusCall.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusCall.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//���ñ���
			StatusCall.wCallBanker=m_wCurrentUser;
			StatusCall.cbDynamicJoin=m_cbDynamicJoin[wChairID];
			CopyMemory(StatusCall.cbPlayStatus,m_cbPlayStatus,sizeof(StatusCall.cbPlayStatus));
			StatusCall.lRoomStorageStart = g_lRoomStorageStart;
			StatusCall.lRoomStorageCurrent = g_lRoomStorageCurrent;

			
			//��ȡ�Զ�������
			tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
			ASSERT(pCustomRule);
			tagCustomAndroid CustomAndroid;
			ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
			CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
			CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
			CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
			CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
			CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;
			CopyMemory(&StatusCall.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));

			//���·����û���Ϣ
			UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);
			
			//Ȩ���ж�
			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
			{
				CMD_S_ADMIN_STORAGE_INFO StorageInfo;
				ZeroMemory(&StorageInfo, sizeof(StorageInfo));
				StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
				StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
				StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
				StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
				StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
				StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
				StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
				m_pITableFrame->SendTableData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				m_pITableFrame->SendLookonData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
			}

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusCall,sizeof(StatusCall));
		}
	case GS_TK_SCORE:	//��ע״̬
		{
			//��������
			CMD_S_StatusScore StatusScore;
			memset(&StatusScore,0,sizeof(StatusScore));

			//��ʷ����
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				StatusScore.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusScore.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//��ע��Ϣ
			StatusScore.lTurnMaxScore=GetUserMaxTurnScore(wChairID);
			StatusScore.wBankerUser=m_wBankerUser;
			StatusScore.cbDynamicJoin=m_cbDynamicJoin[wChairID];
			CopyMemory(StatusScore.cbPlayStatus,m_cbPlayStatus,sizeof(StatusScore.cbPlayStatus));
			StatusScore.lRoomStorageStart = g_lRoomStorageStart;
			StatusScore.lRoomStorageCurrent = g_lRoomStorageCurrent;

			//���û���
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)continue;
				StatusScore.lTableScore[i]=m_lTableScore[i];
			}

			
			//��ȡ�Զ�������
			tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
			ASSERT(pCustomRule);
			tagCustomAndroid CustomAndroid;
			ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
			CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
			CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
			CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
			CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
			CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;
			CopyMemory(&StatusScore.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));
			
			//���·����û���Ϣ
			UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);
			
			//Ȩ���ж�
			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
			{
				CMD_S_ADMIN_STORAGE_INFO StorageInfo;
				ZeroMemory(&StorageInfo, sizeof(StorageInfo));
				StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
				StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
				StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
				StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
				StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
				StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
				StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
				m_pITableFrame->SendTableData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				m_pITableFrame->SendLookonData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
			}

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusScore,sizeof(StatusScore));
		}
	case GS_TK_PLAYING:	//��Ϸ״̬
		{
			//��������
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//��ʷ����
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				StatusPlay.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusPlay.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//������Ϣ
			StatusPlay.lTurnMaxScore=GetUserMaxTurnScore(wChairID);
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.cbDynamicJoin=m_cbDynamicJoin[wChairID];
			CopyMemory(StatusPlay.bOxCard,m_cbOxCard,sizeof(StatusPlay.bOxCard));
			CopyMemory(StatusPlay.cbPlayStatus,m_cbPlayStatus,sizeof(StatusPlay.cbPlayStatus));
			StatusPlay.lRoomStorageStart = g_lRoomStorageStart;
			StatusPlay.lRoomStorageCurrent = g_lRoomStorageCurrent;

			//��ȡ�Զ�������
			tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
			ASSERT(pCustomRule);
			tagCustomAndroid CustomAndroid;
			ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
			CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
			CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
			CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
			CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
			CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;
			CopyMemory(&StatusPlay.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));

			//�����˿�
			for (WORD i=0;i< m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)continue;
				WORD j= i;
				StatusPlay.lTableScore[j]=m_lTableScore[j];
				CopyMemory(StatusPlay.cbHandCardData[j],m_cbHandCardData[j],MAX_COUNT);
			}
			
			//���·����û���Ϣ
			UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);
			
			//Ȩ���ж�
			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
			{
				CMD_S_ADMIN_STORAGE_INFO StorageInfo;
				ZeroMemory(&StorageInfo, sizeof(StorageInfo));
				StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
				StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
				StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
				StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
				StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
				StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
				StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
				m_pITableFrame->SendTableData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				m_pITableFrame->SendLookonData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
			}

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}
	//Ч�����
	ASSERT(FALSE);

	return false;
}

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam)
{

	switch(dwTimerID)
	{
	case IDI_DELAY_ENDGAME:
		{
			//�������˴洢��
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//��ȡ�û�
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
				if(!pIServerUserItem)
					continue;
				if(!pIServerUserItem->IsAndroidUser())
					continue;
				m_pITableFrame->SendTableData(i,SUB_S_ANDROID_BANKOPERATOR);
			}

			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			m_pITableFrame->KillGameTimer(IDI_DELAY_ENDGAME);

			//���·����û���Ϣ
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				//��ȡ�û�
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

				if (!pIServerUserItem)
				{
					continue;
				}

				UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
			}

			//return true;
		}
	case IDI_SO_OPERATE:
		{
			//ɾ��ʱ��
			m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

			//��Ϸ״̬
			switch( m_pITableFrame->GetGameStatus() )
			{
			case GS_TK_CALL:			//�û���ׯ
				{
					OnUserCallBanker(m_wCurrentUser, 0);
					break;
				}
			case GS_TK_SCORE:			//��ע����
				{
					for(WORD i=0;i<m_wPlayerCount;i++)
					{
						if(m_lTableScore[i]>0L || m_cbPlayStatus[i]==FALSE || i==m_wBankerUser)
							continue;

						if ( m_lTurnMaxScore[i] > 0 )
						{
							OnUserAddScore(i,m_lTurnMaxScore[i]/8);
						}
						else
						{
							OnUserAddScore(i,1);
						}
					}

					break;
				}
			case GS_TK_PLAYING:			//�û�����
				{
					for(WORD i=0;i<m_wPlayerCount;i++)
					{
						if(m_cbOxCard[i]<2 || m_cbPlayStatus[i]==FALSE)continue;
						OnUserOpenCard(i, 0);
					}

					break;
				}
			default:
				{
					break;
				}
			}

			if(m_pITableFrame->GetGameStatus()!=GS_TK_FREE)
				m_pITableFrame->SetGameTimer(IDI_SO_OPERATE,TIME_SO_OPERATE,1,0);
			return true;
		}
	}
	return false;
}

//��Ϸ��Ϣ���� 
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	bool bResult=false;
	switch (wSubCmdID)
	{
	case SUB_C_CALL_BANKER:			//�û���ׯ
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_CallBanker));
			if (wDataSize!=sizeof(CMD_C_CallBanker)) return false;

			//��������
			CMD_C_CallBanker * pCallBanker=(CMD_C_CallBanker *)pDataBuffer;

			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT(IsUserPlaying(pUserData->wChairID));
			if (!IsUserPlaying(pUserData->wChairID)) return false;

			//��Ϣ����
			bResult=OnUserCallBanker(pUserData->wChairID,pCallBanker->bBanker);
			break;
		}
	case SUB_C_ADD_SCORE:			//�û���ע
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_AddScore));
			if (wDataSize!=sizeof(CMD_C_AddScore)) return false;

			//��������
			CMD_C_AddScore * pAddScore=(CMD_C_AddScore *)pDataBuffer;

			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT(IsUserPlaying(pUserData->wChairID));
			if (!IsUserPlaying(pUserData->wChairID)) return false;

			//��Ϣ����
			bResult=OnUserAddScore(pUserData->wChairID,pAddScore->lScore);
			break;
		}
	case SUB_C_OPEN_CARD:			//�û�̯��
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_OxCard));
			if (wDataSize!=sizeof(CMD_C_OxCard)) return false;

			//��������
			CMD_C_OxCard * pOxCard=(CMD_C_OxCard *)pDataBuffer;

			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT(m_cbPlayStatus[pUserData->wChairID]!=FALSE);
			if(m_cbPlayStatus[pUserData->wChairID]==FALSE)return false;

			//��Ϣ����
			bResult=OnUserOpenCard(pUserData->wChairID,pOxCard->bOX);
			break;
		}
	case SUB_C_STORAGE:
		{
			ASSERT(wDataSize==sizeof(CMD_C_UpdateStorage));
			if(wDataSize!=sizeof(CMD_C_UpdateStorage)) return false;

			//Ȩ���ж�
			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
				return false;

			CMD_C_UpdateStorage *pUpdateStorage=(CMD_C_UpdateStorage *)pDataBuffer;
			g_lRoomStorageCurrent = pUpdateStorage->lRoomStorageCurrent;
			g_lStorageDeductRoom = pUpdateStorage->lRoomStorageDeduct;
			
			//20��������¼
			if (g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				g_ListOperationRecord.RemoveHead();
			}

			CString strOperationRecord;
			CTime time = CTime::GetCurrentTime();
			strOperationRecord.Format(TEXT("����ʱ��: %d/%d/%d-%d:%d:%d, �����˻�[%s],�޸ĵ�ǰ���Ϊ %I64d,˥��ֵΪ %I64d"),
				time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), 
				g_lRoomStorageCurrent, g_lStorageDeductRoom);

			g_ListOperationRecord.AddTail(strOperationRecord);
			
			//д����־
			strOperationRecord += TEXT("\n");
			WriteInfo(strOperationRecord);
			
			//��������
			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
			WORD wIndex = 0;//�����±�
			while(posListRecord)
			{
				CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

				CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
				wIndex++;
			}
			
			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//��������
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			return true;
		}
	case SUB_C_STORAGEMAXMUL:
		{
			ASSERT(wDataSize==sizeof(CMD_C_ModifyStorage));
			if(wDataSize!=sizeof(CMD_C_ModifyStorage)) return false;

			//Ȩ���ж�
			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
				return false;

			CMD_C_ModifyStorage *pModifyStorage = (CMD_C_ModifyStorage *)pDataBuffer;
			g_lStorageMax1Room = pModifyStorage->lMaxRoomStorage[0];
			g_lStorageMax2Room = pModifyStorage->lMaxRoomStorage[1];
			g_lStorageMul1Room = (SCORE)(pModifyStorage->wRoomStorageMul[0]);
			g_lStorageMul2Room = (SCORE)(pModifyStorage->wRoomStorageMul[1]);

			//20��������¼
			if (g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				g_ListOperationRecord.RemoveHead();
			}

			CString strOperationRecord;
			CTime time = CTime::GetCurrentTime();
			strOperationRecord.Format(TEXT("����ʱ��: %d/%d/%d-%d:%d:%d,�����˻�[%s], �޸Ŀ������ֵ1Ϊ %I64d,Ӯ�ָ���1Ϊ %I64d,����ֵ2Ϊ %I64d,Ӯ�ָ���2Ϊ %I64d"),
				time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), g_lStorageMax1Room, g_lStorageMul1Room, g_lStorageMax2Room, g_lStorageMul2Room);

			g_ListOperationRecord.AddTail(strOperationRecord);

			//д����־
			strOperationRecord += TEXT("\n");
			WriteInfo(strOperationRecord);
			
			//��������
			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
			WORD wIndex = 0;//�����±�
			while(posListRecord)
			{
				CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

				CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
				wIndex++;
			}
			
			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//��������
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			return true;
		}
	case SUB_C_REQUEST_QUERY_USER:
		{
			ASSERT(wDataSize == sizeof(CMD_C_RequestQuery_User));
			if (wDataSize != sizeof(CMD_C_RequestQuery_User)) 
			{
				return false;
			}

			//Ȩ���ж�
			if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser())
			{
				return false;
			}
			
			CMD_C_RequestQuery_User *pQuery_User = (CMD_C_RequestQuery_User *)pDataBuffer;

			//����ӳ��
			POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
			DWORD dwUserID = 0;
			ROOMUSERINFO userinfo;
			ZeroMemory(&userinfo, sizeof(userinfo));

			CMD_S_RequestQueryResult QueryResult;
			ZeroMemory(&QueryResult, sizeof(QueryResult));

			while(ptHead)
			{
				g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);
				if (pQuery_User->dwGameID == userinfo.dwGameID || _tcscmp(pQuery_User->szNickName, userinfo.szNickName) == 0)
				{
					//�����û���Ϣ����
					QueryResult.bFind = true;
					CopyMemory(&(QueryResult.userinfo), &userinfo, sizeof(userinfo));

					ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
					CopyMemory(&(g_CurrentQueryUserInfo), &userinfo, sizeof(userinfo));
				}
			}
			
			//��������
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));

			return true;
		}
	case SUB_C_USER_CONTROL:
		{
			ASSERT(wDataSize == sizeof(CMD_C_UserControl));
			if (wDataSize != sizeof(CMD_C_UserControl)) 
			{
				return false;
			}

			//Ȩ���ж�
			if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			CMD_C_UserControl *pUserControl = (CMD_C_UserControl *)pDataBuffer;

			//����ӳ��
			POSITION ptMapHead = g_MapRoomUserInfo.GetStartPosition();
			DWORD dwUserID = 0;
			ROOMUSERINFO userinfo;
			ZeroMemory(&userinfo, sizeof(userinfo));
			
			//20��������¼
			if (g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				g_ListOperationRecord.RemoveHead();
			}

			//��������
			CMD_S_UserControl serverUserControl;
			ZeroMemory(&serverUserControl, sizeof(serverUserControl));

			//�������
			if (pUserControl->userControlInfo.bCancelControl == false)
			{
				ASSERT(pUserControl->userControlInfo.control_type == CONTINUE_WIN || pUserControl->userControlInfo.control_type == CONTINUE_LOST);

				while(ptMapHead)
				{
					g_MapRoomUserInfo.GetNextAssoc(ptMapHead, dwUserID, userinfo);
					if (pUserControl->dwGameID == userinfo.dwGameID || _tcscmp(pUserControl->szNickName, userinfo.szNickName) == 0)
					{
						//������Ʊ�ʶ
						bool bEnableControl = false;
						IsSatisfyControl(userinfo, bEnableControl);
						
						//�������
						if (bEnableControl)
						{
							ROOMUSERCONTROL roomusercontrol;
							ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));
							CopyMemory(&(roomusercontrol.roomUserInfo), &userinfo, sizeof(userinfo));
							CopyMemory(&(roomusercontrol.userControl), &(pUserControl->userControlInfo), sizeof(roomusercontrol.userControl));
							
							
							//������������
							TravelControlList(roomusercontrol);

							//ѹ��������ѹ��ͬGAMEID��NICKNAME)
							g_ListRoomUserControl.AddHead(roomusercontrol);
							
							//��������
							serverUserControl.dwGameID = userinfo.dwGameID;
							CopyMemory(serverUserControl.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
							serverUserControl.controlResult = CONTROL_SUCCEED;
							serverUserControl.controlType = pUserControl->userControlInfo.control_type;
							serverUserControl.cbControlCount = pUserControl->userControlInfo.cbControlCount;
							
							//������¼
							CString strOperationRecord;
							CString strControlType;
							GetControlTypeString(serverUserControl.controlType, strControlType);
							CTime time = CTime::GetCurrentTime();
							strOperationRecord.Format(TEXT("����ʱ��: %d/%d/%d-%d:%d:%d, �����˻�[%s], �������%s,%s,���ƾ���%d "),
								time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserControl.szNickName, strControlType, serverUserControl.cbControlCount);

							g_ListOperationRecord.AddTail(strOperationRecord);

							//д����־
							strOperationRecord += TEXT("\n");
							WriteInfo(strOperationRecord);
						}
						else	//������
						{
							//��������
							serverUserControl.dwGameID = userinfo.dwGameID;
							CopyMemory(serverUserControl.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
							serverUserControl.controlResult = CONTROL_FAIL;
							serverUserControl.controlType = pUserControl->userControlInfo.control_type;
							serverUserControl.cbControlCount = 0;

							//������¼
							CString strOperationRecord;
							CString strControlType;
							GetControlTypeString(serverUserControl.controlType, strControlType);
							CTime time = CTime::GetCurrentTime();
							strOperationRecord.Format(TEXT("����ʱ��: %d/%d/%d-%d:%d:%d, �����˻�[%s], �������%s,%s,ʧ�ܣ�"),
								time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserControl.szNickName, strControlType);

							g_ListOperationRecord.AddTail(strOperationRecord);

							//д����־
							strOperationRecord += TEXT("\n");
							WriteInfo(strOperationRecord);
						}

						//��������
						m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
						m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
						
						CMD_S_Operation_Record OperationRecord;
						ZeroMemory(&OperationRecord, sizeof(OperationRecord));
						POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
						WORD wIndex = 0;//�����±�
						while(posListRecord)
						{
							CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

							CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
							wIndex++;
						}

						ASSERT(wIndex <= MAX_OPERATION_RECORD);

						//��������
						m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
						m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

						return true;
					}
				}

				ASSERT(FALSE);
				return false;
			}
			else	//ȡ������
			{
				ASSERT(pUserControl->userControlInfo.control_type == CONTINUE_CANCEL);

				POSITION ptListHead = g_ListRoomUserControl.GetHeadPosition();
				POSITION ptTemp;
				ROOMUSERCONTROL roomusercontrol;
				ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

				//��������
				while(ptListHead)
				{
					ptTemp = ptListHead;
					roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);
					if (pUserControl->dwGameID == roomusercontrol.roomUserInfo.dwGameID || _tcscmp(pUserControl->szNickName, roomusercontrol.roomUserInfo.szNickName) == 0)
					{
						//��������
						serverUserControl.dwGameID = roomusercontrol.roomUserInfo.dwGameID;
						CopyMemory(serverUserControl.szNickName, roomusercontrol.roomUserInfo.szNickName, sizeof(roomusercontrol.roomUserInfo.szNickName));
						serverUserControl.controlResult = CONTROL_CANCEL_SUCCEED;
						serverUserControl.controlType = pUserControl->userControlInfo.control_type;
						serverUserControl.cbControlCount = 0;

						//�Ƴ�Ԫ��
						g_ListRoomUserControl.RemoveAt(ptTemp);

						//��������
						m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
						m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
						
						//������¼
						CString strOperationRecord;
						CTime time = CTime::GetCurrentTime();
						strOperationRecord.Format(TEXT("����ʱ��: %d/%d/%d-%d:%d:%d, �����˻�[%s], ȡ�������%s�Ŀ��ƣ�"),
							time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserControl.szNickName);

						g_ListOperationRecord.AddTail(strOperationRecord);

						//д����־
						strOperationRecord += TEXT("\n");
						WriteInfo(strOperationRecord);
						
						CMD_S_Operation_Record OperationRecord;
						ZeroMemory(&OperationRecord, sizeof(OperationRecord));
						POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
						WORD wIndex = 0;//�����±�
						while(posListRecord)
						{
							CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

							CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
							wIndex++;
						}

						ASSERT(wIndex <= MAX_OPERATION_RECORD);

						//��������
						m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
						m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

						return true;
					}
				}

				//��������
				serverUserControl.dwGameID = pUserControl->dwGameID;
				CopyMemory(serverUserControl.szNickName, pUserControl->szNickName, sizeof(serverUserControl.szNickName));
				serverUserControl.controlResult = CONTROL_CANCEL_INVALID;
				serverUserControl.controlType = pUserControl->userControlInfo.control_type;
				serverUserControl.cbControlCount = 0;
		
				//��������
				m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
				m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));

				//������¼
				CString strOperationRecord;
				CTime time = CTime::GetCurrentTime();
				strOperationRecord.Format(TEXT("����ʱ��: %d/%d/%d-%d:%d:%d, �����˻�[%s], ȡ�������%s�Ŀ��ƣ�������Ч��"),
					time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserControl.szNickName);

				g_ListOperationRecord.AddTail(strOperationRecord);

				//д����־
				strOperationRecord += TEXT("\n");
				WriteInfo(strOperationRecord);

				CMD_S_Operation_Record OperationRecord;
				ZeroMemory(&OperationRecord, sizeof(OperationRecord));
				POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
				WORD wIndex = 0;//�����±�
				while(posListRecord)
				{
					CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

					CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
					wIndex++;
				}

				ASSERT(wIndex <= MAX_OPERATION_RECORD);

				//��������
				m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
				m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			}

			return true;
		}
	case SUB_C_REQUEST_UDPATE_ROOMINFO:
		{
			//Ȩ���ж�
			if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			CMD_S_RequestUpdateRoomInfo_Result RoomInfo_Result;
			ZeroMemory(&RoomInfo_Result, sizeof(RoomInfo_Result));

			RoomInfo_Result.lRoomStorageCurrent = g_lRoomStorageCurrent;

			
			DWORD dwKeyGameID = g_CurrentQueryUserInfo.dwGameID;
			TCHAR szKeyNickName[LEN_NICKNAME];	
			ZeroMemory(szKeyNickName, sizeof(szKeyNickName));
			CopyMemory(szKeyNickName, g_CurrentQueryUserInfo.szNickName, sizeof(szKeyNickName));

			//����ӳ��
			POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
			DWORD dwUserID = 0;
			ROOMUSERINFO userinfo;
			ZeroMemory(&userinfo, sizeof(userinfo));

			while(ptHead)
			{
				g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);
				if (dwKeyGameID == userinfo.dwGameID && _tcscmp(szKeyNickName, userinfo.szNickName) == 0)
				{
					//�����û���Ϣ����
					CopyMemory(&(RoomInfo_Result.currentqueryuserinfo), &userinfo, sizeof(userinfo));

					ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
					CopyMemory(&(g_CurrentQueryUserInfo), &userinfo, sizeof(userinfo));
				}
			}
		
			//
			//��������
			POSITION ptListHead = g_ListRoomUserControl.GetHeadPosition();
			POSITION ptTemp;
			ROOMUSERCONTROL roomusercontrol;
			ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

			//��������
			while(ptListHead)
			{
				ptTemp = ptListHead;
				roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);

				//Ѱ�����
				if ((dwKeyGameID == roomusercontrol.roomUserInfo.dwGameID) &&
					_tcscmp(szKeyNickName, roomusercontrol.roomUserInfo.szNickName) == 0)
				{
					RoomInfo_Result.bExistControl = true;
					CopyMemory(&(RoomInfo_Result.currentusercontrol), &(roomusercontrol.userControl), sizeof(roomusercontrol.userControl));
					break;				
				}
			}

			//��������
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT, &RoomInfo_Result, sizeof(RoomInfo_Result));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT, &RoomInfo_Result, sizeof(RoomInfo_Result));
			
			return true;
		}
	case SUB_C_CLEAR_CURRENT_QUERYUSER:
		{
			//Ȩ���ж�
			if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}
			
			ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));

			return true;
		}
	}

	//������ʱ��
	if(bResult)
	{
		m_pITableFrame->SetGameTimer(IDI_SO_OPERATE,TIME_SO_OPERATE,1,0);
		return true;
	}

	return false;
}

//�����Ϣ����
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}


//��ׯ�¼�
bool CTableFrameSink::OnUserCallBanker(WORD wChairID, BYTE bBanker)
{
	//״̬Ч��
	ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_CALL);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_CALL) return true;
	ASSERT(m_wCurrentUser==wChairID);
	if (m_wCurrentUser!=wChairID) return true;

	//���ñ���
	m_cbCallStatus[wChairID]=TRUE;

	//��ׯ����
	WORD wCallUserCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbPlayStatus[i]==TRUE && m_cbCallStatus[i]==TRUE) wCallUserCount++;
		else if(m_cbPlayStatus[i]!=TRUE) wCallUserCount++;
	}

	if(bBanker==FALSE && wCallUserCount==m_wPlayerCount)
	{
		m_wBankerUser=m_wFisrtCallUser;
	}

	

	//��ע��ʼ
	if(bBanker==TRUE || wCallUserCount==m_wPlayerCount)
	{
		//ʼ���û�
		if(bBanker==TRUE)
		{
			m_wBankerUser=wChairID;
		}
		m_wCurrentUser=INVALID_CHAIR;
		
		//�������һ����ׯ�û�ǿ�����
		while(m_cbPlayStatus[m_wBankerUser]==FALSE)
		{
			m_wBankerUser=(m_wBankerUser+1)%GAME_PLAYER;
		}
		m_bBuckleServiceCharge[m_wBankerUser]=true;
		//����״̬
		m_pITableFrame->SetGameStatus(GS_TK_SCORE);

		//���·����û���Ϣ
		for (WORD i=0; i<m_wPlayerCount; i++)
		{
			//��ȡ�û�
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem != NULL)
			{
				UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
			}
		}

		//ׯ�һ���
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wBankerUser);
		LONGLONG lBankerScore=pIServerUserItem->GetUserScore();

		//�����ע
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]!=TRUE || i==m_wBankerUser)continue;

			//��ע���� �ͻ�Ҫ��
			m_lTurnMaxScore[i]=GetUserMaxTurnScore(i);
		}

		//���ñ���
		CMD_S_GameStart GameStart;
		GameStart.wBankerUser=m_wBankerUser;
		GameStart.lTurnMaxScore=0;

		//��������
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)continue;
			GameStart.lTurnMaxScore=m_lTurnMaxScore[i];
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		}
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}
	else		 //�û���ׯ
	{
		//�����¸����
		do{
			m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
		}while(m_cbPlayStatus[m_wCurrentUser]==FALSE);

		//���ñ���
		CMD_S_CallBanker CallBanker;
		CallBanker.wCallBanker=m_wCurrentUser;
		CallBanker.bFirstTimes=false;

		//��������
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)continue;
			m_pITableFrame->SendTableData(i,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));
		}
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));
	}
	return true;
}

//��ע�¼�
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONGLONG lScore)
{
	//״̬Ч��
	ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_SCORE);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_SCORE) return true;

	//���Ч��
	if(m_cbPlayStatus[wChairID]==TRUE)
	{
		ASSERT(lScore>0 && lScore<=m_lTurnMaxScore[wChairID]);
		if (lScore<=0 || lScore>m_lTurnMaxScore[wChairID]) return false;
	}
	else //û��ע���ǿ��
	{
		ASSERT(lScore==0);
		if (lScore!=0) return false;
	}

	if(!UserCanAddScore(wChairID,lScore)) return false;

	if(lScore>0L)
	{
		//��ע���
		m_lTableScore[wChairID]=lScore;
		m_bBuckleServiceCharge[wChairID]=true;
		//��������
		CMD_S_AddScore AddScore;
		AddScore.wAddScoreUser=wChairID;
		AddScore.lAddScoreCount=m_lTableScore[wChairID];

		//��������
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)continue;
			m_pITableFrame->SendTableData(i,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
		}
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
	}

	//��ע����
	BYTE bUserCount=0;
	for(WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_lTableScore[i]>0L && m_cbPlayStatus[i]==TRUE)bUserCount++;
		else if(m_cbPlayStatus[i]==FALSE || i==m_wBankerUser)bUserCount++;
	}

	//�м�ȫ��
	if(bUserCount==m_wPlayerCount)
	{
		//����״̬
		m_pITableFrame->SetGameStatus(GS_TK_PLAYING);

		//���·����û���Ϣ
		for (WORD i=0; i<m_wPlayerCount; i++)
		{
			//��ȡ�û�
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem != NULL)
			{
				UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
			}
		}

		//��������
		CMD_S_SendCard SendCard;
		ZeroMemory(SendCard.cbCardData,sizeof(SendCard.cbCardData));

		//�����˿�
		AnalyseCard();
		
		//��������
		ROOMUSERCONTROL roomusercontrol;
		ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));
		POSITION posKeyList;

		//����
		if( m_pServerControl != NULL && AnalyseRoomUserControl(roomusercontrol, posKeyList))
		{
			//У������
			ASSERT(roomusercontrol.roomUserInfo.wChairID != INVALID_CHAIR && roomusercontrol.userControl.cbControlCount != 0 
				&& roomusercontrol.userControl.control_type != CONTINUE_CANCEL);
	
			if(m_pServerControl->ControlResult(m_cbHandCardData, roomusercontrol))
			{
				//��ȡԪ��
				ROOMUSERCONTROL &tmproomusercontrol = g_ListRoomUserControl.GetAt(posKeyList);
				
				//У������
				ASSERT(roomusercontrol.userControl.cbControlCount == tmproomusercontrol.userControl.cbControlCount);

				//���ƾ���
				tmproomusercontrol.userControl.cbControlCount--;

				CMD_S_UserControlComplete UserControlComplete;
				ZeroMemory(&UserControlComplete, sizeof(UserControlComplete));
				UserControlComplete.dwGameID = roomusercontrol.roomUserInfo.dwGameID;
				CopyMemory(UserControlComplete.szNickName, roomusercontrol.roomUserInfo.szNickName, sizeof(UserControlComplete.szNickName));
				UserControlComplete.controlType = roomusercontrol.userControl.control_type;
				UserControlComplete.cbRemainControlCount = tmproomusercontrol.userControl.cbControlCount;

				for (WORD i=0; i<GAME_PLAYER; i++)
				{
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
					if (!pIServerUserItem)
					{
						continue;
					}
					if (pIServerUserItem->IsAndroidUser() == true || CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false)
					{
						continue;
					}

					//��������
					m_pITableFrame->SendTableData(i, SUB_S_USER_CONTROL_COMPLETE, &UserControlComplete, sizeof(UserControlComplete));
					m_pITableFrame->SendLookonData(i, SUB_S_USER_CONTROL_COMPLETE, &UserControlComplete, sizeof(UserControlComplete));

				}
			}			
		}

		//�����˿�
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==FALSE)continue;

			//�ɷ��˿�
			CopyMemory(SendCard.cbCardData[i],m_cbHandCardData[i],MAX_COUNT);
		}

		//��������
		for (WORD i=0;i< m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)continue;

			m_pITableFrame->SendTableData(i,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
		}
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
	}

	return true;
}

//̯���¼�
bool CTableFrameSink::OnUserOpenCard(WORD wChairID, BYTE bOx)
{
	//״̬Ч��
	ASSERT (m_pITableFrame->GetGameStatus()==GS_TK_PLAYING);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_PLAYING) return true;
	if (m_cbOxCard[wChairID]!=0xff) return true;

	//Ч������
	ASSERT(bOx<=TRUE);
	if(bOx>TRUE)return false;

	//Ч������
	if(bOx)
	{
		ASSERT(m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT)>0);
		if(!(m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT)>0))return false;
	}
	else if(m_cbPlayStatus[wChairID]==TRUE)
	{
		if(m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT)>=OX_THREE_SAME)bOx=TRUE;
	}

	//ţţ����
	m_cbOxCard[wChairID] = bOx;

	//̯������
	BYTE bUserCount=0;
	for(WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbOxCard[i]<2 && m_cbPlayStatus[i]==TRUE)bUserCount++;
		else if(m_cbPlayStatus[i]==FALSE)bUserCount++;
	}

	 //�������
	CMD_S_Open_Card OpenCard;
	ZeroMemory(&OpenCard,sizeof(OpenCard));

	//���ñ���
	OpenCard.bOpen=bOx;
	OpenCard.wPlayerID=wChairID;

	//��������
	for (WORD i=0;i< m_wPlayerCount;i++)
	{
		if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)continue;
		m_pITableFrame->SendTableData(i,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));
	}
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));	

	//������Ϸ
	if(bUserCount == m_wPlayerCount)
	{
		return OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);
	}

	return true;
}

//�˿˷���
void CTableFrameSink::AnalyseCard()
{
	//��������
	bool bIsAiBanker = false;
	WORD wAiCount = 0;
	WORD wPlayerCount = 0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem!=NULL)
		{
			if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)continue;
			if(pIServerUserItem->IsAndroidUser()) 
			{
				wAiCount++ ;
				if(!bIsAiBanker && i==m_wBankerUser)bIsAiBanker = true;
			}
			wPlayerCount++; 
		}
	}

	//ȫ������
	if(wPlayerCount == wAiCount || wAiCount==0)return;

	//�˿˱���
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

	//ţţ����
	BOOL bUserOxData[GAME_PLAYER];
	ZeroMemory(bUserOxData,sizeof(bUserOxData));
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==FALSE)continue;
		bUserOxData[i] = (m_GameLogic.GetCardType(cbUserCardData[i],MAX_COUNT)>0)?TRUE:FALSE;
	}

	//�����˿�
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_GameLogic.SortCardList(cbUserCardData[i],MAX_COUNT);
	}

	//��������
	LONGLONG lAndroidScore=0;

	//��������
	BYTE cbCardTimes[GAME_PLAYER];
	ZeroMemory(cbCardTimes,sizeof(cbCardTimes));

	//���ұ���
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE)
		{
			cbCardTimes[i]=m_GameLogic.GetTimes(cbUserCardData[i],MAX_COUNT);
		}
	}

	//����ׯ��
	if(bIsAiBanker)
	{
		//�Ա��˿�
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//�û�����
			if ((i==m_wBankerUser)||(m_cbPlayStatus[i]==FALSE)) continue;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

			//��������
			if ((pIServerUserItem!=NULL)&&(pIServerUserItem->IsAndroidUser())) continue;

			//�Ա��˿�
			if (m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,bUserOxData[i],bUserOxData[m_wBankerUser])==true)
			{
				lAndroidScore-=cbCardTimes[i]*m_lTableScore[i];
			}
			else
			{
				lAndroidScore+=cbCardTimes[m_wBankerUser]*m_lTableScore[i];
			}
		}
	}
	else//�û�ׯ��
	{
		//�Ա��˿�
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

			//�û�����
			if ((i==m_wBankerUser)||(pIServerUserItem==NULL)||!(pIServerUserItem->IsAndroidUser())) continue;

			//�Ա��˿�
			if (m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,bUserOxData[i],bUserOxData[m_wBankerUser])==true)
			{
				lAndroidScore+=cbCardTimes[i]*m_lTableScore[i];
			}
			else
			{
				lAndroidScore-=cbCardTimes[m_wBankerUser]*m_lTableScore[i];
			}
		}
	}

	//��������
	WORD wMaxUser=INVALID_CHAIR;
	WORD wMinAndroid=INVALID_CHAIR;
	WORD wMaxAndroid=INVALID_CHAIR;

	//�����������
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if(pIServerUserItem==NULL) continue;

		//�������
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			//��ʼ����
			if(wMaxUser==INVALID_CHAIR) wMaxUser=i;

			//��ȡ�ϴ���
			if(m_GameLogic.CompareCard(cbUserCardData[i],m_cbHandCardData[wMaxUser],MAX_COUNT,bUserOxData[i],bUserOxData[wMaxUser])==true)
			{
				wMaxUser=i;
			}
		}

		//�������
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//��ʼ����
			if(wMinAndroid==INVALID_CHAIR) wMinAndroid=i;
			if(wMaxAndroid==INVALID_CHAIR) wMaxAndroid=i;

			//��ȡ��С��
			if(m_GameLogic.CompareCard(cbUserCardData[i],m_cbHandCardData[wMinAndroid],MAX_COUNT,bUserOxData[i],bUserOxData[wMinAndroid])==false)
			{
				wMinAndroid=i;
			}

			//��ȡ�ϴ���
			if(m_GameLogic.CompareCard(cbUserCardData[i],m_cbHandCardData[wMaxAndroid],MAX_COUNT,bUserOxData[i],bUserOxData[wMaxAndroid])==true)
			{
				wMaxAndroid=i;
			}
		}
	}	

	
	LONGLONG lGameEndStorage = g_lRoomStorageCurrent+lAndroidScore;
	//����ж�
	if(g_lRoomStorageCurrent+lAndroidScore<0 || (lGameEndStorage < (g_lRoomStorageCurrent * (double)(1 - (double)5 / (double)100))))
	{
		//��������
		WORD wWinUser=wMaxUser;

		//������ׯ
		if(bIsAiBanker)
		{
			//��������
			for (WORD i=0;i<m_wPlayerCount;i++)
			{

				//�û�����
				if (m_cbPlayStatus[i]==FALSE) continue;

				//��ȡ�ϴ���
				if (m_GameLogic.CompareCard(cbUserCardData[i],m_cbHandCardData[wWinUser],MAX_COUNT,bUserOxData[i],bUserOxData[wWinUser])==true)
				{
					wWinUser=i;
				}
			}
		}
		else
		{
			//��ȡ��С��
			if (m_GameLogic.CompareCard(cbUserCardData[wMaxAndroid],m_cbHandCardData[wWinUser],MAX_COUNT,bUserOxData[wMaxAndroid],bUserOxData[wWinUser])==false)
			{
				wWinUser=wMaxAndroid;				
			}
		}

		if(bIsAiBanker)
		{
			//��������
			BYTE cbTempData[MAX_COUNT];
			CopyMemory(cbTempData,m_cbHandCardData[m_wBankerUser],MAX_COUNT);
			CopyMemory(m_cbHandCardData[m_wBankerUser],m_cbHandCardData[wWinUser],MAX_COUNT);
			CopyMemory(m_cbHandCardData[wWinUser],cbTempData,MAX_COUNT);
		}
		else
		{
			BYTE bUser = m_wBankerUser;

			//ÿһ����Ҵ���ׯ�ң�ֱ��������ָ����Ϊֹ
			do 
			{
				bUser = (bUser+1)%GAME_PLAYER;

				if(m_cbPlayStatus[bUser]==TRUE)
				{
					//��������
					BYTE cbTempData[MAX_COUNT];
					CopyMemory(cbTempData,m_cbHandCardData[m_wBankerUser],MAX_COUNT);
					CopyMemory(m_cbHandCardData[m_wBankerUser],m_cbHandCardData[bUser],MAX_COUNT);
					CopyMemory(m_cbHandCardData[bUser],cbTempData,MAX_COUNT);
				}

			} while (!JudgeStock() && (bUser != m_wBankerUser));

		}
	}
	else if(g_lRoomStorageCurrent>0 && lAndroidScore>0 && g_lRoomStorageCurrent > g_lStorageMax2Room && g_lRoomStorageCurrent - lAndroidScore > 0 && rand()%100 < g_lStorageMul2Room)
	{
		if (m_GameLogic.CompareCard(cbUserCardData[wMaxAndroid],m_cbHandCardData[wMaxUser],MAX_COUNT,bUserOxData[wMaxAndroid],bUserOxData[wMaxUser])==true)
		{
			//��������
			BYTE cbTempData[MAX_COUNT];
			CopyMemory(cbTempData,m_cbHandCardData[wMaxUser],MAX_COUNT);
			CopyMemory(m_cbHandCardData[wMaxUser],m_cbHandCardData[wMaxAndroid],MAX_COUNT);
			CopyMemory(m_cbHandCardData[wMaxAndroid],cbTempData,MAX_COUNT);

			//��治��������
			if (JudgeStock() == false)
			{
				CopyMemory(cbTempData,m_cbHandCardData[wMaxUser],MAX_COUNT);
				CopyMemory(m_cbHandCardData[wMaxUser],m_cbHandCardData[wMaxAndroid],MAX_COUNT);
				CopyMemory(m_cbHandCardData[wMaxAndroid],cbTempData,MAX_COUNT);
			}
		}			
	}
	else if(g_lRoomStorageCurrent>0 && lAndroidScore>0 && g_lRoomStorageCurrent > g_lStorageMax1Room && g_lRoomStorageCurrent - lAndroidScore > 0 && rand()%100 < g_lStorageMul1Room)
	{
		if (m_GameLogic.CompareCard(cbUserCardData[wMaxAndroid],m_cbHandCardData[wMaxUser],MAX_COUNT,bUserOxData[wMaxAndroid],bUserOxData[wMaxUser])==true)
		{
			//��������
			BYTE cbTempData[MAX_COUNT];
			CopyMemory(cbTempData,m_cbHandCardData[wMaxUser],MAX_COUNT);
			CopyMemory(m_cbHandCardData[wMaxUser],m_cbHandCardData[wMaxAndroid],MAX_COUNT);
			CopyMemory(m_cbHandCardData[wMaxAndroid],cbTempData,MAX_COUNT);

			//��治��������
			if (JudgeStock() == false)
			{
				CopyMemory(cbTempData,m_cbHandCardData[wMaxUser],MAX_COUNT);
				CopyMemory(m_cbHandCardData[wMaxUser],m_cbHandCardData[wMaxAndroid],MAX_COUNT);
				CopyMemory(m_cbHandCardData[wMaxAndroid],cbTempData,MAX_COUNT);
			}
		}			
	}

	return;
}



//�жϿ��
bool CTableFrameSink::JudgeStock()
{

	//��������
	bool bIsAiBanker = false;
	WORD wAiCount = 0;
	WORD wPlayerCount = 0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem!=NULL)
		{
			if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)continue;
			if(pIServerUserItem->IsAndroidUser()) 
			{
				wAiCount++ ;
				if(!bIsAiBanker && i==m_wBankerUser)bIsAiBanker = true;
			}
			wPlayerCount++; 
		}
	}

	//�˿˱���
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

	//ţţ����
	BOOL bUserOxData[GAME_PLAYER];
	ZeroMemory(bUserOxData,sizeof(bUserOxData));
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==FALSE)continue;
		bUserOxData[i] = (m_GameLogic.GetCardType(cbUserCardData[i],MAX_COUNT)>0)?TRUE:FALSE;
	}

	//�����˿�
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_GameLogic.SortCardList(cbUserCardData[i],MAX_COUNT);
	}

	//��������
	LONGLONG lAndroidScore=0;

	//��������
	BYTE cbCardTimes[GAME_PLAYER];
	ZeroMemory(cbCardTimes,sizeof(cbCardTimes));

	//���ұ���
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE)
		{
			cbCardTimes[i]=m_GameLogic.GetTimes(cbUserCardData[i],MAX_COUNT);
		}
	}

	//����ׯ��
	if(bIsAiBanker)
	{
		//�Ա��˿�
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//�û�����
			if ((i==m_wBankerUser)||(m_cbPlayStatus[i]==FALSE)) continue;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

			//��������
			if ((pIServerUserItem!=NULL)&&(pIServerUserItem->IsAndroidUser())) continue;

			//�Ա��˿�
			if (m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,bUserOxData[i],bUserOxData[m_wBankerUser])==true)
			{
				lAndroidScore-=cbCardTimes[i]*m_lTableScore[i];
			}
			else
			{
				lAndroidScore+=cbCardTimes[m_wBankerUser]*m_lTableScore[i];
			}
		}
	}
	else//�û�ׯ��
	{
		//�Ա��˿�
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

			//�û�����
			if ((i==m_wBankerUser)||(pIServerUserItem==NULL)||!(pIServerUserItem->IsAndroidUser())) continue;

			//�Ա��˿�
			if (m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,bUserOxData[i],bUserOxData[m_wBankerUser])==true)
			{
				lAndroidScore+=cbCardTimes[i]*m_lTableScore[i];
			}
			else
			{
				lAndroidScore-=cbCardTimes[m_wBankerUser]*m_lTableScore[i];
			}
		}
	}

	//��������
	WORD wMaxUser=INVALID_CHAIR;
	WORD wMinAndroid=INVALID_CHAIR;
	WORD wMaxAndroid=INVALID_CHAIR;

	//�����������
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if(pIServerUserItem==NULL) continue;

		//�������
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			//��ʼ����
			if(wMaxUser==INVALID_CHAIR) wMaxUser=i;

			//��ȡ�ϴ���
			if(m_GameLogic.CompareCard(cbUserCardData[i],m_cbHandCardData[wMaxUser],MAX_COUNT,bUserOxData[i],bUserOxData[wMaxUser])==true)
			{
				wMaxUser=i;
			}
		}

		//�������
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//��ʼ����
			if(wMinAndroid==INVALID_CHAIR) wMinAndroid=i;
			if(wMaxAndroid==INVALID_CHAIR) wMaxAndroid=i;

			//��ȡ��С��
			if(m_GameLogic.CompareCard(cbUserCardData[i],m_cbHandCardData[wMinAndroid],MAX_COUNT,bUserOxData[i],bUserOxData[wMinAndroid])==false)
			{
				wMinAndroid=i;
			}

			//��ȡ�ϴ���
			if(m_GameLogic.CompareCard(cbUserCardData[i],m_cbHandCardData[wMaxAndroid],MAX_COUNT,bUserOxData[i],bUserOxData[wMaxAndroid])==true)
			{
				wMaxAndroid=i;
			}
		}
	}	

	return g_lRoomStorageCurrent+lAndroidScore>0;

}

//��ѯ�Ƿ�۷����
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pUserItem=m_pITableFrame->GetTableUserItem(i);
		if(pUserItem==NULL) continue;
		
		if (m_bBuckleServiceCharge[i]&&i==wChairID)
		{
			return true;
		}
		
	}
	return false;
}


bool CTableFrameSink::TryWriteTableScore(tagScoreInfo ScoreInfoArray[])
{
	//�޸Ļ���
	tagScoreInfo ScoreInfo[GAME_PLAYER];
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
	memcpy(&ScoreInfo,ScoreInfoArray,sizeof(ScoreInfo));
	//��¼�쳣
	LONGLONG beforeScore[GAME_PLAYER];
	ZeroMemory(beforeScore,sizeof(beforeScore));
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		IServerUserItem *pItem=m_pITableFrame->GetTableUserItem(i);
		if(pItem!=NULL)
		{
			beforeScore[i]=pItem->GetUserScore();

		}
	}
	m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
	LONGLONG afterScore[GAME_PLAYER];
	ZeroMemory(afterScore,sizeof(afterScore));
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		IServerUserItem *pItem=m_pITableFrame->GetTableUserItem(i);
		if(pItem!=NULL)
		{
			afterScore[i]=pItem->GetUserScore();

			if(afterScore[i]<0)
			{
				//�쳣д����־

				CString strInfo;
				strInfo.Format(TEXT("[%s] ���ָ���"),pItem->GetNickName());
				NcaTextOut(strInfo, m_pGameServiceOption->szServerName);

				strInfo.Format(TEXT("д��ǰ������%I64d"),beforeScore[i]);
				NcaTextOut(strInfo, m_pGameServiceOption->szServerName);

				strInfo.Format(TEXT("д����Ϣ��д����� %I64d��˰�� %I64d"),ScoreInfoArray[i].lScore,ScoreInfoArray[i].lRevenue);
				NcaTextOut(strInfo, m_pGameServiceOption->szServerName);

				strInfo.Format(TEXT("д�ֺ������%I64d"),afterScore[i]);
				NcaTextOut(strInfo, m_pGameServiceOption->szServerName);

			}

		}
	}
	return true;
}

//����·�
SCORE CTableFrameSink::GetUserMaxTurnScore(WORD wChairID)
{

	SCORE lMaxTurnScore=0L;
	if(wChairID==m_wBankerUser)  return 0;
	//ׯ�һ���
	IServerUserItem *pIBankerItem=m_pITableFrame->GetTableUserItem(m_wBankerUser);
	LONGLONG lBankerScore=0L;
	if(pIBankerItem!=NULL)
		lBankerScore=pIBankerItem->GetUserScore();

	//�������
	WORD wUserCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
		if(m_cbPlayStatus[i]==TRUE )wUserCount++;

	//��ȡ�û�
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);

	if(pIServerUserItem!=NULL)
	{
		//��ȡ����
		LONGLONG lScore=pIServerUserItem->GetUserScore();

		lMaxTurnScore=__min(lBankerScore/(wUserCount-1)/MAX_TIMES,lScore/MAX_TIMES);
	}

	return lMaxTurnScore;
	

}

//�Ƿ�ɼ�
bool CTableFrameSink::UserCanAddScore(WORD wChairID, LONGLONG lAddScore)
{

	//��ȡ�û�
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);

	if(pIServerUserItem!=NULL)
	{
		//��ȡ����
		LONGLONG lScore=pIServerUserItem->GetUserScore();

		if(lAddScore>lScore/MAX_TIMES)
		{
			return false;
		}
	}
	return true;

}

//��ѯ�޶�
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	SCORE consumeQuota=0L;
	/*SCORE lMinTableScore=m_pGameServiceOption->lMinTableScore;
	if(m_pITableFrame->GetGameStatus()==GAME_STATUS_FREE)
	{
		consumeQuota=pIServerUserItem->GetUserScore()-lMinTableScore;

	}*/
	return consumeQuota;
}

//�Ƿ�˥��
bool CTableFrameSink::NeedDeductStorage()
{
	for ( int i = 0; i < GAME_PLAYER; ++i )
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL ) continue; 

		if(!pIServerUserItem->IsAndroidUser())
		{
			return true;
		}
	}

	return false;

}

//��ȡ����
void CTableFrameSink::ReadConfigInformation()
{	
	//��ȡ�Զ�������
	tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
	ASSERT(pCustomRule);
	
	g_lRoomStorageStart = pCustomRule->lRoomStorageStart;
	g_lRoomStorageCurrent = pCustomRule->lRoomStorageStart;
	g_lStorageDeductRoom = pCustomRule->lRoomStorageDeduct;
	g_lStorageMax1Room = pCustomRule->lRoomStorageMax1;
	g_lStorageMul1Room = pCustomRule->lRoomStorageMul1;
	g_lStorageMax2Room = pCustomRule->lRoomStorageMax2;
	g_lStorageMul2Room = pCustomRule->lRoomStorageMul2;

	if( g_lStorageDeductRoom < 0 || g_lStorageDeductRoom > 1000 )
		g_lStorageDeductRoom = 0;
	if ( g_lStorageDeductRoom > 1000 )
		g_lStorageDeductRoom = 1000;
	if (g_lStorageMul1Room < 0 || g_lStorageMul1Room > 100) 
		g_lStorageMul1Room = 50;
	if (g_lStorageMul2Room < 0 || g_lStorageMul2Room > 100) 
		g_lStorageMul2Room = 80;
}

//���·����û���Ϣ
void CTableFrameSink::UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction)
{
	//��������
	ROOMUSERINFO roomUserInfo;
	ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));

	roomUserInfo.dwGameID = pIServerUserItem->GetGameID();
	CopyMemory(&(roomUserInfo.szNickName), pIServerUserItem->GetNickName(), sizeof(roomUserInfo.szNickName));
	roomUserInfo.cbUserStatus = pIServerUserItem->GetUserStatus();
	roomUserInfo.cbGameStatus = m_pITableFrame->GetGameStatus();

	roomUserInfo.bAndroid = pIServerUserItem->IsAndroidUser();

	//�û����º�����
	if (userAction == USER_SITDOWN || userAction == USER_RECONNECT)
	{
		roomUserInfo.wChairID = pIServerUserItem->GetChairID();
		roomUserInfo.wTableID = pIServerUserItem->GetTableID() + 1;
	}
	else if (userAction == USER_STANDUP || userAction == USER_OFFLINE)
	{
		roomUserInfo.wChairID = INVALID_CHAIR;
		roomUserInfo.wTableID = INVALID_TABLE;
	}
	
	g_MapRoomUserInfo.SetAt(pIServerUserItem->GetUserID(), roomUserInfo);
}

//����ͬ���û�����
void CTableFrameSink::UpdateUserControl(IServerUserItem *pIServerUserItem)
{
	//��������
	POSITION ptListHead;
	POSITION ptTemp;
	ROOMUSERCONTROL roomusercontrol;

	//��ʼ��
	ptListHead = g_ListRoomUserControl.GetHeadPosition();
	ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

	//��������
	while(ptListHead)
	{
		ptTemp = ptListHead;
		roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);

		//Ѱ���Ѵ��ڿ������
		if ((pIServerUserItem->GetGameID() == roomusercontrol.roomUserInfo.dwGameID) &&
			_tcscmp(pIServerUserItem->GetNickName(), roomusercontrol.roomUserInfo.szNickName) == 0)
		{
			//��ȡԪ��
			ROOMUSERCONTROL &tmproomusercontrol = g_ListRoomUserControl.GetAt(ptTemp);

			//�������
			tmproomusercontrol.roomUserInfo.wChairID = pIServerUserItem->GetChairID();
			tmproomusercontrol.roomUserInfo.wTableID = m_pITableFrame->GetTableID() + 1;

			return;
		}
	}
}

//�����û�����
void CTableFrameSink::TravelControlList(ROOMUSERCONTROL keyroomusercontrol)
{
	//��������
	POSITION ptListHead;
	POSITION ptTemp;
	ROOMUSERCONTROL roomusercontrol;

	//��ʼ��
	ptListHead = g_ListRoomUserControl.GetHeadPosition();
	ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

	//��������
	while(ptListHead)
	{
		ptTemp = ptListHead;
		roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);

		//Ѱ���Ѵ��ڿ����������һ�������л�����
		if ((keyroomusercontrol.roomUserInfo.dwGameID == roomusercontrol.roomUserInfo.dwGameID) &&
			_tcscmp(keyroomusercontrol.roomUserInfo.szNickName, roomusercontrol.roomUserInfo.szNickName) == 0)
		{
			g_ListRoomUserControl.RemoveAt(ptTemp);
		}
	}
}

//�Ƿ������������
void CTableFrameSink::IsSatisfyControl(ROOMUSERINFO &userInfo, bool &bEnableControl)
{
	if (userInfo.wChairID == INVALID_CHAIR || userInfo.wTableID == INVALID_TABLE)
	{
		bEnableControl = FALSE;
		return;
	}

	if (userInfo.cbUserStatus == US_SIT || userInfo.cbUserStatus == US_READY || userInfo.cbUserStatus == US_PLAYING)
	{
		bEnableControl = TRUE;
		return;	
	}
	else
	{
		bEnableControl = FALSE;
		return;
	}
}

//���������û�����
bool CTableFrameSink::AnalyseRoomUserControl(ROOMUSERCONTROL &Keyroomusercontrol, POSITION &ptList)
{
	//��������
	POSITION ptListHead;
	POSITION ptTemp;
	ROOMUSERCONTROL roomusercontrol;
	
	//��������
	for (WORD i=0; i<GAME_PLAYER; i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (!pIServerUserItem)
		{
			continue;
		}
		
		//��ʼ��
		ptListHead = g_ListRoomUserControl.GetHeadPosition();
		ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

		//��������
		while(ptListHead)
		{
			ptTemp = ptListHead;
			roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);
			
			//Ѱ�����
			if ((pIServerUserItem->GetGameID() == roomusercontrol.roomUserInfo.dwGameID) &&
				_tcscmp(pIServerUserItem->GetNickName(), roomusercontrol.roomUserInfo.szNickName) == 0)
			{
				//��տ��ƾ���Ϊ0��Ԫ��
				if (roomusercontrol.userControl.cbControlCount == 0)
				{
					g_ListRoomUserControl.RemoveAt(ptTemp);
					break;
				}

				if (roomusercontrol.userControl.control_type == CONTINUE_CANCEL)
				{
					g_ListRoomUserControl.RemoveAt(ptTemp);
					break;
				}

				//��������
				CopyMemory(&Keyroomusercontrol, &roomusercontrol, sizeof(roomusercontrol));
				ptList = ptTemp;

				return true;
			}

		}
		
	}
	
	return false;
}

void CTableFrameSink::GetControlTypeString(CONTROL_TYPE &controlType, CString &controlTypestr)
{
	switch(controlType)
	{
	case CONTINUE_WIN:
		{
			controlTypestr = TEXT("��������Ϊ��Ӯ");
			break;
		}
	case CONTINUE_LOST:
		{
			controlTypestr = TEXT("��������Ϊ����");
			break;
		}
	case CONTINUE_CANCEL:
		{
			controlTypestr = TEXT("��������Ϊȡ������");
			break;
		}
	}
}

//д��־�ļ�
void CTableFrameSink::WriteInfo(LPCTSTR pszString)
{
	//������������
	char* old_locale = _strdup( setlocale(LC_CTYPE,NULL) );
	setlocale( LC_CTYPE, "chs" );

	CStdioFile myFile;
	CString strFileName = TEXT("��ţţ����LOG.txt");
	BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
	if ( bOpen )
	{	
		myFile.SeekToEnd();
		myFile.WriteString( pszString );
		myFile.Flush();
		myFile.Close();
	}

	//��ԭ�����趨
	setlocale( LC_CTYPE, old_locale );
	free( old_locale );
}






//////////////////////////////////////////////////////////////////////////
