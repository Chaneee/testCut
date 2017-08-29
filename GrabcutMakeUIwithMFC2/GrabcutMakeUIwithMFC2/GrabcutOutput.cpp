// GrabcutOutput.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GrabcutMakeUIwithMFC2.h"
#include "GrabcutMakeUIwithMFC2Dlg.h"
#include "GrabcutOutput.h"
#include "afxdialogex.h"

//전역변수들////////////////////
Mat callBG;
Mat GrabCutImg = imread("output.png");
CPoint mouseMovePoint;
bool isLeftMouseDown = false; // True : 마우스 왼쪽 누르고 있는 상태 / false : 뗀 상태

///////////////////////////////

// GrabcutOutput 대화 상자입니다.

IMPLEMENT_DYNAMIC(GrabcutOutput, CDialog)

GrabcutOutput::GrabcutOutput(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_Output, pParent)
{
	//Mat outputImg = imread("ouput.jpg");
	//DisplayOutput(IDC_OUTPUT, outputImg);
}

GrabcutOutput::~GrabcutOutput()
{
}

void GrabcutOutput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(GrabcutOutput, CDialog)
	ON_WM_RBUTTONDBLCLK()
	ON_STN_CLICKED(IDC_OUTPUT, &GrabcutOutput::OnStnClickedOutput)
//	ON_BN_CLICKED(IDOK, &GrabcutOutput::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CALLBACKGROUND, &GrabcutOutput::OnBnClickedCallbackground)
	ON_BN_CLICKED(IDC_PASTEGRAB, &GrabcutOutput::OnBnClickedPastegrab)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_STN_CLICKED(IDC_GRABIMG, &GrabcutOutput::OnStnClickedGrabimg)
END_MESSAGE_MAP()


// GrabcutOutput 메시지 처리기입니다.


void GrabcutOutput::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
	CDialog::OnRButtonDblClk(nFlags, point);
}


void GrabcutOutput::OnStnClickedOutput()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

//이미지 디스플레이를 위한 함수 : 배경
void GrabcutOutput::DisplayOutput(int IDC_PICTURE_TARGET, Mat targetMat)
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

		if(isLeftMouseDown)
		//dcImageTraget.StretchBlt(100, 100, inputImg.cols-100, inputImg.rows-100, &dcImageTraget, 0, 0, inputImg.cols+100, inputImg.rows+100,SRCCOPY);
		cvReleaseImage(&tempImage);
	}
}

//이미지 디스플레이를 위한 함수 : 붙여넣기
void GrabcutOutput::DisplayPasteGrabcut(int IDC_PICTURE_TARGET, Mat targetMat)
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
		cvReleaseImage(&tempImage);
		//if (isLeftMouseDown)
		//	dcImageTraget.StretchBlt(0, 0, GrabCutImg.cols-100, GrabCutImg.rows-100, &dcImageTraget, 0, 0, GrabCutImg.cols, GrabCutImg.rows,SRCCOPY);
			
	}
}


void GrabcutOutput::OnBnClickedCallbackground()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	char openFilter[] = "Image (*.BMP, *.GIF, *.JPG, *.PNG) | *.BMP;*.GIF;*.JPG;*.PNG;*.bmp;*.gif;*.jpg;*.png | All Files(*.*)|*.*||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, openFilter, AfxGetMainWnd());
	if (dlg.DoModal() == IDOK)
	{
		CString cstrImgPath = dlg.GetPathName();
		//AfxMessageBox(cstrImgPath);

		callBG = imread(string(cstrImgPath));
		DisplayOutput(IDC_OUTPUT, callBG);

	}
}


void GrabcutOutput::OnBnClickedPastegrab()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	
	DisplayPasteGrabcut(IDC_GRABIMG, GrabCutImg);

}


void GrabcutOutput::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	isLeftMouseDown = true;
	CDialog::OnLButtonDown(nFlags, point);
}


void GrabcutOutput::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
	isLeftMouseDown = false;
	CDialog::OnLButtonUp(nFlags, point);
}


void GrabcutOutput::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.


	mouseMovePoint = point;
	CDialog::OnMouseMove(nFlags, point);
}


void GrabcutOutput::OnStnClickedGrabimg()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
