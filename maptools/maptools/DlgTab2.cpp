
#include "stdafx.h"
#include "maptools.h"
#include "DlgTab2.h"
#include "afxdialogex.h"
#include <algorithm>
#include <gdiplus.h>

using namespace Gdiplus;

#define distance(ax,ay,bx,by) sqrt(pow(ax - bx,2) + pow(ay - by, 2))
#define findMin(a,b) (a) < (b) ? (a) : (b)

//전역변수들////////////////////
Mat BGimg, originBGimg;
Mat originGrabImg, GrabCutImg;
Mat canny;
CPoint imgCenterPoint;
CPoint distPoint2Center;
Mat originGrab;
//bool isLMouseDown = false; // True : 마우스 왼쪽 누르고 있는 상태 / false : 뗀 상태
bool isStartEdit = false; //OnPaint부분에서 GrabCutImg를 부르기 때문에 써야하는 변수

bool isTransportClick = false; // True : 마우스 왼쪽 누르고 있는 상태 / false : 뗀 상태
bool isResizeClick = false; // True : 마우스 왼쪽 누르고 있는 상태 / false : 뗀 상태
bool isRotationClick= false; // True : 마우스 오른쪽 누르고 있는 상태 / false : 뗀 상태
bool isMouseHover = false;
Mat saveImg, saveOrigin;
CPoint LUpoint, RUpoint, LDpoint, RDpoint;
CPoint IDC_BACK_PIVOT(32, 24);
//////////////////////////////CPoint resizePoint, originCoor;
int clickPointIdx;
int nColPos, nRowPos;
Point2d rotatePivot;
cv::Size scale;
CPoint rBtnStart;
bool isRbtnClick = false;
bool isAutoColor = false;
bool isBgCalled = false;

//심카빙을 위한 변수
vector<int> xEnergyVector, yEnergyVector;
vector<int> rowSeam;
vector<int> downEv;
vector<int> tempEv, originEv;
bool isSeamDel = false;//심카빙삭제버튼 클릭
bool isSeamProtect = false;//심카빙보존버튼 클릭
CPoint delStartP, delEndP;
bool isObjresize = false;
///////////////////////////////

// CDlgTab2 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgTab2, CDialog)

CDlgTab2::CDlgTab2(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG2, pParent)
{
	
}

CDlgTab2::~CDlgTab2()
{
}

void CDlgTab2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, widthSlide);
	DDX_Control(pDX, IDC_SLIDER2, heightSlide);
	DDX_Control(pDX, IDC_SLIDER3, allSlide);
}


BEGIN_MESSAGE_MAP(CDlgTab2, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CDlgTab2::OnNMCustomdrawSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, &CDlgTab2::OnNMCustomdrawSlider2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER3, &CDlgTab2::OnNMCustomdrawSlider3)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_CallBGD, &CDlgTab2::OnBnClickedCallbgd)
//	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_BN_CLICKED(IDC_SeamDelete, &CDlgTab2::OnBnClickedSeamdelete)
	ON_BN_CLICKED(IDC_AutoColorBtn, &CDlgTab2::OnBnClickedAutocolorbtn)
	ON_BN_CLICKED(IDC_SeamProtect, &CDlgTab2::OnBnClickedSeamprotect)
	ON_BN_CLICKED(IDC_SAVEBUTTON, &CDlgTab2::OnBnClickedSavebutton)
END_MESSAGE_MAP()

//이미지 디스플레이를 위한 함수 : 배경
void CDlgTab2::DisplayOutput(int IDC_PICTURE_TARGET, Mat targetMat)
{
	if (targetMat.cols > 956)
		cv::resize(targetMat, targetMat, cv::Size(956, targetMat.rows *  956 / targetMat.cols), 0, 0, CV_INTER_NN);
	if (targetMat.rows > 700)
		cv::resize(targetMat, targetMat, cv::Size(targetMat.cols * 700 / targetMat.rows, 700), 0, 0, CV_INTER_NN);

	if (targetMat.cols % 8 != 0)
		cv::resize(targetMat, targetMat, cv::Size(targetMat.cols - targetMat.cols % 8, targetMat.rows), 0, 0, CV_INTER_NN);

	IplImage* targetIplImage = new IplImage(targetMat);
	if (targetIplImage != nullptr) {
		CWnd* pWnd_ImageTraget = GetDlgItem(IDC_PICTURE_TARGET);
		CClientDC dcImageTraget(pWnd_ImageTraget);
		RECT rcImageTraget;
		pWnd_ImageTraget->GetClientRect(&rcImageTraget);
		rcImageTraget.top = 0;
		rcImageTraget.left = 0;
		rcImageTraget.bottom = targetMat.rows;
		rcImageTraget.right = targetMat.cols;

		BITMAPINFO bitmapInfo;
		memset(&bitmapInfo, 0, sizeof(bitmapInfo));
		bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfo.bmiHeader.biPlanes = 1;
		bitmapInfo.bmiHeader.biCompression = BI_RGB;
		bitmapInfo.bmiHeader.biWidth = targetIplImage->width;
		bitmapInfo.bmiHeader.biHeight = -targetIplImage->height;

		IplImage *tempImage = nullptr;

		if (targetIplImage->nChannels == 1)
		{
			tempImage = cvCreateImage(cvSize(targetIplImage->width, targetIplImage->height), IPL_DEPTH_8U, 3);
			cvCvtColor(targetIplImage, tempImage, CV_GRAY2BGR);
		}
		else if (targetIplImage->nChannels == 3 )
		{
			tempImage = cvCloneImage(targetIplImage);
		}
		else if (targetIplImage->nChannels == 4)
		{
			tempImage = cvCreateImage(cvSize(targetIplImage->width, targetIplImage->height), IPL_DEPTH_32F, 3);
			cvCvtColor(targetIplImage, tempImage, CV_BGRA2BGR);
		}

		bitmapInfo.bmiHeader.biBitCount = tempImage->depth * tempImage->nChannels;
		//RedrawWindow();
		dcImageTraget.SetStretchBltMode(COLORONCOLOR);
		::StretchDIBits(dcImageTraget.GetSafeHdc(), rcImageTraget.left, rcImageTraget.top, rcImageTraget.right, rcImageTraget.bottom,
			0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);

		if (isTransportClick)
			//dcImageTraget.StretchBlt(100, 100, inputImg.cols-100, inputImg.rows-100, &dcImageTraget, 0, 0, inputImg.cols+100, inputImg.rows+100,SRCCOPY);
			cvReleaseImage(&tempImage);
	}
	delete targetIplImage;
}

//이미지 디스플레이를 위한 함수 : (mode=0 : 붙여넣기, mode=1 : 이동, mode=2 : 사이즈조절)
void CDlgTab2::DisplayPasteGrabcut(int IDC_PICTURE_TARGET, Mat targetMat, int mode)
{
	IplImage* targetIplImage = new IplImage(targetMat);
	if (targetIplImage != nullptr) {
		CWnd* pWnd_ImageTraget = GetDlgItem(IDC_PICTURE_TARGET);
		CClientDC dcImageTraget(pWnd_ImageTraget);
		//CRect rcImageTarget;			

		/*pWnd_ImageTraget->GetClientRect(&rcImageTarget);

		rcImageTarget.top = 0;
		rcImageTarget.left = 0;
		rcImageTarget.bottom = targetMat.rows;
		rcImageTarget.right = targetMat.cols;*/

		if (mode == 0)
		{
			LUpoint = imgCenterPoint - CPoint(targetMat.cols, targetMat.rows);
			RUpoint = LUpoint + CPoint(targetMat.cols, 0);
			LDpoint = LUpoint + CPoint(0, targetMat.rows);
			RDpoint = LUpoint + CPoint(targetMat.cols, targetMat.rows);
		}
		else	//-> mode 0을 제외하면 IDC_BACK에 그려지는데 이때는 좌표 (32, 24)을 보정해줘야함
		{
			LUpoint = imgCenterPoint - IDC_BACK_PIVOT - CPoint(targetMat.cols / 2, targetMat.rows / 2);
			RUpoint = LUpoint + CPoint(targetMat.cols, 0);
			LDpoint = LUpoint + CPoint(0, targetMat.rows);
			RDpoint = LUpoint + CPoint(targetMat.cols, targetMat.rows);
		}


		BITMAPINFO bitmapInfo;
		memset(&bitmapInfo, 0, sizeof(bitmapInfo));
		bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfo.bmiHeader.biPlanes = 1;
		bitmapInfo.bmiHeader.biCompression = BI_RGB;
		bitmapInfo.bmiHeader.biWidth = targetIplImage->width;
		bitmapInfo.bmiHeader.biHeight = -targetIplImage->height;

		IplImage *tempImage = nullptr;

		if (targetIplImage->nChannels == 1)
		{
			tempImage = cvCreateImage(cvSize(targetIplImage->width, targetIplImage->height), IPL_DEPTH_8U, 3);
			cvCvtColor(targetIplImage, tempImage, CV_GRAY2BGR);
		}
		else if (targetIplImage->nChannels == 3 || targetIplImage->nChannels == 4)
		{
			tempImage = cvCloneImage(targetIplImage);
		}

		bitmapInfo.bmiHeader.biBitCount = tempImage->depth * tempImage->nChannels;

		dcImageTraget.SetStretchBltMode(COLORONCOLOR);

		if (mode == 0)
		{
			::StretchDIBits(dcImageTraget.GetSafeHdc(), 0, 0, targetMat.cols, targetMat.rows,
				0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
			/*::TransparentBlt(dcImageTraget.GetSafeHdc(), rcImageTarget.left, 0, rcImageTarget.top, rcImageTarget.right, dcImageTraget.GetSafeHdc(), 0, 0, bitmapInfo.bmiHeader.biWidth, bitmapInfo.bmiHeader.biHeight, RGB(0, 0, 0));
			cvReleaseImage(&tempImage);*/

		}
		else if (mode == 1) //이동
		{
			DisplayOutput(IDC_Back, BGimg);
			//DisplayPasteGrabcut(IDC_Paste, GrabCutImg, 0);
			::StretchDIBits(dcImageTraget.GetSafeHdc(), LUpoint.x, LUpoint.y, targetMat.cols, targetMat.rows,
				0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
			cvReleaseImage(&tempImage);
		}
		else if (mode == 2) //크기조절
		{
			DisplayOutput(IDC_Back, BGimg);
			::StretchDIBits(dcImageTraget.GetSafeHdc(), LUpoint.x, LUpoint.y, targetMat.cols, targetMat.rows,
				0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
			cvReleaseImage(&tempImage);
		}
		else if (mode == 3) //회전
		{
			DisplayOutput(IDC_Back, BGimg);

			//CDC sourceDC, destDC;
			//sourceDC.CreateCompatibleDC(NULL);
			//destDC.CreateCompatibleDC(NULL);
			//float cosine = (float)cos(45);
			//float sine = (float)sin(45);
			//int x1 = (int)(targetMat.rows * sine);
			//int y1 = (int)(targetMat.rows * cosine);
			//int x2 = (int)(targetMat.cols * cosine + targetMat.rows * sine);
			//int y2 = (int)(targetMat.rows * cosine - targetMat.cols * sine);
			//int x3 = (int)(targetMat.cols * cosine);
			//int y3 = (int)(-targetMat.cols * sine);
			//int minx = min(0, min(x1, min(x2, x3)));
			//int miny = min(0, min(y1, min(y2, y3)));
			//int maxx = max(0, max(x1, max(x2, x3)));
			//int maxy = max(0, max(y1, max(y2, y3)));

			//int w = maxx - minx;
			//int h = maxy - miny;
			//HBITMAP hbmResult = ::CreateCompatibleBitmap(CClientDC(NULL), w, h);

			//HBITMAP hbmOldSource = (HBITMAP)::SelectObject(sourceDC.m_hDC, );
			//HBITMAP hbmOldDest = (HBITMAP)::SelectObject(destDC.m_hDC, hbmResult);

			//// Draw the background color before we change mapping mode
			//HBRUSH hbrBack = CreateSolidBrush(RGB(255,0,0));
			//HBRUSH hbrOld = (HBRUSH)::SelectObject(destDC.m_hDC, hbrBack);
			//destDC.PatBlt(0, 0, w, h, PATCOPY);
			//::DeleteObject(::SelectObject(destDC.m_hDC, hbrOld));

			//// We will use world transform to rotate the bitmap
			//SetGraphicsMode(destDC.m_hDC, GM_ADVANCED);
			//XFORM xform;
			//xform.eM11 = cosine;
			//xform.eM12 = -sine;
			//xform.eM21 = sine;
			//xform.eM22 = cosine;
			//xform.eDx = (float)-minx;
			//xform.eDy = (float)-miny;

			//SetWorldTransform(destDC.m_hDC, &xform);

			//// Now do the actual rotating - a pixel at a time
			//destDC.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &sourceDC, 0, 0, SRCCOPY);

			//// Restore DCs
			//::SelectObject(sourceDC.m_hDC, hbmOldSource);
			//::SelectObject(destDC.m_hDC, hbmOldDest);

			::StretchDIBits(dcImageTraget.GetSafeHdc(), LUpoint.x, LUpoint.y, targetMat.cols, targetMat.rows,
				0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
			//::TransparentBlt(dcImageTraget.GetSafeHdc(), rcImageTarget.left, rcImageTarget.right, rcImageTarget.top, rcImageTarget.right, dcImageTraget.GetSafeHdc(), 0, 0, bitmapInfo.bmiHeader.biWidth, bitmapInfo.bmiHeader.biHeight, RGB(0, 0, 0));
			cvReleaseImage(&tempImage);
		}

		else if (mode == 4) //마우스오버
		{

			CBrush brush;
			brush.CreateSolidBrush(RGB(255, 230, 110));     // 채움색 생성
			CBrush* oldBrush = dcImageTraget.SelectObject(&brush);
			dcImageTraget.Rectangle(LUpoint.x, LUpoint.y, LUpoint.x + 10, LUpoint.y + 10);
			dcImageTraget.Rectangle(RUpoint.x, RUpoint.y, RUpoint.x - 10, RUpoint.y + 10);
			dcImageTraget.Rectangle(LDpoint.x, LDpoint.y, LDpoint.x + 10, LDpoint.y - 10);
			dcImageTraget.Rectangle(RDpoint.x, RDpoint.y, RDpoint.x - 10, RDpoint.y - 10);
		
		}
		//if (isLeftMouseDown)
		//	dcImageTraget.StretchBlt(0, 0, GrabCutImg.cols-100, GrabCutImg.rows-100, &dcImageTraget, 0, 0, GrabCutImg.cols, GrabCutImg.rows,SRCCOPY);

	}
	delete targetIplImage;
}

//평균 구하는 함수
int CDlgTab2::kMeanss(int clusterMemberCount, int sum, int mode)
{
	int A;

	
	A = sum / clusterMemberCount;


	return A;
}

const float eps = 1.0e-5;
//RGB값을 YCbCr값으로
cv::Vec3d CDlgTab2::RGBtoYCbCr(cv::Vec3d bgr)
{
/*	int A;
//	for (int y = 0; y < img.rows; y++)
//	{
//		for (int x = 0; x < img.cols; x++)
		{
	if (mode == 0) // Y
		A = 0.257 * img.at<Vec3b>(y, x)[2] + (0.504 * img.at<Vec3b>(y, x)[1]) + (0.0098 * img.at<Vec3b>(y, x)[0]) + 16;
	//			A.push_back((0.299 * img.at<Vec3b>(y, x)[2]) + (0.587 * img.at<Vec3b>(y, x)[1]) + (0.114 * img.at<Vec3b>(y, x)[0]));
	else if (mode == 1) // Cb
		A = (-0.148) * img.at<Vec3b>(y, x)[2] - (0.291 * img.at<Vec3b>(y, x)[1]) + (0.439 * img.at<Vec3b>(y, x)[0]) + 128;
	//			A.push_back(((-0.16874) * img.at<Vec3b>(y, x)[2]) - (0.33126 * img.at<Vec3b>(y, x)[1]) + (0.5 * img.at<Vec3b>(y, x)[0]));
	else if (mode == 2) // Cr
		A = (0.439 * img.at<Vec3b>(y, x)[2]) - (0.368 * img.at<Vec3b>(y, x)[1]) - (0.071 * img.at<Vec3b>(y, x)[0]) + 128;
	//			A.push_back((0.5 * img.at<Vec3b>(y, x)[2]) - (0.41869 * img.at<Vec3b>(y, x)[1]) - (0.08131 * img.at<Vec3b>(y, x)[0]));
//		}
//	}
	return A;*/

	cv::Vec3d YCbCr;

	double Y = 0.257 * bgr[2] + (0.504 * bgr[1]) + (0.0098 * bgr[0]) + 16;
	double Cb = (-0.148) * bgr[2] - (0.291 * bgr[1]) + (0.439 * bgr[0]) + 128;
	double Cr = (0.439 * bgr[2]) - (0.368 * bgr[1]) - (0.071 * bgr[0]) + 128;
	YCbCr[0] = Y > eps ? Y : eps;
	YCbCr[1] = Cb > eps ? Cb : eps;
	YCbCr[2] = Cr > eps ? Cr : eps;
	return YCbCr;
}


//YCbCr값을 RGB값으로
cv::Vec3d CDlgTab2::YCbCrtoRGB(cv::Vec3d YCbCr) {
	cv::Vec3d bgr;

	/*	double L = 1 / std::sqrt(3) * lab[0] + 1 / std::sqrt(6) * lab[1] + 1 / std::sqrt(2) * lab[2];
	double M = 1 / std::sqrt(3) * lab[0] + 1 / std::sqrt(6) * lab[1] - 1 / std::sqrt(2) * lab[2];
	double S = 1 / std::sqrt(3) * lab[0] - 2 / std::sqrt(6) * lab[1];

	LMS[0] = L > -5 ? std::pow(10, L) : eps;
	LMS[1] = M > -5 ? std::pow(10, M) : eps;
	LMS[2] = S > -5 ? std::pow(10, S) : eps;

	bgr[0] = 0.0497 * LMS[0] - 0.2439 * LMS[1] + 1.2045 * LMS[2];
	bgr[1] = -1.2186 * LMS[0] + 2.3809 * LMS[1] - 0.1624 * LMS[2];
	bgr[2] = 4.4679 * LMS[0] - 3.5873 * LMS[1] + 0.1193 * LMS[2];
	*/

	double Y = (1.164 * (YCbCr[0] - 16)) + (2.018 * (YCbCr[1] - 128));
	double Cb = (1.164 * (YCbCr[0] - 16)) - (0.813 * (YCbCr[2] - 128)) - (0.391 * (YCbCr[1] - 128));
	double Cr = (1.164 * (YCbCr[0] - 16)) + (1.596 * (YCbCr[2] - 128));

	bgr[0] = Y > -5 ? Y : eps;
	bgr[1] = Cb > -5 ? Cb : eps;
	bgr[2] = Cr > -5 ? Cr : eps;

	return bgr;
}

//두 이미지를 합성하는 함수
void CDlgTab2::Compose(Mat BGMat, Mat originMat, int target_X, int target_Y, int mode)
{
	int clusterMemberCount = 0, firstCount = 0;
	float bgRatio = 0, objectRatio = 1, dist = 0;
	int arr[24][2] = { 
			{ -2,-2 }, { -2,-1 }, { -2,0 }, { -2,1 }, { -2,2 },
			{ -1,-2 }, { -1,-1 }, { -1,0 }, { -1,1 }, { -1,2 },
			{ 0,-2 },  { 0,-1 },			{ 0,1 },  { 0,2 },
			{ 1,2 },   { 1,-1 }, { 1,0 },   { 1,1 },  { 1,2 },
			{ 2,-2 },  { 2,-1 }, { 2,0 },   { 2,1 },  { 2,2 }
					};

	//YCbCr
	cv::Mat objBGR, objYCbCr, bgBGR, bgYCbCr, dst, dstBGR;
	cv::Vec3d objAvr, objSd, bgAvr, bgSd;
//	Mat obj, bg;

	originMat.convertTo(objBGR, CV_64FC3, 1 / 255.0);
	BGMat.convertTo(bgBGR, CV_64FC3, 1 / 255.0);

	objYCbCr = cv::Mat(objBGR.size(), CV_64FC3);
	bgYCbCr = cv::Mat(bgBGR.size(), CV_64FC3);

	objAvr = cv::Vec3d(0, 0, 0);
	bgAvr = cv::Vec3d(0, 0, 0);

	for (int i = 0;i < originMat.cols;i++)
	{
		for (int j = 0;j < originMat.rows;j++)
		{
			//RGB -> YCbCr
			objYCbCr.at<cv::Vec3d>(j, i) = RGBtoYCbCr(objBGR.at<cv::Vec3d>(j, i));

			if (j + target_Y >= BGMat.rows || i + target_X >= BGMat.cols || j + target_Y < 0 || i + target_X < 0)	continue;
			else bgYCbCr.at<cv::Vec3d>(j + target_Y, i + target_X) = RGBtoYCbCr(bgBGR.at<cv::Vec3d>(j + target_Y, i + target_X));
		}
	}

	for (int i = 0;i < originMat.cols;i++)
	{

		for (int j = 0;j < originMat.rows;j++)
		{
			if (originMat.at<Vec3b>(j, i)[0] != 4 || originMat.at<Vec3b>(j, i)[1] != 8 || originMat.at<Vec3b>(j, i)[2] != 6)
			{
				cv::Vec3d* objBuf = &objYCbCr.at<cv::Vec3d>(j, i);
				objAvr[0] += (*objBuf)[0];
				objAvr[1] += (*objBuf)[1];
				objAvr[2] += (*objBuf)[2];
				if (j + target_Y >= BGMat.rows || i + target_X >= BGMat.cols || j + target_Y < 0 || i + target_X < 0)	continue;
				else
				{
					cv::Vec3d* bgBuf = &bgYCbCr.at<cv::Vec3d>(j + target_Y, i + target_X);
					bgAvr[0] += (*bgBuf)[0];
					bgAvr[1] += (*bgBuf)[1];
					bgAvr[2] += (*bgBuf)[2];
				}
				clusterMemberCount++;
			}
		}
	}
	//YCbCr 평균구하기
	for (int c = 0; c < 3; c++)
	{
		objAvr[c] /= clusterMemberCount;
		bgAvr[c] /= clusterMemberCount;
	}

	//표준편차 계산
	objSd = cv::Vec3d(0, 0, 0);
	bgSd = cv::Vec3d(0, 0, 0);
	for (int i = 0;i < originMat.cols;i++)
	{

		for (int j = 0;j < originMat.rows;j++)
		{
			if (originMat.at<Vec3b>(j, i)[0] != 4 || originMat.at<Vec3b>(j, i)[1] != 8 || originMat.at<Vec3b>(j, i)[2] != 6)
			{
				cv::Vec3d* objBuf = &objYCbCr.at<cv::Vec3d>(j, i);
				objSd[0] += pow(((*objBuf)[0] - objAvr[0]), 2);
				objSd[1] += pow(((*objBuf)[1] - objAvr[1]), 2);
				objSd[2] += pow(((*objBuf)[2] - objAvr[2]), 2);

				if (j + target_Y >= BGMat.rows || i + target_X >= BGMat.cols || j + target_Y < 0 || i + target_X < 0)	continue;
				else {
					cv::Vec3d* bgBuf = &bgYCbCr.at<cv::Vec3d>(j + target_Y, i + target_X);
					bgSd[0] += pow(((*bgBuf)[0] - bgAvr[0]), 2);
					bgSd[1] += pow(((*bgBuf)[1] - bgAvr[1]), 2);
					bgSd[2] += pow(((*bgBuf)[2] - bgAvr[2]), 2);
				}
			}
		}
	}
	for (int c = 0; c < 3; ++c) {
		objSd[c] /= clusterMemberCount;
		objSd[c] = std::sqrt(objSd[c]);

		bgSd[c] /= clusterMemberCount;
		bgSd[c] = std::sqrt(bgSd[c]);
	}

	//더한값 뿌리기
	if (isAutoColor)
	{
		for (int i = 0;i < originMat.cols;i++)
		{

			for (int j = 0;j < originMat.rows;j++)
			{
				if (originMat.at<Vec3b>(j, i)[0] != 4 || originMat.at<Vec3b>(j, i)[1] != 8 || originMat.at<Vec3b>(j, i)[2] != 6)
				{
					cv::Vec3d* buf = &objYCbCr.at<cv::Vec3d>(j, i);
					float Y = (bgSd[0] * 3) / objSd[0] * ((*buf)[0] - objAvr[0]) + bgAvr[0];
					float Cb = (bgSd[1] * 3) / objSd[1] * ((*buf)[1] - objAvr[1]) + bgAvr[1];
					float Cr = (bgSd[2] * 3) / objSd[2] * ((*buf)[2] - objAvr[2]) + bgAvr[2];

					objBGR.at<cv::Vec3d>(j, i) = YCbCrtoRGB(cv::Vec3d(Y, Cb, Cr));
				}
			}
		}
		objBGR.convertTo(saveImg, CV_8UC3, 255.0);
	}
	else saveOrigin.copyTo(saveImg);
	for (int i = 0;i < originMat.cols;i++)
	{
		for (int j = 0;j < originMat.rows;j++)
		{
			int secretCount = 0;
			//외관선 부드럽게
		/*	if (canny.data && canny.at<uchar>(j, i) != 0)
			{
			float aa = sqrt(pow(saveImg.cols / 2 - i, 2));
			dist = (sqrt(pow(saveImg.cols / 2 - i, 2)) / (saveImg.cols / 2)) + (sqrt(pow(saveImg.rows / 2 - j, 2)) / (saveImg.rows / 2));
			bgRatio = dist * 0.5;
			bgRatio = bgRatio < 0 ? 0 : bgRatio;
			bgRatio = bgRatio > 1 ? 1 : bgRatio;
			objectRatio = 1 - bgRatio;
			if (j + target_Y >= BGMat.rows || i + target_X >= BGMat.cols || j + target_Y < 0 || i + target_X < 0)	continue;
			else {
				originMat.at<Vec3b>(j, i)[0] = (BGMat.at<Vec3b>(j + target_Y, i + target_X)[0] * bgRatio) + (originMat.at<Vec3b>(j, i)[0] * objectRatio);
				originMat.at<Vec3b>(j, i)[1] = (BGMat.at<Vec3b>(j + target_Y, i + target_X)[1] * bgRatio) + (originMat.at<Vec3b>(j, i)[1] * objectRatio);
				originMat.at<Vec3b>(j, i)[2] = (BGMat.at<Vec3b>(j + target_Y, i + target_X)[2] * bgRatio) + (originMat.at<Vec3b>(j, i)[2] * objectRatio);
			}
			}*/


			if (originMat.at<Vec3b>(j, i)[0] == 4 && originMat.at<Vec3b>(j, i)[1] == 8 && originMat.at<Vec3b>(j, i)[2] == 6)
			{
				if (j + target_Y >= BGMat.rows || i + target_X >= BGMat.cols || j + target_Y < 0 || i + target_X < 0)
				{//배경 밖을 나가면 회색으로 덮음
					saveImg.at<Vec3b>(j, i)[0] = 240;
					saveImg.at<Vec3b>(j, i)[1] = 240;
					saveImg.at<Vec3b>(j, i)[2] = 240;
				}
				else saveImg.at<Vec3b>(j, i) = BGimg.at<Vec3b>(j + target_Y, i + target_X);
			}
			else if (j + target_Y >= BGMat.rows || i + target_X >= BGMat.cols || j + target_Y < 0 || i + target_X < 0)
			{//배경 밖을 나가면 회색으로 덮음
				saveImg.at<Vec3b>(j, i)[0] = 240;
				saveImg.at<Vec3b>(j, i)[1] = 240;
				saveImg.at<Vec3b>(j, i)[2] = 240;
			}
		}
	}
}

//심카빙을 위한 에너지 계산
void CDlgTab2::calcEnergy(const Mat& img, Mat& sobelMat)
{

	int energy = 0;
	int arr[8][2] = {
			 { -1,-1 }, { -1,0 }, { -1,1 },
			 { 0,-1 },			  { 0,1 },
			 { 1,-1 }, { 1,0 },   { 1,1 } };


	for (int y = 0; y < img.rows; y++)
	{
		//printf("%d  \n", y);
		for (int x = 0; x < img.cols; x++)
		{
			for (int i = 0; i < 8; i++)
			{
				if (y + arr[i][0] < 0 || x + arr[i][1] < 0 || y + arr[i][0] >= img.rows || x + arr[i][1] >= img.cols)
					continue;

				energy += sqrt(pow(sobelMat.at<uchar>(y, x) - sobelMat.at<uchar>(y + arr[i][0], x + arr[i][1]), 2));
			}

		//	if (y == 0) xEnergyVector.push_back(energy);
			downEv.push_back(energy);
			
			//		isVist.push_back(energy);

			energy = 0;
		}


	}
}

//심카빙 실행
int CDlgTab2::seamcarving(int startPoint, int mode, int cutCount)
{
	//mode0 : 넓이 축소/확대용 계산, mode1 : 높이 축소/확대용 계산, mode2 : 실제 축소(심카빙) 작동, mode3 : 실제 확대(심카빙) 작동
	
	int ret = 0;
	//심카빙 넓이 축소/확대용 계산
	if (mode == 0)
	{
		for (int startX = 0; startX < BGimg.cols; startX++)
		{
			if (startX == 0 || startX == BGimg.cols - cutCount || startPoint == 0 || startPoint == BGimg.rows) ret = 0;

			else
			{
				//printf("%d   \n", startPointX);
				int minTemp = BGimg.cols * BGimg.rows * 1000;
				int maxTemp = 0;
				int tmpX = 0;
				for (int x = -1; x <= 1; x++)
				{

					if (x + startX < 0 || x + startX > BGimg.cols - cutCount - 1) continue;

					minTemp = findMin(downEv[(startPoint - 1) * BGimg.cols + startX + x], minTemp);
					if (minTemp == downEv[(startPoint - 1) * BGimg.cols + startX + x])	tmpX = x + startX;
				}

				downEv[startPoint * BGimg.cols + tmpX] += minTemp;
				
				ret += downEv[startPoint * BGimg.cols + tmpX];
				//startX = tmpX;
			}

			if (startPoint == BGimg.rows - 1)
				tempEv.push_back(downEv[(BGimg.rows - 1) * BGimg.cols + startX]);
		}
	}

	//높이 축소/확대용 계산
	else if (mode == 1)
	{
		for (int startY = 0; startY < BGimg.rows; startY++)
		{
			if (startY == 0 || startY == BGimg.rows - cutCount || startPoint == 0 || startPoint == BGimg.cols) ret = 0;

			else
			{
				//printf("%d   \n", startPointX);
				int minTemp = BGimg.cols * BGimg.rows * 1000;
				int maxTemp = 0;
				int tmpY = 0;
				for (int y = -1; y <= 1; y++)
				{

					if (y + startY < 0 || y + startY > BGimg.rows - cutCount - 1) continue;

					minTemp = findMin(downEv[(startY + y) * BGimg.cols + (startPoint-1)], minTemp);
					if (minTemp == downEv[(startY + y) * BGimg.cols + (startPoint-1)])	tmpY = y + startY;
				}

				downEv[tmpY * BGimg.cols + startPoint] += minTemp;
				ret += downEv[tmpY * BGimg.cols + startPoint];
				//startX = tmpX;
			}
		}
	}

	//넓이 축소
	else if (mode == 2)
	{
		for (int y = BGimg.rows - 1; y > 0; y--)
		{
			if (startPoint == 0 || startPoint == BGimg.cols - cutCount || y == 0 || y == BGimg.rows) ret = 0;

			else
			{
				//printf("%d   \n", startPointX);
				int minTemp = BGimg.cols * BGimg.rows * 1000;
				int maxTemp = 0;
				int tmpX = 0;
				for (int x = -1; x <= 1; x++)
				{

					if (x + startPoint < 0 || x + startPoint > BGimg.cols - cutCount - 1) continue;

					minTemp = findMin(downEv[(y - 1) * BGimg.cols + startPoint + x], minTemp);
					if (minTemp == downEv[(y - 1) * BGimg.cols + startPoint + x])	tmpX = x + startPoint;
				}

				//ret += minTemp;
				startPoint = tmpX;
			}

			for (int c = startPoint; c < BGimg.cols - cutCount; c++)
			{
				if (c == BGimg.cols - cutCount - 1)
				{
					BGimg.at<Vec3b>(y - 1, c) = 0;
					///////if (y != 1)	downEv.erase(downEv.begin() + (y - 1) * BGimg.cols - cutCount);
				}

				else
				{
					//a.at<uchar>(y, c) = a.at<uchar>(y, c + 1);
					downEv[(y - 1) * BGimg.cols + c] = downEv[(y - 1) * BGimg.cols + c + 1];
					BGimg.at<Vec3b>(y - 1, c) = BGimg.at<Vec3b>(y - 1, c + 1);

					//		if (y == image.rows - 1)	ev[(image.rows - 1) * image.cols + c] = ev[(image.rows - 1) * image.cols + c + 1];
				}
			}
		}
	}

	//넓이 확대
	else if (mode == 3)
	{
		originEv.assign(downEv.begin(), downEv.end());
		for (int y = BGimg.rows - 1; y > 0; y--)
		{
			if (startPoint == 0 || startPoint == BGimg.cols || y == 0 || y == BGimg.rows) ret = 0;

			else
			{
				//printf("%d   \n", startPointX);
				int minTemp = BGimg.cols * BGimg.rows * 1000;
				int maxTemp = 0;
				int tmpX = 0;
				for (int x = -1; x <= 1; x++)
				{

					if (x + startPoint < 0 || x + startPoint > BGimg.cols - 1) continue;

					minTemp = findMin(downEv[(y - 1) * BGimg.cols + startPoint + x], minTemp);
					if (minTemp == downEv[(y - 1) * BGimg.cols + startPoint + x])	tmpX = x + startPoint;
				}

				//ret += minTemp;
				startPoint = tmpX;
			}

			for (int c = startPoint; c < BGimg.cols; c++)
			{
				if (c == BGimg.cols - 1)
					BGimg.at<Vec3b>(y - 1, c) = originBGimg.at<Vec3b>(y - 1, c);
					
				else
				{
					downEv[(y - 1) * BGimg.cols + c + 1] = originEv[(y - 1) * BGimg.cols + c ];
					BGimg.at<Vec3b>(y - 1, c + 1) = originBGimg.at<Vec3b>(y - 1, c);

				}
			}
		}
	}


	return ret;
}

// CDlgTab2 메시지 처리기입니다.

void CDlgTab2::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.
	if (!isStartEdit)
	{
		originGrab = imread("originInput.png", CV_LOAD_IMAGE_UNCHANGED);
		GrabCutImg = imread("output.png", CV_LOAD_IMAGE_UNCHANGED);
		if (GrabCutImg.cols % 8 != 0)
			cv::resize(GrabCutImg, GrabCutImg, cv::Size(GrabCutImg.cols - GrabCutImg.cols % 8, GrabCutImg.rows), 0, 0, CV_INTER_NN);
		if (originGrab.cols % 8 != 0)
			cv::resize(originGrab, originGrab, cv::Size(originGrab.cols - originGrab.cols % 8, originGrab.rows), 0, 0, CV_INTER_NN);

		for (int i = 0;i < GrabCutImg.cols;i++)
		{
			for (int j = 0;j < GrabCutImg.rows;j++)
			{
				if (GrabCutImg.at<Vec3b>(j, i) == originGrab.at<Vec3b>(j, i) * 0.2)
				{
					GrabCutImg.at<Vec3b>(j, i)[0] = 4;
					GrabCutImg.at<Vec3b>(j, i)[1] = 8;
					GrabCutImg.at<Vec3b>(j, i)[2] = 6;
					originGrab.at<Vec3b>(j, i)[0] = 4;
					originGrab.at<Vec3b>(j, i)[1] = 8;
					originGrab.at<Vec3b>(j, i)[2] = 6;
				}
			}
		}
		GrabCutImg.copyTo(originGrabImg);
		GrabCutImg.copyTo(saveImg);
		GrabCutImg.copyTo(saveOrigin);
	//	GrabCutImg.copyTo(canny);
	//	cv::cvtColor(GrabCutImg, canny, CV_BGR2GRAY);
	//	cv::Canny(canny, canny, 200, 200);
		//EdgePosition();
		imgCenterPoint.x = 1000 + GrabCutImg.cols / 2;
		imgCenterPoint.y = 20 + GrabCutImg.rows / 2;
		scale = cv::Size(GrabCutImg.cols, GrabCutImg.rows);
		DisplayPasteGrabcut(IDC_Paste, GrabCutImg, 0);
	}
}

void CDlgTab2::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CDlgTab2::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	*pResult = 0;

	int nPos;
	CString strPos;
}


void CDlgTab2::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CDlgTab2::OnNMCustomdrawSlider3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CDlgTab2::OnLButtonDown(UINT nFlags, CPoint point)
{//(GrabCutImg.rows / 2)
	if (!GrabCutImg.data)
		return;

	CPoint revisionPoint = RevisionPoint(point, IDC_BACK_PIVOT);
	if (revisionPoint.x > LUpoint.x && revisionPoint.y > LUpoint.y && revisionPoint.x < RDpoint.x && revisionPoint.y < RDpoint.y)
	{
		if (revisionPoint.x > LUpoint.x && revisionPoint.x < LUpoint.x + 10 && revisionPoint.y > LUpoint.y && revisionPoint.y < LUpoint.y + 10)
		{
			if (!GrabCutImg.data)
				return;

			isResizeClick = true;
			clickPointIdx = 1;
		}
		else if (revisionPoint.x > RUpoint.x - 10 && revisionPoint.x < RUpoint.x && revisionPoint.y > RUpoint.y && revisionPoint.y < RUpoint.y + 10)
		{
			if (!GrabCutImg.data)
				return;

			isResizeClick = true;
			clickPointIdx = 2;
		}
		else if (revisionPoint.x > LDpoint.x && revisionPoint.x < LDpoint.x + 10 && revisionPoint.y > LDpoint.y - 10 && revisionPoint.y < LDpoint.y)
		{
			if (!GrabCutImg.data)
				return;

			isResizeClick = true;
			clickPointIdx = 3;
		}
		else if (revisionPoint.x > RDpoint.x - 10 && revisionPoint.x < RDpoint.x && revisionPoint.y > RDpoint.y - 10 && revisionPoint.y < RDpoint.y)
		{
			if (!GrabCutImg.data)
				return;

			isResizeClick = true;
			clickPointIdx = 4;
		}
		//꼭지점 제외구간 클릭하면 이동 
		else /*((point.x <= imgCenterPoint.x + 50 && point.x >= imgCenterPoint.x - 50)
			&& (point.y <= imgCenterPoint.y + 50 && point.y >= imgCenterPoint.y - 50) && !isResizeClick)*/
		{
			distPoint2Center.x = sqrt(pow(point.x - imgCenterPoint.x, 2));
			distPoint2Center.y = sqrt(pow(point.y - imgCenterPoint.y, 2));
			isTransportClick = true;
			imgCenterPoint = point;
			DisplayPasteGrabcut(IDC_Back, saveImg, 1);
		}
	}
	CDialog::OnLButtonDown(nFlags, point);
}


void CDlgTab2::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (isTransportClick)
	{
		isTransportClick = false;
		Compose(BGimg, saveOrigin, LUpoint.x, LUpoint.y, 1);
		RedrawWindow();
		imgCenterPoint = point;
		DisplayPasteGrabcut(IDC_Back, saveImg, 1);
	}

	if (isResizeClick)
	{
		CPoint revisionPoint = RevisionPoint(point, IDC_BACK_PIVOT);

		isResizeClick = false;
		saveImg.copyTo(canny);
		cv::cvtColor(saveImg, canny, CV_BGR2GRAY);
		cv::Canny(canny, canny, 400, 400);
		Compose(BGimg, saveOrigin, LUpoint.x, LUpoint.y, 1);
		RedrawWindow();
		DisplayPasteGrabcut(IDC_Back, saveImg, 2);
		if (isObjresize) seamEnergyDown(1);

	}

	CDialog::OnLButtonUp(nFlags, point);
}




void CDlgTab2::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint revisionPoint = RevisionPoint(point, IDC_BACK_PIVOT);

	/*if (revisionPoint.x > LUpoint.x && revisionPoint.x < RUpoint.x && revisionPoint.y > LUpoint.y && revisionPoint.y < LDpoint.y)
	{
		isMouseHover = true;
		DisplayPasteGrabcut(IDC_Back, saveImg, 1);
	}
	else
	{
		isMouseHover = false;
		DisplayPasteGrabcut(IDC_Back, saveImg, 1);
	}*/

	//마우스오버
	if (point.x > LUpoint.x+32 && point.y > LUpoint.y+24 && point.x < LUpoint.x + saveImg.cols + 32 && point.y < LUpoint.y + saveImg.rows + 24)
	{
		DisplayPasteGrabcut(IDC_Back, saveImg, 4);
	}
	else if(isBgCalled)
	{
		DisplayPasteGrabcut(IDC_Back, saveImg, 2);
	}

	//이동
	if (isTransportClick)
	{
		imgCenterPoint = point;
		DisplayPasteGrabcut(IDC_Back, saveImg, 1);

		//밖에 나갈때 예외처리
	/*	if (mouseMovePoint.x - (saveImg.cols / 2) < 0)
			point = 0;*/
	}

	//크기조절
	if (isResizeClick)
	{
		switch (clickPointIdx)
		{
		case 1:
			scale = cv::Size((RDpoint.x - (int)revisionPoint.x), (RDpoint.y - (int)revisionPoint.y));
			break;
		case 2:
			scale = cv::Size((int)revisionPoint.x - LDpoint.x, LDpoint.y - (int)revisionPoint.y);
			break;
		case 3:
			scale = cv::Size(RUpoint.x - (int)revisionPoint.x, (int)revisionPoint.y - RUpoint.y);
			break;
		case 4:
			scale = cv::Size((int)revisionPoint.x - LUpoint.x, (int)revisionPoint.y - LUpoint.y);
			break;
		default:
			break;
		}

		resize(GrabCutImg, saveImg, scale, 0, 0, CV_INTER_NN);
		resize(originGrab, saveOrigin, scale, 0, 0, CV_INTER_NN);

		if (saveImg.cols % 8 != 0)
			cv::resize(saveImg, saveImg, cv::Size(saveImg.cols - saveImg.cols % 8, saveImg.rows), 0, 0, CV_INTER_NN);
		if (saveOrigin.cols % 8 != 0)
			cv::resize(saveOrigin, saveOrigin, cv::Size(saveOrigin.cols - saveOrigin.cols % 8, saveOrigin.rows), 0, 0, CV_INTER_NN);
		
		DisplayPasteGrabcut(IDC_Back, saveImg, 2);
	}

	//회전
	if (isRotationClick)
	{
		resize(GrabCutImg, saveImg, scale, 0, 0, CV_INTER_NN);
		resize(originGrab, saveOrigin, scale, 0, 0, CV_INTER_NN);
		rotatePivot = Point2d(saveImg.cols / 2, saveImg.rows / 2);

		switch (clickPointIdx)
		{
		case 1:
			if (abs(LUpoint.x - revisionPoint.x) > abs(LUpoint.y - revisionPoint.y))
			{
				warpAffine(saveImg, saveImg, getRotationMatrix2D(rotatePivot, ((LUpoint.x - revisionPoint.x) / (float)saveImg.cols) * 90, 1), saveImg.size(), 2);
				warpAffine(saveOrigin, saveOrigin, getRotationMatrix2D(rotatePivot, ((LUpoint.x - revisionPoint.x) / (float)saveImg.cols) * 90, 1), saveImg.size(), 2);
			}
			else
			{
				warpAffine(saveImg, saveImg, getRotationMatrix2D(rotatePivot, -((LUpoint.y - revisionPoint.y) / (float)saveImg.rows) * 90, 1), saveImg.size(), 2);
				warpAffine(saveOrigin, saveOrigin, getRotationMatrix2D(rotatePivot, -((LUpoint.y - revisionPoint.y) / (float)saveImg.rows) * 90, 1), saveImg.size(), 2);
			}
			break;
		case 2:
			if (abs(RUpoint.x - revisionPoint.x) > abs(RUpoint.y - revisionPoint.y))
			{
				warpAffine(saveImg, saveImg, getRotationMatrix2D(rotatePivot, ((RUpoint.x - revisionPoint.x) / (float)saveImg.cols) * 90, 1), saveImg.size(), 2);
				warpAffine(saveOrigin, saveOrigin, getRotationMatrix2D(rotatePivot, ((RUpoint.x - revisionPoint.x) / (float)saveImg.cols) * 90, 1), saveImg.size(), 2);
			}
			else
			{
				warpAffine(saveImg, saveImg, getRotationMatrix2D(rotatePivot, ((RUpoint.y - revisionPoint.y) / (float)saveImg.rows) * 90, 1), saveImg.size(), 2);
				warpAffine(saveOrigin, saveOrigin, getRotationMatrix2D(rotatePivot, ((RUpoint.y - revisionPoint.y) / (float)saveImg.rows) * 90, 1), saveImg.size(), 2);
			}
			break;
		case 3:
			if (abs(LDpoint.x - revisionPoint.x) > abs(LDpoint.y - revisionPoint.y))
			{
				warpAffine(saveImg, saveImg, getRotationMatrix2D(rotatePivot, -((LDpoint.x - revisionPoint.x) / (float)saveImg.cols) * 90, 1), saveImg.size(), 2);
				warpAffine(saveOrigin, saveOrigin, getRotationMatrix2D(rotatePivot, -((LDpoint.x - revisionPoint.x) / (float)saveImg.cols) * 90, 1), saveImg.size(), 2);
			}
			else
			{
				warpAffine(saveImg, saveImg, getRotationMatrix2D(rotatePivot, -((LDpoint.y - revisionPoint.y) / (float)saveImg.rows) * 90, 1), saveImg.size(), 2);
				warpAffine(saveOrigin, saveOrigin, getRotationMatrix2D(rotatePivot, -((LDpoint.y - revisionPoint.y) / (float)saveImg.rows) * 90, 1), saveImg.size(), 2);
			}
			break;
		case 4:
			if (abs(RDpoint.x - revisionPoint.x) > abs(RDpoint.y - revisionPoint.y))
			{
				warpAffine(saveImg, saveImg, getRotationMatrix2D(rotatePivot, -((RDpoint.x - revisionPoint.x) / (float)saveImg.cols) * 90, 1), saveImg.size(), 2);
				warpAffine(saveOrigin, saveOrigin, getRotationMatrix2D(rotatePivot, -((RDpoint.x - revisionPoint.x) / (float)saveImg.cols) * 90, 1), saveImg.size(), 2);
			}
			else
			{
				warpAffine(saveImg, saveImg, getRotationMatrix2D(rotatePivot, ((RDpoint.y - revisionPoint.y) / (float)saveImg.rows) * 90, 1), saveImg.size(), 2);
				warpAffine(saveOrigin, saveOrigin, getRotationMatrix2D(rotatePivot, ((RDpoint.y - revisionPoint.y) / (float)saveImg.rows) * 90, 1), saveImg.size(), 2);
			}
			break;
		default:
			break;
		}

		if (saveImg.cols % 8 != 0)
		{
			cv::resize(saveImg, saveImg, cv::Size(saveImg.cols - saveImg.cols % 8, saveImg.rows), 0, 0, CV_INTER_NN);
			cv::resize(saveOrigin, saveOrigin, cv::Size(saveImg.cols - saveImg.cols % 8, saveImg.rows), 0, 0, CV_INTER_NN);
		}

		RedrawWindow();
		DisplayPasteGrabcut(IDC_Back, saveImg, 3);
	}

	if ((isSeamDel && isRbtnClick) || (isSeamProtect && isRbtnClick)) {
		//mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, nowMousePos.x * 65535 / 1920, nowMousePos.y * 65535 / 1080, 0, 0);
		CRect grabRect;
		CClientDC dc(this);

		CDC* cdc = GetDC();
		cdc->SetROP2(R2_NOT);//지워지는 효과
		grabRect.SetRect(delStartP.x, delStartP.y, point.x, BGimg.rows+24);
		cdc->Rectangle(grabRect);

		//새로 그려줌
		grabRect.SetRect(delStartP.x, delStartP.y, point.x, BGimg.rows+24);
		cdc->Rectangle(grabRect);

		ReleaseDC(cdc);
		//CDialog::OnMouseMove(nFlags, point);

	}
	CDialog::OnMouseMove(nFlags, point);
}

//배경 불러오기
void CDlgTab2::OnBnClickedCallbgd()
{
	CString openFilter = _T("Image (*.BMP, *.GIF, *.JPG, *.PNG) | *.BMP;*.GIF;*.JPG;*.PNG;*.bmp;*.gif;*.jpg;*.png | All Files(*.*)|*.*||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, openFilter, AfxGetMainWnd());
	if (dlg.DoModal() == IDOK)
	{
		CString cstrImgPath = dlg.GetPathName();
		CT2CA pszConvertedAnsiString(cstrImgPath);
		std::string cstrImgPathString(pszConvertedAnsiString);
		//AfxMessageBox(cstrImgPath);
		BGimg = imread(string(cstrImgPathString), CV_LOAD_IMAGE_UNCHANGED);

		//988 - 32
		if(BGimg.cols > 956)
			cv::resize(BGimg, BGimg, cv::Size(956, BGimg.rows * 956 / BGimg.cols), 0, 0, CV_INTER_NN);
		if (BGimg.rows > 700)
			cv::resize(BGimg, BGimg, cv::Size(BGimg.cols * 700/ BGimg.rows, 700), 0, 0, CV_INTER_NN);

		if (BGimg.cols % 8 != 0)
			cv::resize(BGimg, BGimg, cv::Size(BGimg.cols - BGimg.cols % 8, BGimg.rows), 0, 0, CV_INTER_NN);
		RedrawWindow();
		DisplayOutput(IDC_Back, BGimg);
		isBgCalled = true;

		//심카빙을 위한 계산
		tempEv.clear();
		Mat sobelMat(BGimg.size(), CV_8UC3, cv::Scalar(0, 0, 0));
		cv::cvtColor(BGimg, sobelMat, CV_BGR2GRAY);
		cv::Sobel(sobelMat, sobelMat, CV_8U, 0, 1, 1, 0.9, 255);
		cv::Sobel(sobelMat, sobelMat, CV_8U, 1, 0, 1, 0.9, 1);
		calcEnergy(BGimg, sobelMat);
		for (int y = 1; y < BGimg.rows; y++)	seamcarving(y, 0, 0);
		std::sort(tempEv.begin(), tempEv.end());
		//높이줄이는 계산 추가해야함

		//png파일 되게하려면 건드려봐
/*		cv::Mat bg(BGimg.size(), CV_8UC3);
		for (int i = 0;i < bg.cols;i++)
		{
			for (int j = 0;j < bg.rows;j++)
			{
				bg.at<Vec3b>(j, i)[0] = BGimg.at<Vec3b>(j, i)[0];
				bg.at<Vec3b>(j, i)[1] = BGimg.at<Vec3b>(j, i)[1];
				bg.at<Vec3b>(j, i)[2] = BGimg.at<Vec3b>(j, i)[2];
			}
		}
		
		BGimg = bg;*/
		



		//슬라이더 초기화
		widthSlide.SetRange(10, BGimg.cols);
		widthSlide.SetPos(GrabCutImg.cols);
		widthSlide.SetTicFreq(1);
		widthSlide.SetPageSize(1);
		nColPos = GrabCutImg.cols;

		heightSlide.SetRange(10, BGimg.rows);
		heightSlide.SetPos(GrabCutImg.rows);
		heightSlide.SetTicFreq(1);
		heightSlide.SetPageSize(1);
		nRowPos = GrabCutImg.rows;

		allSlide.SetRange(10, (BGimg.rows < BGimg.cols ? BGimg.rows : BGimg.cols) - 10);
		allSlide.SetPos(GrabCutImg.rows < GrabCutImg.cols ? GrabCutImg.rows : GrabCutImg.cols);
		allSlide.SetTicFreq(1);
		allSlide.SetPageSize(1);
		//nRowPos = GrabCutImg.rows;
		//////////////////////////

		//nowCutImgPoint.x = 840 + GrabCutImg.cols / 2;
		//nowCutImgPoint.y = 20 + GrabCutImg.rows / 2;
	}
	isStartEdit = true;
}

void CDlgTab2::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//if (pScrollBar)
	//{
	//	// 어떤 슬라이더인지 검사
	//	if (pScrollBar == (CScrollBar*)&widthSlide || pScrollBar == (CScrollBar*)&allSlide)
	//	{
	//		if (pScrollBar == (CScrollBar*)&allSlide)  widthSlide.SetPos((widthSlide.GetPos() + allSlide.GetPos()) / 2);
	//		else allSlide.SetPos((widthSlide.GetPos() + heightSlide.GetPos()) / 2);
	//		// 슬라이더의 위치를 검사한다.
	//		nColPos = widthSlide.GetPos();
	//		

	//		resize(GrabCutImg, saveImg, cv::Size(nColPos, (int)saveImg.rows), 0, 0, CV_INTER_NN);
	//		if (saveImg.cols % 8 != 0)
	//			cv::resize(saveImg, saveImg, cv::Size(saveImg.cols - saveImg.cols % 8, saveImg.rows), 0, 0, CV_INTER_NN);

	//		resize(originGrab, saveOrigin, cv::Size(nColPos, (int)saveOrigin.rows), 0, 0, CV_INTER_NN);
	//		if (saveOrigin.cols % 8 != 0)
	//			cv::resize(saveOrigin, saveOrigin, cv::Size(saveOrigin.cols - saveOrigin.cols % 8, saveOrigin.rows), 0, 0, CV_INTER_NN);

	//		saveImg.copyTo(canny);
	//		cv::cvtColor(saveImg, canny, CV_BGR2GRAY);
	//		cv::Canny(canny, canny, 400, 400);
	////		cv::namedWindow("Original Image");
	////		cv::imshow("Original Image", canny);
	//		
	//		Compose(BGimg, saveOrigin, mouseMovePoint.x - saveImg.cols / 2, mouseMovePoint.y - saveImg.rows / 2, 2);
	//		DisplayPasteGrabcut(IDC_Back, saveImg, 2);
	//	}

	//	if (pScrollBar == (CScrollBar*)&heightSlide || pScrollBar == (CScrollBar*)&allSlide)
	//	{
	//		if (pScrollBar == (CScrollBar*)&allSlide) heightSlide.SetPos((heightSlide.GetPos() + allSlide.GetPos()) / 2);
	//		else allSlide.SetPos((widthSlide.GetPos() + heightSlide.GetPos()) / 2);
	//		// 슬라이더의 위치를 검사한다.
	//		nRowPos = heightSlide.GetPos();
	//		

	//		resize(GrabCutImg, saveImg, cv::Size((int)saveImg.cols, nRowPos), 0, 0, CV_INTER_NN);
	//		if (saveImg.cols % 8 != 0)
	//			cv::resize(saveImg, saveImg, cv::Size(saveImg.cols - saveImg.cols % 8, saveImg.rows), 0, 0, CV_INTER_NN);

	//		resize(originGrab, saveOrigin, cv::Size((int)saveOrigin.cols, nRowPos), 0, 0, CV_INTER_NN);
	//		if (saveOrigin.cols % 8 != 0)
	//			cv::resize(saveOrigin, saveOrigin, cv::Size(saveOrigin.cols - saveOrigin.cols % 8, saveOrigin.rows), 0, 0, CV_INTER_NN);

	//		saveImg.copyTo(canny);
	//		cv::cvtColor(saveImg, canny, CV_BGR2GRAY);
	//		cv::Canny(canny, canny, 400, 400);

	//		Compose(BGimg, saveOrigin, mouseMovePoint.x - saveImg.cols / 2, mouseMovePoint.y - saveImg.rows / 2, 2);
	//		DisplayPasteGrabcut(IDC_Back, saveImg, 2);


	//	}

	//}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CDlgTab2::OnRButtonDown(UINT nFlags, CPoint point)
{
	isRbtnClick = true;
	if((point.x <= imgCenterPoint.x + 50 && point.x >= imgCenterPoint.x - 50)
		&& (point.y <= imgCenterPoint.y - (saveImg.rows/2) + 50 && point.y >= imgCenterPoint.y - (saveImg.rows/2) - 50) && !isResizeClick && !isTransportClick)
		isRotationClick = true;
	

	if (!GrabCutImg.data)
		return;

	if (isSeamDel || isSeamProtect)	delStartP = point;

	CPoint revisionPoint = RevisionPoint(point, IDC_BACK_PIVOT);

	if (revisionPoint.x > LUpoint.x && revisionPoint.x < LUpoint.x + 10 && revisionPoint.y > LUpoint.y && revisionPoint.y < LUpoint.y + 10)
	{
		if (!GrabCutImg.data)
			return;

		isRotationClick = true;
		clickPointIdx = 1;
	}
	if (revisionPoint.x > RUpoint.x - 10 && revisionPoint.x < RUpoint.x && revisionPoint.y > RUpoint.y && revisionPoint.y < RUpoint.y + 10)
	{
		if (!GrabCutImg.data)
			return;

		isRotationClick = true;
		clickPointIdx = 2;
	}
	if (revisionPoint.x > LDpoint.x && revisionPoint.x < LDpoint.x + 10 && revisionPoint.y > LDpoint.y - 10 && revisionPoint.y < LDpoint.y)
	{
		if (!GrabCutImg.data)
			return;

		isRotationClick = true;
		clickPointIdx = 3;
	}
	if (revisionPoint.x > RDpoint.x - 10 && revisionPoint.x < RDpoint.x && revisionPoint.y > RDpoint.y - 10 && revisionPoint.y < RDpoint.y)
	{
		if (!GrabCutImg.data)
			return;

		isRotationClick = true;
		clickPointIdx = 4;
	}

	CDialog::OnRButtonDown(nFlags, point);
}


void CDlgTab2::OnRButtonUp(UINT nFlags, CPoint point)
{
	isRotationClick = false;
	isRbtnClick = false;

	//심카빙 삭제
	if (isSeamDel) {
		delEndP = point;
		delEndP.y = BGimg.rows +24;
		CRect grabRect;
		grabRect.SetRect(delStartP.x, delStartP.y, delEndP.x, delEndP.y);

		CClientDC dc(this);
		CPen pen;
		pen.CreatePen(PS_DASH, 2, RGB(255, 255, 0));    // 빨간색 펜을 생성
		CPen* oldPen = dc.SelectObject(&pen);
		CBrush brush;
		brush.CreateStockObject(NULL_BRUSH);    // 투명 브러시
		CBrush* pOldBrush = dc.SelectObject(&brush);
		dc.Rectangle(grabRect);
		dc.SelectObject(pOldBrush);
		//inputImg.at<uchar>(point.x, point.y) = GC_BGD;
		if(!isObjresize)	seamEnergyDown(0);//에너지 낮추기
		isObjresize = true;
		isSeamDel = false;
	}

	//심카빙 보존
	if (isSeamProtect) {
		delEndP = point;
		delEndP.y = BGimg.rows + 24;
		CRect grabRect;
		grabRect.SetRect(delStartP.x, delStartP.y, delEndP.x, delEndP.y);

		CClientDC dc(this);
		CPen pen;
		pen.CreatePen(PS_DASH, 2, RGB(125, 230, 125));    // 펜을 생성
		CPen* oldPen = dc.SelectObject(&pen);
		CBrush brush;
		brush.CreateStockObject(NULL_BRUSH);    // 투명 브러시
		CBrush* pOldBrush = dc.SelectObject(&brush);
		dc.Rectangle(grabRect);
		dc.SelectObject(pOldBrush);
		//inputImg.at<uchar>(point.x, point.y) = GC_BGD;
		if (!isObjresize)	seamEnergyDown(3);//에너지 높이기
		isObjresize = true;
		isSeamProtect = false;

	}

	CDialog::OnRButtonUp(nFlags, point);
}


//배경에서 심카빙으로 삭제 버튼
void CDlgTab2::OnBnClickedSeamdelete()
{
	isObjresize = false;
	isSeamDel = true;
}

void CDlgTab2::seamEnergyDown(int mode)
{
	int rectRows = delEndP.y - delStartP.y;
	int rectCols = delEndP.x - delStartP.x;
	if (mode != 3)
	{
		for (int y = delStartP.y - 24; y < delEndP.y - 24; y++)
		{
			for (int x = delStartP.x - 32; x < delEndP.x - 32; x++)
			{
				downEv[y*BGimg.cols + x] -= 100;
			}
		}
	}
	else if (mode == 3)
	{
		for (int y = delStartP.y - 24; y < delEndP.y - 24; y++)
		{
			for (int x = delStartP.x - 32; x < delEndP.x - 32; x++)
			{
				downEv[y*BGimg.cols + x] += 100;
			}
		}
	}

	for (int y = 1; y < BGimg.rows; y++)	seamcarving(y, 0, 0);

	std::sort(tempEv.begin(), tempEv.end());
	int cutCount = 0;

	if (saveImg.cols > rectCols)
	{//확대
		int Count = 0;
		//for (int i = 0; i < 500; i++)
		while (cutCount < saveImg.cols - rectCols)
		{
			if (Count != 0 && tempEv[Count] == tempEv[Count - 1]) Count;
			else
			{
				for (int j = 0;j < BGimg.cols;j++)
				{
					if (tempEv[Count] == downEv[(BGimg.rows - 1) * BGimg.cols + j])
					{
						cutCount++;
						copyMakeBorder(BGimg, BGimg, 0, 0, 0, 1, BORDER_REPLICATE);
						for (int q = 1; q <= BGimg.rows; q++)
						{
							if (q == BGimg.rows) downEv.push_back(0);
							else downEv.insert(downEv.begin() + (BGimg.cols * q) - 1, 0);
						}
						BGimg.copyTo(originBGimg);
						seamcarving(j, 3, cutCount);

						//if (BGimg.cols % 8 != 0)
						//	cv::resize(BGimg, BGimg, cv::Size(BGimg.cols - BGimg.cols % 8, BGimg.rows), 0, 0, CV_INTER_NN);
						
						
						//////RedrawWindow();
						DisplayOutput(IDC_Back, BGimg);
						if (mode == 0 || mode == 3)	DisplayPasteGrabcut(IDC_Paste, GrabCutImg, 0);
						if (mode == 1)	DisplayPasteGrabcut(IDC_Back, saveImg, 1);

						j = BGimg.cols;
					}
				}
			}
			Count++;
			if (cutCount == saveImg.cols - rectCols) break;
		}
	}
	
	else if (saveImg.cols < rectCols)
	{//축소
		int Count = 0;
		//for (int i = 0; i < 500; i++)
		while (cutCount < rectCols - saveImg.cols)
		{	
			if (cutCount == rectCols - saveImg.cols)
				break;
			if (tempEv.size() - Count == 1) break;
			if (Count != 0 && tempEv[Count] == tempEv[Count - 1]) Count;
			else
			{
				for (int j = 0;j < BGimg.cols - Count;j++)
				{
					if (tempEv[Count] == downEv[(BGimg.rows - 1) * BGimg.cols + j])
					{
						cutCount++;
						
						/*if (BGimg.cols % 8 != 0)
						{
							cv::Mat tmpBG(BGimg.size(), CV_8UC3, cv::Scalar(128, 128, 128, 0));
							copyMakeBorder(BGimg, tmpBG, 0, 0, 0, 8 - BGimg.cols % 8, BORDER_CONSTANT);
							DisplayOutput(IDC_Back, tmpBG);
						}
						else DisplayOutput(IDC_Back, BGimg);*/
						seamcarving(j, 2, cutCount);

					/*	for (int q = 1; q <= BGimg.rows; q++)
						{
							if (q == BGimg.rows) downEv.push_back(0);
							else downEv.insert(downEv.begin() + (BGimg.cols * q) - 1, 0);
						}*/
						//BGimg.copyTo(originBGimg);
						
						
						///////RedrawWindow();
						DisplayOutput(IDC_Back, BGimg);
						if (mode == 0 || mode == 3)	DisplayPasteGrabcut(IDC_Paste, GrabCutImg, 0);
						if (mode == 1)	DisplayPasteGrabcut(IDC_Back, saveImg, 1);
						
						
						j = BGimg.cols;
					}
				}
			}
			Count++;
			if (cutCount == rectCols - saveImg.cols) break;
		}
		cv::Rect cutRect(0, 0, BGimg.cols - cutCount, BGimg.rows);
		BGimg = BGimg(cutRect);
		if (BGimg.cols % 8 != 0)
			cv::resize(BGimg, BGimg, cv::Size(BGimg.cols - BGimg.cols % 8, BGimg.rows), 0, 0, CV_INTER_NN);
	}
	else return;
	RedrawWindow();
	DisplayOutput(IDC_Back, BGimg);
	if (mode == 0 || mode == 3)	//영역설정 끝나면 영역으로 이미지 자동배치
	{
		imgCenterPoint.x = delEndP.x - saveImg.cols / 2;
		imgCenterPoint.y = delEndP.y - saveImg.rows / 2;
		LUpoint = imgCenterPoint - IDC_BACK_PIVOT - CPoint(saveImg.cols/2, saveImg.rows/2);
		Compose(BGimg, saveImg, LUpoint.x, LUpoint.y, 1);
		DisplayPasteGrabcut(IDC_Back, saveImg, 1);
	}
	else if (mode == 1)	DisplayPasteGrabcut(IDC_Back, saveImg, 1);

}

CPoint CDlgTab2::RevisionPoint(CPoint point, CPoint pivot)
{
	return point - pivot;
}

void CDlgTab2::OnBnClickedAutocolorbtn()
{
	isAutoColor = isAutoColor == true ? false : true;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgTab2::OnBnClickedSeamprotect()
{
	isObjresize = false;
	isSeamProtect = true;
}


void CDlgTab2::OnBnClickedSavebutton()
{
	for (int i = 0;i < saveImg.cols;i++)
	{
		for (int j = 0;j < saveImg.rows;j++)
		{
			BGimg.at<Vec3b>(j + LUpoint.y, i + LUpoint.x)[0] = saveImg.at<Vec3b>(j, i)[0];
			BGimg.at<Vec3b>(j + LUpoint.y, i + LUpoint.x)[1] = saveImg.at<Vec3b>(j, i)[1];
			BGimg.at<Vec3b>(j + LUpoint.y, i + LUpoint.x)[2] = saveImg.at<Vec3b>(j, i)[2];
		}
	}
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, AfxGetMainWnd());
	if (dlg.DoModal() == IDOK)
	{
		CString cstrImgPath = dlg.GetPathName();
		CT2CA pszConvertedAnsiString(cstrImgPath);
		std::string cstrImgPathString(pszConvertedAnsiString);
		//m_Img.Load(openFilter);

		cv::imwrite(string(cstrImgPathString)+".jpg", BGimg);
	}
}
