#ifndef CARD_CONTROL_HEAD_FILE
#define CARD_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "GameLogic.h"
#include "cassert"

//////////////////////////////////////////////////////////////////////////

//X ���з�ʽ
enum enXCollocateMode 
{ 
	enXLeft,						//�����
	enXCenter,						//�ж���
	enXRight,						//�Ҷ���
};

//Y ���з�ʽ
enum enYCollocateMode 
{ 
	enYTop,							//�϶���
	enYCenter,						//�ж���
	enYBottom,						//�¶���
};

//����ö��
enum enDirection
{
	Direction_East,					//����   ��
	Direction_South,				//����
	Direction_West,					//����
	Direction_North,				//����
};

//////////////////////////////////////////////////////////////////////////

//�˿�ͼƬ
class CCardListImage
{
	//λ�ñ���
protected:
	int								m_nItemWidth;						//����߶�
	int								m_nItemHeight;						//������
	int								m_nViewWidth;						//����߶�
	int								m_nViewHeight;						//������

	//��Դ����
protected:
	CD3DTexture						m_CardListImage;					//ͼƬ��Դ
					

	//��������
public:
	//���캯��
	CCardListImage();
	//��������
	virtual ~CCardListImage();

	//��Ϣ����
public:
	//��ȡ���
	int GetViewWidth() { return m_nViewWidth; }
	//��ȡ�߶�
	int GetViewHeight() { return m_nViewHeight; }

	//��Դ����
public:
	//������Դ
	bool LoadResource(CD3DDevice * pD3DDevice, LPCTSTR uResourceID, int nViewWidth, int nViewHeight);
	//�ͷ���Դ
	bool DestroyResource();

	//���ܺ���
public:
	//��ȡλ��
	inline int GetImageIndex(BYTE cbCardData);
	//�滭�˿�
	inline bool DrawCardItem(CD3DDevice * pD3DDevice, BYTE cbCardData, int xDest, int yDest);
};

//////////////////////////////////////////////////////////////////////////

//�˿���Դ
class CCardResource
{
	//�û��˿�
public:
	CD3DTexture						m_ImageUserTop;						//ͼƬ��Դ
	CCardListImage					m_ImageUserBottom;					//ͼƬ��Դ

	CD3DTexture						m_ImageUserLeft;					//ͼƬ��Դ
	CD3DTexture						m_ImageUserRight;					//ͼƬ��Դ

	//�����˿�
public:
	CCardListImage					m_ImageTableTop;					//ͼƬ��Դ
	CCardListImage					m_ImageTableLeft;					//ͼƬ��Դ
	CCardListImage					m_ImageTableRight;					//ͼƬ��Դ
	CCardListImage					m_ImageTableBottom;					//ͼƬ��Դ
	CCardListImage					m_TextureUserTableBottom;			//ͼƬ��Դ

	//�ƶ��˿�
public:
	CD3DTexture						m_ImageHeapDoubleLV;				//��ߵĶ���
	CD3DTexture						m_ImageHeapSingleLV;				//��ߵĶ���

	CD3DTexture						m_ImageHeapDoubleRV;				//�ұߵĶ���
	CD3DTexture						m_ImageHeapSingleRV;				//�ұߵĶ���

	CD3DTexture						m_ImageHeapDoubleTV;				//�ϱߵĶ���
	CD3DTexture						m_ImageHeapSingleTV;				//�ϱߵĶ���

	CD3DTexture						m_ImageHeapDoubleBV;				//�±ߵĶ���
	CD3DTexture						m_ImageHeapSingleBV;				//�±ߵĶ���


	//��������
public:
	//���캯��
	CCardResource();
	//��������
	virtual ~CCardResource();

	//���ܺ���
public:
	//������Դ
	bool LoadResource(CD3DDevice * pD3DDevice);
	//������Դ
	bool DestroyResource();
};

//////////////////////////////////////////////////////////////////////////

//�����˿�
class CHeapCard
{
	//���Ʊ���
protected:
	CPoint							m_ControlPoint;						//��׼λ��
	enDirection						m_CardDirection;					//�˿˷���

	//�˿˱���
protected:
	WORD							m_wFullCount;						//ȫ����Ŀ
	WORD							m_wMinusHeadCount;					//ͷ����ȱ
	WORD							m_wMinusLastCount;					//β����ȱ

	//��������
public:
	//���캯��
	CHeapCard();
	//��������
	virtual ~CHeapCard();

	//���ܺ���
public:
	//�滭�˿�
	void DrawCardControl(CD3DDevice * pD3DDevice);
	//�����˿�
	bool SetCardData(WORD wMinusHeadCount, WORD wMinusLastCount, WORD wFullCount);

	//�ؼ�����
public:
	//���÷���
	void SetDirection(enDirection Direction) { m_CardDirection=Direction; }
	//��׼λ��
	void SetControlPoint(int nXPos, int nYPos) { m_ControlPoint.SetPoint(nXPos,nYPos); }
};

//////////////////////////////////////////////////////////////////////////

//����˿�
class CWeaveCard
{
	//���Ʊ���
protected:
	bool							m_bDisplayItem;						//��ʾ��־
	CPoint							m_ControlPoint;						//��׼λ��
	enDirection						m_CardDirection;					//�˿˷���
	BYTE							m_cbDirectionCardPos;				//�����˿�
	bool							m_bGameEnd;							//

	//�˿�����
protected:
	WORD							m_wCardCount;						//�˿���Ŀ
	BYTE							m_cbCardData[4];					//�˿�����

	//��������
public:
	//���캯��
	CWeaveCard();
	//��������
	virtual ~CWeaveCard();

	//���ܺ���
public:
	//�滭�˿�
	void DrawCardControl(CD3DDevice * pD3DDevice);
	//�滭�˿�
	void DrawCardControl(CD3DDevice * pD3DDevice, int nXPos, int nYPos);
	//�����˿�
	bool SetCardData(const BYTE cbCardData[], WORD wCardCount);

	//�ؼ�����
public:
	//������ʾ
	void SetDisplayItem(bool bDisplayItem) { m_bDisplayItem=bDisplayItem; }
	//���÷���
	void SetDirection(enDirection Direction) { m_CardDirection=Direction; }
	//��׼λ��
	void SetControlPoint(int nXPos, int nYPos) { m_ControlPoint.SetPoint(nXPos,nYPos); }
	//������
	void SetDirectionCardPos(BYTE cbPos) { assert(cbPos <= 3); m_cbDirectionCardPos = cbPos;}
	//
	void SetGameEnd( bool bGameEnd ) { m_bGameEnd = bGameEnd; }

	//�ڲ�����
private:
	//��ȡ�˿�
	inline BYTE GetCardData(WORD wIndex);
};

//////////////////////////////////////////////////////////////////////////

//�û��˿�
class CUserCard
{
	//�˿�����
protected:
	WORD							m_wCardCount;						//�˿���Ŀ
	bool							m_bCurrentCard;						//��ǰ��־

	//���Ʊ���
protected:
	CPoint							m_ControlPoint;						//��׼λ��
	enDirection						m_CardDirection;					//�˿˷���

	//��������
public:
	//���캯��
	CUserCard();
	//��������
	virtual ~CUserCard();

	//���ܺ���
public:
	//�滭�˿�
	void DrawCardControl(CD3DDevice * pD3DDevice);
	//�����˿�
	bool SetCurrentCard(bool bCurrentCard);
	//�����˿�
	bool SetCardData(WORD wCardCount, bool bCurrentCard);

	//�ؼ�����
public:
	//���÷���
	void SetDirection(enDirection Direction) { m_CardDirection=Direction; }
	//��׼λ��
	void SetControlPoint(int nXPos, int nYPos) { m_ControlPoint.SetPoint(nXPos,nYPos); }
};

//////////////////////////////////////////////////////////////////////////

//�����˿�
class CDiscardCard
{
	//�˿�����
protected:
	WORD							m_wCardCount;						//�˿���Ŀ
	BYTE							m_cbCardData[24];					//�˿�����

	//���Ʊ���
protected:
	CPoint							m_ControlPoint;						//��׼λ��
	enDirection						m_CardDirection;					//�˿˷���

	//��������
public:
	//���캯��
	CDiscardCard();
	//��������
	virtual ~CDiscardCard();

	//���ܺ���
public:
	//�滭�˿�
	void DrawCardControl(CD3DDevice * pD3DDevice);
	//�����˿�
	bool AddCardItem(BYTE cbCardData);
	//�����˿�
	bool SetCardData(const BYTE cbCardData[], WORD wCardCount);
	//��ȡλ��
	CPoint GetLastCardPosition();

	//�ؼ�����
public:
	//���÷���
	void SetDirection(enDirection Direction) { m_CardDirection=Direction; }
	//��׼λ��
	void SetControlPoint(int nXPos, int nYPos) { m_ControlPoint.SetPoint(nXPos,nYPos); }
};

//////////////////////////////////////////////////////////////////////////

//�����˿�
class CTableCard
{
	//�˿�����
protected:
	WORD							m_wCardCount;						//�˿���Ŀ
	BYTE							m_cbCardData[14];					//�˿�����

	//���Ʊ���
protected:
	CPoint							m_ControlPoint;						//��׼λ��
	enDirection						m_CardDirection;					//�˿˷���

	//��������
public:
	//���캯��
	CTableCard();
	//��������
	virtual ~CTableCard();

	//���ܺ���
public:
	//�滭�˿�
	void DrawCardControl(CD3DDevice * pD3DDevice);
	//�����˿�
	bool SetCardData(const BYTE cbCardData[], WORD wCardCount);

	//�ؼ�����
public:
	//���÷���
	void SetDirection(enDirection Direction) { m_CardDirection=Direction; }
	//��׼λ��
	void SetControlPoint(int nXPos, int nYPos) { m_ControlPoint.SetPoint(nXPos,nYPos); }
};

//////////////////////////////////////////////////////////////////////////

//�˿˽ṹ
struct tagCardItem
{
	bool							bShoot;								//�����־
	BYTE							cbCardData;							//�˿�����
};

//�˿˿ؼ�
class CCardControl
{
	//״̬����
protected:
	bool							m_bPositively;						//��Ӧ��־
	bool							m_bDisplayItem;						//��ʾ��־

	//λ�ñ���
protected:
	CPoint							m_BenchmarkPos;						//��׼λ��
	enXCollocateMode				m_XCollocateMode;					//��ʾģʽ
	enYCollocateMode				m_YCollocateMode;					//��ʾģʽ

	//�˿�����
protected:
	WORD							m_wCardCount;						//�˿���Ŀ
	WORD							m_wHoverItem;						//��������
	tagCardItem						m_CurrentCard;						//��ǰ�˿�
	tagCardItem						m_CardItemArray[13];				//�˿�����

	//��Դ����
protected:
	CSize							m_ControlSize;						//�ؼ���С
	CPoint							m_ControlPoint;						//�ؼ�λ��

	//��������
public:
	//���캯��
	CCardControl();
	//��������
	virtual ~CCardControl();

	//��ѯ����
public:
	//��ȡ�˿�
	BYTE GetHoverCard();
	//��ȡ�˿�
	BYTE GetCurrentCard() { return m_CurrentCard.cbCardData; }

	//�˿˿���
public:
	//�����˿�
	bool SetCurrentCard(BYTE cbCardData);
	//�����˿�
	bool SetCurrentCard(tagCardItem CardItem);
	//�����˿�
	bool SetCardData(const BYTE cbCardData[], WORD wCardCount, BYTE cbCurrentCard);
	//�����˿�
	bool SetCardItem(const tagCardItem CardItemArray[], WORD wCardCount);

	//�ؼ�����
public:
	//������Ӧ
	void SetPositively(bool bPositively) { m_bPositively=bPositively; }
	//������ʾ
	void SetDisplayItem(bool bDisplayItem) { m_bDisplayItem=bDisplayItem; }
	//��׼λ��
	void SetBenchmarkPos(int nXPos, int nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);
	//��׼λ��
	void SetBenchmarkPos(const CPoint & BenchmarkPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);

	//�¼�����
public:
	//�滭�˿�
	void DrawCardControl(CD3DDevice * pD3DDevice);
	//�����Ϣ
	bool OnEventSetCursor(CPoint Point, bool & bRePaint);

	//�ڲ�����
private:
	//�����л�
	WORD SwitchCardPoint(CPoint & MousePoint);
};

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

//��������
//��������
extern CCardResource				g_CardResource;						//�˿���Դ

//////////////////////////////////////////////////////////////////////////

#endif
