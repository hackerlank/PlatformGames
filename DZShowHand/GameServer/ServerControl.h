#pragma once

//��Ϸ���ƻ���
class IServerControl
{
public:
	IServerControl(void){};
	virtual ~IServerControl(void){};

public:
	//����������
	virtual bool __cdecl ServerControl(BYTE cbHandCardData[GAME_PLAYER][2], BYTE cbCenterCardData[MAX_CENTERCOUNT],ITableFrame * pITableFrame) = NULL;
};
