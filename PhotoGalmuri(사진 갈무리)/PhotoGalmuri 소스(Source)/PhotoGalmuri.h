
// PhotoGalmuri.h : PhotoGalmuri ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CPhotoGalmuriApp:
// �� Ŭ������ ������ ���ؼ��� PhotoGalmuri.cpp�� �����Ͻʽÿ�.
//

class CPhotoGalmuriApp : public CWinApp
{
public:
	CPhotoGalmuriApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CPhotoGalmuriApp theApp;
