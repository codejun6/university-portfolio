#pragma once

// CGALImageFeature 명령 대상입니다.

class CGALImageFeature : public CObject
{
	DECLARE_SERIAL(CGALImageFeature)

public:
	// 색상 계열 상수
	enum ColorType {
		CT_NONE,     // 해당 없음 = 0
		CT_RED,      // 빨강색 계열 = 1
		CT_ORANGE,   // 주황색 계열 = 2
		CT_YELLOW,   // 노랑색 계열 = 3
		CT_GREEN,    // 초록색 계열 = 4
		CT_SKYBLUE,  // 하늘색 계열 = 5
		CT_BLUE,     // 파랑색 계열 = 6
		CT_PURPLE,   // 보라색 계열 = 7
		CT_PINK      // 분홍색 계열 = 8
	};

public:
	CString m_strFilePath;    // 파일전체경로
	CString m_strFileName;    // 파일명
	CString m_strFileType;    // 파일 종류
	double m_nFileSize;       // 파일 크기
	CTime m_timeCaptured;     // 찍은 날짜(기본값은 파일수정 날짜)
	CStringArray m_arrTag;    // 태그 배열
	CSize m_sizeImage;        // 이미지 크기
	int m_nImageBPP;          // 이미지 비트수

	int m_nAvgBright;         // 평균 밝기값
	ColorType m_ctColorType;  // 색상 계열
	BOOL m_bIsBNWPhoto;       // 흑백사진 여부
	BOOL m_bIsSelfPhoto;      // 자가촬영사진 여부

public:
	CGALImageFeature();
	CGALImageFeature(const CString& filePath, const CImage& image);
	virtual ~CGALImageFeature();

	virtual void Serialize(CArchive& ar);
	void SetColorTypeByInt(int colorType);
	void ExtractImageFeature(const CString& filePath, const CImage& image);
};


