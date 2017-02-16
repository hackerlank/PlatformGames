#include "StdAfx.h"
#include "ScoreControl.h"

//////////////////////////////////////////////////////////////////////////


//���캯��
CScoreControl::CScoreControl()
{
	//���ñ���
	m_ChiHuRight.SetEmpty();
	ZeroMemory(&m_ScoreInfo,sizeof(m_ScoreInfo));

	return;
}

//��������
CScoreControl::~CScoreControl()
{
}

//������Ϣ
VOID CScoreControl::OnWindowMovie()
{
	return;
}

//������Ϣ
VOID CScoreControl::OnWindowCreate(CD3DDevice * pD3DDevice)
{
	//��������
	m_FontScore.CreateFont(90,TEXT("����"),0L);

	//������Դ
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_PngNum.LoadImage(pD3DDevice, hResInstance, TEXT("NUM_ORDER"), TEXT("PNG"));
	m_ImageGameWin.LoadImage(pD3DDevice, hResInstance, TEXT("GAME_WIN"), TEXT("PNG"));
	m_ImageGameLost.LoadImage(pD3DDevice, hResInstance, TEXT("GAME_LOST"), TEXT("PNG"));

	return;
}

//������Ϣ
VOID CScoreControl::OnWindowDestory(CD3DDevice * pD3DDevice)
{
	//ɾ������
	m_FontScore.DeleteFont();

	//������Դ
	if (m_PngNum.IsNull()==false) m_PngNum.Destory();
	if (m_ImageGameWin.IsNull()==false) m_ImageGameWin.Destory();
	if (m_ImageGameLost.IsNull()==false) m_ImageGameLost.Destory();

	return;
}

//����¼�
VOID CScoreControl::OnEventMouse(UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos)
{
	return;
}

//��ť�¼�
VOID CScoreControl::OnEventButton(UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos)
{
	return;
}

//�滭����
VOID CScoreControl::OnEventDrawWindow(CD3DDevice * pD3DDevice, INT nXOriginPos, INT nYOriginPos)
{
	int nXPos = nXOriginPos;
	int nYPos = nYOriginPos;
	D3DCOLOR crColor=D3DCOLOR_XRGB(255, 245, 163);

	//�滭����
	if( m_ScoreInfo.lGameScore > 0 )
		m_ImageGameWin.DrawImage(pD3DDevice, nXPos, nYPos);
	else m_ImageGameLost.DrawImage(pD3DDevice, nXPos, nYPos);

	nXPos += 60;
	nYPos += 80;

	//��������
	if( m_ScoreInfo.cbWinOrder > 0 )
	{
		DrawNumberString(pD3DDevice, &m_PngNum, TEXT("0123456789"), m_ScoreInfo.cbWinOrder, nXOriginPos+20,nYOriginPos+100, DT_LEFT);
	}

	//������Ϣ
	TCHAR szBuffer[256] = TEXT("");
	if( !m_ChiHuRight.IsEmpty() )
	{
		if(m_ScoreInfo.wMeChairId == m_ScoreInfo.wProvideUser)
			lstrcpy(szBuffer, TEXT("����"));
		else 
		{
			lstrcpy(szBuffer, TEXT("Ӯ��"));
			TCHAR szName[10] = TEXT("");
			lstrcpyn(szName, m_ScoreInfo.szUserName[m_ScoreInfo.wProvideUser], sizeof(szName)/sizeof(TCHAR));
			lstrcat(szBuffer,szName);
			if (lstrlen(m_ScoreInfo.szUserName[m_ScoreInfo.wProvideUser]) > 8)
				lstrcat(szBuffer, TEXT("..."));
			lstrcat(szBuffer, TEXT(" ����"));
		}
		m_FontScore.DrawText(pD3DDevice, szBuffer, nXPos, nYPos, DT_LEFT|DT_BOTTOM, crColor);
		nYPos += 19;

		DWORD dwRight[] = {
			CHR_SHU_FAN,CHR_TIAN_HU,CHR_DI_HU,
			CHR_DA_DUI_ZI,CHR_QING_YI_SE,CHR_QI_XIAO_DUI,CHR_DAI_YAO,CHR_JIANG_DUI,CHR_QING_DUI,
			CHR_LONG_QI_DUI,CHR_QING_QI_DUI,CHR_QING_YAO_JIU,CHR_QING_LONG_QI_DUI,
			CHR_GANG_SHANG_PAO,CHR_GANG_KAI
		};
		LPCTSTR pszRight[] = {
			TEXT("�ط� 1�� "),TEXT("��� 6�� "),TEXT("�غ� 6�� "),
			TEXT("����� 2�� "),TEXT("��һɫ 3�� "),TEXT("��С�� 3�� "),TEXT("���� 3�� "),TEXT("���� 4�� "),
			TEXT("��� 4�� "),TEXT("���߶� 5�� "),TEXT("���߶� 5�� "),TEXT("���۾� 5�� "),TEXT("�����߶� 6�� "),
			TEXT("������ 1�� "),TEXT("���ϻ� 1�� ")
		};
		szBuffer[0] = 0;
		for( BYTE j = 0; j < CountArray(pszRight); j++ )
		{
			if( !(m_ChiHuRight&dwRight[j]).IsEmpty() )
				lstrcat( szBuffer,pszRight[j] );
		}
		//��
		if( m_ScoreInfo.cbGenCount > 0 )
		{
			TCHAR szGen[8];
			_sntprintf( szGen,CountArray(szGen),TEXT("�� %d��"),m_ScoreInfo.cbGenCount );
			lstrcat( szBuffer,szGen );
		}
		m_FontScore.DrawText(pD3DDevice, szBuffer,nXPos,nYPos,DT_LEFT|DT_BOTTOM ,crColor);
		nYPos += 19;
	}

	//������
	szBuffer[0] = 0;
	bool bFirstDraw = true;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (i == m_ScoreInfo.wMeChairId) continue;

		if (m_ScoreInfo.wLostFanShu[i] > 0)
		{
			if (bFirstDraw)
			{
				bFirstDraw = false;
				lstrcpy(szBuffer, TEXT("��� "));
			}
			else lstrcpy(szBuffer, TEXT("     "));

			TCHAR szName[10] = TEXT("");
			lstrcpyn(szName, m_ScoreInfo.szUserName[i], sizeof(szName)/sizeof(TCHAR));
			lstrcat(szBuffer, szName );
			if (lstrlen(m_ScoreInfo.szUserName[i]) > 5)
			{
				lstrcat(szBuffer, TEXT("..."));
			}
			_sntprintf(szBuffer, CountArray(szBuffer), TEXT("%s -%d"), szBuffer, m_ScoreInfo.wLostFanShu[i]);
			m_FontScore.DrawText(pD3DDevice, szBuffer, nXPos, nYPos, DT_LEFT|DT_BOTTOM, crColor);
			nYPos += 19;
		}
	}

	//�黨��Ӯ
	szBuffer[0] = 0;
	bFirstDraw = true;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (i == m_ScoreInfo.wMeChairId) continue;

		if (m_ScoreInfo.lLostHuaZhuScore[i] > 0)
		{
			if (bFirstDraw)
			{
				bFirstDraw = false;
				lstrcpy(szBuffer, TEXT("�黨��"));
				m_FontScore.DrawText(pD3DDevice, szBuffer, nXPos, nYPos, DT_LEFT|DT_BOTTOM, crColor);
				nYPos += 19;
				lstrcpy(szBuffer, TEXT("    Ӯ "));
			}
			else lstrcpy(szBuffer, TEXT("       "));

			TCHAR szName[10] = TEXT("");
			lstrcpyn(szName, m_ScoreInfo.szUserName[i], sizeof(szName)/sizeof(TCHAR));
			lstrcat(szBuffer, szName);
			if (lstrlen(m_ScoreInfo.szUserName[i]) > 5)
			{
				lstrcat(szBuffer, TEXT("..."));
			}
			_sntprintf(szBuffer, CountArray(szBuffer), TEXT("%s %d"), szBuffer, m_ScoreInfo.lLostHuaZhuScore[i]);
			m_FontScore.DrawText(pD3DDevice, szBuffer, nXPos, nYPos, DT_LEFT|DT_BOTTOM, crColor);
			nYPos += 19;
		}
	}

	//�������
	szBuffer[0] = 0;
	bFirstDraw = true;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (i == m_ScoreInfo.wMeChairId) continue;

		if (m_ScoreInfo.lHuaZhuScore[i] > 0)
		{
			if (bFirstDraw)
			{
				bFirstDraw = false;
				lstrcpy(szBuffer, TEXT("����"));
				m_FontScore.DrawText(pD3DDevice, szBuffer, nXPos, nYPos, DT_LEFT|DT_BOTTOM, crColor);
				nYPos += 19;
				lstrcpy(szBuffer, TEXT("��� "));
			}
			else lstrcpy(szBuffer, TEXT("     "));

			TCHAR szName[10] = TEXT("");
			lstrcpyn(szName, m_ScoreInfo.szUserName[i], sizeof(szName)/sizeof(TCHAR));
			lstrcat(szBuffer, szName);
			if (lstrlen(m_ScoreInfo.szUserName[i]) > 5)
			{
				lstrcat(szBuffer, TEXT("..."));
			}
			_sntprintf(szBuffer, CountArray(szBuffer), TEXT("%s -%d"), szBuffer, m_ScoreInfo.lHuaZhuScore[i]);
			m_FontScore.DrawText(pD3DDevice, szBuffer, nXPos, nYPos, DT_LEFT|DT_BOTTOM, crColor);
			nYPos += 19;
		}
	}

	//����Ӯ
	szBuffer[0] = 0;
	bFirstDraw = true;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (i == m_ScoreInfo.wMeChairId) continue;

		if (m_ScoreInfo.lLostChaJiaoScore[i] > 0)
		{
			if (bFirstDraw)
			{
				bFirstDraw = false;
				lstrcpy(szBuffer, TEXT("���У�"));
				m_FontScore.DrawText(pD3DDevice, szBuffer, nXPos, nYPos, DT_LEFT|DT_BOTTOM, crColor);
				nYPos += 19;
				lstrcpy(szBuffer, TEXT("    Ӯ "));
			}
			else lstrcpy(szBuffer, TEXT("       "));

			TCHAR szName[10] = TEXT("");
			lstrcpyn(szName, m_ScoreInfo.szUserName[i], sizeof(szName)/sizeof(TCHAR));
			lstrcat(szBuffer, szName);
			if (lstrlen(m_ScoreInfo.szUserName[i]) > 5)
			{
				lstrcat(szBuffer, TEXT("..."));
			}
			_sntprintf(szBuffer, CountArray(szBuffer), TEXT("%s %d"), szBuffer, m_ScoreInfo.lLostChaJiaoScore[i]);
			m_FontScore.DrawText(pD3DDevice, szBuffer, nXPos, nYPos, DT_LEFT|DT_BOTTOM, crColor);
			nYPos += 19;
		}
	}

	//������
	szBuffer[0] = 0;
	bFirstDraw = true;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (i == m_ScoreInfo.wMeChairId) continue;

		if (m_ScoreInfo.lChaJiaoScore[i] > 0)
		{
			if (bFirstDraw)
			{
				bFirstDraw = false;
				lstrcpy(szBuffer, TEXT("δ���ƣ�"));
				m_FontScore.DrawText(pD3DDevice, szBuffer, nXPos, nYPos, DT_LEFT|DT_BOTTOM, crColor);
				nYPos += 19;
				lstrcpy(szBuffer, TEXT("  ��� "));

			}
			else lstrcpy(szBuffer, TEXT("       "));

			TCHAR szName[10] = TEXT("");
			lstrcpyn(szName, m_ScoreInfo.szUserName[i], sizeof(szName)/sizeof(TCHAR));
			lstrcat(szBuffer, szName);
			if (lstrlen(m_ScoreInfo.szUserName[i]) > 5)
			{
				lstrcat(szBuffer, TEXT("..."));
			}
			_sntprintf(szBuffer, CountArray(szBuffer), TEXT("%s -%d"), szBuffer, m_ScoreInfo.lChaJiaoScore[i]);
			m_FontScore.DrawText(pD3DDevice, szBuffer, nXPos, nYPos, DT_LEFT|DT_BOTTOM, crColor);
			nYPos += 19;
		}
	}

	//�η�����
	if( m_ScoreInfo.lGangScore != 0 )
	{
		_sntprintf(szBuffer, CountArray(szBuffer), TEXT("�η����꣺%I64d"), m_ScoreInfo.lGangScore );
		m_FontScore.DrawText(pD3DDevice, szBuffer, nXPos, nYPos, DT_LEFT|DT_BOTTOM, crColor);
	}

	//�ܼ�
	_sntprintf(szBuffer, CountArray(szBuffer), TEXT("%I64d"), m_ScoreInfo.lGameScore );
	m_FontScore.DrawText(pD3DDevice, szBuffer, nXOriginPos+170, nYOriginPos+60, DT_LEFT|DT_BOTTOM, crColor);

	return;
}

//���ش���
VOID CScoreControl::CloseControl()
{
	//�رմ���
	if (IsWindowActive()==true)
	{
		//��������
		ZeroMemory(&m_ScoreInfo,sizeof(m_ScoreInfo));

		//�رմ���
		DeleteWindow();
	}

	return;
}

//���û���
void CScoreControl::SetScoreInfo(const tagScoreInfo & ScoreInfo)
{
	//���ñ���
	m_ScoreInfo=ScoreInfo;

	m_ChiHuRight.SetRightData( ScoreInfo.dwChiHuRight,MAX_RIGHT_COUNT );

	return;
}

//�滭����
void CScoreControl::DrawNumberString(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/)
{
	TCHAR szOutNum[128] = {0};
	_sntprintf(szOutNum,CountArray(szOutNum),SCORE_STRING,lOutNum);

	// ������Դ
	INT nNumberHeight=ImageNumber->GetHeight();
	INT nNumberWidth=ImageNumber->GetWidth()/lstrlen(szImageNum);

	if ( uFormat == DT_CENTER )
	{
		nXPos -= (INT)(((DOUBLE)(lstrlen(szOutNum)) / 2.0) * nNumberWidth);
	}
	else if ( uFormat == DT_RIGHT )
	{
		nXPos -= lstrlen(szOutNum) * nNumberWidth;
	}

	for ( INT i = 0; i < lstrlen(szOutNum); ++i )
	{
		for ( INT j = 0; j < lstrlen(szImageNum); ++j )
		{
			if ( szOutNum[i] == szImageNum[j] && szOutNum[i] != '\0' )
			{
				ImageNumber->DrawImage(pD3DDevice, nXPos, nYPos, nNumberWidth, nNumberHeight, j * nNumberWidth, 0, nNumberWidth, nNumberHeight);
				nXPos += nNumberWidth;
				break;
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////
