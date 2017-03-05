#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "GALSliderCtrl.h"
#include "GALIntegerEdit.h"


// CGALEditDialog 대화 상자입니다.

class CGALThumbCtrl;

class CGALEditDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CGALEditDialog)

public:
	// 편집 효과 상수
	enum EditEffect {
		EE_NONE,     // 효과 없음 = 0
		EE_BNW,      // 흑백 효과 = 1
		EE_INVERT,   // 반전 효과 = 2
		EE_SEPIA,    // 세피아 효과 = 3
		EE_EMBOSS,   // 양각 효과 = 4
		EE_SKETCH,   // 스케치 효과 = 5
		EE_VINTAGE,  // 빈티지 효과 = 6
		EE_MOSAIC    // 모자이크 효과 = 7
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
	CGALEditDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CGALEditDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_GALEDITDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	// 컨트롤 관련 변수들
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
