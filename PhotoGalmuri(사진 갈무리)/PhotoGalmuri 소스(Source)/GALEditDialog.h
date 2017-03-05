#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "GALSliderCtrl.h"
#include "GALIntegerEdit.h"


// CGALEditDialog ��ȭ �����Դϴ�.

class CGALThumbCtrl;

class CGALEditDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CGALEditDialog)

public:
	// ���� ȿ�� ���
	enum EditEffect {
		EE_NONE,     // ȿ�� ���� = 0
		EE_BNW,      // ��� ȿ�� = 1
		EE_INVERT,   // ���� ȿ�� = 2
		EE_SEPIA,    // ���Ǿ� ȿ�� = 3
		EE_EMBOSS,   // �簢 ȿ�� = 4
		EE_SKETCH,   // ����ġ ȿ�� = 5
		EE_VINTAGE,  // ��Ƽ�� ȿ�� = 6
		EE_MOSAIC    // ������ũ ȿ�� = 7
	};

protected:
	CFont m_fontStatic;
	CFont m_fontButton;
	CGALThumbCtrl* m_pOriginThumb;
	CImage m_imgOrigin;
	CImage m_imgPreview;
	BOOL m_bInitializing;
	EditEffect m_eeEditEffect;
	int m_nRotateAngle;

public:
	void SetOriginThumb(CGALThumbCtrl* pThumbCtrl);
	void UpdateTagListHScroll();
	void DrawPreviewImage();
	void EditImageByEditOption(CImage& src, CImage& dest);
	void ApplyBCSValues(CImage& src, CImage& dest);
	void ApplySharpValue(CImage& image);
	void ApplySmoothValue(CImage& image);
	void ApplyBNWEffect(CImage& image);
	void ApplyInvertEffect(CImage& image);
	void ApplySepiaEffect(CImage& image);
	void ApplyEmbossEffect(CImage& image);
	void ApplySketchEffect(CImage& image);
	void ApplyVintageEffect(CImage& image);
	void ApplyMosaicEffect(CImage& image);
	void ApplyRotation(CImage& src, CImage& dest);

public:
	CGALEditDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CGALEditDialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_GALEDITDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	// ��Ʈ�� ���� ������
	CString m_strCapturedTime;
	CListBox m_listTag;
	CString m_strAddTag;
	CGALSliderCtrl m_sliderBrightness;
	CGALSliderCtrl m_sliderContrast;
	CGALSliderCtrl m_sliderSaturation;
	CGALIntegerEdit m_editBrightness;
	CGALIntegerEdit m_editContrast;
	CGALIntegerEdit m_editSaturation;
	CSpinButtonCtrl m_spinSharp;
	CSpinButtonCtrl m_spinSmooth;

	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedEditTag(UINT nID);
	afx_msg void OnEnChangeEditBCS(UINT nID);
	afx_msg void OnEnChangeEditSharpSmooth(UINT nID);
	afx_msg void OnBnClickedEditRotate(UINT nID);
	afx_msg void OnBnClickedEditEffect(UINT nID);
	afx_msg void OnBnClickedEditInit();
	afx_msg void OnBnClickedEditSave();
	afx_msg void OnBnClickedEditSaveas();
};
