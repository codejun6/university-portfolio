// GALEditDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PhotoGalmuri.h"
#include "GALEditDialog.h"
#include "afxdialogex.h"

#include "math.h"
#include "GALThumbCtrl.h"
#include "GALImageFeature.h"


// CGALEditDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CGALEditDialog, CDialogEx)

CGALEditDialog::CGALEditDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGALEditDialog::IDD, pParent)
	, m_strCapturedTime(_T(""))
	, m_strAddTag(_T(""))
	, m_bInitializing(FALSE)
	, m_eeEditEffect(EE_NONE)
	, m_nRotateAngle(0)
{
	m_fontStatic.CreatePointFont(100, "���� ���");
	m_fontButton.CreatePointFont(85, "���� ���");
}

CGALEditDialog::~CGALEditDialog()
{
	m_fontStatic.DeleteObject();
	m_fontButton.DeleteObject();
}

void CGALEditDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CAPTUREDTIME, m_strCapturedTime);
	DDV_MaxChars(pDX, m_strCapturedTime, 8);
	DDX_Control(pDX, IDC_EDIT_TAGLIST, m_listTag);
	DDX_Text(pDX, IDC_EDIT_TAG, m_strAddTag);
	DDX_Control(pDX, IDC_EDIT_BSLIDER, m_sliderBrightness);
	DDX_Control(pDX, IDC_EDIT_CSLIDER, m_sliderContrast);
	DDX_Control(pDX, IDC_EDIT_SSLIDER, m_sliderSaturation);
	DDX_Control(pDX, IDC_EDIT_BEDIT, m_editBrightness);
	DDX_Control(pDX, IDC_EDIT_CEDIT, m_editContrast);
	DDX_Control(pDX, IDC_EDIT_SEDIT, m_editSaturation);
	DDX_Control(pDX, IDC_EDIT_SHARPSPIN, m_spinSharp);
	DDX_Control(pDX, IDC_EDIT_SMOOTHSPIN, m_spinSmooth);
}


BEGIN_MESSAGE_MAP(CGALEditDialog, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_DRAWITEM()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_EDIT_DELETE, IDC_EDIT_ADD, &CGALEditDialog::OnBnClickedEditTag)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT_BEDIT, IDC_EDIT_SEDIT, &CGALEditDialog::OnEnChangeEditBCS)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT_SHARPEDIT, IDC_EDIT_SMOOTHEDIT, &CGALEditDialog::OnEnChangeEditSharpSmooth)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_EDIT_ROTATELEFT, IDC_EDIT_ROTATERIGHT, &CGALEditDialog::OnBnClickedEditRotate)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_EDIT_NOEFFECT, IDC_EDIT_MOSAICEFFECT, &CGALEditDialog::OnBnClickedEditEffect)
	ON_BN_CLICKED(IDC_EDIT_INIT, &CGALEditDialog::OnBnClickedEditInit)
	ON_BN_CLICKED(IDC_EDIT_SAVE, &CGALEditDialog::OnBnClickedEditSave)
	ON_BN_CLICKED(IDC_EDIT_SAVEAS, &CGALEditDialog::OnBnClickedEditSaveas)
END_MESSAGE_MAP()


void CGALEditDialog::SetOriginThumb(CGALThumbCtrl* pThumbCtrl)
{
	if(!pThumbCtrl)
		return;
	
	m_pOriginThumb = pThumbCtrl;
}

void CGALEditDialog::UpdateTagListHScroll()
{
	// ���� �� �±��� ���̿� ���� ���� ũ�⸦ ������ ���� ��ũ���� �����Ѵ�.
	CString str;
	CSize size;
	int width = 0;
	CDC* pDC = m_listTag.GetDC();
	CFont* pOldFont = pDC->SelectObject(m_listTag.GetFont());
	for(int i = 0; i < m_listTag.GetCount(); i++)
	{
		m_listTag.GetText(i, str);
		size = pDC->GetTextExtent(str);
		width = max(size.cx, width);
	}
	width += 10;
	m_listTag.SetHorizontalExtent(width);
	pDC->SelectObject(pOldFont);
	m_listTag.ReleaseDC(pDC);
}

void CGALEditDialog::DrawPreviewImage()
{
	if(m_bInitializing)
		return;

	// ���� �̹����� �����ϸ� �̸����� �̹����� �׸���.
	if(!m_imgOrigin.IsNull())
	{
		CClientDC dc(this);

		// �̸����� �̹����� �׷��� ����(���� 700px, ���� 700px)
		CRect imageRect(64, 124, 764, 824);

		// ���� ���۸��� ���� �޸�DC�� �غ��Ѵ�.
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		SetStretchBltMode(memDC, COLORONCOLOR);
		CBitmap memBitmap;
		memBitmap.CreateCompatibleBitmap(&dc, imageRect.Width(), imageRect.Height());
		CBitmap* pOldBitmap = memDC.SelectObject(&memBitmap);

		// ���� �̹����� ���� �ɼ��� ������ �̸����� �̹����� �����Ѵ�.
		EditImageByEditOption(m_imgOrigin, m_imgPreview);

		// �̸����� �̹����� �̸����� ������ �߾ӿ� ���Ľ�Ų��.
		int x = (imageRect.Width() - m_imgPreview.GetWidth()) / 2;
		int y = (imageRect.Height() - m_imgPreview.GetHeight()) / 2;

		// �̸����� �̹����� �׸���.
		m_imgPreview.BitBlt(memDC.GetSafeHdc(), x, y);

		// �޸�DC�� ������ ȭ��DC�� ����Ѵ�.
		dc.BitBlt(imageRect.left, imageRect.top, imageRect.Width(), imageRect.Height(), &memDC, 0, 0, SRCCOPY);

		// �޸𸮸� �����Ѵ�.
		memDC.SelectObject(pOldBitmap);
		memDC.DeleteDC();
	}
}

void CGALEditDialog::EditImageByEditOption(CImage& src, CImage& dest)
{
	// ���� �̹����� �纻�� �ӽ� �̹����� �����Ѵ�.
	CImage tempImage;
	tempImage.Create(src.GetWidth(), src.GetHeight(), src.GetBPP());
	src.BitBlt(tempImage.GetDC(), 0, 0);
	tempImage.ReleaseDC();

	// ���� �̹����� ���, ����, ä�� ������ ���� ���� ������ �ӽ� �̹����� �����Ѵ�.
	ApplyBCSValues(src, tempImage);

	// �ӽ� �̹����� �����ϰ� �ɼ��� �����Ѵ�.
	ApplySharpValue(tempImage);

	// �ӽ� �̹����� �帮�� �ɼ��� �����Ѵ�.
	ApplySmoothValue(tempImage);

	// �ӽ� �̹����� ȿ���� �����Ѵ�.
	switch(m_eeEditEffect)
	{
	case EE_BNW:     ApplyBNWEffect(tempImage);     break;
	case EE_INVERT:  ApplyInvertEffect(tempImage);  break;
	case EE_SEPIA:   ApplySepiaEffect(tempImage);   break;
	case EE_EMBOSS:  ApplyEmbossEffect(tempImage);  break;
	case EE_SKETCH:  ApplySketchEffect(tempImage);  break;
	case EE_VINTAGE: ApplyVintageEffect(tempImage); break;
	case EE_MOSAIC:  ApplyMosaicEffect(tempImage);  break;
	}

	// �ӽ� �̹����� ȸ�� ������ ���� ȸ������ ���� �̹����� �����Ѵ�.
	ApplyRotation(tempImage, dest);
}

void CGALEditDialog::ApplyBCSValues(CImage& src, CImage& dest)
{
	BYTE* imagePtr;
	int intR, intG, intB;
	float hue, saturation, brightness;
	float maxSaturation = 0.5;

	int bValue = m_sliderBrightness.GetPos();           // ��⿡ ���� ���� ��
	float cValue = 1.0;                                 // ������ ���� ���� ��
	cValue += 0.005f * m_sliderContrast.GetPos();
	float sValue = (float)m_sliderSaturation.GetPos();  // ä���� ���� ���� ��
	if(sValue < 0) sValue *= maxSaturation * 0.01f;
	else		   sValue *= (1 -maxSaturation) * 0.01f;

	// ���� �̹����� ���, ����, ä�� ������ ���� ���� ������ ���� �̹����� �����Ѵ�.
	for(int y = 0; y < src.GetHeight(); y++)
	{
		for(int x = 0; x < src.GetWidth(); x++)
		{
			// ���� �̹����� �ȼ��� �����´�.
			imagePtr = (BYTE*)src.GetPixelAddress(x, y);
			intB = (int)*imagePtr;
			intG = (int)*(imagePtr + 1);
			intR = (int)*(imagePtr + 2);

			// ��� ���� �����Ѵ�.
			if(bValue != 0)
			{
				intR += bValue;
				intG += bValue;
				intB += bValue;
				if(intR > 255) intR = 255; else if(intR < 0) intR = 0;
				if(intG > 255) intG = 255; else if(intG < 0) intG = 0;
				if(intB > 255) intB = 255; else if(intB < 0) intB = 0;
			}

			// ���� ���� �����Ѵ�.
			if(cValue != 1.0)
			{
				intR = (int)(intR * cValue);
				intG = (int)(intG * cValue);
				intB = (int)(intB * cValue);
				if(intR > 255) intR = 255; else if(intR < 0) intR = 0;
				if(intG > 255) intG = 255; else if(intG < 0) intG = 0;
				if(intB > 255) intB = 255; else if(intB < 0) intB = 0;
			}

			// ä�� ���� �����Ѵ�.
			if(sValue != 0.0)
			{
				float floatR = (float)intR / 255;
				float floatG = (float)intG / 255;
				float floatB = (float)intB / 255;
				float floatMax = (floatR > floatG)? ((floatR > floatB)? floatR : floatB) : ((floatG > floatB)? floatG :floatB);
				float floatMin = (floatR < floatG)? ((floatR < floatB)? floatR : floatB) : ((floatG < floatB)? floatG :floatB);
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
				if(saturation != 0) saturation += sValue;
				if(saturation > 1.0) saturation = 1.0;
				else if(saturation < 0.0) saturation = 0.0;
				if(saturation == 0)
				{
					intR = (int)(brightness * 255);
					intG = (int)(brightness * 255);
					intB = (int)(brightness * 255);
				}
				else
				{
					float var1, var2, var3;
					float h = hue / 60;
					int i = (int)h;
					var1 = brightness * (1.0f - saturation);
					var2 = brightness * (1.0f - saturation * (h - i));
					var3 = brightness * (1.0f - saturation * (1 - (h - i)));
					switch(i)
					{
					case 0:  floatR = brightness; floatG = var3; floatB = var1; break;
					case 1:  floatR = var2; floatG = brightness; floatB = var1; break;
					case 2:	 floatR = var1; floatG = brightness; floatB = var3; break;
					case 3:	 floatR = var1; floatG = var2; floatB = brightness; break;
					case 4:	 floatR = var3; floatG = var1; floatB = brightness; break;
					default: floatR = brightness; floatG = var1; floatB = var2; break;
					}

					intR = (int)(floatR * 255);
					intG = (int)(floatG * 255);
					intB = (int)(floatB * 255);
				}
				if(intR > 255) intR = 255; else if(intR < 0) intR = 0;
				if(intG > 255) intG = 255; else if(intG < 0) intG = 0;
				if(intB > 255) intB = 255; else if(intB < 0) intB = 0;
			}

			// ���� �̹����� �ȼ��� �����Ѵ�.
			imagePtr = (BYTE*)dest.GetPixelAddress(x, y);
			*imagePtr++ = intB;
			*imagePtr++ = intG;
			*imagePtr = intR;
		}
	}
}

void CGALEditDialog::ApplySharpValue(CImage& image)
{
	if(m_spinSharp.GetPos() != 0)
	{
		// ��� �̹����� �纻 �̹����� �����Ѵ�.
		CImage tempImage;
		tempImage.Create(image.GetWidth(), image.GetHeight(), image.GetBPP());
		image.BitBlt(tempImage.GetDC(), 0, 0);
		tempImage.ReleaseDC();

		// ������ ����
		int filter1[3][3] = {{ 0, -1,  0},
							 {-1,  5, -1},
							 { 0, -1,  0}};
		int filter2[3][3] = {{-1, -1, -1},
							 {-1,  9, -1},
							 {-1, -1, -1}};

		// �ܰ迡 ���� ���͸� �����Ѵ�.
		int (*filter)[3];
		switch(m_spinSharp.GetPos())
		{
		case 1: filter = filter1; break;
		case 2: filter = filter2; break;
		}

		// ��� �̹����� ���͸� �����Ѵ�.
		BYTE* imagePtr;
		int intR, intG, intB, sumR, sumG, sumB;
		for(int y = 1; y < tempImage.GetHeight() - 1; y++)
		{
			for(int x = 1; x < tempImage.GetWidth() - 1; x++)
			{
				sumR = sumG = sumB = 0;
				for(int j = -1; j <= 1; j++)
				{
					for(int i = -1; i <= 1; i++)
					{
						imagePtr = (BYTE*)tempImage.GetPixelAddress(x + i, y + j);
						intB = (int)*imagePtr;
						intG = (int)*(imagePtr+1);
						intR = (int)*(imagePtr+2);

						sumB += intB * filter[j+1][i+1];
						sumG += intG * filter[j+1][i+1];
						sumR += intR * filter[j+1][i+1];
					}
				}
				if(sumR > 255) sumR = 255; else if(sumR < 0) sumR = 0;
				if(sumG > 255) sumG = 255; else if(sumG < 0) sumG = 0;
				if(sumB > 255) sumB = 255; else if(sumB < 0) sumB = 0;

				imagePtr = (BYTE*)image.GetPixelAddress(x, y);
				*imagePtr++ = sumB;
				*imagePtr++ = sumG;
				*imagePtr = sumR;
			}
		}
	}
}

void CGALEditDialog::ApplySmoothValue(CImage& image)
{
	if(m_spinSmooth.GetPos() != 0)
	{
		// ��� �̹����� �纻 �̹����� �����Ѵ�.
		CImage tempImage;
		tempImage.Create(image.GetWidth(), image.GetHeight(), image.GetBPP());
		image.BitBlt(tempImage.GetDC(), 0, 0);
		tempImage.ReleaseDC();

		// ����þ� ����
		int filter1[3][3] = {{1, 2, 1},
							 {2, 4, 2},
							 {1, 2, 1}};
		int filter2[7][7] = {{1, 1, 1,  2, 1, 1, 1},
							 {1, 1, 2,  4, 2, 1, 1},
							 {1, 2, 4,  8, 4, 2, 1},
							 {2, 4, 8, 16, 8, 4, 2},
							 {1, 2, 4,  8, 4, 2, 1},
							 {1, 1, 2,  4, 2, 1, 1},
							 {1, 1, 1,  2, 1, 1, 1}};

		int unit = (m_spinSmooth.GetPos() == 1)? 1 : 3;

		// ��� �̹����� ���͸� �����Ѵ�.
		BYTE* imagePtr;
		float floatR, floatG, floatB, sumR, sumG, sumB;
		for(int y = unit; y < tempImage.GetHeight() - unit; y++)
		{
			for(int x = unit; x < tempImage.GetWidth() - unit; x++)
			{
				sumR = sumG = sumB = 0.0;
				for(int j = -unit; j <= unit; j++)
				{
					for(int i = -unit; i <= unit; i++)
					{
						imagePtr = (BYTE*)tempImage.GetPixelAddress(x + i, y + j);
						floatB = (float)*imagePtr;
						floatG = (float)*(imagePtr+1);
						floatR = (float)*(imagePtr+2);

						if(unit == 1)
						{
							sumB += floatB * filter1[j+unit][i+unit] / 16;
							sumG += floatG * filter1[j+unit][i+unit] / 16;
							sumR += floatR * filter1[j+unit][i+unit] / 16;
						}
						else
						{
							sumB += floatB * filter2[j+unit][i+unit] / 128;
							sumG += floatG * filter2[j+unit][i+unit] / 128;
							sumR += floatR * filter2[j+unit][i+unit] / 128;
						}
					}
				}
				if(sumR > 255) sumR = 255; else if(sumR < 0) sumR = 0;
				if(sumG > 255) sumG = 255; else if(sumG < 0) sumG = 0;
				if(sumB > 255) sumB = 255; else if(sumB < 0) sumB = 0;

				imagePtr = (BYTE*)image.GetPixelAddress(x, y);
				*imagePtr++ = (BYTE)sumB;
				*imagePtr++ = (BYTE)sumG;
				*imagePtr = (BYTE)sumR;
			}
		}
	}
}

void CGALEditDialog::ApplyBNWEffect(CImage& image)
{
	BYTE* imagePtr;
	BYTE byteR, byteG, byteB, byteGray;
	for(int y = 0; y < image.GetHeight(); y++)
	{
		for(int x = 0; x < image.GetWidth(); x++)
		{
			imagePtr = (BYTE*)image.GetPixelAddress(x, y);
			byteB = *imagePtr;
			byteG = *(imagePtr+1);
			byteR = *(imagePtr+2);

			// ��� �ȼ� ���� ����Ѵ�.
			byteGray = (BYTE)(byteR * 0.3 + byteG * 0.59 + byteB * 0.11);

			*imagePtr++ = byteGray;
			*imagePtr++ = byteGray;
			*imagePtr = byteGray;
		}
	}
}

void CGALEditDialog::ApplyInvertEffect(CImage& image)
{
	BYTE* imagePtr;
	BYTE byteR, byteG, byteB;
	for(int y = 0; y < image.GetHeight(); y++)
	{
		for(int x = 0; x < image.GetWidth(); x++)
		{
			imagePtr = (BYTE*)image.GetPixelAddress(x, y);
			byteB = *imagePtr;
			byteG = *(imagePtr+1);
			byteR = *(imagePtr+2);

			// �ȼ��� ������Ų��.
			*imagePtr++ = 255 - byteB;
			*imagePtr++ = 255 - byteG;
			*imagePtr = 255 - byteR;
		}
	}
}

void CGALEditDialog::ApplySepiaEffect(CImage& image)
{
	BYTE* imagePtr;
	float floatR, floatG, floatB, sepiaR, sepiaG, sepiaB;
	for(int y = 0; y < image.GetHeight(); y++)
	{
		for(int x = 0; x < image.GetWidth(); x++)
		{
			imagePtr = (BYTE*)image.GetPixelAddress(x, y);
			floatB = (float)*imagePtr;
			floatG = (float)*(imagePtr+1);
			floatR = (float)*(imagePtr+2);

			// ���Ǿ� �ȼ� ���� ����Ѵ�.
			sepiaR = floatR * 0.393f + floatG * 0.769f + floatB * 0.189f;
			sepiaG = floatR * 0.349f + floatG * 0.686f + floatB * 0.168f;
			sepiaB = floatR * 0.272f + floatG * 0.534f + floatB * 0.131f;
			if(sepiaR > 255) sepiaR = 255; else if(sepiaR < 0) sepiaR = 0;
			if(sepiaG > 255) sepiaG = 255; else if(sepiaG < 0) sepiaG = 0;
			if(sepiaB > 255) sepiaB = 255; else if(sepiaB < 0) sepiaB = 0;

			*imagePtr++ = (BYTE)sepiaB;
			*imagePtr++ = (BYTE)sepiaG;
			*imagePtr = (BYTE)sepiaR;
		}
	}
}

void CGALEditDialog::ApplyEmbossEffect(CImage& image)
{
	// ��� ȿ���� �����Ѵ�.
	ApplyBNWEffect(image);

	// ��� �̹����� �纻 �̹����� �����Ѵ�.
	CImage tempImage;
	tempImage.Create(image.GetWidth(), image.GetHeight(), image.GetBPP());
	image.BitBlt(tempImage.GetDC(), 0, 0);
	tempImage.ReleaseDC();

	// �簢 ȿ�� ����
	int filter[3][3] = {{ 1,  2,  1},
						{ 0,  0,  0},
						{-1, -2, -1}};

	BYTE* imagePtr;
	BYTE byteR, byteG, byteB;
	int sum;
	for(int y = 1; y <tempImage.GetHeight() - 1; y++)
	{
		for(int x = 1; x < tempImage.GetWidth() - 1; x++)
		{
			sum = 0;
			for(int j = -1; j <= 1; j++)
			{
				// ������ �� ��° ���� �ǹ̾��� ���̹Ƿ� �����Ѵ�.
				if(j == 0)
					continue;

				for(int i = -1; i <= 1; i++)
				{
					imagePtr = (BYTE*)tempImage.GetPixelAddress(x + i, y + j);
					byteB = *imagePtr;
					byteG = *(imagePtr+1);
					byteR = *(imagePtr+2);

					sum += byteB * filter[j+1][i+1];
					sum += byteG * filter[j+1][i+1];
					sum += byteR * filter[j+1][i+1];
				}
			}
			sum += 128;
			if(sum > 255) sum = 255; else if(sum < 0) sum = 0;

			imagePtr = (BYTE*)image.GetPixelAddress(x, y);
			*imagePtr++ = (BYTE)sum;
			*imagePtr++ = (BYTE)sum;
			*imagePtr = (BYTE)sum;
		}
	}
}

void CGALEditDialog::ApplySketchEffect(CImage& image)
{
	// ��� ȿ���� �����Ѵ�.
	ApplyBNWEffect(image);

	// ��� �̹����� �纻 �̹����� �����Ѵ�.
	CImage tempImage;
	tempImage.Create(image.GetWidth(), image.GetHeight(), image.GetBPP());
	image.BitBlt(tempImage.GetDC(), 0, 0);
	tempImage.ReleaseDC();

	// ����ġ ȿ�� ����
	int filterX[3][3] = {{-1,  0,  1},
						 {-2,  0,  2},
						 {-1,  0,  1}};
	int filterY[3][3] = {{-1, -2, -1},
						 { 0,  0,  0},
						 { 1,  2,  1}};

	BYTE* imagePtr;
	BYTE byteR, byteG, byteB;
	int sum, sumX, sumY;
	for(int y = 1; y < tempImage.GetHeight() - 1; y++)
	{
		for(int x = 1; x < tempImage.GetWidth() - 1; x++)
		{
			sumX = sumY = 0;
			for(int j = -1; j <= 1; j++)
			{
				for(int i = -1; i <= 1; i++)
				{
					imagePtr = (BYTE*)tempImage.GetPixelAddress(x + i, y + j);
					byteB = *imagePtr;
					byteG = *(imagePtr+1);
					byteR = *(imagePtr+2);

					sumX += byteB * filterX[j+1][i+1];
					sumX += byteG * filterX[j+1][i+1];
					sumX += byteR * filterX[j+1][i+1];
					sumY += byteB * filterY[j+1][i+1];
					sumY += byteG * filterY[j+1][i+1];
					sumY += byteR * filterY[j+1][i+1];
				}
			}
			sum = (int)sqrt((double)(sumX * sumX + sumY * sumY));
			if(sum > 255) sum = 255; else if(sum < 0) sum = 0;

			imagePtr = (BYTE*)image.GetPixelAddress(x, y);
			*imagePtr++ = 255 - (BYTE)sum;
			*imagePtr++ = 255 - (BYTE)sum;
			*imagePtr = 255 - (BYTE)sum;
		}
	}
}

void CGALEditDialog::ApplyVintageEffect(CImage& image)
{
	BYTE* imagePtr;
	BYTE byteR, byteG, byteB, vintageR, vintageG, vintageB;
	for(int y = 0; y < image.GetHeight(); y++)
	{
		for(int x = 0; x < image.GetWidth(); x++)
		{
			imagePtr = (BYTE*)image.GetPixelAddress(x, y);
			byteB = *imagePtr;
			byteG = *(imagePtr+1);
			byteR = *(imagePtr+2);

			// ��Ƽ�� �ȼ� ���� ����Ѵ�.
			vintageR = byteR;
			vintageG = byteG;
			vintageB = (BYTE)(byteR * 0.35 + byteG * 0.35 + (255 - byteB) * 0.12);
			if(vintageR > 255) vintageR = 255; else if(vintageR < 0) vintageR = 0;
			if(vintageG > 255) vintageG = 255; else if(vintageG < 0) vintageG = 0;
			if(vintageB > 255) vintageB = 255; else if(vintageB < 0) vintageB = 0;

			*imagePtr++ = vintageB;
			*imagePtr++ = vintageG;
			*imagePtr = vintageR;
		}
	}
}

void CGALEditDialog::ApplyMosaicEffect(CImage& image)
{
	int unit = 10;  // ������ũ �簢���� ����, ���� �ȼ��� 

	BYTE* imagePtr;
	int avgR, avgG, avgB;
	int tempUnit;
	for(int y = 0; y < image.GetHeight(); y += unit)
	{
		for(int x = 0; x < image.GetWidth(); x += unit)
		{
			// ������ ���� �� �ȼ����� ��� �ȼ� ���� ���Ѵ�.
			avgR = avgG = avgB = tempUnit = 0;
			for(int j = 0; j < unit; j++)
			{
				for(int i = 0; i < unit; i++)
				{
					if(x + i < image.GetWidth() && y + j < image.GetHeight())
					{
						imagePtr = (BYTE*)image.GetPixelAddress(x + i, y + j);
						avgB += *imagePtr++;
						avgG += *imagePtr++;
						avgR += *imagePtr;
					}
					else
					{
						tempUnit++;
					}
				}
			}
			avgR = avgR / (unit * unit - tempUnit);
			avgG = avgG / (unit * unit - tempUnit);
			avgB = avgB / (unit * unit - tempUnit);

			// ������ ���� �� �ȼ����� ��� ������ ��� �ȼ� ������ �����Ѵ�.
			for(int j = 0; j < unit; j++)
			{
				for(int i = 0; i < unit; i++)
				{
					if(x + i < image.GetWidth() && y + j < image.GetHeight())
					{
						imagePtr = (BYTE*)image.GetPixelAddress(x + i, y + j);
						*imagePtr++ = avgB;
						*imagePtr++ = avgG;
						*imagePtr = avgR;
					}
				}
			}
		}
	}
}

void CGALEditDialog::ApplyRotation(CImage& src, CImage& dest)
{
	dest.Destroy();
	if(m_nRotateAngle == 0 || m_nRotateAngle == 180)
	{
		dest.Create(src.GetWidth(), src.GetHeight(), src.GetBPP());

		int tempX = 0;
		int tempY = 0;
		int tempWidth = src.GetWidth();
		int tempHeight = src.GetHeight();

		// ȸ�� ������ 180���� ���� ���������� �̹����� �׸���.
		if(m_nRotateAngle == 180)
		{
			tempX = src.GetWidth();
			tempY = src.GetHeight();
			tempWidth = -src.GetWidth();
			tempHeight = -src.GetHeight();
		}

		// ���� �̹����� ���⿡ ���� ���� �̹����� �����Ѵ�.
		src.StretchBlt(dest.GetDC(), tempX, tempY, tempWidth, tempHeight, SRCCOPY);
		dest.ReleaseDC();
	}
	else if(m_nRotateAngle == 90 || m_nRotateAngle == 270)
	{
		dest.Create(src.GetHeight(), src.GetWidth(), src.GetBPP());

		BYTE* srcPtr;
		BYTE* destPtr;
		for(int y = 0; y < src.GetHeight(); y++)
		{
			for(int x = 0; x < src.GetWidth(); x++)
			{
				// ���� �̹����� ������ �ȼ��� ��ġ�� �����Ѵ�.
				int outX, outY;
				if(m_nRotateAngle == 90)
				{																																																					
					outX = y;
					outY = src.GetWidth() - x - 1;
				}
				else
				{
					outX = src.GetHeight() - y - 1;
					outY = x;
				}

				// ���� �̹����� �ȼ��� ���� �̹����� �����Ѵ�.
				srcPtr = (BYTE*)src.GetPixelAddress(x, y);
				destPtr = (BYTE*)dest.GetPixelAddress(outX, outY);
				*destPtr++ = *srcPtr++;
				*destPtr++ = *srcPtr++;
				*destPtr = *srcPtr;
			}
		}
	}
}


// CGALEditDialog �޽��� ó�����Դϴ�.


BOOL CGALEditDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if(m_pOriginThumb)
	{
		// ����� ��Ʈ���� ���� �̹����� �����´�.
		CImage* pOriginImage = m_pOriginThumb->GetOriginImagePtr();

		// �̹����� �׷��� ����(���� 700px, ���� 700px)
		CRect imageRect(0, 0, 700, 700);
	
		// �̹����� �̸����� �������� ũ�ٸ� ��ҽ�Ų��.
		int width = pOriginImage->GetWidth();
		int height = pOriginImage->GetHeight();
		if(width > imageRect.Width() || height > imageRect.Height())
		{
			double ratio = (width > height)? (double)imageRect.Width() / width : (double)imageRect.Height() / height;
			width = (int)(width * ratio);
			height = (int)(height * ratio);
		}

		// ��ҵ� ���� �̹����� �����Ѵ�.
		m_imgOrigin.Create(width, height, pOriginImage->GetBPP());
		SetStretchBltMode(pOriginImage->GetDC(), COLORONCOLOR);
		pOriginImage->StretchBlt(m_imgOrigin.GetDC(), 0, 0, width, height);
		pOriginImage->ReleaseDC();
		m_imgOrigin.ReleaseDC();
	}

	// ����ƽ �ؽ�Ʈ�� ��Ʈ�� �����Ѵ�.
	HWND hChild = ::FindWindowEx(GetSafeHwnd(), NULL, "Static", NULL);
	while(hChild)
	{
		::SendMessage(hChild, WM_SETFONT, (WPARAM)m_fontStatic.GetSafeHandle(), TRUE);
		hChild = ::FindWindowEx(GetSafeHwnd(), hChild, "Static", NULL);
	}

	// ��ư �ؽ�Ʈ�� ��Ʈ�� �����Ѵ�.
	GetDlgItem(IDC_EDIT_DELETE)->SetFont(&m_fontButton);
	GetDlgItem(IDC_EDIT_ADD)->SetFont(&m_fontButton);
	GetDlgItem(IDC_EDIT_ROTATELEFT)->SetFont(&m_fontButton);
	GetDlgItem(IDC_EDIT_ROTATERIGHT)->SetFont(&m_fontButton);
	GetDlgItem(IDC_EDIT_NOEFFECT)->SetFont(&m_fontButton);
	GetDlgItem(IDC_EDIT_BNWEFFECT)->SetFont(&m_fontButton);
	GetDlgItem(IDC_EDIT_INVERTEFFECT)->SetFont(&m_fontButton);
	GetDlgItem(IDC_EDIT_SEPIAEFFECT)->SetFont(&m_fontButton);
	GetDlgItem(IDC_EDIT_EMBOSSEFFECT)->SetFont(&m_fontButton);
	GetDlgItem(IDC_EDIT_SKETCHEFFECT)->SetFont(&m_fontButton);
	GetDlgItem(IDC_EDIT_VINTAGEEFFECT)->SetFont(&m_fontButton);
	GetDlgItem(IDC_EDIT_MOSAICEFFECT)->SetFont(&m_fontButton);
	GetDlgItem(IDC_EDIT_INIT)->SetFont(&m_fontButton);
	GetDlgItem(IDC_EDIT_SAVE)->SetFont(&m_fontButton);
	GetDlgItem(IDC_EDIT_SAVEAS)->SetFont(&m_fontButton);
	GetDlgItem(IDCANCEL)->SetFont(&m_fontButton);

	// ���� �̹��� ������ ���õ� ��Ʈ���� �ʱ�ȭ�Ѵ�.
	if(m_pOriginThumb)
	{
		CGALImageFeature* pFeature = m_pOriginThumb->GetImageFeature();
		CString str;

		GetDlgItem(IDC_EDIT_FILENAME)->SetWindowText(pFeature->m_strFileName);
		GetDlgItem(IDC_EDIT_FILETYPE)->SetWindowText(pFeature->m_strFileType + " ����");
		GetDlgItem(IDC_EDIT_FILEPATH)->SetWindowText(pFeature->m_strFilePath);
		str.Format("%.1lfKB", pFeature->m_nFileSize);
		GetDlgItem(IDC_EDIT_FILESIZE)->SetWindowText(str);
		str.Format("%d(�ʺ�) x %d(����)", pFeature->m_sizeImage.cx, pFeature->m_sizeImage.cy);
		GetDlgItem(IDC_EDIT_IMAGESIZE)->SetWindowText(str);
		str.Format("%d��Ʈ", pFeature->m_nImageBPP);
		GetDlgItem(IDC_EDIT_BPP)->SetWindowText(str);
		GetDlgItem(IDC_EDIT_CAPTUREDTIME)->SetWindowText(pFeature->m_timeCaptured.Format("%Y%m%d"));

		// �±� ����Ʈ ��Ʈ���� �ʱ�ȭ�Ѵ�.
		for(int i = 0; i < pFeature->m_arrTag.GetCount(); i++)
		{
			m_listTag.AddString("#" + pFeature->m_arrTag[i]);
		}

		// ���� �� �±��� ���̿� ���� ���� ��ũ���� �����Ѵ�.
		UpdateTagListHScroll();
	}

	// �⺻ ��Ʈ���� �ʱ�ȭ�Ѵ�.
	m_sliderBrightness.SetRange(-100, 100, TRUE);
	m_sliderContrast.SetRange(-100, 100, TRUE);
	m_sliderSaturation.SetRange(-100, 100, TRUE);
	m_editBrightness.SetWindowText("0");
	m_editContrast.SetWindowText("0");
	m_editSaturation.SetWindowText("0");
	m_spinSharp.SetRange(0, 2);
	m_spinSmooth.SetRange(0, 2);

	return TRUE;
}


BOOL CGALEditDialog::OnEraseBkgnd(CDC* pDC)
{
	// ������ ����� ĥ�Ѵ�.
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect, RGB(0, 0, 0));

	return TRUE;
}


void CGALEditDialog::OnPaint()
{
	CPaintDC dc(this);

	// ���� �ؽ�Ʈ�� �׸���.
	CFont titleFont;
	titleFont.CreateFont(35, 0, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "���� ���");
	CFont* pOldFont = dc.SelectObject(&titleFont);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255, 255, 255));
	dc.TextOut(60, 60, "��ġ��");
	dc.SelectObject(pOldFont);

	// �̸����� ������ Ʋ�� �׸���.
	CRect previewRect(62, 122, 767, 827);
	CPen framePen(PS_SOLID, 4, RGB(255, 255, 255));
	CPen* pOldPen = dc.SelectObject(&framePen);
	CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	dc.Rectangle(&previewRect);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);

	// �̸����� �̹����� �׸���.
	DrawPreviewImage();
}


HBRUSH CGALEditDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	switch(nCtlColor)
	{
	// �ؽ�Ʈ�� ����� �����ϰ� �ϰ�, �ؽ�Ʈ ������ ������� �����Ѵ�.
	case CTLCOLOR_STATIC:
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 255, 255));
		return (HBRUSH)::GetStockObject(BLACK_BRUSH);
	}

	// �⺻���� �������� ������ �ٸ� �귯�ø� ��ȯ�Ѵ�.
	return hbr;
}


void CGALEditDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch(lpDrawItemStruct->CtlType)
	{
	// ��ư�� ���
	case ODT_BUTTON:
	{
		CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

		// ��ư�� ������ ���, ���� �ٲ��ش�.
		CRect rect(lpDrawItemStruct->rcItem);
		if(lpDrawItemStruct->itemState & ODS_SELECTED)
			pDC->FillSolidRect(&rect, RGB(179, 179, 179));
		else
			pDC->FillSolidRect(&rect, RGB(255, 255, 255));

		// ��ư�� ���� �ؽ�Ʈ�� �׸���.
		CString str;
		GetDlgItem(nIDCtl)->GetWindowText(str);
		pDC->DrawText(str, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}
	break;

	// �� ���� ���
	default:
		CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
		break;
	}
}


void CGALEditDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(pScrollBar != NULL)
	{
		if(pScrollBar->m_hWnd == m_sliderBrightness.m_hWnd)
			SetDlgItemInt(IDC_EDIT_BEDIT, m_sliderBrightness.GetPos());
		else if(pScrollBar->m_hWnd == m_sliderContrast.m_hWnd)
			SetDlgItemInt(IDC_EDIT_CEDIT, m_sliderContrast.GetPos());
		else if(pScrollBar->m_hWnd == m_sliderSaturation.m_hWnd)
			SetDlgItemInt(IDC_EDIT_SEDIT, m_sliderSaturation.GetPos());
	}
	
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CGALEditDialog::OnBnClickedEditTag(UINT nID)
{
	switch(nID)
	{
	case IDC_EDIT_ADD:
	{
		UpdateData(TRUE);

		if(m_strAddTag == "")
			return;
		m_strAddTag.Remove(' ');
		m_strAddTag = "#" + m_strAddTag;

		// �ߺ��� �±״� �߰����� �ʴ´�.
		CString str;
		for(int i = 0; i < m_listTag.GetCount(); i++)
		{
			m_listTag.GetText(i, str);
			if(m_strAddTag == str)
			{
				AfxMessageBox("�̹� �����ϴ� �±��Դϴ�.");
				m_strAddTag = "";
				UpdateData(FALSE);
				return;
			}
		}
	
		// �±� ����Ʈ�� �±׸� �߰��Ѵ�.
		m_listTag.AddString(m_strAddTag);
		m_strAddTag = "";
		UpdateData(FALSE);
	}
	break;

	case IDC_EDIT_DELETE:
		// �±� ����Ʈ���� ������ �±׸� �����Ѵ�.
		m_listTag.DeleteString(m_listTag.GetCurSel());
		break;
	}

	// ���� �� �±��� ���̿� ���� ���� ��ũ���� �����Ѵ�.
	UpdateTagListHScroll();
}


void CGALEditDialog::OnEnChangeEditBCS(UINT nID)
{
	if(m_imgOrigin.IsNull())
		return;

	CString str;
	switch(nID)
	{
	case IDC_EDIT_BEDIT:
		m_editBrightness.GetWindowText(str);
		m_sliderBrightness.SetPos(atoi(str));
		break;

	case IDC_EDIT_CEDIT:
		m_editContrast.GetWindowText(str);
		m_sliderContrast.SetPos(atoi(str));
		break;

	case IDC_EDIT_SEDIT:
		m_editSaturation.GetWindowText(str);
		m_sliderSaturation.SetPos(atoi(str));
		break;
	}

	DrawPreviewImage();
}


void CGALEditDialog::OnEnChangeEditSharpSmooth(UINT nID)
{
	if(m_imgOrigin.IsNull())
		return;

	DrawPreviewImage();
}


void CGALEditDialog::OnBnClickedEditRotate(UINT nID)
{
	if(m_imgOrigin.IsNull())
		return;

	// ȸ�� ��ư�� ���� ȸ�� ������ �����Ѵ�.
	switch(nID)
	{
	case IDC_EDIT_ROTATELEFT:  m_nRotateAngle += 90;  break;
	case IDC_EDIT_ROTATERIGHT: m_nRotateAngle += 270; break;
	}
	if(m_nRotateAngle >= 360)
		m_nRotateAngle -= 360;

	DrawPreviewImage();
}


void CGALEditDialog::OnBnClickedEditEffect(UINT nID)
{
	if(m_imgOrigin.IsNull())
		return;

	switch(nID)
	{
	case IDC_EDIT_BNWEFFECT:     m_eeEditEffect = EE_BNW;     break;
	case IDC_EDIT_INVERTEFFECT:  m_eeEditEffect = EE_INVERT;  break;
	case IDC_EDIT_SEPIAEFFECT:   m_eeEditEffect = EE_SEPIA;	  break;
	case IDC_EDIT_EMBOSSEFFECT:	 m_eeEditEffect = EE_EMBOSS;  break;
	case IDC_EDIT_SKETCHEFFECT:	 m_eeEditEffect = EE_SKETCH;  break;
	case IDC_EDIT_VINTAGEEFFECT: m_eeEditEffect = EE_VINTAGE; break;
	case IDC_EDIT_MOSAICEFFECT:  m_eeEditEffect = EE_MOSAIC;  break;
	default:                     m_eeEditEffect = EE_NONE;    break;
	}

	DrawPreviewImage();
}


void CGALEditDialog::OnBnClickedEditInit()
{
	if(m_imgOrigin.IsNull())
		return;

	// �̸����� �̹����� �ʱ�ȭ�Ǵ� ������ �������� �ʱ� ����
	// �̺�Ʈ �ڵ鷯�� ���� DrawPreviewImage() �Լ��� ������� �ʵ��� �Ѵ�.
	m_bInitializing = TRUE;

	// ��� ���� ������ �ʱ�ȭ�Ѵ�.
	m_strCapturedTime = m_pOriginThumb->GetImageFeature()->m_timeCaptured.Format("%Y%m%d");
	m_listTag.ResetContent();
	if(!m_pOriginThumb->GetImageFeature()->m_arrTag.IsEmpty())
	{
		// �±� ����Ʈ ��Ʈ���� �ʱ�ȭ�Ѵ�.
		CGALImageFeature* pFeature = m_pOriginThumb->GetImageFeature();
		for(int i = 0; i < pFeature->m_arrTag.GetCount(); i++)
		{
			m_listTag.AddString("#" + pFeature->m_arrTag[i]);
		}

		// ���� �� �±��� ���̿� ���� ���� ��ũ���� �����Ѵ�.
		UpdateTagListHScroll();
	}
	m_strAddTag = "";
	m_eeEditEffect = EE_NONE;
	m_nRotateAngle = 0;
	m_editBrightness.SetWindowText("0");
	m_editContrast.SetWindowText("0");
	m_editSaturation.SetWindowText("0");
	m_spinSharp.SetPos(0);
	m_spinSmooth.SetPos(0);

	UpdateData(FALSE);

	// ��� �ʱ�ȭ ��, DrawPreviewImage() �Լ��� ���������� ����ǵ��� �Ѵ�.
	m_bInitializing = FALSE;

	// �ʱ�ȭ�� �̸����� �̹����� �׸���.
	DrawPreviewImage();
}


void CGALEditDialog::OnBnClickedEditSave()
{
	UpdateData(TRUE);

	CGALImageFeature* pFeature = m_pOriginThumb->GetImageFeature();
	CString filePath = pFeature->m_strFilePath;

	// ���� ��¥ ���ڿ��� 8������ ��쿡�� �̹��� Ư¡�� ���� ��¥�� �����Ѵ�.
	CTime newTime(pFeature->m_timeCaptured);
	if(m_strCapturedTime.GetLength() == 8)
	{
		int year = atoi(m_strCapturedTime.Left(4));
		int month = atoi(m_strCapturedTime.Mid(4, 2));
		int day = atoi(m_strCapturedTime.Right(2));

		CTime time(year, month, day, 
			pFeature->m_timeCaptured.GetHour(), 
			pFeature->m_timeCaptured.GetMinute(), 
			pFeature->m_timeCaptured.GetSecond());

		newTime = time;
	}

	// ���� �̹����� ���� �ɼ��� �����Ѵ�.
	CImage* pOriginImage = m_pOriginThumb->GetOriginImagePtr();
	EditImageByEditOption(*pOriginImage, *pOriginImage);

	// �ش� ���ϰ�ο� ������ �̹����� �����Ѵ�.
	pOriginImage->Save(filePath);

	// ������ �̹����� �̹��� Ư¡�� �ٽ� �����Ѵ�.
	pFeature->ExtractImageFeature(filePath, *pOriginImage);
	pFeature->m_timeCaptured = newTime;

	// �̹��� Ư¡�� �±� �迭�� �±� ����Ʈ ��Ʈ���� ������ �����Ѵ�.
	CStringArray* pTagArr = &pFeature->m_arrTag;
	pTagArr->RemoveAll();
	CString str;
	for(int i = 0; i < m_listTag.GetCount(); i++)
	{
		m_listTag.GetText(i, str);
		str.Delete(0);
		pTagArr->Add(str);
	}

	// IDC_EDIT_SAVE ���� ��ȯ�ϸ� ��ȭ���ڸ� �ݴ´�.
	EndDialog(IDC_EDIT_SAVE);
}


void CGALEditDialog::OnBnClickedEditSaveas()
{
	// ���͸��� ���� ���ڿ��� �ʱ�ȭ�Ѵ�.
	char filter[] = "JPG �̹���(*.jpg)|*.jpg|"
					"PNG �̹���(*.png)|*.png|"
					"BMP �̹���(*.bmp)|*.bmp|"
					"GIF �̹���(*.gif)|*.gif||";

	// ���ϰ�ΰ� �ߺ��Ǵ� ���, �ٲ��� ������ �������� ��ȭ���ڸ� �ٽ� ����.
	CGALImageFeature* pFeature = m_pOriginThumb->GetImageFeature();
	while(1)
	{
		// �������� ��ȭ���ڸ� �����ϰ� �����Ѵ�.
		CFileDialog fileDlg(FALSE, NULL, pFeature->m_strFileName, OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST, filter);
		CString fileFormat = pFeature->m_strFileType;
		if(fileFormat == "JPG")		 fileDlg.m_ofn.nFilterIndex = 1;
		else if(fileFormat == "PNG") fileDlg.m_ofn.nFilterIndex = 2;
		else if(fileFormat == "BMP") fileDlg.m_ofn.nFilterIndex = 3;
		else if(fileFormat == "GIF") fileDlg.m_ofn.nFilterIndex = 4;

		// �������� ��ȭ���ڸ� ����.
		// ���� ��ư�� ���� ��쿡�� �̹����� �����Ѵ�.
		if(fileDlg.DoModal() == IDOK)
		{
			CString filePath = fileDlg.GetPathName();
			CString fileName = fileDlg.GetFileName();

			// �ùٸ��� ���� Ȯ���ڸ� �Է��� ���
			fileFormat = fileDlg.GetFileExt().MakeUpper();
			if(fileFormat != "JPG" && fileFormat != "PNG" && fileFormat != "BMP" && fileFormat != "GIF")
			{
				// ���ϰ�ο� �����̸��� ���õ� ���Ϳ� �ش��ϴ� Ȯ���ڸ� ���δ�.
				CString fileExtArr[4] = {".bmp", ".gif", ".jpg", ".png"};
				filePath += fileExtArr[fileDlg.m_ofn.nFilterIndex - 1];
				fileName += fileExtArr[fileDlg.m_ofn.nFilterIndex - 1];
			}

			// ���ϰ�ΰ� �ߺ��Ǹ� ���â�� ����.
			// ���� ������ ����� �����ϸ�, �ƴϿ並 ������ �������� ��ȭ���ڸ� �ٽ� ����.
			if(PathFileExists(filePath))
			{
				if(MessageBox(fileName + "��(��) �̹� �ֽ��ϴ�. �ٲٽðڽ��ϱ�?", "�ٸ� �̸����� ���� Ȯ��", MB_YESNO) == IDNO)
					continue;
			}

			// ���� �̹����� ���� �ɼ��� ������ ���� �̹����� �����Ѵ�.
			CImage* pOriginImage = m_pOriginThumb->GetOriginImagePtr();
			CImage saveImage;
			EditImageByEditOption(*pOriginImage, saveImage);

			// �ش� ���ϰ�ο� ���� �̹����� �����Ѵ�.
			saveImage.Save(filePath);

			AfxMessageBox("�ٸ� �̸����� ������ �Ϸ��߽��ϴ�.");
		}
		break;
	}
}
