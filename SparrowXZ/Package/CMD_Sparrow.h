#ifndef CMD_SPARROW_HEAD_FILE
#define CMD_SPARROW_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID						302									//��Ϸ I D
#define GAME_NAME					TEXT("Ѫս�齫")					//��Ϸ����

//�������
#define GAME_PLAYER					4									//��Ϸ����
#define VERSION_SERVER				PROCESS_VERSION(6,0,3)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)				//����汾

//��Ϸ״̬
#define GS_MJ_FREE					GAME_STATUS_FREE					//����״̬
#define GS_MJ_CALL					GAME_STATUS_PLAY					//ѡȱ״̬
#define GS_MJ_PLAY					(GAME_STATUS_PLAY+1)				//��Ϸ״̬

//��������
#define MAX_WEAVE					4									//������
#define MAX_INDEX					34									//�������
#define MAX_COUNT					14									//�����Ŀ
#define MAX_REPERTORY				108									//�����

//�˿˶���
#define HEAP_FULL_COUNT				26									//����ȫ��

#define MAX_RIGHT_COUNT				1									//���ȨλDWORD����			

//////////////////////////////////////////////////////////////////////////

//�������
struct CMD_WeaveItem
{
	BYTE							cbWeaveKind;						//�������
	BYTE							cbCenterCard;						//�����˿�
	BYTE							cbPublicCard;						//������־
	WORD							wProvideUser;						//��Ӧ�û�
};

//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_START			100									//��Ϸ��ʼ
#define SUB_S_CALL_CARD				101									//ȱ������
#define SUB_S_BANKER_INFO			102									//ׯ����Ϣ
#define SUB_S_OUT_CARD				103									//��������
#define SUB_S_SEND_CARD				104									//�����˿�
#define SUB_S_OPERATE_NOTIFY		105									//������ʾ
#define SUB_S_OPERATE_RESULT		106									//��������
#define SUB_S_GAME_END				107									//��Ϸ����
#define SUB_S_TRUSTEE				108									//�û��й�
#define SUB_S_CHI_HU				109									//�û��Ժ�
#define SUB_S_GANG_SCORE			110									//���Ƶ÷�
#define SUB_S_WAIT_OPERATE			111									//�ȴ�����

//��Ϸ״̬
struct CMD_S_StatusFree
{
	LONGLONG						lCellScore;							//�������
	WORD							wBankerUser;						//ׯ���û�
	bool							bTrustee[GAME_PLAYER];				//�Ƿ��й�
	bool							bCheatMode;							//�����ױ�ʶ

	//��ʷ����
	LONGLONG						lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG						lCollectScore[GAME_PLAYER];			//������Ϣ
};

//��Ϸ״̬
struct CMD_S_StatusCall
{
	//��Ϸ����
	LONGLONG						lCellScore;							//�������
	WORD							wBankerUser;						//ׯ���û�
	bool							bTrustee[GAME_PLAYER];				//�Ƿ��й�

	//ȱ����Ϣ
	bool							bCallCard[GAME_PLAYER];				//ȱ��״̬
	BYTE							cbCallCard[GAME_PLAYER];			//ȱ������

	//�˿�����
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbCardData[MAX_COUNT];				//�˿��б�
	BYTE							cbSendCardData;						//�����˿�

	//������Ϣ
	WORD							wHeapHand;							//����ͷ��
	WORD							wHeapTail;							//����β��
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];		//������Ϣ

	//��ʷ����
	LONGLONG						lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG						lCollectScore[GAME_PLAYER];			//������Ϣ
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//��Ϸ����
	LONGLONG						lCellScore;							//��Ԫ����
	WORD							wBankerUser;						//ׯ���û�
	WORD							wCurrentUser;						//��ǰ�û�

	//״̬����
	BYTE							cbActionCard;						//�����˿�
	BYTE							cbActionMask;						//��������
	BYTE							cbLeftCardCount;					//ʣ����Ŀ
	bool							bTrustee[GAME_PLAYER];				//�Ƿ��й�
	WORD							wWinOrder[GAME_PLAYER];				//����˳��
	BYTE							cbCallCard[GAME_PLAYER];			//ȱ����Ϣ

	//������Ϣ
	WORD							wOutCardUser;						//�����û�
	BYTE							cbOutCardData;						//�����˿�
	BYTE							cbDiscardCount[GAME_PLAYER];		//������Ŀ
	BYTE							cbDiscardCard[GAME_PLAYER][60];		//������¼

	//�˿�����
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbCardData[MAX_COUNT];				//�˿��б�
	BYTE							cbSendCardData;						//�����˿�

	//����˿�
	BYTE							cbWeaveCount[GAME_PLAYER];			//�����Ŀ
	CMD_WeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];//����˿�

	//������Ϣ
	WORD							wHeapHand;							//����ͷ��
	WORD							wHeapTail;							//����β��
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];		//������Ϣ

	//��ʷ����
	LONGLONG						lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG						lCollectScore[GAME_PLAYER];			//������Ϣ
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	LONG							lSiceCount;							//���ӵ���
	WORD							wBankerUser;						//ׯ���û�
	WORD							wCurrentUser;						//��ǰ�û�
	BYTE							cbUserAction;						//�û�����
	BYTE							cbCardData[MAX_COUNT*GAME_PLAYER];	//�˿��б�
	WORD							wHeapHand;							//������ͷ
	WORD							wHeapTail;							//������β
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];		//������Ϣ
	BYTE							cbLeftCardCount;					//ʣ���˿�
};

//ȱ������
struct CMD_S_CallCard
{
	WORD							wCallUser;							//�����û�
};

//ׯ����Ϣ
struct CMD_S_BankerInfo
{
	WORD				 			wBankerUser;						//ׯ�����
	WORD				 			wCurrentUser;						//��ǰ���
	BYTE							cbCallCard[GAME_PLAYER];			//ȱ������
};

//��������
struct CMD_S_OutCard
{
	WORD							wOutCardUser;						//�����û�
	BYTE							cbOutCardData;						//�����˿�
};

//�����˿�
struct CMD_S_SendCard
{
	BYTE							cbCardData;							//�˿�����
	BYTE							cbActionMask;						//��������
	WORD							wCurrentUser;						//��ǰ�û�
	bool							bTail;								//ĩβ����
};


//������ʾ
struct CMD_S_OperateNotify
{
	WORD							wResumeUser;						//��ԭ�û�
	BYTE							cbActionMask;						//��������
	BYTE							cbActionCard;						//�����˿�
};

//��������
struct CMD_S_OperateResult
{
	WORD							wOperateUser;						//�����û�
	WORD							wProvideUser;						//��Ӧ�û�
	BYTE							cbOperateCode;						//��������
	BYTE							cbOperateCard;						//�����˿�
};

//��Ϸ����
struct CMD_S_GameEnd
{
	//�˿˱���
	BYTE							cbCardCount[GAME_PLAYER];			//�˿���Ŀ
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];	//�˿�����

	//������Ϣ
	WORD							wProvideUser[GAME_PLAYER];			//��Ӧ�û�
	DWORD							dwChiHuRight[GAME_PLAYER];			//��������

	//������Ϣ
	LONGLONG						lGameScore[GAME_PLAYER];			//��Ϸ����
	LONG							lGameTax[GAME_PLAYER];				//��Ϸ˰��

	WORD							wWinOrder[GAME_PLAYER];				//��������

	LONGLONG						lGangScore[GAME_PLAYER];			//��ϸ�÷�
	BYTE							cbGenCount[GAME_PLAYER];			//��������
	WORD							wLostFanShu[GAME_PLAYER][GAME_PLAYER];//��ʧ����
	LONGLONG						lHuaZhuScore[GAME_PLAYER][GAME_PLAYER];//����÷�
	LONGLONG						lChaJiaoScore[GAME_PLAYER][GAME_PLAYER];//���յ÷�
	LONGLONG						lLostHuaZhuScore[GAME_PLAYER][GAME_PLAYER];//����÷�
	LONGLONG						lLostChaJiaoScore[GAME_PLAYER][GAME_PLAYER];//���յ÷�
	WORD							wLeftUser;							//�����û�
};

//�û��й�
struct CMD_S_Trustee
{
	bool							bTrustee;							//�Ƿ��й�
	WORD							wChairID;							//�й��û�
};

//�Ժ���Ϣ
struct CMD_S_ChiHu
{
	WORD							wChiHuUser;							//�Ժ��û�
	WORD							wProviderUser;						//�ṩ�û�
	BYTE							cbChiHuCard;						//��������
	BYTE							cbCardCount;						//�˿���Ŀ
	LONGLONG						lGameScore;							//��Ϸ����
	BYTE							cbWinOrder;							//����˳��
};

//���Ʒ���
struct CMD_S_GangScore
{
	WORD							wChairId;							//�����û�
	BYTE							cbXiaYu;							//�η�����
	LONGLONG						lGangScore[GAME_PLAYER];			//���Ʒ���
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ

#define SUB_C_CALL_CARD				1									//�û�ȱ��
#define SUB_C_OUT_CARD				2									//��������
#define SUB_C_OPERATE_CARD			3									//�����˿�
#define SUB_C_TRUSTEE				4									//�û��й�

//�û�ȱ��
struct CMD_C_CallCard
{
	BYTE							cbCallCard;							//ȱ������
};

//��������
struct CMD_C_OutCard
{
	BYTE							cbCardData;							//�˿�����
};

//��������
struct CMD_C_OperateCard
{
	BYTE							cbOperateCode;						//��������
	BYTE							cbOperateCard;						//�����˿�
};

//�û��й�
struct CMD_C_Trustee
{
	bool							bTrustee;							//�Ƿ��й�	
};
//////////////////////////////////////////////////////////////////////////

#endif
