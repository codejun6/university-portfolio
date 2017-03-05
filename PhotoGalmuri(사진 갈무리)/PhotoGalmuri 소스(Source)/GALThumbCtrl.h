#pragma once


// CGALThumbCtrl

class CGALImageFeature;

class CGALThumbCtrl : public CButton
{
	DECLARE_DYNAMIC(CGALThumbCtrl)

protected:
	CImage m_imgOrigin;
	CGALImageFeature* m_pImageFeature;
	BOOL m_bIsSelected;

public:
	CGALThumbCtrl();
	CGALThumbCtrl(const CString& filePath);
	virtual ~CGALThumbCtrl();

	BOOL IsNull();
	void SetSelected(BOOL isSelected);
	BOOL IsSelected();
	void LoadOriginImage(const CString& filePath);
	CImage* GetOriginImagePtr();
	void SetImageFeature(CGALImageFeature* pFeature);
	CGALImageFeature* GetImageFeature();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};


