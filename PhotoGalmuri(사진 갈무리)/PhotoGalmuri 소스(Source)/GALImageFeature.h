#pragma once

// CGALImageFeature ��� ����Դϴ�.

class CGALImageFeature : public CObject
{
	DECLARE_SERIAL(CGALImageFeature)

public:
	// ���� �迭 ���
	enum ColorType {
		CT_NONE,     // �ش� ���� = 0
		CT_RED,      // ������ �迭 = 1
		CT_ORANGE,   // ��Ȳ�� �迭 = 2
		CT_YELLOW,   // ����� �迭 = 3
		CT_GREEN,    // �ʷϻ� �迭 = 4
		CT_SKYBLUE,  // �ϴû� �迭 = 5
		CT_BLUE,     // �Ķ��� �迭 = 6
		CT_PURPLE,   // ����� �迭 = 7
		CT_PINK      // ��ȫ�� �迭 = 8
	};

public:
	CString m_strFilePath;    // ������ü���
	CString m_strFileName;    // ���ϸ�
	CString m_strFileType;    // ���� ����
	double m_nFileSize;       // ���� ũ��
	CTime m_timeCaptured;     // ���� ��¥(�⺻���� ���ϼ��� ��¥)
	CStringArray m_arrTag;    // �±� �迭
	CSize m_sizeImage;        // �̹��� ũ��
	int m_nImageBPP;          // �̹��� ��Ʈ��

	int m_nAvgBright;         // ��� ��Ⱚ
	ColorType m_ctColorType;  // ���� �迭
	BOOL m_bIsBNWPhoto;       // ������ ����
	BOOL m_bIsSelfPhoto;      // �ڰ��Կ����� ����

public:
	CGALImageFeature();
	CGALImageFeature(const CString& filePath, const CImage& image);
	virtual ~CGALImageFeature();

	virtual void Serialize(CArchive& ar);
	void SetColorTypeByInt(int colorType);
	void ExtractImageFeature(const CString& filePath, const CImage& image);
};


