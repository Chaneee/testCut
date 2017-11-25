
#include "stdafx.h"
#include "maptools.h"
#include "DlgTab2.h"
#include "afxdialogex.h"

#define distance(ax,ay,bx,by) sqrt(pow(ax - bx,2) + pow(ay - by, 2))

//전역변수들////////////////////
Mat BGimg;
Mat originGrabImg, GrabCutImg;
Mat canny;
CPoint mouseMovePoint, imgCenterPoint;
CPoint distPoint2Center;
Mat originGrab;
//bool isLMouseDown = false; // True : 마우스 왼쪽 누르고 있는 상태 / false : 뗀 상태
bool isStartEdit = false; //OnPaint부분에서 GrabCutImg를 부르기 때문에 써야하는 변수

bool isTransportClick = false; // True : 마우스 왼쪽 누르고 있는 상태 / false : 뗀 상태
bool isResizeClick = false; // True : 마우스 왼쪽 누르고 있는 상태 / false : 뗀 상태
bool isRotationClick= false; // True : 마우스 오른쪽 누르고 있는 상태 / false : 뗀 상태
Mat saveImg, saveOrigin;
CPoint LUpoint, RUpoint, LDpoint, RDpoint;
CPoint resizePoint, originCoor;
int nColPos, nRowPos;
Point2d rotatePivot;
cv::Size scale;
CPoint rBtnStart, rotatePoint;
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
END_MESSAGE_MAP()

//이미지 디스플레이를 위한 함수 : 배경
void CDlgTab2::DisplayOutput(int IDC_PICTURE_TARGET, Mat targetMat)
{
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

		dcImageTraget.SetStretchBltMode(COLORONCOLOR);
		::StretchDIBits(dcImageTraget.GetSafeHdc(), rcImageTraget.left, rcImageTraget.top, rcImageTraget.right, rcImageTraget.bottom,
			0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);

		if (isTransportClick)
			//dcImageTraget.StretchBlt(100, 100, inputImg.cols-100, inputImg.rows-100, &dcImageTraget, 0, 0, inputImg.cols+100, inputImg.rows+100,SRCCOPY);
			cvReleaseImage(&tempImage);
	}
}

//이미지 디스플레이를 위한 함수 : (mode=0 : 붙여넣기, mode=1 : 이동, mode=2 : 사이즈조절)
void CDlgTab2::DisplayPasteGrabcut(int IDC_PICTURE_TARGET, Mat targetMat, int mode)
{
	IplImage* targetIplImage = new IplImage(targetMat);
	if (targetIplImage != nullptr) {
		CWnd* pWnd_ImageTraget = GetDlgItem(IDC_PICTURE_TARGET);
		CClientDC dcImageTraget(pWnd_ImageTraget);
		RECT rcImageTarget;
		pWnd_ImageTraget->GetClientRect(&rcImageTarget);

		rcImageTarget.top = 0;
		rcImageTarget.left = 0;
		rcImageTarget.bottom = targetMat.rows;
		rcImageTarget.right = targetMat.cols;

		LUpoint = CPoint(imgCenterPoint.x - (saveImg.cols / 2) + 32, imgCenterPoint.y - (saveImg.rows / 2) + 24);
		RUpoint = LUpoint + CPoint(saveImg.cols, 0);
		LDpoint = LUpoint + CPoint(0, saveImg.rows);
		RDpoint = LUpoint + CPoint(saveImg.cols, saveImg.rows);

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
			::StretchDIBits(dcImageTraget.GetSafeHdc(), rcImageTarget.left, rcImageTarget.top, rcImageTarget.right, rcImageTarget.bottom,
				0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
			cvReleaseImage(&tempImage);

		}

		else if (mode == 1) //이동
		{
			DisplayOutput(IDC_Back, BGimg);
			//DisplayPasteGrabcut(IDC_Paste, GrabCutImg, 0);
			::StretchDIBits(dcImageTraget.GetSafeHdc(), mouseMovePoint.x - (saveImg.cols / 2), mouseMovePoint.y - (saveImg.rows / 2),
				rcImageTarget.right, rcImageTarget.bottom,
				0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
			cvReleaseImage(&tempImage);
		}

		else if (mode == 2) //크기조절
		{
			DisplayOutput(IDC_Back, BGimg);
			::StretchDIBits(dcImageTraget.GetSafeHdc(), resizePoint.x, resizePoint.y, rcImageTarget.right, rcImageTarget.bottom,
				0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
			cvReleaseImage(&tempImage);

		}
		else if (mode == 3) //회전
		{
			DisplayOutput(IDC_Back, BGimg);
			::StretchDIBits(dcImageTraget.GetSafeHdc(), rotatePoint.x, rotatePoint.y, rcImageTarget.right, rcImageTarget.bottom,
				0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
			cvReleaseImage(&tempImage);

		}
		//if (isLeftMouseDown)
		//	dcImageTraget.StretchBlt(0, 0, GrabCutImg.cols-100, GrabCutImg.rows-100, &dcImageTraget, 0, 0, GrabCutImg.cols, GrabCutImg.rows,SRCCOPY);

	}
}

//평균 구하는 함수
int CDlgTab2::kMeanss(int clusterMemberCount, int sum, int mode)
{
	int A;

	
	A = sum / clusterMemberCount;


	return A;
}

//RGB값을 YCbCr값으로. 0 = Y, 1 = Cb, 2 = Cr
int CDlgTab2::RGBtoYCbCr(Mat img, int mode, int y, int x)
{
	int A;
	/*for (int y = 0; y < img.rows; y++)
	{
		for (int x = 0; x < img.cols; x++)
		{*/
	if (mode == 0) // Y
		A = 0.299 * img.at<Vec3b>(y, x)[2] + (0.587 * img.at<Vec3b>(y, x)[1]) + (0.114 * img.at<Vec3b>(y, x)[0]);
	//			A.push_back((0.299 * img.at<Vec3b>(y, x)[2]) + (0.587 * img.at<Vec3b>(y, x)[1]) + (0.114 * img.at<Vec3b>(y, x)[0]));
	else if (mode == 1) // Cb
		A = (-0.16874) * img.at<Vec3b>(y, x)[2] - (0.33126 * img.at<Vec3b>(y, x)[1]) + (0.5 * img.at<Vec3b>(y, x)[0]);
	//			A.push_back(((-0.16874) * img.at<Vec3b>(y, x)[2]) - (0.33126 * img.at<Vec3b>(y, x)[1]) + (0.5 * img.at<Vec3b>(y, x)[0]));
	else if (mode == 2) // Cr
		A = (0.5 * img.at<Vec3b>(y, x)[2]) - (0.41869 * img.at<Vec3b>(y, x)[1]) - (0.08131 * img.at<Vec3b>(y, x)[0]);
	//			A.push_back((0.5 * img.at<Vec3b>(y, x)[2]) - (0.41869 * img.at<Vec3b>(y, x)[1]) - (0.08131 * img.at<Vec3b>(y, x)[0]));
//		}
//	}
	return A;
}

//두 이미지를 합성하는 함수
void CDlgTab2::Compose(Mat BGMat, Mat originMat, int target_X, int target_Y, int mode)
{
	int sum[3] = { 0, 0, 0}; // 0:Y, 1:Cb, 2:Cr
	int sumTmp[3] = { 0, 0, 0 }; // 0:Y, 1:Cb, 2:Cr
	vector<int> inputY, inputCb, inputCr;
	int avrY, avrCb, avrCr;
	int clusterMemberCount = 0, firstCount = 0;
	float bgRatio = 0, objectRatio = 1, dist = 0;
	int arr[24][2] = { 
			{ -2,-2 }, { -2,-1 }, { -2,0 }, { -2,1 }, { -2,2 },
			{ -1,-2 }, { -1,-1 }, { -1,0 }, { -1,1 }, { -1,2 },
			{ 0,-2 },  { 0,-1 },			{ 0,1 },  { 0,2 },
			{ 1,2 },   { 1,-1 }, { 1,0 },   { 1,1 },  { 1,2 },
			{ 2,-2 },  { 2,-1 }, { 2,0 },   { 2,1 },  { 2,2 }
					};
	
	//mode 0 : 그냥 합성(배경부분 갖다붙이기)
	for (int i = 0;i < originMat.cols;i++)
	{
		for (int j = 0;j < originMat.rows;j++)
		{
			int secretCount = 0;
			if (canny.data && canny.at<uchar>(j, i) != 0)
			{
				float aa = sqrt(pow(saveImg.cols / 2 - i, 2));
				dist = (sqrt(pow(saveImg.cols / 2 - i, 2)) / (saveImg.cols / 2)) + (sqrt(pow(saveImg.rows / 2 - j, 2)) / (saveImg.rows / 2));
				bgRatio = dist * 0.5;
				bgRatio = bgRatio < 0 ? 0 : bgRatio;
				bgRatio = bgRatio > 1 ? 1 : bgRatio;
				objectRatio = 1 - bgRatio;
			//	bgRatio = dist / 300;
			//	bgRatio = 1;
				originMat.at<Vec3b>(j, i)[0] = (BGMat.at<Vec3b>(j + target_Y, i + target_X)[0] * bgRatio) + (originMat.at<Vec3b>(j, i)[0] * objectRatio);
				originMat.at<Vec3b>(j, i)[1] = (BGMat.at<Vec3b>(j + target_Y, i + target_X)[1] * bgRatio) + (originMat.at<Vec3b>(j, i)[1] * objectRatio);
				originMat.at<Vec3b>(j, i)[2] = (BGMat.at<Vec3b>(j + target_Y, i + target_X)[2] * bgRatio) + (originMat.at<Vec3b>(j, i)[2] * objectRatio);
			}
			//if (originMat.at<Vec3b>(j, i) == originGrab.at<Vec3b>(j, i) * 0.2)
			if (originMat.at<Vec3b>(j, i)[0] == 4 && originMat.at<Vec3b>(j, i)[1] == 8 && originMat.at<Vec3b>(j, i)[2] == 6) saveImg.at<Vec3b>(j, i) = BGMat.at<Vec3b>(j + target_Y, i + target_X);
			/*{
				avrY = sumTmp[0] / firstCount;
				avrCb = sumTmp[1] / firstCount;
				avrCr = sumTmp[2] / firstCount;

				saveImg.at<Vec3b>(j, i)[0] = (avrY / 2 ) + (1.772 *  avrCb);
				saveImg.at<Vec3b>(j, i)[1] = (avrY / 2 ) - (0.34414 * avrCb) - (0.71414 * avrCr);
				saveImg.at<Vec3b>(j, i)[2] = (avrY / 2 ) + (1.402 * avrCr);
			}*/
				
			//	
	
			else /*if (canny.at<uchar>(j, i) == 255)*/
			{
				sum[0] += RGBtoYCbCr(BGMat, 0, j + target_Y, i + target_X);
				sum[1] += RGBtoYCbCr(BGMat, 1, j + target_Y, i + target_X);
				sum[2] += RGBtoYCbCr(BGMat, 2, j + target_Y, i + target_X);
				inputY.push_back(RGBtoYCbCr(originMat, 0, j, i));
				inputCb.push_back(RGBtoYCbCr(originMat, 1, j, i));
				inputCr.push_back(RGBtoYCbCr(originMat, 2, j, i));
				clusterMemberCount++;





				/*else if (secretCount > 2)
					objectRatio = 0.3;
				else if (secretCount >= 1)
					objectRatio = 0.5;*/
				
				

		/*		if (canny.data && canny.at<uchar>(j, i) != 0)
				{
				float a = distance(saveImg.cols / 2, saveImg.rows / 2, 0, 0);
				float ba = distance(saveImg.cols / 2, saveImg.rows / 2, i, j);

					dist = distance(saveImg.cols / 2, saveImg.rows / 2, i, j);
					bgRatio = dist / 300;
					objectRatio = 1 - bgRatio;
				}*/

		//		saveImg.at<Vec3b>(j, i)[0] = (BGMat.at<Vec3b>(j + target_Y, i + target_X)[0] * bgRatio) + (originMat.at<Vec3b>(j, i)[0] * objectRatio);
		//		saveImg.at<Vec3b>(j, i)[1] = (BGMat.at<Vec3b>(j + target_Y, i + target_X)[1] * bgRatio) + (originMat.at<Vec3b>(j, i)[1] * objectRatio);
		//		saveImg.at<Vec3b>(j, i)[2] = (BGMat.at<Vec3b>(j + target_Y, i + target_X)[2] * bgRatio) + (originMat.at<Vec3b>(j, i)[2] * objectRatio);
			/*	GrabCutImg.at<Vec3b>(j, i)[0] = BGMat.at<Vec3b>(j + target_Y, i + target_X)[0] * 1 / 2 + originMat.at<Vec3b>(j, i)[0] * 1 / 2;
				GrabCutImg.at<Vec3b>(j, i)[1] = BGMat.at<Vec3b>(j + target_Y, i + target_X)[1] * 1 / 2 + originMat.at<Vec3b>(j, i)[1] * 1 / 2;
				//GrabCutImg.at<Vec3b>(j, i)[1] = /*BGMat.at<Vec3b>(j + target_Y, i + target_X)[1] / 200 * originMat.at<Vec3b>(j, i)[1];
				GrabCutImg.at<Vec3b>(j, i)[2] = BGMat.at<Vec3b>(j + target_Y, i + target_X)[2] * 1 / 2 + originMat.at<Vec3b>(j, i)[2] * 1 / 2;
			*/
				bgRatio = 0, objectRatio = 1;
			}
		}
	}
	
	//현재위치의 YCbCr 평균구함
	avrY = sum[0] / clusterMemberCount;
	avrCb = sum[1] / clusterMemberCount;
	avrCr = sum[2] / clusterMemberCount;
	int t = 0;
	for (int i = 0;i < originMat.cols;i++)
	{
		
		for (int j = 0;j < originMat.rows;j++)
		{
			bgRatio = 0.5, objectRatio = 0.5;
			int secretCount = 0;
	//		if (mode == 2)
	//		{
	//		if (canny.data && canny.at<uchar>(j, i) != 0)
	//		{
				//dist = distance(canny.cols / 2, canny.rows / 2, 0, 0) - distance(canny.cols / 2, canny.rows / 2, i, j);

	//		}
				
				/*int a = distance(canny.cols / 2, canny.rows / 2, canny.cols, canny.rows);
				int b = distance(canny.cols / 2, canny.rows / 2, i, j);//*/ //시험용
	//		}

			//if (originMat.at<Vec3b>(j, i) != originGrab.at<Vec3b>(j, i) * 0.2)
			if(originMat.at<Vec3b>(j, i)[0] != 4 || originMat.at<Vec3b>(j, i)[1] != 8 || originMat.at<Vec3b>(j, i)[2] != 6)
			{
			/*	for (int q = 0; q < 24; q++)
				{
					if (j + arr[q][0] < 0 || i + arr[q][1] < 0 || j + arr[q][0] == saveImg.rows || i + arr[q][1] == saveImg.cols)
						continue;

					if (originMat.at<Vec3b>(j + arr[q][0], i + arr[q][1])[0] == 4 && originMat.at<Vec3b>(j + arr[q][0], i + arr[q][1])[1] == 8 && originMat.at<Vec3b>(j + arr[q][0], i + arr[q][1])[2] == 6)
						secretCount++;
				}*/

			/*	for (int p = -10; p <= 10; p++)
				{
					for (int q = -10; q <= 10; q++)
					{
						if (j + q < 0 || i + p < 0 || j + q == saveImg.rows || i + p == saveImg.cols)
							continue;

						if (originMat.at<Vec3b>(j + q, i + p)[0] == 4 && originMat.at<Vec3b>(j + q, i + p)[1] == 8 && originMat.at<Vec3b>(j + q, i + p)[2] == 6)
							secretCount++;
					}
				}*/


			//	if (secretCount >= 4)
			//		objectRatio = 0.3;

			//	bgRatio = 1 - objectRatio;

				

				saveImg.at<Vec3b>(j, i)[0] = (avrY/2 + (objectRatio * inputY[t])) + (1.772 * ((objectRatio * inputCb[t]) + avrCb * bgRatio));
				saveImg.at<Vec3b>(j, i)[1] = (avrY/2 + (objectRatio * inputY[t])) - (0.34414 * ((objectRatio * inputCb[t]) + avrCb * bgRatio)) - (0.71414 * ((objectRatio * inputCr[t]) + avrCr * bgRatio));
				saveImg.at<Vec3b>(j, i)[2] = (avrY/2 + (objectRatio * inputY[t])) + (1.402 * ((objectRatio * inputCr[t]) + avrCr * bgRatio));

				t++;
				if (t > clusterMemberCount) break;
			}
			
		}
	}

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
		DisplayPasteGrabcut(IDC_Paste, GrabCutImg, 0);
		imgCenterPoint.x = 840 + GrabCutImg.cols / 2;
		imgCenterPoint.y = 20 + GrabCutImg.rows / 2;
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

	//이미지 중앙 클릭하면 이동 
	if ((point.x <= imgCenterPoint.x + 50 && point.x >= imgCenterPoint.x - 50)
		&& (point.y <= imgCenterPoint.y + 50 && point.y >= imgCenterPoint.y - 50) && !isResizeClick)
	{
		distPoint2Center.x = sqrt(pow(point.x - imgCenterPoint.x, 2));
		distPoint2Center.y = sqrt(pow(point.y - imgCenterPoint.y, 2));
		mouseMovePoint = imgCenterPoint;
		DisplayPasteGrabcut(IDC_Back, saveImg, 1);
		isTransportClick = true;
	}

	if ((point.x > LUpoint.x - 20 && point.x < LUpoint.x + 20 && point.y > LUpoint.y - 20 && point.y < LUpoint.y + 20)
		|| (point.x > RUpoint.x - 20 && point.x < RUpoint.x + 20 && point.y > RUpoint.y - 20 && point.y < RUpoint.y + 20)
		|| (point.x > LDpoint.x - 20 && point.x < LDpoint.x + 20 && point.y > LDpoint.y - 20 && point.y < LDpoint.y + 20)
		|| (point.x > RDpoint.x - 20 && point.x < RDpoint.x + 20 && point.y > RDpoint.y - 20 && point.y < RDpoint.y + 20))
	{
		isResizeClick = true;
		resizePoint = point;
		if (!GrabCutImg.data)
			return;

	//	SetTimer(btnIdx, 20, NULL);
	}

	CDialog::OnLButtonDown(nFlags, point);
}


void CDlgTab2::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (isTransportClick)
	{
		isTransportClick = false;
		Compose(BGimg, saveOrigin, point.x - saveImg.cols / 2, point.y - saveImg.rows / 2, 1);
		RedrawWindow();
		imgCenterPoint = point;
		//cv::namedWindow("Original Image");
		//cv::imshow("Original Image", GrabCutImg);
		DisplayPasteGrabcut(IDC_Back, saveImg, 1);

		//nowCutImgPoint = point;
	//	originCoor.x = point.x - saveImg.cols / 2;
	//	originCoor.y = point.y - saveImg.rows / 2;
	}

	if (isResizeClick)
	{
		isResizeClick = false;
		saveImg.copyTo(canny);
		cv::cvtColor(saveImg, canny, CV_BGR2GRAY);
		cv::Canny(canny, canny, 400, 400);
		Compose(BGimg, saveOrigin, originCoor.x, originCoor.y, 1);
		RedrawWindow();
		DisplayPasteGrabcut(IDC_Back, saveImg, 2);
		//nowCutImgPoint = point;
	}

	CDialog::OnLButtonUp(nFlags, point);
}




void CDlgTab2::OnMouseMove(UINT nFlags, CPoint point)
{
	//이동
	if (isTransportClick)
	{
		mouseMovePoint = point;
		DisplayPasteGrabcut(IDC_Back, saveImg, 1);

		//밖에 나갈때 예외처리
	/*	if (mouseMovePoint.x - (saveImg.cols / 2) < 0)
			point = 0;*/
	}

	//크기조절
	if (isResizeClick)
	{
		
		if (point.x > LUpoint.x - 20 && point.x < LUpoint.x + 20 && point.y > LUpoint.y - 20 && point.y < LUpoint.y + 20)
		{
			scale = cv::Size((RDpoint.x - (int)point.x), (RDpoint.y - (int)point.y));
			resizePoint.x = point.x - 32;
			resizePoint.y = point.y - 24;
		
		}
		else if (point.x > RUpoint.x - 20 && point.x < RUpoint.x + 20 && point.y > RUpoint.y - 20 && point.y < RUpoint.y + 20)
		{
			scale = cv::Size((int)point.x - LDpoint.x, LDpoint.y - (int)point.y);
			resizePoint.x = point.x - saveImg.cols - 32;
			resizePoint.y = point.y - 24;
			
		}
		else if (point.x > LDpoint.x - 20 && point.x < LDpoint.x + 20 && point.y > LDpoint.y - 20 && point.y < LDpoint.y + 20)
		{
			scale = cv::Size(RUpoint.x - (int)point.x, (int)point.y - RUpoint.y);
			resizePoint.x = point.x - 32;
			resizePoint.y = point.y - saveImg.rows- 24;	
		}
		else if (point.x > RDpoint.x - 20 && point.x < RDpoint.x + 20 && point.y > RDpoint.y - 20 && point.y < RDpoint.y + 20)
		{
			scale = cv::Size((int)point.x - LUpoint.x, (int)point.y - LUpoint.y);
			resizePoint.x = point.x - saveImg.cols - 32;
			resizePoint.y = point.y - saveImg.rows - 24;
		}



		mouseMovePoint = imgCenterPoint;
		resize(GrabCutImg, saveImg, scale, 0, 0, CV_INTER_NN);
		resize(originGrab, saveOrigin, scale, 0, 0, CV_INTER_NN);
		originCoor = resizePoint;

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
		rotatePivot = Point2d(saveImg.cols / 2, saveImg.rows / 2);

		if (abs(point.x - 32 - rBtnStart.x) > abs(point.y - 24 - rBtnStart.y))
			warpAffine(saveImg, saveImg, getRotationMatrix2D(rotatePivot, -((point.x - 32 - rBtnStart.x) / (float)saveImg.cols) * 90, 1), saveImg.size(), 2);
		else
			warpAffine(saveImg, saveImg, getRotationMatrix2D(rotatePivot, ((point.y - 24 - rBtnStart.y) / (float)saveImg.rows) * 90, 1), saveImg.size(), 2);

		if (saveImg.cols % 8 != 0)
			cv::resize(saveImg, saveImg, cv::Size(saveImg.cols - saveImg.cols % 8, saveImg.rows), 0, 0, CV_INTER_NN);
		RedrawWindow();
		rotatePoint.x = rotatePivot.x + LUpoint.x;
		rotatePoint.y = rotatePivot.y + LUpoint.y;
		DisplayPasteGrabcut(IDC_PIC, saveImg, 3);
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


		if(BGimg.cols > 790)
			cv::resize(BGimg, BGimg, cv::Size(790, BGimg.rows * (1 - (BGimg.cols - 790) / BGimg.cols)), 0, 0, CV_INTER_NN);
		if (BGimg.rows > 700)
			cv::resize(BGimg, BGimg, cv::Size(BGimg.cols * (1 - (BGimg.rows - 700) / BGimg.rows), 700), 0, 0, CV_INTER_NN);

		if (BGimg.cols % 8 != 0)
			cv::resize(BGimg, BGimg, cv::Size(BGimg.cols - BGimg.cols % 8, BGimg.rows), 0, 0, CV_INTER_NN);

		cv::Mat bg(BGimg.size(), CV_8UC3);
		for (int i = 0;i < bg.cols;i++)
		{
			for (int j = 0;j < bg.rows;j++)
			{
				bg.at<Vec3b>(j, i)[0] = BGimg.at<Vec3b>(j, i)[0];
				bg.at<Vec3b>(j, i)[1] = BGimg.at<Vec3b>(j, i)[1];
				bg.at<Vec3b>(j, i)[2] = BGimg.at<Vec3b>(j, i)[2];
			}
		}
		
		BGimg = bg;
		DisplayOutput(IDC_Back, BGimg);



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
	if (pScrollBar)
	{
		// 어떤 슬라이더인지 검사
		if (pScrollBar == (CScrollBar*)&widthSlide || pScrollBar == (CScrollBar*)&allSlide)
		{
			if (pScrollBar == (CScrollBar*)&allSlide)  widthSlide.SetPos((widthSlide.GetPos() + allSlide.GetPos()) / 2);
			else allSlide.SetPos((widthSlide.GetPos() + heightSlide.GetPos()) / 2);
			// 슬라이더의 위치를 검사한다.
			nColPos = widthSlide.GetPos();
			

			resize(GrabCutImg, saveImg, cv::Size(nColPos, (int)saveImg.rows), 0, 0, CV_INTER_NN);
			if (saveImg.cols % 8 != 0)
				cv::resize(saveImg, saveImg, cv::Size(saveImg.cols - saveImg.cols % 8, saveImg.rows), 0, 0, CV_INTER_NN);

			resize(originGrab, saveOrigin, cv::Size(nColPos, (int)saveOrigin.rows), 0, 0, CV_INTER_NN);
			if (saveOrigin.cols % 8 != 0)
				cv::resize(saveOrigin, saveOrigin, cv::Size(saveOrigin.cols - saveOrigin.cols % 8, saveOrigin.rows), 0, 0, CV_INTER_NN);

			saveImg.copyTo(canny);
			cv::cvtColor(saveImg, canny, CV_BGR2GRAY);
			cv::Canny(canny, canny, 400, 400);
	//		cv::namedWindow("Original Image");
	//		cv::imshow("Original Image", canny);
			
			Compose(BGimg, saveOrigin, mouseMovePoint.x - saveImg.cols / 2, mouseMovePoint.y - saveImg.rows / 2, 2);
			DisplayPasteGrabcut(IDC_Back, saveImg, 2);
		}

		if (pScrollBar == (CScrollBar*)&heightSlide || pScrollBar == (CScrollBar*)&allSlide)
		{
			if (pScrollBar == (CScrollBar*)&allSlide) heightSlide.SetPos((heightSlide.GetPos() + allSlide.GetPos()) / 2);
			else allSlide.SetPos((widthSlide.GetPos() + heightSlide.GetPos()) / 2);
			// 슬라이더의 위치를 검사한다.
			nRowPos = heightSlide.GetPos();
			

			resize(GrabCutImg, saveImg, cv::Size((int)saveImg.cols, nRowPos), 0, 0, CV_INTER_NN);
			if (saveImg.cols % 8 != 0)
				cv::resize(saveImg, saveImg, cv::Size(saveImg.cols - saveImg.cols % 8, saveImg.rows), 0, 0, CV_INTER_NN);

			resize(originGrab, saveOrigin, cv::Size((int)saveOrigin.cols, nRowPos), 0, 0, CV_INTER_NN);
			if (saveOrigin.cols % 8 != 0)
				cv::resize(saveOrigin, saveOrigin, cv::Size(saveOrigin.cols - saveOrigin.cols % 8, saveOrigin.rows), 0, 0, CV_INTER_NN);

			saveImg.copyTo(canny);
			cv::cvtColor(saveImg, canny, CV_BGR2GRAY);
			cv::Canny(canny, canny, 400, 400);

			Compose(BGimg, saveOrigin, mouseMovePoint.x - saveImg.cols / 2, mouseMovePoint.y - saveImg.rows / 2, 2);
			DisplayPasteGrabcut(IDC_Back, saveImg, 2);


		}

	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CDlgTab2::OnRButtonDown(UINT nFlags, CPoint point)
{
	if((point.x <= imgCenterPoint.x + 50 && point.x >= imgCenterPoint.x - 50)
		&& (point.y <= imgCenterPoint.y - (saveImg.rows/2) + 50 && point.y >= imgCenterPoint.y - (saveImg.rows/2) - 50) && !isResizeClick && !isTransportClick)
		isRotationClick = true;
	

	if (!GrabCutImg.data)
		return;


	CDialog::OnRButtonDown(nFlags, point);
}


void CDlgTab2::OnRButtonUp(UINT nFlags, CPoint point)
{
	isRotationClick = false;


	CDialog::OnRButtonUp(nFlags, point);
}
