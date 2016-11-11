#ifndef CMD_OX_HEAD_FILE
#define CMD_OX_HEAD_FILE

#pragma pack(push)  
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID							27									//��Ϸ I D
#define GAME_PLAYER						4									//��Ϸ����
#define GAME_NAME						TEXT("ţţ")						//��Ϸ����
#define MAX_COUNT						5									//�����Ŀ
#define MAX_JETTON_AREA					4									//��ע����
#define MAX_TIMES						5									//�������

#define VERSION_SERVER					PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT					PROCESS_VERSION(7,0,1)				//����汾

//��Ϸ״̬
#define GS_TK_FREE						GAME_STATUS_FREE					//�ȴ���ʼ
#define GS_TK_CALL						GAME_STATUS_PLAY					//��ׯ״̬
#define GS_TK_SCORE						GAME_STATUS_PLAY+1					//��ע״̬
#define GS_TK_PLAYING					GAME_STATUS_PLAY+2					//��Ϸ����

//������Ϣ
#define IDM_ADMIN_UPDATE_STORAGE		WM_USER+1001
#define IDM_ADMIN_MODIFY_STORAGE		WM_USER+1011
#define IDM_REQUEST_QUERY_USER			WM_USER+1012
#define IDM_USER_CONTROL				WM_USER+1013
#define IDM_REQUEST_UPDATE_ROOMINFO		WM_USER+1014
#define IDM_CLEAR_CURRENT_QUERYUSER		WM_USER+1015

//������¼
#define MAX_OPERATION_RECORD			20									//������¼����
#define RECORD_LENGTH					128									//ÿ����¼�ֳ�

//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_START				100									//��Ϸ��ʼ
#define SUB_S_ADD_SCORE					101									//��ע���
#define SUB_S_PLAYER_EXIT				102									//�û�ǿ��
#define SUB_S_SEND_CARD					103									//������Ϣ
#define SUB_S_GAME_END					104									//��Ϸ����
#define SUB_S_OPEN_CARD					105									//�û�̯��
#define SUB_S_CALL_BANKER				106									//�û���ׯ
#define SUB_S_ANDROID_BANKOPERATOR		108									//���������в���

#define SUB_S_ADMIN_STORAGE_INFO		112									//ˢ�¿��Ʒ����
#define SUB_S_REQUEST_QUERY_RESULT		113									//��ѯ�û����
#define SUB_S_USER_CONTROL				114									//�û�����
#define SUB_S_USER_CONTROL_COMPLETE		115									//�û��������
#define SUB_S_OPERATION_RECORD		    116									//������¼
#define SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT 117

//////////////////////////////////////////////////////////////////////////////////////

//�����˴��ȡ��
struct tagCustomAndroid
{
	SCORE									lRobotScoreMin;	
	SCORE									lRobotScoreMax;
	SCORE	                                lRobotBankGet; 
	SCORE									lRobotBankGetBanker; 
	SCORE									lRobotBankStoMul; 
};

//��������
typedef enum{CONTINUE_WIN, CONTINUE_LOST, CONTINUE_CANCEL}CONTROL_TYPE;

//���ƽ��      ���Ƴɹ� ������ʧ�� ������ȡ���ɹ� ������ȡ����Ч
typedef enum{CONTROL_SUCCEED, CONTROL_FAIL, CONTROL_CANCEL_SUCCEED, CONTROL_CANCEL_INVALID}CONTROL_RESULT;

//�û���Ϊ
typedef enum{USER_SITDOWN, USER_STANDUP, USER_OFFLINE, USER_RECONNECT}USERACTION;

//������Ϣ
typedef struct
{
	CONTROL_TYPE						control_type;					  //��������
	BYTE								cbControlCount;					  //���ƾ���
	bool							    bCancelControl;					  //ȡ����ʶ
}USERCONTROL;

//�����û���Ϣ
typedef struct
{
	WORD								wChairID;							//����ID
	WORD								wTableID;							//����ID
	DWORD								dwGameID;							//GAMEID
	bool								bAndroid;							//�����˱�ʶ
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE								cbUserStatus;						//�û�״̬
	BYTE								cbGameStatus;						//��Ϸ״̬
}ROOMUSERINFO;

//�����û�����
typedef struct
{
	ROOMUSERINFO						roomUserInfo;						//�����û���Ϣ
	USERCONTROL							userControl;						//�û�����
}ROOMUSERCONTROL;

//////////////////////////////////////////////////////////////////////////////////////

//��Ϸ״̬
struct CMD_S_StatusFree
{
	LONGLONG							lCellScore;							//��������
	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���

	//��ʷ����
	LONGLONG							lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG							lCollectScore[GAME_PLAYER];			//������Ϣ
	tagCustomAndroid					CustomAndroid;						//����������
};

//��Ϸ״̬
struct CMD_S_StatusCall
{
	WORD								wCallBanker;						//��ׯ�û�
	BYTE                                cbDynamicJoin;                      //��̬���� 
	BYTE                                cbPlayStatus[GAME_PLAYER];          //�û�״̬

	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���

	//��ʷ����
	LONGLONG							lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG							lCollectScore[GAME_PLAYER];			//������Ϣ
	tagCustomAndroid					CustomAndroid;						//����������
};

//��Ϸ״̬
struct CMD_S_StatusScore
{
	//��ע��Ϣ
	BYTE                                cbPlayStatus[GAME_PLAYER];          //�û�״̬
	BYTE                                cbDynamicJoin;                      //��̬����
	LONGLONG							lTurnMaxScore;						//�����ע
	LONGLONG							lTableScore[GAME_PLAYER];			//��ע��Ŀ
	WORD								wBankerUser;						//ׯ���û�

	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���

	//��ʷ����
	LONGLONG							lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG							lCollectScore[GAME_PLAYER];			//������Ϣ
	tagCustomAndroid					CustomAndroid;						//����������
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//״̬��Ϣ
	BYTE                                cbPlayStatus[GAME_PLAYER];          //�û�״̬
	BYTE                                cbDynamicJoin;                      //��̬����
	LONGLONG							lTurnMaxScore;						//�����ע
	LONGLONG							lTableScore[GAME_PLAYER];			//��ע��Ŀ
	WORD								wBankerUser;						//ׯ���û�

	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���

	//�˿���Ϣ
	BYTE								cbHandCardData[GAME_PLAYER][MAX_COUNT];//�����˿�
	BYTE								bOxCard[GAME_PLAYER];				//ţţ����

	//��ʷ����
	LONGLONG							lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG							lCollectScore[GAME_PLAYER];			//������Ϣ
	tagCustomAndroid					CustomAndroid;						//����������
};

//�û���ׯ
struct CMD_S_CallBanker
{
	WORD								wCallBanker;						//��ׯ�û�
	bool								bFirstTimes;						//�״ν�ׯ
	BYTE								cbPlayerStatus[GAME_PLAYER];		//���״̬
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	//��ע��Ϣ
	LONGLONG							lTurnMaxScore;						//�����ע
	WORD								wBankerUser;						//ׯ���û�
};

//�û���ע
struct CMD_S_AddScore
{
	WORD								wAddScoreUser;						//��ע�û�
	LONGLONG							lAddScoreCount;						//��ע��Ŀ
};

//��Ϸ����
struct CMD_S_GameEnd
{
	LONGLONG							lGameTax[GAME_PLAYER];				//��Ϸ˰��
	LONGLONG							lGameScore[GAME_PLAYER];			//��Ϸ�÷�
	BYTE								cbCardData[GAME_PLAYER];			//�û��˿�
	BYTE								cbDelayOverGame;
};

//�������ݰ�
struct CMD_S_SendCard
{
	BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];	//�û��˿�
};

//�������ݰ�
struct CMD_S_AllCard
{
	bool								bAICount[GAME_PLAYER];
	BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];	//�û��˿�
};

//�û��˳�
struct CMD_S_PlayerExit
{
	WORD								wPlayerID;							//�˳��û�
};

//�û�̯��
struct CMD_S_Open_Card
{
	WORD								wPlayerID;							//̯���û�
	BYTE								bOpen;								//̯�Ʊ�־
};

struct CMD_S_RequestQueryResult
{
	ROOMUSERINFO						userinfo;							//�û���Ϣ
	bool								bFind;								//�ҵ���ʶ
};

//�û�����
struct CMD_S_UserControl
{
	DWORD									dwGameID;							//GAMEID
	TCHAR									szNickName[LEN_NICKNAME];			//�û��ǳ�
	CONTROL_RESULT							controlResult;						//���ƽ��
	CONTROL_TYPE							controlType;						//��������
	BYTE									cbControlCount;						//���ƾ���
};

//�û�����
struct CMD_S_UserControlComplete
{
	DWORD									dwGameID;							//GAMEID
	TCHAR									szNickName[LEN_NICKNAME];			//�û��ǳ�
	CONTROL_TYPE							controlType;						//��������
	BYTE									cbRemainControlCount;				//ʣ����ƾ���
};

//���Ʒ���˿����Ϣ
struct CMD_S_ADMIN_STORAGE_INFO
{
	LONGLONG	lRoomStorageStart;						//������ʼ���
	LONGLONG	lRoomStorageCurrent;
	LONGLONG	lRoomStorageDeduct;
	LONGLONG	lMaxRoomStorage[2];
	WORD		wRoomStorageMul[2];
};

//������¼
struct CMD_S_Operation_Record
{
	TCHAR		szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];					//��¼���²�����20����¼
};

//������½��
struct CMD_S_RequestUpdateRoomInfo_Result
{
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���
	ROOMUSERINFO						currentqueryuserinfo;				//��ǰ��ѯ�û���Ϣ
	bool								bExistControl;						//��ѯ�û����ڿ��Ʊ�ʶ
	USERCONTROL							currentusercontrol;
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ
#define SUB_C_CALL_BANKER				1									//�û���ׯ
#define SUB_C_ADD_SCORE					2									//�û���ע
#define SUB_C_OPEN_CARD					3									//�û�̯��
#define SUB_C_STORAGE					6									//���¿��
#define	SUB_C_STORAGEMAXMUL				7									//��������
#define SUB_C_REQUEST_QUERY_USER		8									//�����ѯ�û�
#define SUB_C_USER_CONTROL				9									//�û�����

//�����������
#define SUB_C_REQUEST_UDPATE_ROOMINFO	10									//������·�����Ϣ
#define SUB_C_CLEAR_CURRENT_QUERYUSER	11

//�û���ׯ
struct CMD_C_CallBanker
{
	BYTE								bBanker;							//��ׯ��־
};

//�û���ע
struct CMD_C_AddScore
{
	LONGLONG							lScore;								//��ע��Ŀ
};

//�û�̯��
struct CMD_C_OxCard
{
	BYTE								bOX;								//ţţ��־
};

struct CMD_C_UpdateStorage
{
	LONGLONG						lRoomStorageCurrent;					//�����ֵ
	LONGLONG						lRoomStorageDeduct;						//�����ֵ
};

struct CMD_C_ModifyStorage
{
	LONGLONG						lMaxRoomStorage[2];							//�������
	WORD							wRoomStorageMul[2];							//Ӯ�ָ���
};

struct CMD_C_RequestQuery_User
{
	DWORD							dwGameID;								//��ѯ�û�GAMEID
	TCHAR							szNickName[LEN_NICKNAME];			    //��ѯ�û��ǳ�
};

//�û�����
struct CMD_C_UserControl
{
	DWORD									dwGameID;							//GAMEID
	TCHAR									szNickName[LEN_NICKNAME];			//�û��ǳ�
	USERCONTROL								userControlInfo;					//
};

#pragma pack(pop)

#endif
