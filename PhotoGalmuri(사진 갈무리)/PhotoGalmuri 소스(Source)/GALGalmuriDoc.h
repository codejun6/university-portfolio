
// GALGalmuriDoc.h : CGALGalmuriDoc Ŭ������ �������̽�
//


#pragma once


class CGALGalmuriDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CGALGalmuriDoc();
	DECLARE_DYNCREATE(CGALGalmuriDoc)

// Ư���Դϴ�.
public:
	CObArray m_arrFeature;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// �����Դϴ�.
public:
	virtual ~CGALGalmuriDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// �˻� ó���⿡ ���� �˻� �������� �����ϴ� ����� �Լ�
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
