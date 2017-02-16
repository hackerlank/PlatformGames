#include "StdAfx.h"
#include "GameClient.h"
#include "GameLogic.h"
#include "CardControl.h"
#include "ControlWnd.h"

//////////////////////////////////////////////////////////////////////////

//��ť��ʶ
#define IDC_CHIHU					100									//�Ժ���ť
#define IDC_LISTEN					101									//���ư�ť
#define IDC_GIVEUP					102									//������ť

//λ�ñ�ʶ
#define ITEM_WIDTH					90									//������
#define ITEM_HEIGHT					55									//����߶�
#define CONTROL_WIDTH				173									//���ƿ��
#define CONTROL_HEIGHT				41									//���Ƹ߶�

//////////////////////////////////////////////////////////////////////////


//���캯��
CControlWnd::CControlWnd()
{
	//���ñ���
	m_cbAlphaIndex=0;

	//���ñ���
	m_cbActionMask=0;
	m_cbCenterCard=0;
	m_PointBenchmark.SetPoint(0,0);
	ZeroMemory(m_cbGangCard,sizeof(m_cbGangCard));

	//״̬����
	m_cbItemCount=0;
	m_cbCurrentItem=0xFF;

	return;
}

//��������
CControlWnd::~CControlWnd()
{
}

//������Ϣ
VOID CControlWnd::OnWindowMovie()
{
	//�Ҷȶ���
	if ((IsWindowVisible()==true)&&((m_cbAlphaIndex<110L)&&(m_AlphaLapseCount.GetLapseCount(20)>0L)))
	{
		m_cbAlphaIndex+=5L;
	}

	return;
}

//��׼λ��
void CControlWnd::SetBenchmarkPos(int nXPos, int nYPos)
{
	//λ�ñ���
	m_PointBenchmark.SetPoint(nXPos,nYPos);

	//�����ؼ�
	RectifyControl();

	return;
}

//�����˿�
void CControlWnd::SetControlInfo(BYTE cbCenterCard, BYTE cbActionMask, tagGangCardResult & GangCardResult)
{
	//���ñ���
	m_cbItemCount=0;
	m_cbCurrentItem=0xFF;
	m_cbActionMask=cbActionMask;
	m_cbCenterCard=cbCenterCard;

	//������Ϣ
	ZeroMemory(m_cbGangCard,sizeof(m_cbGangCard));
	for (BYTE i=0;i<GangCardResult.cbCardCount;i++) 
	{
		m_cbItemCount++;
		m_cbGangCard[i]=GangCardResult.cbCardData[i];
	}

	//������Ŀ
	BYTE cbItemKind[4]={WIK_LEFT,WIK_CENTER,WIK_RIGHT,WIK_PENG};
	for (BYTE i=0;i<CountArray(cbItemKind);i++) 
		if ((m_cbActionMask&cbItemKind[i])!=0) 
			m_cbItemCount++;

	//��ť����
	m_btChiHu.EnableWindow((cbActionMask&WIK_CHI_HU) != 0);
	//m_btListen.EnableWindow(((cbActionMask&WIK_LISTEN)) != 0);

	//�����ؼ�
	RectifyControl();

	//��ʾ����
	ShowWindow(true);

	return;
}

//�����ؼ�
void CControlWnd::RectifyControl()
{
	//����λ��
	CRect rcRect;
	rcRect.right=m_PointBenchmark.x;
	rcRect.bottom=m_PointBenchmark.y;
	rcRect.left=m_PointBenchmark.x-CONTROL_WIDTH;
	rcRect.top=m_PointBenchmark.y-ITEM_HEIGHT*m_cbItemCount-CONTROL_HEIGHT;
	if (m_cbItemCount>0)
		rcRect.top-=5;

	m_ControlSize.cx = rcRect.Width();
	m_ControlSize.cy = rcRect.Height();
	
	//�ƶ�����
	SetWindowPos(rcRect.left,rcRect.top,m_ControlSize.cx,m_ControlSize.cy,0);

	//��ťλ��
	CRect rcButton;
	m_btChiHu.GetWindowRect(rcButton);

	int nYPos=rcRect.Height()-rcButton.Height()-9;
	m_btChiHu.SetWindowPos(rcRect.Width()-rcButton.Width()*3-12,nYPos, 0, 0, SWP_NOSIZE);
	//m_btListen.SetWindowPos(rcRect.Width()-rcButton.Width()*2-10,nYPos, 0, 0, SWP_NOSIZE);
	m_btGiveUp.SetWindowPos(rcRect.Width()-rcButton.Width()-6,nYPos, 0, 0, SWP_NOSIZE);

	return;
}


//������Ϣ
VOID CControlWnd::OnWindowCreate(CD3DDevice * pD3DDevice)
{
	//������ť
	CRect rcCreate(0,0,0,0);	

	//m_btListen.ActiveWindow(rcCreate,WS_VISIBLE,IDC_LISTEN,GetVirtualEngine(),this);
	//m_btListen.SetButtonImage(pD3DDevice,TEXT("BT_LISTEN"),TEXT("PNG"),AfxGetInstanceHandle());

	m_btGiveUp.ActiveWindow(rcCreate,WS_VISIBLE,IDC_GIVEUP,GetVirtualEngine(),this);
	m_btGiveUp.SetButtonImage(pD3DDevice,TEXT("BT_GIVEUP"),TEXT("PNG"),AfxGetInstanceHandle());

	m_btChiHu.ActiveWindow(rcCreate,WS_VISIBLE,IDC_CHIHU,GetVirtualEngine(),this);
	m_btChiHu.SetButtonImage(pD3DDevice,TEXT("BT_CHIHU"),TEXT("PNG"),AfxGetInstanceHandle());

	//������Դ
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_TextureControlTop.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("CONTROL_TOP"),TEXT("PNG"));
	m_TextureControlMid.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("CONTROL_MID"),TEXT("PNG"));
	m_TextureControlButtom.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("CONTROL_BOTTOM"),TEXT("PNG"));
	m_TextureActionExplain.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("ACTION_EXPLAIN"),TEXT("PNG"));
	m_TextureItemFrame.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("ITEM_FRAME"),TEXT("PNG"));

	return;
}

//������Ϣ
VOID CControlWnd::OnWindowDestory(CD3DDevice * pD3DDevice)
{
	//������Դ
	if (m_TextureControlTop.IsNull()==false) m_TextureControlTop.Destory();
	if (m_TextureControlMid.IsNull()==false) m_TextureControlMid.Destory();
	if (m_TextureControlButtom.IsNull()==false) m_TextureControlButtom.Destory();
	if (m_TextureActionExplain.IsNull()==false) m_TextureActionExplain.Destory();

	return;
}

//����¼�
VOID CControlWnd::OnEventMouse(UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos)
{
	if(512 == uMessage)								//����ƶ�
	{
		if (m_cbItemCount!=0)
		{
			//��ȡλ��
			CPoint MousePoint(nXMousePos, nYMousePos);

			//��������
			BYTE bCurrentItem=0xFF;
			CRect rcItem(5,5,m_ControlSize.cx-5,ITEM_HEIGHT*m_cbItemCount+5);

			if (rcItem.PtInRect(MousePoint))
				bCurrentItem=(BYTE)((MousePoint.y-7)/ITEM_HEIGHT);

			//��������
			if (m_cbCurrentItem!=bCurrentItem)
			{
				m_cbCurrentItem=bCurrentItem;
			}
		}
	}
	else if(514 == uMessage)			//����������
	{
		//�����ж�
		if (m_cbCurrentItem!=0xFF)
		{
			//��������
			BYTE cbIndex=0;
			BYTE cbItemKind[4]={WIK_LEFT,WIK_CENTER,WIK_RIGHT,WIK_PENG};

			CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();

			//��������
			for (BYTE i=0;i<CountArray(cbItemKind);i++)
			{
				if (((m_cbActionMask&cbItemKind[i])!=0)&&(m_cbCurrentItem==cbIndex++))
				{
					pGameFrameEngine->PostMessage(IDM_CARD_OPERATE,cbItemKind[i],m_cbCenterCard);
					return;
				}
			}

			//��������
			for (BYTE i=0;i<CountArray(m_cbGangCard);i++)
			{
				if ((m_cbGangCard[i]!=0)&&(m_cbCurrentItem==cbIndex++))
				{
					pGameFrameEngine->PostMessage(IDM_CARD_OPERATE,WIK_GANG,m_cbGangCard[i]);
					return;
				}
			}

			//�������
			ASSERT(FALSE);
		}
	}
	return;
}

//��ť�¼�
VOID CControlWnd::OnEventButton(UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos)
{
	//������Ϣ
	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	if( pGameFrameEngine )
	{
		if (uButtonID==IDC_LISTEN)
		{
		 	pGameFrameEngine->PostMessage(IDM_LISTEN_CARD,0,0);
		}
		else if (uButtonID==IDC_CHIHU)
		{
			pGameFrameEngine->PostMessage(IDM_CARD_OPERATE,WIK_CHI_HU,0);
		}
		else if (uButtonID==IDC_GIVEUP)
		{
			pGameFrameEngine->PostMessage(IDM_CARD_OPERATE,WIK_NULL,0);
		}
	}

	return;
}

//�滭����
VOID CControlWnd::OnEventDrawWindow(CD3DDevice * pD3DDevice, INT nXOriginPos, INT nYOriginPos)
{
	//�滭����
	int nTopHeight = m_TextureControlTop.GetHeight();
	int nMidHeight = m_TextureControlMid.GetHeight();
	int nMidWidth = m_TextureControlMid.GetWidth();
	int nButtomHeight = m_TextureControlButtom.GetHeight();

	m_TextureControlTop.DrawImage(pD3DDevice, nXOriginPos, nYOriginPos);
	m_TextureControlMid.DrawImage(pD3DDevice, nXOriginPos, nYOriginPos + nTopHeight, m_ControlSize.cx, m_ControlSize.cy - nButtomHeight - nTopHeight, 0, 0, nMidWidth, nMidHeight);
 	m_TextureControlButtom.DrawImage(pD3DDevice, nXOriginPos, nYOriginPos + (m_ControlSize.cy - nButtomHeight));
 
 	//��������
 	int nYPos=5;
 	BYTE cbCurrentItem=0;
 	BYTE cbExcursion[3]={0,1,2};
 	BYTE cbItemKind[4]={WIK_LEFT,WIK_CENTER,WIK_RIGHT,WIK_PENG};
 
 	//�滭�˿�
 	for (BYTE i=0;i<CountArray(cbItemKind);i++)
 	{
 		if ((m_cbActionMask&cbItemKind[i])!=0)
 		{
			//�滭�߿�
			if (cbCurrentItem==m_cbCurrentItem)
			{
				m_TextureItemFrame.DrawImage(pD3DDevice,nXOriginPos + 9,nYOriginPos + nYPos);
			}

 			//�滭�˿�
 			for (BYTE j=0;j<3;j++)
 			{
 				BYTE cbCardData=m_cbCenterCard;
 				if (i<CountArray(cbExcursion))
 					cbCardData=cbCardData+j-cbExcursion[i];
 				g_CardResource.m_ImageTableBottom.DrawCardItem(pD3DDevice,cbCardData,nXOriginPos + j*24+12,nYOriginPos + nYPos+5);
 			}
 
 			//����λ��
 			int nXImagePos=0;
 			int nItemWidth=m_TextureActionExplain.GetWidth()/7;
 			if ((m_cbActionMask&cbItemKind[i])&WIK_PENG)
 				nXImagePos=nItemWidth;
 
 			//�滭��־
 			int nItemHeight=m_TextureActionExplain.GetHeight();
 			m_TextureActionExplain.DrawImage(pD3DDevice,nXOriginPos + 126,nYOriginPos + nYPos+5,nItemWidth,nItemHeight,nXImagePos,0);
 
 			//���ñ���
 			++cbCurrentItem;
 			nYPos+=ITEM_HEIGHT;
 		}
 	}
 
 	//�����˿�
 	for (BYTE i=0;i<CountArray(m_cbGangCard);i++)
 	{
 		if (m_cbGangCard[i]!=0)
 		{
 			//�滭�߿�
 			if (cbCurrentItem==m_cbCurrentItem)
 			{
 				m_TextureItemFrame.DrawImage(pD3DDevice,nXOriginPos + 9,nYOriginPos + nYPos);
 			}
			
			//�滭�˿�
 			for (BYTE j=0;j<4;j++)
 			{
 				g_CardResource.m_ImageTableBottom.DrawCardItem(pD3DDevice,m_cbGangCard[i],nXOriginPos + j*24+12,nYOriginPos + nYPos+5);
 			} 			
 
 			//�滭��־
 			int nItemWidth=m_TextureActionExplain.GetWidth()/7;
 			int nItemHeight=m_TextureActionExplain.GetHeight();
 			m_TextureActionExplain.DrawImage(pD3DDevice,nXOriginPos + 126,nYOriginPos + nYPos+5,nItemWidth,nItemHeight,nItemWidth*3,0);
 
 			//���ñ���
 			cbCurrentItem++;
 			nYPos+=ITEM_HEIGHT;
 		}
 		else break;
 	}

	return;
}

//���ش���
VOID CControlWnd::CloseControl()
{
	//�رմ���
	if (IsWindowActive()==true)
	{
		//�رմ���
		DeleteWindow();
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////
//��������
extern CCardResource						g_CardResource;						//�˿���Դ
//////////////////////////////////////////////////////////////////////////////////
