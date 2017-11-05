// DlgTab2.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "maptools.h"
#include "DlgTab2.h"
#include "afxdialogex.h"

//전역변수들////////////////////
Mat BGimg;
Mat originGrabImg, GrabCutImg;
Mat canny;
CPoint mouseMovePoint;
CPoint nowCutImgPoint = 0;
Mat originGrab = imread("originInput.png", CV_LOAD_IMAGE_UNCHANGED);
bool isLeftMouseDown = false; // True : 마우스 왼쪽 누르고 있는 상태 / false : 뗀 상태
bool isStartEdit = false; //OnPaint부분에서 GrabCutImg를 부르기 때문에 써야하는 변수
							  
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
		else if (targetIplImage->nChannels == 3)
		{
			tempImage = cvCloneImage(targetIplImage);
		}

		bitmapInfo.bmiHeader.biBitCount = tempImage->depth * tempImage->nChannels;

		dcImageTraget.SetStretchBltMode(COLORONCOLOR);
		::StretchDIBits(dcImageTraget.GetSafeHdc(), rcImageTraget.left, rcImageTraget.top, rcImageTraget.right, rcImageTraget.bottom,
			0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);

		if (isLeftMouseDown)
			//dcImageTraget.StretchBlt(100, 100, inputImg.cols-100, inputImg.rows-100, &dcImageTraget, 0, 0, inputImg.cols+100, inputImg.rows+100,SRCCOPY);
			cvReleaseImage(&tempImage);
	}
}

//이미지 디스플레이를 위한 함수 : (mode=0 : 붙여넣기, mode=1 : 이동)
void CDlgTab2::DisplayPasteGrabcut(int IDC_PICTURE_TARGET, Mat targetMat, int mode)
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
		else if (targetIplImage->nChannels == 3)
		{
			tempImage = cvCloneImage(targetIplImage);
		}

		bitmapInfo.bmiHeader.biBitCount = tempImage->depth * tempImage->nChannels;

		dcImageTraget.SetStretchBltMode(COLORONCOLOR);

		if (mode == 0)
		{
			::StretchDIBits(dcImageTraget.GetSafeHdc(), rcImageTraget.left, rcImageTraget.top, rcImageTraget.right, rcImageTraget.bottom,
				0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
			cvReleaseImage(&tempImage);

		}

		else if (mode == 1)
		{
			DisplayOutput(IDC_Back, BGimg);
			::StretchDIBits(dcImageTraget.GetSafeHdc(), mouseMovePoint.x - GrabCutImg.cols / 2, mouseMovePoint.y - GrabCutImg.rows / 2,
				rcImageTraget.right, rcImageTraget.bottom,
				0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
			cvReleaseImage(&tempImage);


		}

		//if (isLeftMouseDown)
		//	dcImageTraget.StretchBlt(0, 0, GrabCutImg.cols-100, GrabCutImg.rows-100, &dcImageTraget, 0, 0, GrabCutImg.cols, GrabCutImg.rows,SRCCOPY);

	}
}

//두 이미지를 합성하는 함수
void CDlgTab2::Compose(Mat BGMat, Mat originMat, int target_X, int target_Y, int mode)
{
	//mode 0 : 그냥 합성(배경부분 갖다붙이기)

	for (int i = 0;i < originMat.cols;i++)
	{
		for (int j = 0;j < originMat.rows;j++)
		{
		if (originMat.at<Vec3b>(j, i) == originGrab.at<Vec3b>(j, i) * 0.2)
				GrabCutImg.at<Vec3b>(j, i) = BGMat.at<Vec3b>(j + target_Y, i + target_X);

			
			else if (canny.at<uchar>(j, i) == 255)
			{

				GrabCutImg.at<Vec3b>(j, i)[0] = BGMat.at<Vec3b>(j + target_Y, i + target_X)[0] * 2 / 3 + originMat.at<Vec3b>(j, i)[0] * 1 / 3;

				GrabCutImg.at<Vec3b>(j, i)[1] = BGMat.at<Vec3b>(j + target_Y, i + target_X)[1] * 2 / 3 + originMat.at<Vec3b>(j, i)[1] * 1 / 3;

				//GrabCutImg.at<Vec3b>(j, i)[1] = /*BGMat.at<Vec3b>(j + target_Y, i + target_X)[1] / 200 * originMat.at<Vec3b>(j, i)[1];
				GrabCutImg.at<Vec3b>(j, i)[2] = BGMat.at<Vec3b>(j + target_Y, i + target_X)[2] * 2 / 3 + originMat.at<Vec3b>(j, i)[2] * 1 / 3;


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
		GrabCutImg = imread("output.png", CV_LOAD_IMAGE_UNCHANGED);
		GrabCutImg.copyTo(originGrabImg);
		GrabCutImg.copyTo(canny);
		cv::cvtColor(GrabCutImg, canny, CV_BGR2GRAY);
		cv::Canny(canny, canny, 200, 200);
		//EdgePosition();
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
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
{
	if ((point.x <= nowCutImgPoint.x + GrabCutImg.cols / 2 && point.x >= nowCutImgPoint.x - GrabCutImg.cols / 2)
		&& (point.y <= nowCutImgPoint.y + GrabCutImg.rows / 2 && point.y >= nowCutImgPoint.y - GrabCutImg.rows / 2))
		isLeftMouseDown = true;

	CDialog::OnLButtonDown(nFlags, point);
}


void CDlgTab2::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (isLeftMouseDown)
	{
		Compose(BGimg, originGrabImg, point.x - GrabCutImg.cols / 2, point.y - GrabCutImg.rows / 2, 1);
		RedrawWindow();

		//cv::namedWindow("Original Image");
		//cv::imshow("Original Image", GrabCutImg);
		DisplayPasteGrabcut(IDC_Back, GrabCutImg, 1);

		nowCutImgPoint = point;
		isLeftMouseDown = false;

	}

	CDialog::OnLButtonUp(nFlags, point);
}


void CDlgTab2::OnMouseMove(UINT nFlags, CPoint point)
{
	if (isLeftMouseDown)
	{
		mouseMovePoint = point;
		DisplayPasteGrabcut(IDC_Back, GrabCutImg, 1);

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

		if (BGimg.cols % 8 != 0)
			cv::resize(BGimg, BGimg, cv::Size(BGimg.cols - BGimg.cols % 8, BGimg.rows), 0, 0, CV_INTER_NN);
		DisplayOutput(IDC_Back, BGimg);
		nowCutImgPoint.x = BGimg.cols + GrabCutImg.cols / 2;
		nowCutImgPoint.y = GrabCutImg.rows / 2;
	}
	isStartEdit = true;
}
