#include "Stdafx.h"
#include "GameClient.h"
#include "GameOption.h"
#include "GameClientEngine.h"

//////////////////////////////////////////////////////////////////////////

//��Ϸ��ʱ��
#define IDI_START_GAME				200									//��ʼ��ʱ��
#define IDI_CALL_CARD				201									//��ȱ��ʱ��
#define IDI_OPERATE_CARD			202									//������ʱ��

//��Ϸ��ʱ��
#define TIME_START_GAME				30									//��ʼ��ʱ��
#define TIME_CALL_CARD				30									//��ȱ��ʱ��
#define TIME_OPERATE_CARD			30									//������ʱ��

#define MAX_TIME_OUT				3									//���ʱ����

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

static CString GetFileDialogPath()
{
	CString strFileDlgPath;
	TCHAR szModuleDirectory[MAX_PATH];	//ģ��Ŀ¼
	GetModuleFileName(AfxGetInstanceHandle(),szModuleDirectory,sizeof(szModuleDirectory));
	int nModuleLen=lstrlen(szModuleDirectory);
	int nProcessLen=lstrlen(AfxGetApp()->m_pszExeName)+lstrlen(TEXT(".EXE")) + 1;
	if (nModuleLen<=nProcessLen) 
		return TEXT("");
	szModuleDirectory[nModuleLen-nProcessLen]=0;
	strFileDlgPath = szModuleDirectory;
	return strFileDlgPath;
}

static void NcaTextOut(CString strInfo)
{
	CTime time = CTime::GetCurrentTime() ;

	CString strName;
	strName.Format(TEXT("%s\\SparrowXZ%04d%02d%02d.log"), 
		GetFileDialogPath(),
		time.GetYear(),
		time.GetMonth(),
		time.GetDay()
		);

	CString strTime ;
	strTime.Format( TEXT( "%04d-%02d-%02d %02d:%02d:%02d" ) ,
		time.GetYear() ,
		time.GetMonth() ,
		time.GetDay() ,
		time.GetHour() ,
		time.GetMinute() ,
		time.GetSecond()
		);
	CString strMsg;
	strMsg.Format(TEXT("%s,%s\r\n"),strTime,strInfo);

	CFile fLog;
	if(fLog.Open( strName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite ))
	{

		fLog.SeekToEnd(); 	
		int strLength=strMsg.GetLength();
#ifdef _UNICODE
		BYTE bom[2] = {0xff, 0xfe};
		fLog.Write(bom,sizeof(BYTE)*2);
		strLength*=2;
#endif
		fLog.Write((LPCTSTR)strMsg,strLength);
		fLog.Close();
	}
}

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_CALL_CARD,OnCallCard)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_CARD_OPERATE,OnCardOperate)
	ON_MESSAGE(IDM_TRUSTEE_CONTROL,OnStusteeControl)
	ON_MESSAGE(IDM_SICE_TWO,OnSiceTwo)
	ON_MESSAGE(IDM_SICE_FINISH,OnSiceFinish)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameClientEngine::CGameClientEngine()
{
	//��Ϸ����
	m_bCheatMode=false;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_cbUserAction = 0;
	m_lSiceCount = 0;

	//ȱ�ű���
	ZeroMemory(m_bCallCard, sizeof(m_bCallCard));
	ZeroMemory(m_cbCallCard, sizeof(m_cbCallCard));

	//��������
	m_wHeapHand=0;
	m_wHeapTail=0;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

	//�йܱ���
	m_bStustee=false;
	m_wTimeOutCount =0;

	//������Ϣ
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//����˿�
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//�˿˱���
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	//�������
	srand((unsigned int)time(NULL));

	return;
}

//��������
CGameClientEngine::~CGameClientEngine()
{
}

//��ʼ����
bool CGameClientEngine::OnInitGameEngine()
{
	//����ͼ��
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//��������
	m_DirectSound.Create( TEXT("OUT_CARD") );

	////TEST+
	//m_GameClientView.m_btStart.ShowWindow(true);
	////TEST+

	return true;
}

//���ÿ��
bool CGameClientEngine::OnResetGameEngine()
{
	//��Ϸ����
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_cbUserAction = 0;
	m_lSiceCount = 0;

	//ȱ�ű���
	ZeroMemory(m_bCallCard, sizeof(m_bCallCard));
	ZeroMemory(m_cbCallCard, sizeof(m_cbCallCard));
	
	//�йܱ���
	m_bStustee=false;
	m_wTimeOutCount =0;

	//��������
	m_wHeapHand=0;
	m_wHeapTail=0;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

	//������Ϣ
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//����˿�
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//�˿˱���
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	//�зְ�ť
	m_GameClientView.m_btCallCard1.ShowWindow(false);
	m_GameClientView.m_btCallCard2.ShowWindow(false);
	m_GameClientView.m_btCallCard3.ShowWindow(false);

	return true;
}

//ʱ��ɾ��
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
	return true;
}

//ʱ����Ϣ
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)
{
	switch (wClockID)
	{
	case IDI_START_GAME:		//��ʼ��Ϸ
		{
			if( m_bStustee && nElapse < TIME_START_GAME )
			{
				if (IsLookonMode() == false) 
				{
					OnStart(0,0);
				}
				return true;
			}
			if (nElapse==0)
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return true;
			}
			if (nElapse <= 5 && IsLookonMode() == false)
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_OPERATE_CARD:		//������ʱ��
		{
			//�Զ�����
			bool bAutoOutCard=false;
			if ((bAutoOutCard==true)&&(m_GameClientView.m_ControlWnd.IsWindowVisible()))
				bAutoOutCard=false;
			if((bAutoOutCard==false)&&(m_bStustee==true))
			{
				bAutoOutCard=true;
			}

			//��ʱ�ж�
			if ((IsLookonMode()==false)&&
				( nElapse==0 || bAutoOutCard || 
				nElapse < TIME_OPERATE_CARD-1&&(m_cbUserAction&WIK_CHI_HU)&&m_cbLeftCardCount<4 )
				)
			{
				//��ȡλ��
				WORD wMeChairID=GetMeChairID();

				//��������
				if (wChairID==wMeChairID)
				{
					if(m_bStustee==false && ++m_wTimeOutCount>=MAX_TIME_OUT )
					{
						m_wTimeOutCount = 0;
						OnStusteeControl(TRUE,0);
						if ( m_pIStringMessage )
							m_pIStringMessage->InsertSystemString(TEXT("��������γ�ʱ���л�Ϊ��ϵͳ�йܡ�ģʽ."));
					}

					if (m_wCurrentUser==wMeChairID)
					{
						if( m_cbLeftCardCount < 4 && (m_cbUserAction&WIK_CHI_HU) )
						{
							OnCardOperate( WIK_CHI_HU,0 );
							return true;
						}

						//��ȡ�˿�
						BYTE cbCardData=m_GameClientView.m_HandCardControl.GetCurrentCard();

						//����Ч��
						if (VerdictOutCard(cbCardData)==false)
						{
							for (BYTE i=0;i<MAX_INDEX;i++)
							{
								//����Ч��
								if (m_cbCardIndex[i]==0) continue;
								if (VerdictOutCard(m_GameLogic.SwitchToCardData(i))==false) 
									continue;

								//���ñ���
								cbCardData=m_GameLogic.SwitchToCardData(i);
							}
						}

						//���ƶ���
						OnOutCard(cbCardData,cbCardData);
					}
					else if( m_cbLeftCardCount < 4 && (m_cbUserAction&WIK_CHI_HU) )
						OnCardOperate( WIK_CHI_HU,0 );
					else
						OnCardOperate(WIK_NULL,0);
				}

				return true;
			}

			//��������
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) 
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_CALL_CARD:			//��ȱ��ʱ��
		{
			//�йܲ���
			if (m_bStustee && nElapse < TIME_CALL_CARD)
			{
				if (IsLookonMode() == false)
				{
					if (m_bCallCard[GetMeChairID()] == false)
					{
						BYTE cbCallCard = rand() % 3 + 1;
						OnCallCard(cbCallCard, cbCallCard);
						return true;
					}
				}
			}

			//��ʱ����
			if (nElapse == 0)
			{
				//ɾ��ʱ��
				KillGameClock(IDI_CALL_CARD);

				if (IsLookonMode() == false)
				{
					if (m_bCallCard[GetMeChairID()] == false)
					{
						BYTE cbCallCard = rand() % 3 + 1;
						OnCallCard(cbCallCard, cbCallCard);
						return true;
					}
				}
			}

			//�������
			if (nElapse <= 5 && IsLookonMode()==false)
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}
		}
	}

	return true;
}

//�Թ�״̬
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	//�˿˿���
	m_GameClientView.m_HandCardControl.SetDisplayItem(IsAllowLookon());
	m_GameClientView.InvalidGameView(0,0,0,0);
	
	return true;
}

//������Ϣ
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//��Ϸ��ʼ
		{
			return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_CALL_CARD:		//�û�ѡȱ
		{
			return OnSubCallCard(pBuffer, wDataSize);
		}
	case SUB_S_BANKER_INFO:		//ׯ����Ϣ
		{
			return OnSubBankerInfo(pBuffer,wDataSize);
		}
	case SUB_S_OUT_CARD:		//�û�����
		{
			return OnSubOutCard(pBuffer,wDataSize);
		}
	case SUB_S_SEND_CARD:		//������Ϣ
		{
			return OnSubSendCard(pBuffer,wDataSize);
		}
	case SUB_S_OPERATE_NOTIFY:	//������ʾ
		{
			return OnSubOperateNotify(pBuffer,wDataSize);
		}
	case SUB_S_OPERATE_RESULT:	//�������
		{
			return OnSubOperateResult(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:		//��Ϸ����
		{
			return OnSubGameEnd(pBuffer,wDataSize);
		}
	case SUB_S_TRUSTEE:			//�û��й�
		{
			return OnSubTrustee(pBuffer,wDataSize);
		}
	case SUB_S_CHI_HU:			//�û��Ժ�
		{
			return OnSubUserChiHu( pBuffer,wDataSize );
		}
	case SUB_S_GANG_SCORE:		//���Ʒ���
		{
			return OnSubGangScore(pBuffer,wDataSize);
		}
	case SUB_S_WAIT_OPERATE:	//�ȴ�����
		{
			m_GameClientView.SetStatusFlag(false, true);
			return true;
		}
	}

	return true;
}

//��Ϸ����
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStation, bool bLookonOther, VOID * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_MJ_FREE:	//����״̬
		{
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//�Ƿ���������
			if( !CGlobalUnits::GetInstance()->m_bAllowSound )
			{
				m_DirectSound.EnableSound(FALSE);
			}

			//��������
			m_bCheatMode=pStatusFree->bCheatMode;
			m_wBankerUser=pStatusFree->wBankerUser;
			m_GameClientView.SetCellScore(pStatusFree->lCellScore);
			m_GameClientView.m_HandCardControl.SetDisplayItem(true);

			//�й�����
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetTrustee(SwitchViewChairID(i),pStatusFree->bTrustee[i]);
			}

			//���ý���
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_cbHeapCardInfo[i][0]=0;
				m_cbHeapCardInfo[i][1]=0;
				m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);
			}

			//���ÿؼ�
			if (IsLookonMode()==false)
			{
				if (!m_bCheatMode)
				{
					m_GameClientView.m_btStart.ShowWindow(TRUE);
					SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
				}
				else
				{
					m_GameClientView.m_btStart.ShowWindow(FALSE);
				}
				m_GameClientView.m_btStustee.ShowWindow(true);
				m_GameClientView.m_btCancelStustee.ShowWindow(false);
				m_GameClientView.m_btStustee.EnableWindow(TRUE);
			}

			//��ʷ�ɼ�
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				//��ȡ����
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//���û���
				pHistoryScore->lTurnScore=pStatusFree->lTurnScore[i];
				pHistoryScore->lCollectScore=pStatusFree->lCollectScore[i];

				//������
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetHistoryScore(wViewChairID,pHistoryScore);
			}
			m_GameClientView.m_btOpenHistoryScore.ShowWindow(true);

			//�Թ۽���
			m_GameClientView.m_btCancelStustee.EnableWindow(!IsLookonMode());
			m_GameClientView.m_btStustee.EnableWindow(!IsLookonMode());

			//����Ч��
			m_GameClientView.SetDiscUser(INVALID_CHAIR);
			m_GameClientView.SetTimer(IDI_DISC_EFFECT, 180, NULL);

			//���½���
			m_GameClientView.InvalidGameView(0,0,0,0);

			return true;
		}
	case GS_MJ_CALL:	//��Ϸ״̬
		{
			//Ч������
			if (wDataSize != sizeof(CMD_S_StatusCall)) return false;
			CMD_S_StatusCall * pStatusCall = (CMD_S_StatusCall *)pBuffer;

			//�Ƿ���������
			if( !CGlobalUnits::GetInstance()->m_bAllowSound )
			{
				m_DirectSound.EnableSound(FALSE);
			}

			//��������
			WORD wViewChairID[GAME_PLAYER]={0,0,0,0};
			for (WORD i=0;i<GAME_PLAYER;i++) wViewChairID[i]=SwitchViewChairID(i);

			//��������
			m_wBankerUser=pStatusCall->wBankerUser;
			m_GameClientView.SetCellScore(pStatusCall->lCellScore);

			//�Թ۽���
			if (IsLookonMode()==true)
			{
				bool bAllowLookon = IsAllowLookon();
				m_GameClientView.m_HandCardControl.SetDisplayItem(bAllowLookon);
			}

			//�й�����
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetTrustee(SwitchViewChairID(i),pStatusCall->bTrustee[i]);
			}
			if( pStatusCall->bTrustee[GetMeChairID()] )
			{
				m_bStustee = true;
				OnStusteeControl(0,0);
			}

			//��������
			m_GameClientView.SetBankerUser(wViewChairID[m_wBankerUser]);

			//ȱ����Ϣ
			CopyMemory(m_bCallCard, pStatusCall->bCallCard, sizeof(m_bCallCard));
			CopyMemory(m_cbCallCard, pStatusCall->cbCallCard, sizeof(m_cbCallCard));

			//������Ϣ
			m_wHeapHand = pStatusCall->wHeapHand;
			m_wHeapTail = pStatusCall->wHeapTail;
			CopyMemory(m_cbHeapCardInfo, pStatusCall->cbHeapCardInfo, sizeof(m_cbHeapCardInfo));

			//��������
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				m_GameClientView.m_HeapCard[wViewChairID[i]].SetCardData(m_cbHeapCardInfo[i][0], m_cbHeapCardInfo[i][1], HEAP_FULL_COUNT);
			}

			//�˿˱���
			m_GameLogic.SwitchToCardIndex(pStatusCall->cbCardData,pStatusCall->cbCardCount,m_cbCardIndex);

			//�˿�����
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				//�û��˿�
				if (i != GetMeChairID())
				{
					BYTE cbCardCount = MAX_COUNT-1;
					WORD wUserCardIndex = (wViewChairID[i]<2) ? wViewChairID[i] : 2;
					m_GameClientView.m_UserCard[wUserCardIndex].SetCardData(cbCardCount, false);
				}
			}

			//�˿�����
			if (m_wBankerUser == GetMeChairID())
			{
				//�����˿�
				if (pStatusCall->cbSendCardData != 0x00)
				{
					//��������
					BYTE cbCardCount = pStatusCall->cbCardCount;
					BYTE cbRemoveCard[] = {pStatusCall->cbSendCardData};

					//�����˿�
					m_GameLogic.RemoveCard(pStatusCall->cbCardData, cbCardCount, cbRemoveCard, 1);
					pStatusCall->cbCardData[pStatusCall->cbCardCount-1] = pStatusCall->cbSendCardData;
				}
				//�����˿�
				BYTE cbCardCount=pStatusCall->cbCardCount;
				m_GameClientView.m_HandCardControl.SetCardData(pStatusCall->cbCardData,cbCardCount-1,pStatusCall->cbCardData[cbCardCount-1]);
			}
			else m_GameClientView.m_HandCardControl.SetCardData(pStatusCall->cbCardData, pStatusCall->cbCardCount, 0);

			//��ʷ�ɼ�
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				//��ȡ����
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//���û���
				pHistoryScore->lTurnScore=pStatusCall->lTurnScore[i];
				pHistoryScore->lCollectScore=pStatusCall->lCollectScore[i];

				//������
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetHistoryScore(wViewChairID,pHistoryScore);
			}
			m_GameClientView.m_btOpenHistoryScore.ShowWindow(true);

			//ȱ������
			if ((IsLookonMode()==false)&&(!m_bCallCard[GetMeChairID()]))
			{
				//������ʾ
				m_GameClientView.ShowSelectTip(true);
				m_GameClientView.ShowWaitChooseTip(false);

				//��ʾ��ť
				m_GameClientView.m_btCallCard1.ShowWindow(true);
				m_GameClientView.m_btCallCard2.ShowWindow(true);
				m_GameClientView.m_btCallCard3.ShowWindow(true);

				//���ð�ť
				m_GameClientView.m_btCallCard1.EnableWindow(true);
				m_GameClientView.m_btCallCard2.EnableWindow(true);
				m_GameClientView.m_btCallCard3.EnableWindow(true);
			}

			//��������
			if (IsLookonMode()==false) 
			{
				m_GameClientView.m_HandCardControl.SetPositively(true);
				m_GameClientView.m_HandCardControl.SetDisplayItem(true);
				m_GameClientView.m_btStustee.EnableWindow(TRUE);
			}

			//����ʱ��
			if (m_wCurrentUser == INVALID_CHAIR)
			{
				//����ʱ��
				SetGameClock(GetMeChairID(), IDI_CALL_CARD, TIME_CALL_CARD);
			}

			//���½���
			m_GameClientView.InvalidGameView(0,0,0,0);

			return true;
		}
	case GS_MJ_PLAY:	//��Ϸ״̬
		{
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			//�Ƿ���������
			if( !CGlobalUnits::GetInstance()->m_bAllowSound )
			{
				m_DirectSound.EnableSound(FALSE);
			}

			//�û�����
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				IClientUserItem * pUserData = GetTableUserItem(i);
				if( pUserData == NULL ) 
				{
					m_szAccounts[i][0] = 0;
					continue;
				}
				lstrcpy(m_szAccounts[i], pUserData->GetNickName());
			}

			WORD wMeChairID=GetMeChairID();

			//���ñ���
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_cbLeftCardCount=pStatusPlay->cbLeftCardCount;

			//�Թ۽���
			if (IsLookonMode()==true)
			{
				bool bAllowLookon = IsAllowLookon();
				m_GameClientView.m_HandCardControl.SetDisplayItem(bAllowLookon);
			}

			//�й�����
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetTrustee(SwitchViewChairID(i),pStatusPlay->bTrustee[i]);
			}
			if( pStatusPlay->bTrustee[wMeChairID] )
			{
				m_bStustee = true;
				OnStusteeControl(0,0);
			}

			//��ʷ����
			m_wOutCardUser=pStatusPlay->wOutCardUser;
			m_cbOutCardData=pStatusPlay->cbOutCardData;
			CopyMemory(m_cbDiscardCard,pStatusPlay->cbDiscardCard,sizeof(m_cbDiscardCard));
			CopyMemory(m_cbDiscardCount,pStatusPlay->cbDiscardCount,sizeof(m_cbDiscardCount));
			if( pStatusPlay->wOutCardUser != INVALID_CHAIR )
				m_cbDiscardCard[pStatusPlay->wOutCardUser][m_cbDiscardCount[pStatusPlay->wOutCardUser]++] = pStatusPlay->cbOutCardData;

			//���ý���
			CopyMemory(m_cbCallCard, pStatusPlay->cbCallCard, sizeof(pStatusPlay->cbCallCard));
			for (WORD i=0; i<GAME_PLAYER; i++)
				m_GameClientView.SetVoidShowing(SwitchViewChairID(i), pStatusPlay->cbCallCard[i]);

			//����Ч��
			if(m_wOutCardUser != INVALID_CHAIR)
				m_GameClientView.SetDiscUser(SwitchViewChairID(m_wOutCardUser));
			m_GameClientView.SetTimer(IDI_DISC_EFFECT, 180, NULL);

			//�˿˱���
			CopyMemory(m_cbWeaveCount,pStatusPlay->cbWeaveCount,sizeof(m_cbWeaveCount));
			CopyMemory(m_WeaveItemArray,pStatusPlay->WeaveItemArray,sizeof(m_WeaveItemArray));
			m_GameLogic.SwitchToCardIndex(pStatusPlay->cbCardData,pStatusPlay->cbCardCount,m_cbCardIndex);

			//��������
			WORD wViewChairID[GAME_PLAYER]={0,0,0,0};
			for (WORD i=0;i<GAME_PLAYER;i++) wViewChairID[i]=SwitchViewChairID(i);

			//��������
			m_GameClientView.SetCellScore(pStatusPlay->lCellScore);
			m_GameClientView.SetBankerUser(wViewChairID[m_wBankerUser]);

			//����˿�
			BYTE cbWeaveCard[4]={0,0,0,0};
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wOperateViewID = SwitchViewChairID(i);
				for (BYTE j=0;j<m_cbWeaveCount[i];j++)
				{
					BYTE cbWeaveKind=m_WeaveItemArray[i][j].cbWeaveKind;
					BYTE cbCenterCard=m_WeaveItemArray[i][j].cbCenterCard;
					BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbCenterCard,cbWeaveCard);
					m_GameClientView.m_WeaveCard[wViewChairID[i]][j].SetCardData(cbWeaveCard,cbWeaveCardCount);
					if ((cbWeaveKind&WIK_GANG)&&(m_WeaveItemArray[i][j].wProvideUser==i)) 
						m_GameClientView.m_WeaveCard[wViewChairID[i]][j].SetDisplayItem(false);
					WORD wProviderViewID = SwitchViewChairID(m_WeaveItemArray[i][j].wProvideUser);
					m_GameClientView.m_WeaveCard[wOperateViewID][j].SetDirectionCardPos(3-(wOperateViewID-wProviderViewID+4)%4);
				}
			}

			//�û��˿�
			if (m_wCurrentUser==GetMeChairID())
			{
				//�����˿�
				if (pStatusPlay->cbSendCardData!=0x00)
				{
					//��������
					BYTE cbCardCount=pStatusPlay->cbCardCount;
					BYTE cbRemoveCard[]={pStatusPlay->cbSendCardData};

					//�����˿�
					m_GameLogic.RemoveCard(pStatusPlay->cbCardData,cbCardCount,cbRemoveCard,1);
					pStatusPlay->cbCardData[pStatusPlay->cbCardCount-1]=pStatusPlay->cbSendCardData;
				}

				//�����˿�
				BYTE cbCardCount=pStatusPlay->cbCardCount;
				m_GameClientView.m_HandCardControl.SetCardData(pStatusPlay->cbCardData,cbCardCount-1,pStatusPlay->cbCardData[cbCardCount-1]);
			}
			else 
			{
				m_GameClientView.m_HandCardControl.SetCardData(pStatusPlay->cbCardData,pStatusPlay->cbCardCount,0); 
			}

			//�˿�����
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//�û��˿�
				if (i!=GetMeChairID())
				{
					BYTE cbCardCount=13-m_cbWeaveCount[i]*3;
					WORD wUserCardIndex=(wViewChairID[i]<2)?wViewChairID[i]:2;
					m_GameClientView.m_UserCard[wUserCardIndex].SetCardData(cbCardCount,(m_wCurrentUser==i));
				}

				//�����˿�
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.m_DiscardCard[wViewChairID].SetCardData(m_cbDiscardCard[i],m_cbDiscardCount[i]);
			}

			//��������
			if (IsLookonMode()==false) 
			{
				m_GameClientView.m_HandCardControl.SetPositively(true);
				m_GameClientView.m_HandCardControl.SetDisplayItem(true);
				m_GameClientView.m_btStustee.EnableWindow(TRUE);
			}

			//�Թ۽���
			m_GameClientView.m_btCancelStustee.EnableWindow(!IsLookonMode());
			m_GameClientView.m_btStustee.EnableWindow(!IsLookonMode());

			//������Ϣ
			m_wHeapHand = pStatusPlay->wHeapHand;
			m_wHeapTail = pStatusPlay->wHeapTail;
			CopyMemory(m_cbHeapCardInfo,pStatusPlay->cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

			//��������
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.m_HeapCard[wViewChairID[i]].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);
			}

			//��ʷ�˿�
			if (m_wOutCardUser!=INVALID_CHAIR)
			{
				WORD wOutChairID=SwitchViewChairID(m_wOutCardUser);
				m_GameClientView.SetOutCardInfo(wOutChairID,m_cbOutCardData);
			}

			//����״̬
			for( WORD i = 0; i < CountArray(pStatusPlay->wWinOrder); i++ )
			{
				if( pStatusPlay->wWinOrder[i] != INVALID_CHAIR )
				{
					m_GameClientView.SetWinOrder( SwitchViewChairID(pStatusPlay->wWinOrder[i]),i+1 );
				}
			}

			//��������
			if ((IsLookonMode()==false)&&(pStatusPlay->cbActionMask!=WIK_NULL))
			{
				//��ȡ����
				BYTE cbActionMask=pStatusPlay->cbActionMask;
				BYTE cbActionCard=pStatusPlay->cbActionCard;

				m_cbUserAction = pStatusPlay->cbActionMask;

				//��������
				tagGangCardResult GangCardResult;
				ZeroMemory(&GangCardResult,sizeof(GangCardResult));

				//�����ж�
				if ((cbActionMask&WIK_GANG)!=0)
				{
					//�������
					if ((m_wCurrentUser==INVALID_CHAIR)&&(cbActionCard!=0))
					{
						GangCardResult.cbCardCount=1;
						GangCardResult.cbCardData[0]=cbActionCard;
					}

					//�Լ�����
					if ((m_wCurrentUser==GetMeChairID())||(cbActionCard==0))
					{
						WORD wMeChairID=GetMeChairID();
						m_GameLogic.AnalyseGangCard(m_cbCardIndex,m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID],GangCardResult);
					}
				}

				//���ý���
				if (m_wCurrentUser==INVALID_CHAIR)
					SetGameClock(GetMeChairID(),IDI_OPERATE_CARD,TIME_OPERATE_CARD);
				if (IsLookonMode()==false) 
					m_GameClientView.m_ControlWnd.SetControlInfo(cbActionCard,cbActionMask,GangCardResult);
			}

			//��ʷ�ɼ�
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				//��ȡ����
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//���û���
				pHistoryScore->lTurnScore=pStatusPlay->lTurnScore[i];
				pHistoryScore->lCollectScore=pStatusPlay->lCollectScore[i];

				//������
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetHistoryScore(wViewChairID,pHistoryScore);
			}
			m_GameClientView.m_btOpenHistoryScore.ShowWindow(true);

			//����ʱ��
			if (m_wCurrentUser!=INVALID_CHAIR)
			{
				//����ʱ��
				SetGameClock(m_wCurrentUser, IDI_OPERATE_CARD, TIME_OPERATE_CARD);
			}

			//����Ч��
			m_GameClientView.SetDiscUser(SwitchViewChairID(m_wOutCardUser));
			m_GameClientView.SetTimer(IDI_DISC_EFFECT, 180, NULL);

			//���½���
			m_GameClientView.InvalidGameView(0,0,0,0);

			return true;
		}
	}

	return false;
}

//��Ϸ��ʼ
bool CGameClientEngine::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//��ʼʱ��
	DWORD dwStartTime = GetTickCount();

	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//��������
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//�û�����
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		IClientUserItem * pUserData = GetTableUserItem( i );
		if( pUserData == NULL ) 
		{
			m_szAccounts[i][0] = 0;
			continue;
		}
		lstrcpy( m_szAccounts[i],pUserData->GetNickName() );
	}

	//����״̬
	SetGameStatus(GS_MJ_CALL);
	m_GameClientView.SetCurrentUser(INVALID_CHAIR);

	//���ñ���
	m_wBankerUser=pGameStart->wBankerUser;
	m_wCurrentUser=pGameStart->wCurrentUser;
	m_cbLeftCardCount=pGameStart->cbLeftCardCount;
	m_cbUserAction = pGameStart->cbUserAction;
	m_lSiceCount = pGameStart->lSiceCount;

	//������Ϣ
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//����˿�
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//�����˿�
	BYTE cbCardCount=(GetMeChairID()==m_wBankerUser)?MAX_COUNT:(MAX_COUNT-1);
	m_GameLogic.SwitchToCardIndex(pGameStart->cbCardData,cbCardCount,m_cbCardIndex);

	//�Թ۽���
	if (IsLookonMode()==true)
	{
		//���ý���
		m_GameClientView.SetDiscUser(INVALID_CHAIR);
		m_GameClientView.SetStatusFlag(false,false);
		m_GameClientView.SetBankerUser(INVALID_CHAIR);
		m_GameClientView.SetUserAction(INVALID_CHAIR,0);
		m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);
		m_GameClientView.SetWinOrder(INVALID_CHAIR,0);

		//�˿�����
		m_GameClientView.m_UserCard[0].SetCardData(0,false);
		m_GameClientView.m_UserCard[1].SetCardData(0,false);
		m_GameClientView.m_UserCard[2].SetCardData(0,false);
		m_GameClientView.m_HandCardControl.SetCardData(NULL,0,0);

		//�˿�����
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_GameClientView.m_ScoreControl[i].CloseControl();
			m_GameClientView.m_TableCard[i].SetCardData(NULL,0);
			m_GameClientView.m_DiscardCard[i].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[i][0].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[i][1].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[i][2].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[i][3].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[i][0].SetGameEnd(false);
			m_GameClientView.m_WeaveCard[i][1].SetGameEnd(false);
			m_GameClientView.m_WeaveCard[i][2].SetGameEnd(false);
			m_GameClientView.m_WeaveCard[i][3].SetGameEnd(false);
			m_GameClientView.m_WeaveCard[i][0].SetDisplayItem(true);
			m_GameClientView.m_WeaveCard[i][1].SetDisplayItem(true);
			m_GameClientView.m_WeaveCard[i][2].SetDisplayItem(true);
			m_GameClientView.m_WeaveCard[i][3].SetDisplayItem(true);
		}

		//�����˿�
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_cbHeapCardInfo[i][0]=0;
			m_cbHeapCardInfo[i][1]=0;
			m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);
		}
	}

	//������Ϣ
	m_wHeapHand = pGameStart->wHeapHand;
	m_wHeapTail = pGameStart->wHeapTail;
	CopyMemory(m_cbHeapCardInfo,pGameStart->cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

	//���ý���
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	m_GameClientView.m_HandCardControl.SetPositively(false);
	if ((m_lSiceCount&0xFFFF0000)==0)m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

	//��������
	PlayGameSound(AfxGetInstanceHandle(),TEXT("DRAW_SICE"));

	//ҡɫ�Ӷ���
	m_GameClientView.m_DrawSiceWnd.SetSiceInfo(m_GameClientView.GetDC(),200,(WORD)(pGameStart->lSiceCount>>16),(WORD)pGameStart->lSiceCount);
	m_GameClientView.m_DrawSiceWnd.ShowWindow(SW_SHOW);

	//ʱ����
	DWORD dwEndTime = GetTickCount();
	DWORD dwDiffenTime = dwEndTime - dwStartTime;

	//д���ռ�
	CString strValue;
	strValue.Format(TEXT("��Ϸ��ʼ����ʼʱ��=%d������ʱ��=%d��ʱ����=%d��"), dwStartTime, dwEndTime, dwDiffenTime);
	NcaTextOut(strValue);

	return true;
}

//�û�ѡȱ
bool CGameClientEngine::OnSubCallCard(const void * pBuffer, WORD wDataSize)
{
	//Ч����Ϣ
	ASSERT(wDataSize == sizeof(CMD_S_CallCard));
	if (wDataSize != sizeof(CMD_S_CallCard)) return false;

	//��Ϣ����
	CMD_S_CallCard * pCallCard = (CMD_S_CallCard *)pBuffer;

	//���ñ���
	m_bCallCard[pCallCard->wCallUser] = true;

	return true;
}

//ׯ����Ϣ
bool CGameClientEngine::OnSubBankerInfo(const void * pBuffer, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_S_BankerInfo));
	if (wDataSize!=sizeof(CMD_S_BankerInfo)) return false;

	//��������
	CMD_S_BankerInfo * pBankerInfo=(CMD_S_BankerInfo *)pBuffer;

	//����״̬
	SetGameStatus(GS_MJ_PLAY);

	//������
	if (m_bStustee==false) ActiveGameFrame();

	//���ñ���
	m_wBankerUser = pBankerInfo->wBankerUser;
	m_wCurrentUser = pBankerInfo->wCurrentUser;

	//���ý���
	for (WORD i=0; i<GAME_PLAYER; i++)
	{
		m_GameClientView.SetVoidShowing(SwitchViewChairID(i), pBankerInfo->cbCallCard[i]);
	}
	m_GameClientView.ShowSelectTip(false);
	m_GameClientView.ShowWaitChooseTip(false);

	//������ʾ
	if ((IsLookonMode()==false)&&(m_wCurrentUser!=INVALID_CHAIR))
	{
		WORD wMeChairID=GetMeChairID();
		if (m_wCurrentUser==wMeChairID) m_GameClientView.SetStatusFlag(true, false);
	}

	//��������
	if ((IsLookonMode()==false)&&(m_cbUserAction!=WIK_NULL))
	{
		ShowOperateControl(m_cbUserAction, 0);
	}

	//����ʱ��
	m_GameClientView.SetCurrentUser(SwitchViewChairID(m_wCurrentUser));
	SetGameClock(m_wCurrentUser, IDI_OPERATE_CARD, TIME_OPERATE_CARD);
	m_GameClientView.m_HandCardControl.SetPositively(IsLookonMode()==false);

	//��������
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	return true;
}

//�û�����
bool CGameClientEngine::OnSubOutCard(const void * pBuffer, WORD wDataSize)
{
	//Ч����Ϣ
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard)) return false;

	//��Ϣ����
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;

	//��������
	WORD wMeChairID=GetMeChairID();
	WORD wOutViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);

	//���ñ���
	m_wCurrentUser=INVALID_CHAIR;
	m_wOutCardUser=pOutCard->wOutCardUser;
	m_cbOutCardData=pOutCard->cbOutCardData;

	//�����û�
	if ((IsLookonMode()==true)||(pOutCard->wOutCardUser!=wMeChairID))
	{
		//��������
		KillGameClock(IDI_OPERATE_CARD); 

		//��������
		PlayCardSound(pOutCard->wOutCardUser,pOutCard->cbOutCardData);

		//���ƽ���
		m_GameClientView.SetUserAction(INVALID_CHAIR,0);
		m_GameClientView.SetOutCardInfo(wOutViewChairID,pOutCard->cbOutCardData);

		//�����˿�
		if (wOutViewChairID==2)
		{
			//ɾ���˿�
			BYTE cbCardData[MAX_COUNT];
			m_GameLogic.RemoveCard(m_cbCardIndex,pOutCard->cbOutCardData);

			//�����˿�
			BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData);
			m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,0);
		}
		else
		{
			WORD wUserIndex=(wOutViewChairID>2)?2:wOutViewChairID;
			m_GameClientView.m_UserCard[wUserIndex].SetCurrentCard(false);
		}
	}

	return true;
}

//������Ϣ
bool CGameClientEngine::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_SendCard));
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;

	//��������
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

	//���ñ���
	WORD wMeChairID=GetMeChairID();
	m_wCurrentUser=pSendCard->wCurrentUser;

	//���ؿؼ�
	m_GameClientView.m_ControlWnd.ShowWindow( SW_HIDE );
	//ɾ����ʱ��
	KillGameClock( IDI_OPERATE_CARD );

	//�����˿�
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		//�����˿�
		WORD wOutViewChairID=SwitchViewChairID(m_wOutCardUser);
		m_GameClientView.m_DiscardCard[wOutViewChairID].AddCardItem(m_cbOutCardData);
		m_GameClientView.SetDiscUser(wOutViewChairID);

		//���ñ���
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}

	//���ƴ���
	if (pSendCard->cbCardData!=0)
	{
		//ȡ�ƽ���
		WORD wViewChairID=SwitchViewChairID(m_wCurrentUser);
		if (wViewChairID!=2)
		{
			WORD wUserIndex=(wViewChairID>2)?2:wViewChairID;
			m_GameClientView.m_UserCard[wUserIndex].SetCurrentCard(true);
		}
		else
		{
			m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pSendCard->cbCardData)]++;
			m_GameClientView.m_HandCardControl.SetCurrentCard(pSendCard->cbCardData);
		}

		//�۳��˿�
		DeductionTableCard(pSendCard->bTail==false);
	}

	//��ǰ�û�
	if ((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID))
	{
		//������
		ActiveGameFrame();

		//��������
		if (pSendCard->cbActionMask!=WIK_NULL)
		{
			//��ȡ����
			BYTE cbActionCard=pSendCard->cbCardData;
			BYTE cbActionMask=pSendCard->cbActionMask;

			m_cbUserAction = pSendCard->cbActionMask;

			//��������
			tagGangCardResult GangCardResult;
			ZeroMemory(&GangCardResult,sizeof(GangCardResult));

			//�����ж�
			if ((cbActionMask&WIK_GANG)!=0)
			{
				WORD wMeChairID=GetMeChairID();
				m_GameLogic.AnalyseGangCard(m_cbCardIndex,m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID],GangCardResult);
			}

			//���ý���
			m_GameClientView.m_ControlWnd.SetControlInfo(cbActionCard,cbActionMask,GangCardResult);
		}
	}

	//������ʾ
	m_GameClientView.SetStatusFlag((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID),false);

	//���½���
	m_GameClientView.InvalidGameView(0,0,0,0);

	//����ʱ��
	WORD wTimeCount=TIME_OPERATE_CARD;

	//����ʱ��
	m_GameClientView.SetCurrentUser(SwitchViewChairID(m_wCurrentUser));
	SetGameClock(m_wCurrentUser,IDI_OPERATE_CARD,wTimeCount);

	return true;
}

//������ʾ
bool CGameClientEngine::OnSubOperateNotify(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_OperateNotify));
	if (wDataSize!=sizeof(CMD_S_OperateNotify)) 
		return false;

	//��������
	CMD_S_OperateNotify * pOperateNotify=(CMD_S_OperateNotify *)pBuffer;

	//�û�����
	if ((IsLookonMode()==false)&&(pOperateNotify->cbActionMask!=WIK_NULL))
	{
		//��ȡ����
		WORD wMeChairID=GetMeChairID();
		BYTE cbActionMask=pOperateNotify->cbActionMask;
		BYTE cbActionCard=pOperateNotify->cbActionCard;

		m_cbUserAction = pOperateNotify->cbActionMask;

		//��������
		tagGangCardResult GangCardResult;
		ZeroMemory(&GangCardResult,sizeof(GangCardResult));

		//�����ж�
		if ((cbActionMask&WIK_GANG)!=0)
		{
			//�������
			if ((m_wCurrentUser==INVALID_CHAIR)&&(cbActionCard!=0))
			{
				GangCardResult.cbCardCount=1;
				GangCardResult.cbCardData[0]=cbActionCard;
			}

			//�Լ�����
			if ((m_wCurrentUser==wMeChairID)||(cbActionCard==0))
			{
				WORD wMeChairID=GetMeChairID();
				m_GameLogic.AnalyseGangCard(m_cbCardIndex,m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID],GangCardResult);
			}
		}

		//���ý���
		ActiveGameFrame();
		m_GameClientView.m_ControlWnd.SetControlInfo(cbActionCard,cbActionMask,GangCardResult);

		//����ʱ��
		m_GameClientView.SetCurrentUser(INVALID_CHAIR);
		SetGameClock(GetMeChairID(),IDI_OPERATE_CARD,TIME_OPERATE_CARD);
	}

	return true;
}

//�������
bool CGameClientEngine::OnSubOperateResult(const void * pBuffer, WORD wDataSize)
{
	//Ч����Ϣ
	ASSERT(wDataSize==sizeof(CMD_S_OperateResult));
	if (wDataSize!=sizeof(CMD_S_OperateResult)) 
		return false;

	//��Ϣ����
	CMD_S_OperateResult * pOperateResult=(CMD_S_OperateResult *)pBuffer;

	//��������
	BYTE cbPublicCard=TRUE;
	WORD wOperateUser=pOperateResult->wOperateUser;
	BYTE cbOperateCard=pOperateResult->cbOperateCard;
	WORD wOperateViewID=SwitchViewChairID(wOperateUser);
	WORD wProviderViewID = SwitchViewChairID(pOperateResult->wProvideUser);

	//���Ʊ���
	if (pOperateResult->cbOperateCode!=WIK_NULL)
	{
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}

	//���ñ���
	m_wCurrentUser=pOperateResult->wOperateUser;

	//�������
	if ((pOperateResult->cbOperateCode&WIK_GANG)!=0)
	{
		m_wCurrentUser = INVALID_CHAIR;

		//�����ж�
		cbPublicCard=(pOperateResult->wProvideUser==wOperateUser)?FALSE:TRUE;

		//����˿�
		BYTE cbWeaveIndex=0xFF;
		for (BYTE i=0;i<m_cbWeaveCount[wOperateUser];i++)
		{
			BYTE cbWeaveKind=m_WeaveItemArray[wOperateUser][i].cbWeaveKind;
			BYTE cbCenterCard=m_WeaveItemArray[wOperateUser][i].cbCenterCard;
			if ((cbCenterCard==cbOperateCard)&&(cbWeaveKind==WIK_PENG))
			{
				cbWeaveIndex=i;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;
				break;
			}
		}

		//����˿�
		if (cbWeaveIndex==0xFF)
		{
			//�����˿�
			cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=cbPublicCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;
		}

		//��Ͻ���
		BYTE cbWeaveCard[4]={0,0,0,0},cbWeaveKind=pOperateResult->cbOperateCode;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetCardData(cbWeaveCard,cbWeaveCardCount);
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetDisplayItem((m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard==TRUE)?true:false);
		if (m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard==FALSE) m_GameClientView.m_WeaveCard[2][cbWeaveIndex].SetGameEnd(true);

		//�˿�����
		if (GetMeChairID()==wOperateUser)
		{
			m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pOperateResult->cbOperateCard)]=0;
		}

		//�����˿�
		if (GetMeChairID()==wOperateUser)
		{
			BYTE cbCardData[MAX_COUNT];
			BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData);
			m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,0);
		}
		else
		{
			WORD wUserIndex=(wOperateViewID>=3)?2:wOperateViewID;
			BYTE cbCardCount=MAX_COUNT-m_cbWeaveCount[wOperateUser]*3;
			m_GameClientView.m_UserCard[wUserIndex].SetCardData(cbCardCount-1,false);
		}
	}
	else if (pOperateResult->cbOperateCode!=WIK_NULL)
	{
		//�������
		BYTE cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;

		//��Ͻ���
		BYTE cbWeaveCard[4]={0,0,0,0},cbWeaveKind=pOperateResult->cbOperateCode;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetCardData(cbWeaveCard,cbWeaveCardCount);
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetDirectionCardPos(3-(wOperateViewID-wProviderViewID+4)%4);

		//ɾ���˿�
		if (GetMeChairID()==wOperateUser)
		{
			VERIFY( m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1) );
			VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex,cbWeaveCard,cbWeaveCardCount-1) );
		}

		//�����˿�
		if (GetMeChairID()==wOperateUser)
		{
			BYTE cbCardData[MAX_COUNT];
			BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData);
			m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount-1,cbCardData[cbCardCount-1]);
		}
		else
		{
			WORD wUserIndex=(wOperateViewID>=3)?2:wOperateViewID;
			BYTE cbCardCount=MAX_COUNT-m_cbWeaveCount[wOperateUser]*3;
			m_GameClientView.m_UserCard[wUserIndex].SetCardData(cbCardCount-1,true);
		}
	}

	//���ý���
	m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	m_GameClientView.SetUserAction(wOperateViewID,pOperateResult->cbOperateCode);
	m_GameClientView.SetStatusFlag((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()),false);

	//���½���
	m_GameClientView.InvalidGameView(0,0,0,0);

	//��������
	PlayActionSound(wOperateUser,pOperateResult->cbOperateCode);

	//����ʱ��
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		//����ʱ��
		WORD wTimeCount=TIME_OPERATE_CARD;

		//����ʱ��
		m_GameClientView.SetCurrentUser(SwitchViewChairID(m_wCurrentUser));
		SetGameClock(m_wCurrentUser,IDI_OPERATE_CARD,wTimeCount);
	}

	return true;
}

//��Ϸ����
bool CGameClientEngine::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//��Ϣ����
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//����״̬
	SetGameStatus(GS_MJ_FREE);
	m_GameClientView.SetStatusFlag(false,false);

	//ɾ����ʱ��
	KillGameClock(IDI_OPERATE_CARD);

	//���ÿؼ�
	m_GameClientView.ShowSelectTip(false);
	m_GameClientView.ShowWaitChooseTip(false);
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	m_GameClientView.m_HandCardControl.SetPositively(false);

	//�����˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_WeaveCard[i][0].SetGameEnd(true);
		m_GameClientView.m_WeaveCard[i][1].SetGameEnd(true);
		m_GameClientView.m_WeaveCard[i][2].SetGameEnd(true);
		m_GameClientView.m_WeaveCard[i][3].SetGameEnd(true);
	}
	
	//��������
	tagScoreInfo ScoreInfo;
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

	//���û���
	CString strTemp ,strEnd;
	if( pGameEnd->wLeftUser != INVALID_CHAIR )
	{
		strEnd.AppendFormat( TEXT("���[]���ܣ����ջعη��������ã����渶������ҵĹη����꣬��δ�����߻���"),
			m_szAccounts[pGameEnd->wLeftUser] );
		if ( m_pIStringMessage )
			m_pIStringMessage->InsertSystemString( strEnd );
	}
	strEnd = TEXT("���ֽ���,�ɼ�ͳ�ƣ�\r\n");
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( m_szAccounts[i][0] == 0 ) continue;

		lstrcpy( ScoreInfo.szUserName[SwitchViewChairID(i)],m_szAccounts[i] );
		strTemp.Format(TEXT("%s�� %I64d\n"),m_szAccounts[i],pGameEnd->lGameScore[i]);
		strEnd += strTemp;
	}
	//��Ϣ����
	if ( m_pIStringMessage )
		m_pIStringMessage->InsertSystemString((LPCTSTR)strEnd);

	//�ɼ�����
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		WORD wViewId = SwitchViewChairID(i);
		ScoreInfo.wMeChairId = wViewId;

		WORD j = 0;
		for( j = 0; j < CountArray(pGameEnd->wWinOrder); j++ )
		{
			if(pGameEnd->wWinOrder[j] == i)
			{
				ScoreInfo.cbWinOrder = (BYTE)j+1;
				break;
			}
		}
		if (j == CountArray(pGameEnd->wWinOrder)) ScoreInfo.cbWinOrder = 0;
		
		ScoreInfo.lGameTax = pGameEnd->lGameTax[i];
		ScoreInfo.cbGenCount = pGameEnd->cbGenCount[i];
		ScoreInfo.lGameScore = pGameEnd->lGameScore[i];
		ScoreInfo.lGangScore = pGameEnd->lGangScore[i];
		ScoreInfo.wProvideUser = SwitchViewChairID(pGameEnd->wProvideUser[i]);
		CopyMemory(ScoreInfo.dwChiHuRight, &pGameEnd->dwChiHuRight[i], sizeof(DWORD)*MAX_RIGHT_COUNT);
		ZeroMemory(ScoreInfo.wLostFanShu, sizeof(ScoreInfo.wLostFanShu));
		for (WORD j = 0; j < GAME_PLAYER; j++)
		{
			if (j == i) continue;
			ScoreInfo.wLostFanShu[SwitchViewChairID(j)] = pGameEnd->wLostFanShu[i][j];
			ScoreInfo.lHuaZhuScore[SwitchViewChairID(j)] = pGameEnd->lHuaZhuScore[i][j];
			ScoreInfo.lChaJiaoScore[SwitchViewChairID(j)] = pGameEnd->lChaJiaoScore[i][j];
			ScoreInfo.lLostHuaZhuScore[SwitchViewChairID(j)] = pGameEnd->lLostHuaZhuScore[i][j];
			ScoreInfo.lLostChaJiaoScore[SwitchViewChairID(j)] = pGameEnd->lLostChaJiaoScore[i][j];
		}

		m_GameClientView.m_ScoreControl[wViewId].SetScoreInfo(ScoreInfo);
	}

	//��ʾ�ɼ�
	m_GameClientView.ShowGameScoreInfo();

	//�û��˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wViewChairID=SwitchViewChairID(i);
		m_GameClientView.m_TableCard[wViewChairID].SetCardData(pGameEnd->cbCardData[i],pGameEnd->cbCardCount[i]);
	}

	//��ʷ�ɼ�
	for (WORD i=0; i<GAME_PLAYER; i++)
	{
		tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
		m_HistoryScore.OnEventUserScore(i,pGameEnd->lGameScore[i]);
	}

	//�����˿�
	m_GameClientView.m_UserCard[0].SetCardData(0,false);
	m_GameClientView.m_UserCard[1].SetCardData(0,false);
	m_GameClientView.m_UserCard[2].SetCardData(0,false);
	m_GameClientView.m_HandCardControl.SetCardData(NULL,0,0);

	//��������
	LONGLONG lScore=pGameEnd->lGameScore[GetMeChairID()];
	if (lScore>0L)
	{
		PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	}
	else if (lScore<0L)
	{
		PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	}
	else 
	{
		PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_END"));
	}

	//���ý���
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(TRUE);
		m_GameClientView.SetCurrentUser(INVALID_CHAIR);
		SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
	}

	//���ذ�ť
	m_GameClientView.m_btCallCard1.ShowWindow(false);
	m_GameClientView.m_btCallCard2.ShowWindow(false);
	m_GameClientView.m_btCallCard3.ShowWindow(false);

	//ȡ���й�
	if(m_bStustee)
		OnStusteeControl(0,0);

	//���½���
	m_GameClientView.InvalidGameView(0,0,0,0);

	return true;
}

//�û��й�
bool CGameClientEngine::OnSubTrustee(const void * pBuffer,WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_Trustee));
	if (wDataSize!=sizeof(CMD_S_Trustee)) return false;

	//��Ϣ����
	CMD_S_Trustee * pTrustee=(CMD_S_Trustee *)pBuffer;
	m_GameClientView.SetTrustee(SwitchViewChairID(pTrustee->wChairID),pTrustee->bTrustee);
	if ((IsLookonMode()==true)||(pTrustee->wChairID!=GetMeChairID()))
	{
		IClientUserItem * pUserData = GetTableUserItem(pTrustee->wChairID);
		TCHAR szBuffer[256];
		if(pTrustee->bTrustee==true)
			_sntprintf(szBuffer,sizeof(szBuffer),TEXT("���[%s]ѡ�����йܹ��ܡ�"),pUserData->GetNickName());
		else
			_sntprintf(szBuffer,sizeof(szBuffer),TEXT("���[%s]ȡ�����йܹ��ܡ�"),pUserData->GetNickName());
		if ( m_pIStringMessage )
			m_pIStringMessage->InsertSystemString(szBuffer);
	}

	return true;
}

//������Ϣ
bool CGameClientEngine::OnSubUserChiHu( const void *pBuffer,WORD wDataSize )
{
	//Ч����Ϣ
	ASSERT(wDataSize==sizeof(CMD_S_ChiHu));
	if (wDataSize!=sizeof(CMD_S_ChiHu)) return false;

	//��Ϣ����
	CMD_S_ChiHu * pChiHu=(CMD_S_ChiHu *)pBuffer;

	WORD wViewId = SwitchViewChairID( pChiHu->wChiHuUser );

	//���ؿؼ�
	m_GameClientView.m_ControlWnd.ShowWindow( SW_HIDE );
	//ɾ����ʱ��
	KillGameClock( IDI_OPERATE_CARD );

	BYTE cbCardData[MAX_COUNT];
	ZeroMemory( cbCardData,sizeof(cbCardData) );
	cbCardData[pChiHu->cbCardCount-1] = pChiHu->cbChiHuCard;

	//�����˿�,����
	if( wViewId == 2 )
	{
		m_GameClientView.m_HandCardControl.SetPositively( false );
		m_GameClientView.m_HandCardControl.SetCardData( NULL,0,0 );
		m_GameClientView.m_TableCard[2].SetCardData( cbCardData,pChiHu->cbCardCount );
	}
	else
	{
		WORD wUserIndex=(wViewId>2)?2:wViewId;
		m_GameClientView.m_UserCard[wUserIndex].SetCardData( 0,false );
		
		m_GameClientView.m_TableCard[wViewId].SetCardData( cbCardData,pChiHu->cbCardCount );
	}
	m_GameClientView.SetWinOrder(wViewId, pChiHu->cbWinOrder);

	////��ʾ��Ϣ
	//TCHAR szBuffer[128] = TEXT("");
	//_sntprintf( szBuffer,CountArray(szBuffer),TEXT("\n���[%s]����,�÷�:%+I64d\n"),m_szAccounts[pChiHu->wChiHuUser],pChiHu->lGameScore );
	//InsertSystemString( szBuffer );

	//���ý���
	m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);
	BYTE cbAction = WIK_CHI_HU;
	if (pChiHu->wChiHuUser == pChiHu->wProviderUser) cbAction |= WIK_ZI_MO;
	m_GameClientView.SetUserAction(wViewId,cbAction);
	m_GameClientView.SetStatusFlag(false,false);

	//���½���
	m_GameClientView.InvalidGameView(0,0,0,0);

	//��������
	PlayActionSound(pChiHu->wChiHuUser,WIK_CHI_HU);

	return true;
}

//���ų�������
void CGameClientEngine::PlayCardSound(WORD wChairID, BYTE cbCardData)
{
	if(m_GameLogic.IsValidCard(cbCardData) == false)
	{
		return;
	}
	if(wChairID < 0 || wChairID > 3)
	{
		return;
	}

	//�ж��Ա�
	IClientUserItem * pUserData = GetTableUserItem(wChairID);
	if(pUserData == 0)
	{
		assert(0 && "�ò��������Ϣ");
		return;
	}
	bool bBoy = (pUserData->GetGender() == GENDER_FEMALE ? false : true);
	BYTE cbType= (cbCardData & MASK_COLOR);
	BYTE cbValue= (cbCardData & MASK_VALUE);
	CString strSoundName;
	switch(cbType)
	{
	case 0X30:	//��
		{
			switch(cbValue) 
			{
			case 1:
				{
					strSoundName = _T("F_1");
					break;
				}
			case 2:
				{
					strSoundName = _T("F_2");
					break;
				}
			case 3:
				{
					strSoundName = _T("F_3");
					break;
				}
			case 4:
				{
					strSoundName = _T("F_4");
					break;
				}
			case 5:
				{
					strSoundName = _T("F_5");
					break;
				}
			case 6:
				{
					strSoundName = _T("F_6");
					break;
				}
			case 7:
				{
					strSoundName = _T("F_7");
					break;
				}
			default:
				{
					strSoundName=_T("BU_HUA");
				}

			}
			break;
		}		
	case 0X20:	//Ͳ
		{
			strSoundName.Format(_T("T_%d"), cbValue);
			break;
		}

	case 0X10:	//��
		{
			strSoundName.Format(_T("S_%d"), cbValue);
			break;
		}
	case 0X00:	//��
		{
			strSoundName.Format(_T("W_%d"), cbValue);
			break;
		}
	}	

	if(bBoy)
	{
		strSoundName = _T("BOY_") +strSoundName;
	}
	else
	{
		strSoundName = _T("GIRL_") + strSoundName;
	}
	m_DirectSound.Play();
	PlayGameSound(AfxGetInstanceHandle(), strSoundName);
}

//��������
void CGameClientEngine::PlayActionSound(WORD wChairID,BYTE cbAction)
{
	//�ж��Ա�
	IClientUserItem * pUserData = GetTableUserItem(wChairID);
	if(pUserData == 0)
	{
		assert(0 && "�ò��������Ϣ");
		return;
	}
	if(wChairID < 0 || wChairID > 3)
	{
		return;
	}
	bool bBoy = (pUserData->GetGender() == GENDER_FEMALE ? false : true);

	switch (cbAction)
	{
	case WIK_NULL:		//ȡ��
		{
			if(bBoy)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("BOY_OUT_CARD"));
			else
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GIRL_OUT_CARD"));
			break;
		}
	case WIK_LEFT:
	case WIK_CENTER:
	case WIK_RIGHT:		//����
		{
			if(bBoy)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("BOY_CHI"));
			else
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GIRL_CHI"));		
			break;
		}
	case WIK_PENG:		//����
		{
			if(bBoy)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("BOY_PENG"));
			else
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GIRL_PENG"));	
			break;
		}
	case WIK_GANG:		//����
		{
			if(bBoy)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("BOY_GANG"));
			else
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GIRL_GANG"));		
			break;
		}
	case WIK_CHI_HU:	//�Ժ�
		{

			if(bBoy)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("BOY_CHI_HU"));
			else
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GIRL_CHI_HU"));
			break;
		}
	}

	return;
}
//�����ж�
bool CGameClientEngine::VerdictOutCard(BYTE cbCardData)
{
	return true;
}

//�۳��˿�
void CGameClientEngine::DeductionTableCard(bool bHeadCard)
{
	if (bHeadCard==true)
	{
		//�л�����
		BYTE cbHeapCount=m_cbHeapCardInfo[m_wHeapHand][0]+m_cbHeapCardInfo[m_wHeapHand][1];
		if (cbHeapCount==HEAP_FULL_COUNT)
			m_wHeapHand=(m_wHeapHand+1)%CountArray(m_cbHeapCardInfo);

		//�����˿�
		m_cbLeftCardCount--;
		m_cbHeapCardInfo[m_wHeapHand][0]++;

		//�����˿�
		WORD wHeapViewID=SwitchViewChairID(m_wHeapHand);
		WORD wMinusHeadCount=m_cbHeapCardInfo[m_wHeapHand][0];
		WORD wMinusLastCount=m_cbHeapCardInfo[m_wHeapHand][1];
		m_GameClientView.m_HeapCard[wHeapViewID].SetCardData(wMinusHeadCount,wMinusLastCount,HEAP_FULL_COUNT);
	}
	else
	{
		//�л�����
		BYTE cbHeapCount=m_cbHeapCardInfo[m_wHeapTail][0]+m_cbHeapCardInfo[m_wHeapTail][1];
		if (cbHeapCount==HEAP_FULL_COUNT)
			m_wHeapTail=(m_wHeapTail+3)%CountArray(m_cbHeapCardInfo);

		//�����˿�
		m_cbLeftCardCount--;
		m_cbHeapCardInfo[m_wHeapTail][1]++;

		//�����˿�
		WORD wHeapViewID=SwitchViewChairID(m_wHeapTail);
		WORD wMinusHeadCount=m_cbHeapCardInfo[m_wHeapTail][0];
		WORD wMinusLastCount=m_cbHeapCardInfo[m_wHeapTail][1];
		m_GameClientView.m_HeapCard[wHeapViewID].SetCardData(wMinusHeadCount,wMinusLastCount,HEAP_FULL_COUNT);
	}

	return;
}

//��ʾ����
bool CGameClientEngine::ShowOperateControl(BYTE cbUserAction, BYTE cbActionCard)
{
	//��������
	tagGangCardResult GangCardResult;
	ZeroMemory(&GangCardResult,sizeof(GangCardResult));

	//�����ж�
	if ((cbUserAction&WIK_GANG)!=0)
	{
		//�������
		if (cbActionCard!=0)
		{
			GangCardResult.cbCardCount=1;
			GangCardResult.cbCardData[0]=cbActionCard;
		}

		//�Լ�����
		if (cbActionCard==0)
		{
			WORD wMeChairID=GetMeChairID();
			m_GameLogic.AnalyseGangCard(m_cbCardIndex,m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID],GangCardResult);
		}
	}

	//��ʾ����
	if (IsLookonMode()==false)
		m_GameClientView.m_ControlWnd.SetControlInfo(cbActionCard,cbUserAction,GangCardResult);

	return true;
}

//��ʼ��ť
LRESULT CGameClientEngine::OnStart(WPARAM wParam, LPARAM lParam)
{
	////TEST+
	//BYTE cbTempDebug[56] = 
	//{
	//	0x04,0x04,0x04,0x05,0x06,0x07,0x09,0x01,0x02,0x03,0x01,0x02,0x03,
	//	0x04,0x04,0x04,0x05,0x06,0x07,0x09,0x01,0x02,0x03,0x01,0x02,0x03,
	//	0x04,0x04,0x04,0x05,0x06,0x07,0x09,0x01,0x02,0x03,0x01,0x02,0x03,
	//	0x04,0x04,0x04,0x05,0x06,0x07,0x09,0x01,0x02,0x03,0x01,0x02,0x03,
	//};

	//m_GameClientView.m_UserCard[0].SetCardData(10,true);
	//m_GameClientView.m_UserCard[1].SetCardData(10,true);
	//m_GameClientView.m_UserCard[2].SetCardData(10,true);
	//m_GameClientView.m_HandCardControl.SetDisplayItem(true);
	//m_GameClientView.m_HandCardControl.SetPositively(true);
	//m_GameClientView.m_HandCardControl.SetCardData(cbTempDebug, 10, 0x01);

	//m_GameClientView.m_btCallCard1.ShowWindow(true);
	//m_GameClientView.m_btCallCard2.ShowWindow(true);
	//m_GameClientView.m_btCallCard3.ShowWindow(true);

	//WORD wMeChairID = 2;
	//BYTE cbCardWeave[4] = {0x01,0x01,0x01,0x01};

	////�˿�����
	//for (WORD i=0;i<GAME_PLAYER;i++)
	//{
	//	//m_GameClientView.m_TableCard[i].SetCardData(cbTempDebug,10);
	//	//m_GameClientView.m_DiscardCard[i].SetCardData(cbTempDebug,20);
	//	m_GameClientView.m_WeaveCard[i][0].SetCardData(cbCardWeave,4);
	//	m_GameClientView.m_WeaveCard[i][1].SetCardData(NULL,0);
	//	m_GameClientView.m_WeaveCard[i][2].SetCardData(NULL,0);
	//	m_GameClientView.m_WeaveCard[i][3].SetCardData(NULL,0);
	//	m_GameClientView.m_WeaveCard[i][0].SetGameEnd(i==wMeChairID);
	//	m_GameClientView.m_WeaveCard[i][1].SetGameEnd(i==wMeChairID);
	//	m_GameClientView.m_WeaveCard[i][2].SetGameEnd(i==wMeChairID);
	//	m_GameClientView.m_WeaveCard[i][3].SetGameEnd(i==wMeChairID);
	//	m_GameClientView.m_WeaveCard[i][0].SetDisplayItem(false);
	//	m_GameClientView.m_WeaveCard[i][1].SetDisplayItem(true);
	//	m_GameClientView.m_WeaveCard[i][2].SetDisplayItem(true);
	//	m_GameClientView.m_WeaveCard[i][3].SetDisplayItem(true);
	//}

	////�����˿�
	//for (WORD i=0;i<GAME_PLAYER;i++)
	//{
	//	m_cbHeapCardInfo[i][0]=0;
	//	m_cbHeapCardInfo[i][1]=0;
	//	m_GameClientView.m_HeapCard[i].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);
	//}

	//////��������
	////BYTE cbActionCard = 0x01;
	////BYTE cbActionMask = WIK_PENG;
	////tagGangCardResult GangCardResult;
	////ZeroMemory(&GangCardResult,sizeof(GangCardResult));
	////GangCardResult.cbCardCount=1;
	////GangCardResult.cbCardData[0]=cbActionCard;
	////m_GameClientView.m_ControlWnd.SetControlInfo(cbActionCard,cbActionMask,GangCardResult);
	////m_GameClientView.ShowGameScoreInfo();
	//return 0L;
	////TEST+

	//��������
	KillGameClock(IDI_START_GAME);
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);

	//���ý���
	m_GameClientView.SetDiscUser(INVALID_CHAIR);
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.SetBankerUser(INVALID_CHAIR);
	m_GameClientView.SetUserAction(INVALID_CHAIR,0);
	m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);
	m_GameClientView.SetWinOrder(INVALID_CHAIR,0);
	m_GameClientView.SetVoidShowing(INVALID_CHAIR,0);

	//�˿�����
	m_GameClientView.m_UserCard[0].SetCardData(0,false);
	m_GameClientView.m_UserCard[1].SetCardData(0,false);
	m_GameClientView.m_UserCard[2].SetCardData(0,false);
	m_GameClientView.m_HandCardControl.SetCardData(NULL,0,0);

	//�ɼ�����
	for (WORD i=0; i<GAME_PLAYER; i++)
	{
		m_GameClientView.m_ScoreControl[i].CloseControl();
	}

	//�˿�����
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_TableCard[i].SetCardData(NULL,0);
		m_GameClientView.m_DiscardCard[i].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][0].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][1].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][2].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][3].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][0].SetGameEnd(false);
		m_GameClientView.m_WeaveCard[i][1].SetGameEnd(false);
		m_GameClientView.m_WeaveCard[i][2].SetGameEnd(false);
		m_GameClientView.m_WeaveCard[i][3].SetGameEnd(false);
		m_GameClientView.m_WeaveCard[i][0].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][1].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][2].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][3].SetDisplayItem(true);
	}

	//�����˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_cbHeapCardInfo[i][0]=0;
		m_cbHeapCardInfo[i][1]=0;
		m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);
	}

	//��Ϸ����
	m_wCurrentUser=INVALID_CHAIR;

	//������Ϣ
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//����˿�
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//�����˿�
	m_wHeapHand=0;
	m_wHeapTail=0;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

	//�˿˱���
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	//������Ϣ
	SendUserReady(NULL,0);

	return 0;
}

//��ȱ��Ϣ
LRESULT CGameClientEngine::OnCallCard(WPARAM wParam, LPARAM lParam)
{
	////ɾ��ʱ��
	//KillGameClock(IDI_CALL_CARD);

	//���ý���
	m_GameClientView.ShowSelectTip(false);
	m_GameClientView.ShowWaitChooseTip(true);

	//���ý���
	m_GameClientView.m_btCallCard1.ShowWindow(false);
	m_GameClientView.m_btCallCard2.ShowWindow(false);
	m_GameClientView.m_btCallCard3.ShowWindow(false);

	//��������
	CMD_C_CallCard CallCard;
	CallCard.cbCallCard = (BYTE)wParam;
	SendSocketData(SUB_C_CALL_CARD, &CallCard, sizeof(CallCard));

	return 0;
}

//���Ʋ���
LRESULT CGameClientEngine::OnOutCard(WPARAM wParam, LPARAM lParam)
{
	//�����ж�
	if ((IsLookonMode()==true)||(m_wCurrentUser!=GetMeChairID()))
		return 0;

	//�����ж�
	if (VerdictOutCard((BYTE)wParam)==false)
	{
		if ( m_pIStringMessage )
			m_pIStringMessage->InsertSystemString(TEXT("�����Ʋ�������Ϸ����!"));
		return 0;
	}

	//���ñ���
	m_cbUserAction = 0;
	m_wCurrentUser=INVALID_CHAIR;
	BYTE cbOutCardData=(BYTE)wParam;
	m_GameLogic.RemoveCard(m_cbCardIndex,cbOutCardData);

	//�����˿�
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData);
	m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,0);

	//���ý���
	KillGameClock(IDI_OPERATE_CARD);
	m_GameClientView.InvalidGameView(0,0,0,0);
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.SetUserAction(INVALID_CHAIR,0);
	m_GameClientView.SetOutCardInfo(2,cbOutCardData);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);

	//��������
	CMD_C_OutCard OutCard;
	OutCard.cbCardData=cbOutCardData;
	SendSocketData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard));

	//��������
	PlayCardSound(GetMeChairID(),cbOutCardData);

	return 0;
}

//�˿˲���
LRESULT CGameClientEngine::OnCardOperate(WPARAM wParam, LPARAM lParam)
{
	//��������
	BYTE cbOperateCode=(BYTE)(wParam);
	BYTE cbOperateCard=(BYTE)(LOWORD(lParam));

	//״̬�ж�
	if ((m_wCurrentUser==GetMeChairID())&&(cbOperateCode==WIK_NULL))
	{
		m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
		return 0;
	}

	//ɾ��ʱ��
	KillGameClock(IDI_OPERATE_CARD);

	//��������
	m_cbUserAction = 0;
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);

	//��������
	CMD_C_OperateCard OperateCard;
	OperateCard.cbOperateCode=cbOperateCode;
	OperateCard.cbOperateCard=cbOperateCard;
	SendSocketData(SUB_C_OPERATE_CARD,&OperateCard,sizeof(OperateCard));

	return 0;
}
//�Ϲܿ���
LRESULT CGameClientEngine::OnStusteeControl(WPARAM wParam, LPARAM lParam)
{
	//���ñ���
	m_wTimeOutCount=0;

	//����״̬
	if (m_bStustee==true)
	{
		m_bStustee=false;
		m_GameClientView.m_btStustee.ShowWindow(true);
		m_GameClientView.m_btStustee.EnableWindow(true);
		m_GameClientView.m_btCancelStustee.ShowWindow(false);

		if ( m_pIStringMessage )m_pIStringMessage->InsertSystemString(_T("��ȡ�����йܹ���."));

		CMD_C_Trustee Trustee;
		Trustee.bTrustee = false;
		SendSocketData(SUB_C_TRUSTEE,&Trustee,sizeof(Trustee));
	}
	else
	{
		m_bStustee=true;
		m_GameClientView.m_btStustee.ShowWindow(false);
		m_GameClientView.m_btCancelStustee.ShowWindow(true);
		m_GameClientView.m_btCancelStustee.EnableWindow(true);

		if ( m_pIStringMessage ) m_pIStringMessage->InsertSystemString(_T("��ѡ�����йܹ���."));

		CMD_C_Trustee Trustee;
		Trustee.bTrustee = true;
		SendSocketData(SUB_C_TRUSTEE,&Trustee,sizeof(Trustee));
	}

	return 0;
}

//�ڶ���ҡɫ����Ϣ
LRESULT CGameClientEngine::OnSiceTwo(WPARAM wParam, LPARAM lParam)
{
	//��������
	PlayGameSound(AfxGetInstanceHandle(),TEXT("DRAW_SICE"));

	//���ý���
	m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

	return 0;
}

//ҡɫ�ӽ�����Ϣ
LRESULT CGameClientEngine::OnSiceFinish(WPARAM wParam, LPARAM lParam)
{
	//��ʼʱ��
	DWORD dwStartTime = GetTickCount();

	//���ý���
	m_GameClientView.ShowSelectTip(true);
	m_GameClientView.ShowWaitChooseTip(false);
	m_GameClientView.m_DrawSiceWnd.ShowWindow(SW_HIDE);
	bool bPlayerMode = (IsLookonMode()==false);

	//�˿�����
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//��������
		WORD wViewChairID=SwitchViewChairID(i);

		//��Ͻ���
		m_GameClientView.m_WeaveCard[i][0].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][1].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][2].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][3].SetDisplayItem(true);
		m_GameClientView.m_HeapCard[wViewChairID].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);

		//�Թ۽���
		if (bPlayerMode==false)
		{
			m_GameClientView.m_TableCard[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_DiscardCard[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][0].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][1].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][2].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][3].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl.SetDisplayItem(IsAllowLookon());
		}
		
		if( GetTableUserItem(i) == NULL ) continue;

		//�û��˿�
		if (wViewChairID != 2)
		{
			WORD wIndex = (wViewChairID>=3) ? 2 : wViewChairID;
			m_GameClientView.m_UserCard[wIndex].SetCardData(MAX_COUNT-1, (i==m_wBankerUser));
		}
		else
		{
			BYTE cbCardData[MAX_COUNT];
			m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData);
			BYTE cbBankerCard = (i == m_wBankerUser ? cbCardData[MAX_COUNT-1] : 0);
			m_GameClientView.m_HandCardControl.SetCardData(cbCardData, MAX_COUNT-1, cbBankerCard);
		}
	}

	//��ʾ��ť
	if ((IsLookonMode()==false))
	{
		//���ư�ť
		m_GameClientView.m_btCallCard1.EnableWindow(true);
		m_GameClientView.m_btCallCard2.EnableWindow(true);
		m_GameClientView.m_btCallCard3.EnableWindow(true);

		//��ʾ��ť
		m_GameClientView.m_btCallCard1.ShowWindow(true);
		m_GameClientView.m_btCallCard2.ShowWindow(true);
		m_GameClientView.m_btCallCard3.ShowWindow(true);
	}

	//���½���
	m_GameClientView.InvalidGameView(0,0,0,0);

	//����ʱ��
	SetGameClock(GetMeChairID(), IDI_CALL_CARD, TIME_CALL_CARD);

	//ʱ����
	DWORD dwEndTime = GetTickCount();
	DWORD dwDiffenTime = dwEndTime - dwStartTime;

	//д���ռ�
	CString strValue;
	strValue.Format(TEXT("������������ʼʱ��=%d������ʱ��=%d��ʱ����=%d��"), dwStartTime, dwEndTime, dwDiffenTime);
	NcaTextOut(strValue);

	return 0;
}

//�η�����
bool CGameClientEngine::OnSubGangScore( const void *pBuffer, WORD wDataSize )
{
	//Ч����Ϣ
	ASSERT(wDataSize==sizeof(CMD_S_GangScore));
	if (wDataSize!=sizeof(CMD_S_GangScore)) return false;

	//��Ϣ����
	CMD_S_GangScore * pGangScore=(CMD_S_GangScore *)pBuffer;

	//������Ϣ
	CString strTemp ,strEnd;
	strEnd.Format( TEXT("\n----------%s----------\n"),pGangScore->cbXiaYu==TRUE?TEXT("����"):TEXT("�η�") );
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		strTemp.Format(TEXT("%s: %I64d\n"),m_szAccounts[i],pGangScore->lGangScore[i]);
		strEnd += strTemp;
	}
	strTemp = TEXT("------------------------\n");
	strEnd += strTemp;

	//��Ϣ����
	if ( m_pIStringMessage )
		m_pIStringMessage->InsertSystemString(strEnd);

	//���ý���
	LONGLONG lGangScore[GAME_PLAYER];
	for( WORD i = 0; i < GAME_PLAYER; i++ )
		lGangScore[SwitchViewChairID(i)] = pGangScore->lGangScore[i];
	m_GameClientView.SetGangScore( SwitchViewChairID(pGangScore->wChairId),pGangScore->cbXiaYu,lGangScore );

	return true;
}

//////////////////////////////////////////////////////////////////////////
