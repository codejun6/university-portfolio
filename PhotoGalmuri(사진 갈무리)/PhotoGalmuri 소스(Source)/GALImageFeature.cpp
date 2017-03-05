// GALImageFeature.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PhotoGalmuri.h"
#include "GALImageFeature.h"


// CGALImageFeature

IMPLEMENT_SERIAL(CGALImageFeature, CObject, 1)

CGALImageFeature::CGALImageFeature()
	: m_ctColorType(CT_NONE)
	, m_bIsBNWPhoto(FALSE)
	, m_bIsSelfPhoto(FALSE)
	, m_nAvgBright(0)
	, m_nFileSize(0)
	, m_nImageBPP(0)
{
}

CGALImageFeature::CGALImageFeature(const CString& filePath, const CImage& image)
	: m_ctColorType(CT_NONE)
	, m_bIsBNWPhoto(FALSE)
	, m_bIsSelfPhoto(FALSE)
	, m_nAvgBright(0)
	, m_nFileSize(0)
	, m_nImageBPP(0)
{
	ExtractImageFeature(filePath, image);
}

CGALImageFeature::~CGALImageFeature()
{
}


// CGALImageFeature ��� �Լ�

void CGALImageFeature::Serialize(CArchive& ar)
{
	// ���� �κ�
	if (ar.IsStoring())
	{
		ar << m_strFilePath;
		ar << m_strFileType;
		ar << m_nFileSize;
		ar << m_timeCaptured;
		ar << m_arrTag.GetSize();
		for(int i = 0; i < m_arrTag.GetSize(); i++)
		{
			ar << m_arrTag[i];
		}
		ar << m_sizeImage;
		ar << m_nImageBPP;
		ar << m_nAvgBright;
		ar << (int)m_ctColorType;
		ar << m_bIsBNWPhoto;
		ar << m_bIsSelfPhoto;
	}
	// �ε� �κ�
	else
	{
		ar >> m_strFilePath;
		m_strFileName = PathFindFileName(m_strFilePath);
		ar >> m_strFileType;
		ar >> m_nFileSize;
		ar >> m_timeCaptured;
		int tagCount = 0;
		ar >> tagCount;
		for(int i = 0; i < tagCount; i++)
		{
			CString str;
			ar >> str;
			m_arrTag.Add(str);
		}
		ar >> m_sizeImage;
		ar >> m_nImageBPP;
		ar >> m_nAvgBright;
		int colorType = 0;
		ar >> colorType;
		SetColorTypeByInt(colorType);
		ar >> m_bIsBNWPhoto;
		ar >> m_bIsSelfPhoto;
	}
}

void CGALImageFeature::SetColorTypeByInt(int colorType)
{
	switch(colorType)
	{
	case 0:  m_ctColorType = CT_NONE;    break;
	case 1:  m_ctColorType = CT_RED;     break;
	case 2:  m_ctColorType = CT_ORANGE;  break;
	case 3:  m_ctColorType = CT_YELLOW;  break;
	case 4:  m_ctColorType = CT_GREEN;   break;
	case 5:  m_ctColorType = CT_SKYBLUE; break;
	case 6:  m_ctColorType = CT_BLUE;    break;
	case 7:  m_ctColorType = CT_PURPLE;  break;
	case 8:  m_ctColorType = CT_PINK;    break;
	}
}

void CGALImageFeature::ExtractImageFeature(const CString& filePath, const CImage& image)
{
	if(filePath.IsEmpty() || image.IsNull() || !PathFileExists(filePath))
		return;

	// ���ϰ�ο� ���õ� Ư¡���� �����Ѵ�.
	m_strFilePath = filePath;
	m_strFileName = PathFindFileName(filePath);
	m_strFileType = PathFindExtension(filePath);
	if(!m_strFileType.CompareNoCase(".jpg"))	   m_strFileType = "JPG";
	else if(!m_strFileType.CompareNoCase(".png"))  m_strFileType = "PNG";
	else if(!m_strFileType.CompareNoCase(".bmp"))  m_strFileType = "BMP";
	else										   m_strFileType = "GIF";

	// ���ϻ��¿� ���õ� Ư¡���� �����Ѵ�.
	CFileStatus status;
	CFile::GetStatus(filePath, status);
	m_nFileSize = (double)status.m_size / 1024;  // Byte�� ������ ������ KByte�� ��ȯ�Ѵ�.
	m_timeCaptured = status.m_mtime;             // ���� ��¥�� �⺻���� ���ϻ��� ��¥�� �ʱ�ȭ�Ѵ�.

	// �̹����� �⺻������ �ش��ϴ� Ư¡���� �����Ѵ�.
	m_sizeImage.SetSize(image.GetWidth(), image.GetHeight());
	m_nImageBPP = image.GetBPP();

	// �̹����� ��ȸ�ϸ� Ư¡���� �����Ѵ�.
	BYTE* pixelPtr;
	BYTE byteR, byteG, byteB;
	float floatR, floatG, floatB;
	BYTE luminance, cb, cr;
	float hue, saturation, brightness;
	UINT colorTypeCount[8] = {0,};
	UINT skinColorCount = 0;
	m_nAvgBright = 0;
	m_ctColorType = CT_NONE;
	m_bIsBNWPhoto = TRUE;
	for(int y = 0; y < image.GetHeight(); y++)
	{
		for(int x = 0; x < image.GetWidth(); x++)
		{
			pixelPtr = (BYTE*)image.GetPixelAddress(x, y);
			byteB = *pixelPtr++;
			byteG = *pixelPtr++;
			byteR = *pixelPtr;

			floatB = (float)byteB / 255;
			floatG = (float)byteG / 255;
			floatR = (float)byteR / 255;
			float floatMax = (floatR > floatG)? ((floatR > floatB)? floatR : floatB) : ((floatG > floatB)? floatG :floatB);
			float floatMin = (floatR < floatG)? ((floatR < floatB)? floatR : floatB) : ((floatG < floatB)? floatG :floatB);

			// YCbCr ���� Y(luminance), Cb, Cr ���� ���Ѵ�.
			luminance = (BYTE)(0.299 * byteB + 0.587 * byteG + 0.114 * byteB);
			cb = (BYTE)(0.5643 * (byteB - luminance) + 128);
			cr = (BYTE)(0.7132 * (byteR - luminance) + 128);

			// HSB(HSV) ���� Hue, Saturation, Brightness ���� ���Ѵ�.
			brightness = floatMax;
			float delta = floatMax - floatMin;
			if(delta == 0)
			{
				hue = saturation = 0;
			}
			else
			{
				saturation = delta / floatMax;

				if(floatR == floatMax)       hue = (floatG - floatB) / delta;
				else if(floatG == floatMax)  hue = (floatB - floatR) / delta + 2;
				else						 hue = (floatR - floatG) / delta + 4;
				hue *= 60;
				if(hue < 0)
					hue += 360;
			}

			// ��� ��Ⱚ�� ���ϱ� ���� ��Ⱚ(0~255)�� ���Ѵ�.
			m_nAvgBright += (BYTE)(brightness * 255);

			// ���� �迭�� �����ϱ� ���� ī��Ʈ�� ������Ų��.
			// (Ư�� ä���� ��� �̻��� �ȼ��鸸 ���� �迭�� ������ �ش�.)
			if(saturation >= 0.3 && brightness >= 0.5)
			{
				if((hue >= 0 && hue <= 20) || (hue >= 346 && hue <= 360))
					colorTypeCount[0]++;
				else if(hue >= 21 && hue <= 45)
					colorTypeCount[1]++;
				else if(hue >= 46 && hue <= 70)
					colorTypeCount[2]++;
				else if(hue >= 71 && hue <= 165)
					colorTypeCount[3]++;
				else if(hue >= 166 && hue <= 210)
					colorTypeCount[4]++;
				else if(hue >= 211 && hue <= 265)
					colorTypeCount[5]++;
				else if(hue >= 266 && hue <= 290)
					colorTypeCount[6]++;
				else
					colorTypeCount[7]++;
			}

			// ä���� 0�� �ƴ� ���, �������� �ƴϴ�.
			if(saturation != 0)
			{
				m_bIsBNWPhoto = FALSE;
			}

			// ����� �ش��ϴ� ���, ��� ī��Ʈ�� ������Ų��.
			if((cb >= 73 && cb <= 158) && (cr >= 128 && cr <= 170)
				&& (hue > 0 && hue < 50) && (saturation > 0.23 && saturation < 0.68))
			{
				if((byteR > 95 && byteG > 40 && byteB > 20)
					&& (max(byteR, max(byteG, byteB) - min(byteR, min(byteG, byteB))) > 15)
					&& (abs(byteR - byteG) > 15 && byteR > byteG && byteR > byteB))
				{
					skinColorCount ++;
				}

				skinColorCount++;
			}
		}
	}

	// ��� ��Ⱚ�� ���Ѵ�.
	m_nAvgBright /= image.GetWidth() * image.GetHeight();

	// ���� ū ���� �迭 ī��Ʈ�� ���� ���� �迭�� �����Ѵ�.
	if(!m_bIsBNWPhoto)
	{
		UINT max = colorTypeCount[0];
		int index = 0;
		for(int i = 1; i < 8; i++)
		{
			if(colorTypeCount[i] > max)
			{
				max = colorTypeCount[i];
				index = i;
			}
		}
		switch(index)
		{
		case 0:  m_ctColorType = CT_RED;     break;
		case 1:  m_ctColorType = CT_ORANGE;  break;
		case 2:  m_ctColorType = CT_YELLOW;  break;
		case 3:  m_ctColorType = CT_GREEN;   break;
		case 4:  m_ctColorType = CT_SKYBLUE; break;
		case 5:  m_ctColorType = CT_BLUE;    break;
		case 6:  m_ctColorType = CT_PURPLE;  break;
		case 7:  m_ctColorType = CT_PINK;    break;
		}
	}

	// ��� ī��Ʈ�� ��ü �ȼ��� �������� ������, �ڰ��Կ��������� �Ǵ��Ѵ�.
	if(!m_bIsBNWPhoto && skinColorCount > image.GetWidth() * image.GetHeight() * 0.4)
		m_bIsSelfPhoto = TRUE;
}

