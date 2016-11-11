#include "StdAfx.h"
#include "servercontrolitemsink.h"

//////////////////////////////////////////////////////////////////////////

//
CServerControlItemSink::CServerControlItemSink(void)
{
}

CServerControlItemSink::~CServerControlItemSink( void )
{

}

//���ؿ�������
bool __cdecl CServerControlItemSink::ControlResult(BYTE cbControlCardData[GAME_PLAYER][MAX_COUNT], ROOMUSERCONTROL Keyroomusercontrol)
{
	ASSERT(Keyroomusercontrol.roomUserInfo.wChairID < GAME_PLAYER);

	//��������
	BYTE bCardData[MAX_COUNT];
	BYTE bHandCardData[GAME_PLAYER][MAX_COUNT];
	ZeroMemory(bCardData, sizeof(bCardData));
	ZeroMemory(bHandCardData, sizeof(bHandCardData));

	//������
	BYTE bMaxUser = INVALID_BYTE;
	//��С���
	BYTE bMinUser = INVALID_BYTE;
	
	//����ʤ��
	if (Keyroomusercontrol.userControl.control_type == CONTINUE_WIN)
	{
		for(BYTE i=0;i<GAME_PLAYER;i++)
		{
			CopyMemory(bHandCardData[i], cbControlCardData[i], sizeof(BYTE)*MAX_COUNT);

			if(INVALID_BYTE == bMaxUser)
			{
				if(cbControlCardData[i][0] != 0)
				{
					bMaxUser = i;
					CopyMemory(bCardData,cbControlCardData[i],sizeof(bCardData));
				}
			}
		}

		//�������
		BYTE i = 0;
		for(i=bMaxUser+1;i<GAME_PLAYER;i++)
		{
			if(cbControlCardData[i][0] == 0)continue;

			bool bFistOx=m_GameLogic.GetOxCard(bCardData,MAX_COUNT);
			bool bNextOx=m_GameLogic.GetOxCard(cbControlCardData[i],MAX_COUNT);

			if(m_GameLogic.CompareCard(bCardData,cbControlCardData[i],MAX_COUNT,bFistOx,bNextOx) == false)
			{
				CopyMemory(bCardData,cbControlCardData[i],sizeof(bCardData));
				bMaxUser=i;
			}
		}

		CopyMemory(cbControlCardData[Keyroomusercontrol.roomUserInfo.wChairID], bCardData, sizeof(bCardData));

		if(Keyroomusercontrol.roomUserInfo.wChairID != bMaxUser)
		{
			CopyMemory(cbControlCardData[bMaxUser], bHandCardData[Keyroomusercontrol.roomUserInfo.wChairID], sizeof(bHandCardData[i]));
		}

		return true;
	}
	else if (Keyroomusercontrol.userControl.control_type == CONTINUE_LOST)
	{	
		for(BYTE i=0;i<GAME_PLAYER;i++)
		{
			CopyMemory(bHandCardData[i], cbControlCardData[i], sizeof(BYTE)*MAX_COUNT);

			if(INVALID_BYTE == bMinUser)
			{
				if(cbControlCardData[i][0] != 0)
				{
					bMinUser = i;
					CopyMemory(bCardData,cbControlCardData[i],sizeof(bCardData));
				}
			}
		}

		//��С����
		BYTE i = 0;
		for(i=bMinUser+1;i<GAME_PLAYER;i++)
		{
			if(cbControlCardData[i][0] == 0)continue;

			bool bFistOx=m_GameLogic.GetOxCard(cbControlCardData[i],MAX_COUNT);
			bool bNextOx=m_GameLogic.GetOxCard(bCardData,MAX_COUNT);

			if(m_GameLogic.CompareCard(cbControlCardData[i],bCardData,MAX_COUNT,bFistOx,bNextOx) == false)
			{
				CopyMemory(bCardData,cbControlCardData[i],sizeof(bCardData));
				bMinUser=i;
			}
		}

		CopyMemory(cbControlCardData[Keyroomusercontrol.roomUserInfo.wChairID], bCardData, sizeof(bCardData));

		if(Keyroomusercontrol.roomUserInfo.wChairID != bMinUser)
		{
			CopyMemory(cbControlCardData[bMinUser], bHandCardData[Keyroomusercontrol.roomUserInfo.wChairID], sizeof(bHandCardData[i]));
		}

		return true;
	}

	ASSERT(FALSE);

	return false;
}