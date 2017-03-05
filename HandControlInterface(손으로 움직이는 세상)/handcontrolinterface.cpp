#ifdef _DEBUG // 디버그 모드
#pragma comment(lib, "cxcored.lib")
#pragma comment(lib, "cvd.lib")
#pragma comment(lib, "highguid.lib")
#pragma comment(lib, "cvcam.lib")
#else if NDEBUG // 릴리즈 모드
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "cv.lib")
#pragma comment(lib, "highgui.lib")
#pragma comment(lib, "cvcam.lib")
#endif

#include <cv.h>
#include <highgui.h>

#include <stdio.h>
#include <time.h>

// 영상처리 관련 매크로 상수
#define THRESHOLD_BINARY 40
#define MAX_CUR_COLOR 1000
#define THRESHOLD_HAND_AREA 2000
#define MAX_HAND_AREA 320*240
#define POINT_RATE 0.5	// 기본값: 0.6
#define CIRCLE_SIZE 80
#define PALMDIST_RATE 1.4  // 기본값: 1.18
#define CETA2RADIAN 0.01745329

// 마우스 이벤트 관련 매크로 상수
#define THRESHOLD_LCLICK 10000
#define MOUSE_MODE_INTERVAL 5
#define MODE_STAY 0
#define MODE_MOVE 1
#define MODE_LCLICK 2
#define MODE_RCLICK 3
#define MODE_DRAG 4

//////////////////////////////////////////////////
///// Class Definition ///////////////////////////
//////////////////////////////////////////////////
class PX
{
private:
	double accRate;		// 가속도 상수
	double linRate;		// 선형변환 상수

public:
	int px, py;			// 화면에 있는 마우스 포인트
	int ix, iy;			// 상대적 이동값 계산을 위한 포인트

	PX(){
		accRate = 0.3;	// 기본값: 0.7
		linRate = 3;	// 기본값: 3
		px = py = ix = iy = 0;
	}
	~PX(){}

	void setI(int ix, int iy){
		int offx = ix - this->ix;
		int offy = iy - this->iy;
		this->ix = ix;
		this->iy = iy;
		
		if( abs(offx) + abs(offy) > 300 )
			return;
		
		if( offx >= 0 )
			offx = int((offx * offx * accRate + offx) * linRate);
		else
			offx = int((-offx * offx * accRate + offx) * linRate);
		
		if( offy >= 0 )
			offy = int((offy * offy * (accRate+0.1) + offy) * (linRate+1));
		else
			offy = int((-offy * offy * (accRate+0.1) + offy) * (linRate+1));
		
		px += offx;
		py += offy;
		
		if( px < 0 )
			px = 0;
		if( py < 0 )
			py = 0;
	}
	void setP(int px, int py){
		this->px = px;
		this->py = py;
	}
};

//////////////////////////////////////////////////
///// Global Variable Declaration ////////////////
//////////////////////////////////////////////////
// 윈도우 관련 전역 변수
CvSize g_sizeWin = cvSize( 320, 240 );
char* g_winOrg1 = "Original Image(Frame1)";
char* g_winOrg2 = "Original Image(Frame2)";
char* g_winBinary = "Binary Image";
char* g_winCenter = "Image with Center Point";
char* g_winFinger = "Finger Image";
char* g_winResult = "Result Image";

// 영상 관련 전역 변수
CvCapture* g_capture;		// 캡쳐 영상
IplImage* g_imgOrg1;		// 원본 영상1
IplImage* g_imgOrg2;		// 원본 영상2
IplImage* g_imgBinary;		// 이진 영상
IplImage* g_imgCenter;		// 중심점 검출 영상
IplImage* g_imgFinger;		// 손가락 검출 영상
IplImage* g_imgResult;		// 최종 결과 영상

// 영상처리 관련 전역 변수
int g_monitorWidth;			// 모니터 해상도 너비
int g_monitorHeight;		// 모니터 해상도 높이
IplConvKernel* g_element;	// 모폴로지 연산용 커널
bool g_bTrackingMode;		// 추적 모드 판단
int g_preMaxBlobArea;		// 이전 최대 크기 Blob의 크기
int g_maxBlobArea;			// 최대 크기 Blob의 크기
int g_maxBlobAreaDif;		// 최대 크기 Blob의 이전 크기와의 차이
CvPoint g_ptCenter;			// 손의 중심점
CvPoint g_ptAvgCenter;		// 손의 평균 중심점
CvPoint g_ptPreCenter;		// 이전 손의 중심점
CvPoint g_ptPreFingertip;	// 이전 손가락의 끝점
CvPoint g_ptFingertip;		// 현재 손가락의 끝점
PX g_pxMouse;				// 실제 마우스 포인트
int g_palmCircle;			// 손바닥 원의 크기
int g_fingerCount;			// 손가락 개수 카운터

// 마우스 이벤트 관련 전역 변수
int g_mouseMode;			// 마우스 모드 판단
bool g_bEnterMove;			// MOVE 모드 진입 판단
bool g_bEnterDrag;			// DRAG 모드 진입 판단
bool g_bEnterLclick;		// LCLICK 모드 진입 판단
int g_frameCount;			// 프레임 카운터
int g_moveCount;			// MOVE 모드 간격 카운터
int g_lclickCount;			// LCLICK 모드 간격 카운터
int g_rclickCount;			// RCLICK 모드 간격 카운터

//////////////////////////////////////////////////
///// Global Function Declaration ////////////////
//////////////////////////////////////////////////
void msInit();
void msProcedure();
void msDestroy();

void msPreprocess();
void msLabeling();
bool msLabeling_push(int* stackX, int* stackY, int valX, int valY, int& top, int size);
bool msLabeling_pop(int* stackX, int* stackY, int& valX, int& valY, int& top);
void msDetectCenter();
void msDistanceTransform();
void msClickPerformed();
void msDetectFingers();
void msDetectFingertip();
void msMouseEvent();
bool msMouseEvent_checkInterval(int mode);
void msMouseEvent2();

//////////////////////////////////////////////////
///// Main Function //////////////////////////////
//////////////////////////////////////////////////
void main()
{
	msInit();
	msProcedure();
	msDestroy();
}

//////////////////////////////////////////////////
///// Global Function Definition /////////////////
//////////////////////////////////////////////////
void msInit()
{
	// 전역 변수 초기화
	g_capture = cvCreateCameraCapture(0);
	g_imgBinary = cvCreateImage( g_sizeWin, IPL_DEPTH_8U, 1 );
	g_imgBinary->origin = 1;
	g_imgCenter = cvCreateImage( g_sizeWin, IPL_DEPTH_8U, 3 );
	g_imgCenter->origin = 1;
	cvZero( g_imgCenter );
	g_imgFinger = cvCreateImage( g_sizeWin, IPL_DEPTH_8U, 1 );
	g_imgFinger->origin = 1;
	cvZero( g_imgFinger );
	g_imgResult = cvCreateImage( g_sizeWin, IPL_DEPTH_8U, 3 );
	g_imgResult->origin = 1;
	cvZero( g_imgResult );

	g_monitorWidth = GetSystemMetrics(SM_CXSCREEN);
	g_monitorHeight = GetSystemMetrics(SM_CYSCREEN);
	g_element = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_ELLIPSE, NULL);
	g_bTrackingMode = false;
	g_preMaxBlobArea = 0;
	g_maxBlobAreaDif = 0;
	g_fingerCount = 0;

	g_mouseMode = MODE_MOVE;
	g_bEnterMove = false;
	g_bEnterLclick = false;
	g_frameCount = 0;
	g_moveCount = 0;
	g_lclickCount = 0;
	g_rclickCount = 0;

	// 윈도우 창 초기화
	cvNamedWindow( g_winOrg1, CV_WINDOW_AUTOSIZE );
//	cvNamedWindow( g_winOrg2, CV_WINDOW_AUTOSIZE );
	cvNamedWindow( g_winBinary, CV_WINDOW_AUTOSIZE );
	cvNamedWindow( g_winCenter, CV_WINDOW_AUTOSIZE );
	cvNamedWindow( g_winFinger, CV_WINDOW_AUTOSIZE );
	cvNamedWindow( g_winResult, CV_WINDOW_AUTOSIZE );
}
void msProcedure()
{
	int sTime;
	char key;

//	g_imgOrg1 = cvQueryFrame( g_capture );

	while(1)
	{
		sTime = clock();

		g_imgOrg1 = cvQueryFrame( g_capture );
//		g_imgOrg2 = cvQueryFrame( g_capture );
		if( !g_imgOrg1 /*|| !g_imgOrg2*/ )
			break;

		//cvFlip( g_imgOrg1, g_imgOrg1, 1 );

		////////////////////////
		///// 주 동작 부분 /////
		msPreprocess();
		msLabeling();

		if( g_bTrackingMode )
		{
			printf("[Tracking Mode]\n");

/*			if( g_maxBlobAreaDif > THRESHOLD_LCLICK )
			{
				g_bEnterLclick = true;
			}*/

			msDetectCenter();
			msDistanceTransform();
//			msClickPerformed();
			msDetectFingers();
//			msDetectFingertip();
			msMouseEvent();
//			msMouseEvent2();
		}
		else
			printf("[Non Tracking Mode]\n");
		////////////////////////

		cvShowImage( g_winOrg1, g_imgOrg1 );
//		cvShowImage( g_winOrg2, g_imgOrg2 );
		cvShowImage( g_winBinary, g_imgBinary );
		cvShowImage( g_winCenter, g_imgCenter );
		cvShowImage( g_winFinger, g_imgFinger );
		cvShowImage( g_winResult, g_imgResult );

		printf("경과된 시간: %d \n", clock() - sTime);
		printf("fingerCount: %d \n", g_fingerCount);
		printf("mouseMode: %d \n", g_mouseMode);
		printf("maxBlobAreaDif: %d \n", g_maxBlobAreaDif);

		key = cvWaitKey(33);
		if( key == 27 ) break;
	}
}
void msDestroy()
{
	cvReleaseCapture( &g_capture );
	cvReleaseImage( &g_imgBinary );
	cvReleaseImage( &g_imgCenter );
	cvReleaseImage( &g_imgFinger );
	cvReleaseImage( &g_imgResult );

	cvReleaseStructuringElement( &g_element );

	cvDestroyWindow( g_winOrg1 );
//	cvDestroyWindow( g_winOrg2 );
	cvDestroyWindow( g_winBinary );
	cvDestroyWindow( g_winCenter );
	cvDestroyWindow( g_winFinger );
	cvDestroyWindow( g_winResult );
}

void msPreprocess()
{
	// 원본 영상에 가우시안 필터 적용
	cvSmooth( g_imgOrg1, g_imgOrg1, CV_GAUSSIAN, 5, 5 );

	// Threshold값을 기준으로 이진화
	IplImage* imgTmpBinary = cvCreateImage( g_sizeWin, IPL_DEPTH_8U, 3 );
	cvThreshold( g_imgOrg1, imgTmpBinary, THRESHOLD_BINARY, 255, CV_THRESH_BINARY );
	cvConvertImage( imgTmpBinary, g_imgBinary );

	// 필요 없는 부분 지우기
	int x, y, index;
	for(y = 50; y > 0; y--)
	{
		index = y * g_imgBinary->width;

		for(x = 0; x < g_imgBinary->width; x++)
		{
			g_imgBinary->imageData[index + x] = 0;
		}
	}

	cvReleaseImage( &imgTmpBinary );

	cvZero( g_imgResult );
}
void msLabeling()
{
	cvDilate( g_imgBinary, g_imgBinary, g_element);

	g_maxBlobArea = 0;

	int imgWidth = g_imgBinary->width;
	int imgHeight = g_imgBinary->height;
	int imgSize = imgWidth * imgHeight;

	int x, y, m, n, r, c;
	int stackTop, area, index;
	int blobArea[MAX_CUR_COLOR], maxBlobColor = 0;
	short curColor = 0;

	// 스택으로 사용할 메모리 할당
	int* stackX = new int[imgSize];
	int* stackY = new int[imgSize];

	// 라벨링된 픽셀을 저장하기 위한 메모리 할당
	short* label = new short[imgSize];
	for(int i = 0; i < imgSize; i++)   //초기화
		label[i] = 0;

	for(y = 0; y < imgHeight; y++)
	{
		index = y * imgWidth;

		for(x = 0; x < imgWidth; x++)
		{
			// 이미 방문한 점이거나 픽셀값이 255가 아니면 처리안함
			if( label[index + x] != 0 || (byte)g_imgBinary->imageData[index + x] != 255 )
				continue;

			r = y;
			c = x;
			stackTop = 0;
			area = 1;
			curColor++;

			while(1)
			{
				GRASSFIRE:
				for(m = r-1; m <= r+1; m++)
				{
					index = m * imgWidth;

					for(n = c-1; n <= c+1; n++)
					{
						// 영상경계를 벗어나면 처리안함
						if( m < 0 || m >= imgHeight || n < 0 || n >= imgWidth)
							continue;

						if( (byte)g_imgBinary->imageData[index + n] == 255 && label[index + n] == 0 )
						{
							label[index + n] = curColor;	// 현재 레이블로 마크
							if( !msLabeling_push(stackX, stackY, m, n, stackTop, imgSize) )
								continue;

							r = m;
							c = n;
							area++;
							goto GRASSFIRE;
						}
					}
				}

				if( !msLabeling_pop(stackX, stackY, r, c, stackTop) )
					break;
			}

			if( curColor < MAX_CUR_COLOR )
			{
				blobArea[curColor] = area;

				if( g_maxBlobArea < blobArea[curColor] )
				{
					g_maxBlobArea = area;				// 가장 큰 블랍 영역				 
					maxBlobColor = curColor;		// 가장 큰 블랍 색인
				}
			}
		}
	}

	// 손 크기 임계점보다 크면 남기기
	if( THRESHOLD_HAND_AREA <= g_maxBlobArea && g_maxBlobArea <= MAX_HAND_AREA )
	{
		// 가장 큰 블랍 영역만 남기기
		for(y = 0; y < imgHeight; y++)
		{
			index = y * imgWidth;

			for(x = 0; x < imgWidth; x++)
			{
				if( label[index + x] == maxBlobColor )
					g_imgBinary->imageData[index + x] = (byte)255;
				else
					g_imgBinary->imageData[index + x] = 0;
			}
		}

		cvMorphologyEx( g_imgBinary, g_imgBinary, NULL, g_element, CV_MOP_CLOSE, 2 );

		g_bTrackingMode = true;
	}
	else
	{
		for(y = 0; y < imgHeight; y++)
		{
			index = y * imgWidth;

			for(x = 0; x < imgWidth; x++)
			{
				g_imgBinary->imageData[index + x] = 0;
			}
		}

		g_bTrackingMode = false;
	}

	g_maxBlobAreaDif = g_maxBlobArea - g_preMaxBlobArea;
	g_preMaxBlobArea = g_maxBlobArea;

	delete []stackX;
	delete []stackY;
	delete []label;
}
bool msLabeling_push(int* stackX, int* stackY, int valX, int valY, int& top, int size)
{
	if( top >= size )
		return false;

	top++;
	stackX[top] = valX;
	stackY[top] = valY;
	return true;
}
bool msLabeling_pop(int* stackX, int* stackY, int& valX, int& valY, int& top)
{
	if( top == 0 )
		return false;

	valX = stackX[top];
	valY = stackY[top];
	top--;
	return true;
}
void msDetectCenter()
{
	cvConvertImage( g_imgBinary, g_imgCenter );

	IplImage* imgTemp = cvCreateImage( g_sizeWin, IPL_DEPTH_8U, 1 );

	int imgWidth = g_imgBinary->width;
	int imgHeight = g_imgBinary->height;

	int x, y, index, count, sumX, sumY;

	// g_imgBinary를 g_imgCenter로 복사
	for(y = 0; y < imgHeight; y++)
	{
		index = y * imgWidth;

		for(x = 0; x < imgWidth; x++)
		{
			imgTemp->imageData[index + x] = g_imgBinary->imageData[index + x];
		}
	}

	// 픽셀의 개수를 세어 각 좌표의 합을 구함
	count = sumX = sumY = 0;
	for(y = 0; y < imgHeight; y++)
	{
		index = y * imgWidth;

		for(x = 0; x < imgWidth; x++)
		{
			if( (byte)imgTemp->imageData[index + x] == 255 )
			{
				count++;
				sumX += x;
				sumY += y;
			}
		}
	}

	// 좌표의 합을 이용해 중심점(무게중심)을 구함
	if( count != 0 )
	{
		g_ptCenter.x = sumX / count;
		g_ptCenter.y = sumY / count;
	}
	else
		g_ptCenter.x = g_ptCenter.y = 0;

	// 평균 중심점을 구함
	if( g_ptAvgCenter.x == 0 && g_ptAvgCenter.y == 0 )
		g_ptAvgCenter = g_ptCenter;
	else
	{
		g_ptAvgCenter.x = (int)((double)g_ptCenter.x * POINT_RATE + (double)g_ptAvgCenter.x * (1-POINT_RATE));
		g_ptAvgCenter.y = (int)((double)g_ptCenter.y * POINT_RATE + (double)g_ptAvgCenter.y * (1-POINT_RATE));
	}

	// 중심점을 그림
	cvCircle( g_imgCenter, g_ptAvgCenter, 5, CV_RGB(0, 255, 0), CV_FILLED );
//	cvCircle( g_imgCenter, g_ptAvgCenter, CIRCLE_SIZE, CV_RGB(0, 255, 0) );

	cvReleaseImage( &imgTemp );
}
void msDistanceTransform()
{
	IplImage* imgTemp = cvCreateImage( g_sizeWin, IPL_DEPTH_8U, 1 );
	IplImage* imgDist32F1C = cvCreateImage( g_sizeWin, IPL_DEPTH_32F, 1 );
	IplImage* imgDist32S1C = cvCreateImage( g_sizeWin, IPL_DEPTH_32S, 1 );

	int imgWidth = imgTemp->width;
	int imgHeight = imgTemp->height;

	CvPoint ptCenter, ptAvgCenter = g_ptAvgCenter;

	// 거리 변환 행렬 생성
	float mask[3];
	mask[0] = 1.0f;
	mask[1] = 1.5f;
	mask[2] = 2.0f;

	// 거리 변환(Distance Transform)
	cvDistTransform( g_imgBinary, imgDist32F1C, CV_DIST_USER, 5, mask, NULL );

	// 볼 수 있게 변환
	cvConvertScale( imgDist32F1C, imgTemp, 1.5);

	// 중심점을 구함
	int maxPalmdist = 0;
	int x, y, index;
	for(y = 0; y < imgHeight; y++)
	{
		index = y * imgWidth;
		
		for(x = 0; x < imgWidth; x++)
		{
			if( (byte)imgTemp->imageData[index + x] > maxPalmdist )
			{
				maxPalmdist = (byte)imgTemp->imageData[index + x];
				ptCenter.x = x;
				ptCenter.y = y;
			}
		}
	}

	g_palmCircle = (int)(PALMDIST_RATE * maxPalmdist);

	// 평균 중심점을 구함
	if( ptAvgCenter.x == 0 && ptAvgCenter.y == 0 )
		ptAvgCenter = ptCenter;
	else
	{
		ptAvgCenter.x = (int)((double)ptCenter.x * POINT_RATE + (double)ptAvgCenter.x * (1-POINT_RATE));
		ptAvgCenter.y = (int)((double)ptCenter.y * POINT_RATE + (double)ptAvgCenter.y * (1-POINT_RATE));
	}

	// 중심점과 원을 그려줌
	cvCircle( g_imgCenter, ptAvgCenter, 5, CV_RGB(0, 0, 255), CV_FILLED );
	cvCircle( g_imgCenter, ptAvgCenter, g_palmCircle, CV_RGB(0, 0, 255) );

	// 최종 중심점을 구함
	g_ptCenter.x = (g_ptCenter.x + ptCenter.x)/2;
	g_ptCenter.y = (g_ptCenter.y + ptCenter.y)/2;
	g_ptAvgCenter.x = (g_ptAvgCenter.x + ptAvgCenter.x)/2;
	g_ptAvgCenter.y = (g_ptAvgCenter.y + ptAvgCenter.y)/2;

	cvCircle( g_imgCenter, g_ptAvgCenter, 5, CV_RGB(255, 0, 0), CV_FILLED );
	cvCircle( g_imgCenter, g_ptAvgCenter, g_palmCircle, CV_RGB(255, 0, 0) );

	cvCircle( g_imgResult, g_ptAvgCenter, 5, CV_RGB(255, 0, 0), CV_FILLED );
	cvCircle( g_imgResult, g_ptAvgCenter, g_palmCircle, CV_RGB(255, 0, 0) );

	cvReleaseImage( &imgTemp );
	cvReleaseImage( &imgDist32F1C );
	cvReleaseImage( &imgDist32S1C );
}
void msClickPerformed()
{
}
void msDetectFingers()
{
	IplImage* imgTemp = cvCreateImage( g_sizeWin, IPL_DEPTH_8U, 1 );

	cvZero( imgTemp );
	cvCircle( imgTemp, g_ptAvgCenter, g_palmCircle, CV_RGB(255, 255, 255) );
	cvAnd( g_imgBinary, imgTemp, imgTemp );

	int imgWidth = g_imgBinary->width;
	int imgHeight = g_imgBinary->height;
	int imgSize = imgWidth * imgHeight;

	int x, y, m, n, r, c;
	int stackTop, index;
	short curColor = 0;

	for(y = g_ptAvgCenter.y + 2; y > 0; y--)
	{
		index = y * imgWidth;

		for(x = 0; x < imgWidth; x++)
		{
			imgTemp->imageData[index + x] = 0;
		}
	}

	// 스택으로 사용할 메모리 할당
	int* stackX = new int[imgSize];
	int* stackY = new int[imgSize];

	// 라벨링된 픽셀을 저장하기 위한 메모리 할당
	short* label = new short[imgSize];
	for(int i = 0; i < imgSize; i++)   //초기화
		label[i] = 0;
	
	for(y = 0; y < imgHeight; y++)
	{
		index = y * imgWidth;

		for(x = 0; x < imgWidth; x++)
		{
			// 이미 방문한 점이거나 픽셀값이 255가 아니면 처리안함
			if( label[index + x] != 0 || (byte)imgTemp->imageData[index + x] != 255 )
				continue;

			r = y;
			c = x;
			stackTop = 0;
			curColor++;

			while(1)
			{
				GRASSFIRE:
				for(m = r-1; m <= r+1; m++)
				{
					index = m * imgWidth;

					for(n = c-1; n <= c+1; n++)
					{
						// 영상경계를 벗어나면 처리안함
						if( m < 0 || m >= imgHeight || n < 0 || n >= imgWidth)
							continue;

						if( (byte)imgTemp->imageData[index + n] == 255 && label[index + n] == 0 )
						{
							label[index + n] = curColor;	// 현재 레이블로 마크
							if( !msLabeling_push(stackX, stackY, m, n, stackTop, imgSize) )
								continue;

							r = m;
							c = n;
							goto GRASSFIRE;
						}
					}
				}

				if( !msLabeling_pop(stackX, stackY, r, c, stackTop) )
					break;
			}
		}
	}

	cvCopy( imgTemp, g_imgFinger );

	g_fingerCount = curColor;

	delete []stackX;
	delete []stackY;
	delete []label;
	cvReleaseImage( &imgTemp );
}
void msDetectFingertip()
{
	int imgWidth = g_imgBinary->width;
	int imgHeight = g_imgBinary->height;

	int x, y, index, count, sumX, sumY;
	int ceta, dist;
	double radian;

	// 손가락의 끝점을 구함
	if( g_ptCenter.x != 0 && g_ptCenter.y != 0 )
	{
		CvPoint ptScanStart, ptScanEnd;
		bool bFoundFingertip = false;

		// 손가락의 끝점을 탐색할 구역 설정
		ptScanStart.x = g_ptPreFingertip.x - 10;
		if( ptScanStart.x < 0 )
			ptScanStart.x = 0;
		ptScanStart.y = g_ptPreFingertip.y - 10;
		if( ptScanStart.y < 0 )
			ptScanStart.y = 0;
		ptScanEnd.x = g_ptPreFingertip.x + 10;
		if( ptScanEnd.x > imgWidth )
			ptScanEnd.x = imgWidth;
		ptScanEnd.y = g_ptPreFingertip.y + 10;
		if( ptScanEnd.y > imgHeight )
			ptScanEnd.y = imgHeight;

		// 탐색 구역에서 손가락의 끝점을 찾음
		for(y = ptScanStart.y; y < ptScanEnd.y; y++)
		{
			index = y * imgWidth;

			for(x = ptScanStart.x; x < ptScanEnd.x; x++)
			{
				if( (byte)g_imgBinary->imageData[index + x] == 255 )
				{
					g_ptFingertip.x = x;
					g_ptFingertip.y = y;
					bFoundFingertip = true;

					x = ptScanEnd.x;
					y = ptScanEnd.y;
				}
			}
		}

		// 탐색 구역에서 찾아지지 않을 시 주어진 각도 내에서 찾음
		if( !bFoundFingertip )
		{
			for(ceta = 240; ceta < 275; ceta++)
			{
				radian = ceta * CETA2RADIAN;

				for(dist = 10; dist < 140; dist++)
				{
					x = (int)(dist * cos(radian)) + g_ptCenter.x;
					y = (int)(dist * sin(radian)) + g_ptCenter.y;

					index = y * imgWidth + x;

					if( (byte)g_imgBinary->imageData[index] == 255 )
					{
						if( g_ptFingertip.y > y )
						{
							g_ptFingertip.x = x;
							g_ptFingertip.y = y;
						}
					}
				}
			}
		}

		// 찾아낸 손가락의 끝점을 평균을 이용해 다듬음
		count = sumX = sumY = 0;
		for(y = g_ptFingertip.y; y < g_ptFingertip.y + 10; y++)
		{
			for(x = g_ptFingertip.x - 10; x < g_ptFingertip.x + 10; x++)
			{
				index = y * imgWidth + x;

				if( (byte)g_imgBinary->imageData[index] == 255 )
				{
					count++;
					sumX += x;
					sumY += y;
				}
			}
		}

		if( count != 0 )
		{
			g_ptFingertip.x = sumX / count;
			g_ptFingertip.y = sumY / count;
		}
		else
			g_ptFingertip.x = g_ptFingertip.y = 0;
	}

	// 손가락의 끝점의 평균값을 구함
	if( g_ptPreFingertip.x == 0 && g_ptPreFingertip.y == 0 )
		g_ptPreFingertip = g_ptFingertip;
	else
	{
		g_ptPreFingertip.x = (int)((double)g_ptFingertip.x * POINT_RATE + (double)g_ptPreFingertip.x * (1-POINT_RATE));
		g_ptPreFingertip.y = (int)((double)g_ptFingertip.y * POINT_RATE + (double)g_ptPreFingertip.y * (1-POINT_RATE));
	}

	g_ptFingertip = g_ptPreFingertip;

	// 손가락의 끝점을 그림
	cvCircle( g_imgResult, g_ptFingertip, 7, CV_RGB(0, 0, 255), CV_FILLED );

	// 각 손가락 나누기
	count = 0;
	for(ceta = 0; ceta <= 360; ceta++)
	{
		radian = ceta * CETA2RADIAN;

		// 필요 없는 부분
		if( 0 <= ceta && ceta < 145 )
			continue;

		// 엄지 손가락에 해당되면
		if( 145 <= ceta && ceta <= 215 )
		{
			for(dist = 90; dist <= 100; dist++)
			{
				x = (int)(dist * cos(radian)) + g_ptAvgCenter.x;
				y = (int)(dist * sin(radian)) + g_ptAvgCenter.y;

				if( x <= 0 || x >= imgWidth )
					continue;
				if( y <= 0 || y >= imgHeight )
					continue;

				index = y * imgWidth + x;

				if( (byte)g_imgBinary->imageData[index] == 255 )
				{
					count++;
					g_imgFinger->imageData[index] = (byte)255;
				}
				
				// 엄지 손가락의 마지막 부분에 도달하면
				if( ceta == 215 && count > 30 )
					count = 0;
			}
		}
		// 나머지 손가락에 해당되면
		else
		{
			for(dist = 90; dist <= 100; dist++)
			{
				x = (int)(dist * cos(radian)) + g_ptAvgCenter.x;
				y = (int)(dist * sin(radian)) + g_ptAvgCenter.y;

				if( x <= 0 || x >= imgWidth )
					continue;
				if( y <= 0 || y >= imgHeight )
					continue;

				index = y * imgWidth + x;

				if( (byte)g_imgBinary->imageData[index] == 255 )
				{
					count++;
					g_imgFinger->imageData[index] = (byte)255;
				}
				else if( count > 10 )
				{
					count = 0;
					dist = 90;
				}
			}
		}
	}

	// 팽창 연산을 통해 뚜렷이 보이게 함
	cvDilate( g_imgFinger, g_imgFinger, g_element);
	cvDilate( g_imgFinger, g_imgFinger, g_element);
	cvDilate( g_imgFinger, g_imgFinger, g_element);

	// Contour를 구한 뒤 내부를 칠함
	CvMemStorage* contourStorage = cvCreateMemStorage(0);
	CvSeq* contour = 0;
	double area;

	cvFindContours( g_imgFinger, contourStorage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
	for(; contour != 0; contour = contour->h_next)
	{
		area = fabs( cvContourArea(contour, CV_WHOLE_SEQ) );
		cvDrawContours( g_imgFinger, contour, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), -1, CV_FILLED, 8 );
		g_fingerCount++;
	}
	cvReleaseMemStorage( &contourStorage );
}
void msMouseEvent()
{
	if( g_fingerCount == 2 && g_bEnterDrag )   // 원본: g_fingerCount == 1
	{
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		g_bEnterDrag = false;
		g_lclickCount = 0;
	}

	switch( g_mouseMode )
	{
	case MODE_STAY:
		{
			if( g_fingerCount == 5 )
			{
				if( msMouseEvent_checkInterval(MODE_MOVE) )
				{
					g_mouseMode = MODE_MOVE;
				}
				else if( g_moveCount == 30 )
					g_moveCount = 0;
			}
			else
			{
				g_moveCount = 0;
				g_lclickCount = 0;
				g_rclickCount = 0;
			}

			break;
		}
		
	case MODE_MOVE:
		{
//			if( g_fingerCount == 0 )
//				g_mouseMode = MODE_STAY;

			if( !g_bEnterMove )
			{
				//g_pxMouse.ix = g_ptFingertip.x;
				//g_pxMouse.iy = g_ptFingertip.y;
				g_pxMouse.ix = g_ptAvgCenter.x;
				g_pxMouse.iy = g_ptAvgCenter.y;
				g_bEnterMove = true;
			}

/*			if( g_ptFingertip.y < 120 )
			{
				POINT tmpPoint;
				GetCursorPos(&tmpPoint);
				g_pxMouse.setP(tmpPoint.x, tmpPoint.y);
				g_pxMouse.setI(g_ptFingertip.x, g_ptFingertip.y);
				SetCursorPos(g_pxMouse.px, g_pxMouse.py);
			}*/
//			if( g_ptAvgCenter.y < 120 )
//			{
				POINT tmpPoint;
				GetCursorPos(&tmpPoint);
				g_pxMouse.setP(tmpPoint.x, tmpPoint.y);
				g_ptAvgCenter.y = g_monitorHeight - g_ptAvgCenter.y;
				g_pxMouse.setI(g_ptAvgCenter.x, g_ptAvgCenter.y);
				SetCursorPos(g_pxMouse.px, g_pxMouse.py);
				g_ptAvgCenter.y = g_monitorHeight - g_ptAvgCenter.y;
//			}

/*			if( g_fingerCount == 3 )
			{
				if( msMouseEvent_checkInterval(MODE_RCLICK) )
				{
					g_bEnterMove = false;
					g_mouseMode = MODE_RCLICK;
				}
				else if( g_rclickCount == 30 )
					g_rclickCount = 0;
			}*/
			if( g_fingerCount == 2 )   // 원본: g_fingerCount == 2
			{
				if( msMouseEvent_checkInterval(MODE_LCLICK) )
				{
					g_bEnterMove = false;
					g_mouseMode = MODE_LCLICK;
				}
			}
			else if( g_fingerCount == 0 )
			{
				g_rclickCount++;
				g_lclickCount = g_moveCount = 0;
				//if( msMouseEvent_checkInterval(MODE_RCLICK) )
				if( g_rclickCount == 30 )
				{
					g_bEnterMove = false;
					g_mouseMode = MODE_RCLICK;
				}
				else if( g_rclickCount == 40 ) // 기본값: 30
					g_rclickCount = 0;
			}

			break;
		}

	case MODE_LCLICK:
		{
//			if( g_fingerCount == 0 )
//				g_mouseMode = MODE_STAY;

			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
//			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);   // 추가
			g_bEnterDrag = true;
			g_mouseMode = MODE_MOVE;

			break;
		}

	case MODE_RCLICK:
		{
//			if( g_fingerCount == 0 )
//				g_mouseMode = MODE_STAY;

			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			g_mouseMode = MODE_MOVE;

			break;
		}

	default:
		printf("마우스 이벤트 에러 발생\n");
	}
}
bool msMouseEvent_checkInterval(int mode)
{
	int tmpCount;

	switch( mode )
	{
	case MODE_MOVE:
		{
			tmpCount = ++g_moveCount;
			g_lclickCount = 0;
			g_rclickCount = 0;
			break;
		}
		
	case MODE_LCLICK:
		{
			tmpCount = ++g_lclickCount;
			g_moveCount = 0;
			g_rclickCount = 0;
			break;
		}
		
	case MODE_RCLICK:
		{
			tmpCount = ++g_rclickCount;
			g_moveCount = 0;
			g_lclickCount = 0;
			break;
		}

	default:
		printf("마우스 이벤트 간격 체크 에러 발생\n");
	}

	if( tmpCount == MOUSE_MODE_INTERVAL )
		return true;
	return false;
}
void msMouseEvent2()
{
	switch( g_mouseMode )
	{
	case MODE_MOVE:
		{
			if( !g_bEnterMove )
			{
				g_pxMouse.ix = g_ptAvgCenter.x;
				g_pxMouse.iy = g_ptAvgCenter.y;
				g_bEnterMove = true;
			}

			// 좌클릭 여부 확인
			if( g_maxBlobAreaDif > THRESHOLD_LCLICK )
			{
				g_ptPreCenter = g_ptAvgCenter;
				g_mouseMode = MODE_LCLICK;

				break;
			}

			if( g_bEnterLclick )
			{
				g_pxMouse.setP(g_ptPreCenter.x, g_ptPreCenter.y);
				SetCursorPos(g_pxMouse.px, g_pxMouse.py);
			}
			else
			{
				POINT tmpPoint;
				GetCursorPos(&tmpPoint);
				g_pxMouse.setP(tmpPoint.x, tmpPoint.y);
				g_ptAvgCenter.y = g_monitorHeight - g_ptAvgCenter.y;
				g_pxMouse.setI(g_ptAvgCenter.x, g_ptAvgCenter.y);
				SetCursorPos(g_pxMouse.px, g_pxMouse.py);
				g_ptAvgCenter.y = g_monitorHeight - g_ptAvgCenter.y;
			}

			break;
		}

	case MODE_LCLICK:
		{
			// 일정 시간이 되기 전에 손 영역이 축소되면 클릭 동작
			if( g_maxBlobAreaDif < -THRESHOLD_LCLICK )
			{
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				g_lclickCount = 0;
				g_bEnterMove = false;
				g_mouseMode = MODE_MOVE;

				break;
			}

			// 일정 시간이 되면 DRAG 모드 진입
			if( msMouseEvent_checkInterval(MODE_LCLICK) )
			{
				g_lclickCount = 0;
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				g_mouseMode = MODE_DRAG;
			}

			break;
		}

	case MODE_RCLICK:
		{
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			g_mouseMode = MODE_MOVE;

			break;
		}

	case MODE_DRAG:
		{
			if( g_maxBlobAreaDif < -THRESHOLD_LCLICK )
			{
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				g_bEnterMove = false;
				g_mouseMode = MODE_MOVE;
			}
			else
			{
				POINT tmpPoint;
				GetCursorPos(&tmpPoint);
				g_pxMouse.setP(tmpPoint.x, tmpPoint.y);
				g_ptAvgCenter.y = g_monitorHeight - g_ptAvgCenter.y;
				g_pxMouse.setI(g_ptAvgCenter.x, g_ptAvgCenter.y);
				SetCursorPos(g_pxMouse.px, g_pxMouse.py);
				g_ptAvgCenter.y = g_monitorHeight - g_ptAvgCenter.y;
			}

			break;
		}

	default:
		printf("마우스 이벤트 에러 발생\n");
	}
}


