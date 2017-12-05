// DlgTab1.cpp : 구현 파일입니다.
//
#include <vector>

#include "stdafx.h"
#include "maptools.h"
#include "DlgTab1.h"
#include "afxdialogex.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

//전역변수들///////////////////////
bool isDrawingBox = false;// 사각형 만들 때 True : 마우스 왼쪽 누르고 있는 상태 / false : 뗀 상태
bool isRMouseDown = false; // True : 마우스 오른쪽 누르고 있는 상태 / false : 뗀 상태
bool isReadyInput = false;
bool isGrabCutFinsh = false; //그랩컷 알고리즘이 끝남을 알림
int backCount = 0;

Mat inputImg, originInput, outPutImg;
vector<Mat> imgStore;
cv::Mat result, tempFG, tempPRFG; // 분할 (4자기 가능한 값)

								  //마우스 시작점, 끝점
CPoint mouseStart, mouseEnd;
CPoint originRangeStart, originRangeEnd;

Rect firstRect;
//////////////////////////////////


// CDlgTab1 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgTab1, CDialog)

CDlgTab1::CDlgTab1(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG1, pParent)
	, m_bWaitCursor(false)
{
	
}

CDlgTab1::~CDlgTab1()
{
}

void CDlgTab1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgTab1, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgTab1::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgTab1::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON4, &CDlgTab1::OnBnClickedButton4)

//	ON_STN_CLICKED(IDC_PIC, &CDlgTab1::OnStnClickedPic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_Back, &CDlgTab1::OnClickedBack)
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Mat CDlgTab1::doGrabcut(Mat targetMat, int mode)
{
	int grabcutMode;
	if (mode == 0)
		grabcutMode = cv::GC_INIT_WITH_RECT;
	else
		grabcutMode = mode;

	// Mat -> IplImage* 형 변환 왜하는거더라 이거
/*	IplImage* imageCopy;
	IplImage temp;
	temp = targetMat;
	imageCopy = &temp;

	IplImage* tempForCopy = cvCloneImage(imageCopy);
	CPoint Temp = mouseEnd;
	::GetCursorPos(&Temp);
	if (Temp.y > originInput.rows)
		mouseEnd.y = originInput.rows - 10;
	if (Temp.x > originInput.cols)
		mouseEnd.x = originInput.cols - 10;*/

	cv::Rect rectangle(mouseStart.x, mouseStart.y, mouseEnd.x, mouseEnd.y);
	
	if (mode == 0)
	{	//	mouseEnd.x = mouseEnd.x - (originInput.cols*0.3);
		//	mouseEnd.y = mouseEnd.y - (originInput.rows*0.3);
		int startX = mouseStart.x, startY = mouseStart.y, endX = mouseEnd.x - mouseStart.x, endY = mouseEnd.y - mouseStart.y;
		cv::Rect rect(startX, startY, endX, endY ); //잘라내기위함
		firstRect = rect;
		Mat cutOrigin(targetMat.size(), CV_8UC3) ;
		targetMat.copyTo(cutOrigin);
		cutOrigin = targetMat(rect);

		cv::imwrite("originInput.png", cutOrigin);
	}
	//사각형 안그려진 이미지를 다시 로드


	cv::Mat bgModel, fgModel; // 모델 (초기 사용)

	cv::grabCut(targetMat,    // 입력 영상
		result,    // 분할 결과
		rectangle,   // 전경을 포함하는 직사각형
		bgModel, fgModel, // 모델
		1,     // 반복 횟수
		grabcutMode);

	cv::compare(result, cv::GC_PR_FGD, tempPRFG, cv::CMP_EQ);
	cv::compare(result, cv::GC_FGD, tempFG, cv::CMP_EQ);

	// 전경일 가능성이 있는 화소를 마크한 것을 가져오기
	cv::Mat foreground(targetMat.size(), CV_8UC3, cv::Scalar(4, 8, 6, 0));

	for (int i = 0;i < targetMat.cols;i++)
		for (int j = 0;j < targetMat.rows;j++)
			foreground.at<Vec3b>(j, i) = targetMat.at<Vec3b>(j, i) * 0.2;

	// 결과 영상 생성
	targetMat.copyTo(foreground, tempPRFG);
	targetMat.copyTo(foreground, tempFG);

	foreground.copyTo(inputImg);
	m_bWaitCursor = FALSE;
	DisplayImage(IDC_PIC, foreground);
	isGrabCutFinsh = true;


	imgStore.push_back(foreground);
	//backCount = 0; //초기화
	return foreground;

}

//디스플레이 함수
void CDlgTab1::DisplayImage(int IDC_PICTURE_TARGET, Mat targetMat)
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

		/////////////////////
		//dcImageTraget.StretchBlt(100, 100, inputImg.cols-100, inputImg.rows-100, &dcImageTraget, 0, 0, inputImg.cols+100, inputImg.rows+100,SRCCOPY);
		cvReleaseImage(&tempImage);
	}

	//Mat saveImg;
	//targetMat.copyTo(saveImg);
	//saveImg = targetMat(Range(originRangeStart.x, originRangeStart.y), Range(originRangeEnd.x, originRangeEnd.y));
	outPutImg = targetMat;
	Mat cutGrab;
	cutGrab = targetMat(firstRect);
	cv::imwrite("output.png", cutGrab);
	//cv::imwrite("output.png", targetMat);
	delete targetIplImage;
}


// CDlgTab1 메시지 처리기입니다.

void CDlgTab1::OnBnClickedMfcbutton1()
{
	
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

}

/*
void CDlgTab1::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}*/

void CDlgTab1::OnBnClickedButton2()
{
	cv::imwrite("output.png", outPutImg);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//MessageBox(TEXT("Hello??????!"));
}

//불러오기
void CDlgTab1::OnBnClickedButton1()
{
	CString openFilter = _T("Image (*.BMP, *.GIF, *.JPG, *.PNG) | *.BMP;*.GIF;*.JPG;*.PNG;*.bmp;*.gif;*.jpg;*.png | All Files(*.*)|*.*||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, openFilter, AfxGetMainWnd());
	if (dlg.DoModal() == IDOK)
	{
		CString cstrImgPath = dlg.GetPathName();
		CT2CA pszConvertedAnsiString(cstrImgPath);
		std::string cstrImgPathString(pszConvertedAnsiString);
		//m_Img.Load(openFilter);

		//AfxMessageBox(cstrImgPath);
		inputImg = imread(string(cstrImgPathString), CV_LOAD_IMAGE_UNCHANGED);
		
		//크기조절
		if(inputImg.rows > 388)
			cv::resize(inputImg, inputImg, cv::Size(inputImg.cols * 388 / inputImg.rows, 388), 0, 0, CV_INTER_CUBIC);

		if (inputImg.cols > 360)
			cv::resize(inputImg, inputImg, cv::Size(360, inputImg.rows * 360 / inputImg.cols), 0, 0, CV_INTER_CUBIC);

		if (inputImg.cols % 8 != 0)
			cv::resize(inputImg, inputImg, cv::Size(inputImg.cols - inputImg.cols % 8, inputImg.rows), 0, 0, CV_INTER_CUBIC);

		imgStore.push_back(inputImg);
		inputImg.copyTo(originInput);
		
		
		DisplayImage(IDC_PIC, inputImg);
		isReadyInput = true;
	}
}


void CDlgTab1::OnBnClickedButton4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}



//마우스 왼쪽 이벤트
void CDlgTab1::OnLButtonDown(UINT nFlags, CPoint point)
{

	if (!inputImg.data)
		return;

	if (isReadyInput || isGrabCutFinsh) {
		if (!isGrabCutFinsh)
			originRangeStart = point;

		mouseStart = point;
		isDrawingBox = true;
		oldMousePoint = point;
	}

	CDialog::OnLButtonDown(nFlags, point);
}


void CDlgTab1::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (isDrawingBox) {
		if (point.x >= inputImg.cols)	mouseEnd.x = inputImg.cols;
		else mouseEnd.x = point.x;

		if (point.y >= inputImg.rows)	mouseEnd.y = inputImg.rows;
		else mouseEnd.y = point.y;

	//	mouseEnd.x = mouseEnd.x - (originInput.cols*0.3);
	//	mouseEnd.y = mouseEnd.y - (originInput.rows*0.3);

		//GetCursorPos(&mouseEnd);
		CRect grabRect;
		grabRect.SetRect(mouseStart.x, mouseStart.y, mouseEnd.x, mouseEnd.y);

		CClientDC dc(this);
		CPen pen;
		pen.CreatePen(PS_DOT, 2, RGB(0, 255, 0));    // 빨간색 펜을 생성
		CPen* oldPen = dc.SelectObject(&pen);
		CBrush brush;
		brush.CreateStockObject(NULL_BRUSH);    // 투명 브러시
		CBrush* pOldBrush = dc.SelectObject(&brush);
		dc.Rectangle(grabRect);
		dc.SelectObject(pOldBrush);
		m_bWaitCursor = TRUE;
		if (!isGrabCutFinsh)
		{
			originRangeEnd = point;
			doGrabcut(inputImg, 0); //그랩컷 실행
		}

		else if (isGrabCutFinsh)
			doGrabcut(originInput, 2); //그랩컷 실행
		
		isDrawingBox = false;
		isReadyInput = false;
	}

	CDialog::OnLButtonUp(nFlags, point);
}

//마우스 오른쪽 이벤트
void CDlgTab1::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (isGrabCutFinsh)
	{
		mouseStart = point;
		isRMouseDown = true;
		oldMousePoint = point;
	}

	CDialog::OnRButtonDown(nFlags, point);
}


void CDlgTab1::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (isRMouseDown) {
		mouseEnd = point;
		CRect grabRect;
		grabRect.SetRect(mouseStart.x, mouseStart.y, mouseEnd.x, mouseEnd.y);

		CClientDC dc(this);
		CPen pen;
		pen.CreatePen(PS_DASH, 2, RGB(255, 0, 0));    // 빨간색 펜을 생성
		CPen* oldPen = dc.SelectObject(&pen);
		CBrush brush;
		brush.CreateStockObject(NULL_BRUSH);    // 투명 브러시
		CBrush* pOldBrush = dc.SelectObject(&brush);
		dc.Rectangle(grabRect);
		dc.SelectObject(pOldBrush);
		//inputImg.at<uchar>(point.x, point.y) = GC_BGD;
		doGrabcut(originInput, 1); //그랩컷 실행

		isRMouseDown = false;
	}

	CDialog::OnRButtonUp(nFlags, point);
}

//마우스 드래그
void CDlgTab1::OnMouseMove(UINT nFlags, CPoint point)
{
	nowMousePos = point;
	//nowMousePos.x = nowMousePos.x - (originInput.cols*0.3);
	//nowMousePos.y = nowMousePos.y - (originInput.rows*0.3);
	
	//::GetCursorPos(&nowMousePos);
	//SetCursorPos(nowMousePos.x -(originInput.cols*0.3), nowMousePos.y-(originInput.rows*0.3));
	CPen mouseMovePen;
	//Invalidate();

	// 마우스 누른채로 드래그하면 상자 생성
	if (isDrawingBox || isRMouseDown) {
		//mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, nowMousePos.x * 65535 / 1920, nowMousePos.y * 65535 / 1080, 0, 0);
		CRect grabRect;
		CClientDC dc(this);

		CDC* cdc = GetDC();
		cdc->SetROP2(R2_NOT);//지워지는 효과
		grabRect.SetRect(mouseStart.x, mouseStart.y, oldMousePoint.x, oldMousePoint.y);
		cdc->Rectangle(grabRect);

		//새로 그려줌
		grabRect.SetRect(mouseStart.x, mouseStart.y, nowMousePos.x, nowMousePos.y);
		cdc->Rectangle(grabRect);

		oldMousePoint = point;       // 끝점을 저장해준다.
		ReleaseDC(cdc);
		//CDialog::OnMouseMove(nFlags, point);

	}

	//그랩컷 종료 후 버튼 활성화
	if (isGrabCutFinsh)
	{
		//되돌리기버튼 비활성
		if (imgStore.size() >= 1)
			GetDlgItem(IDC_Back)->EnableWindow(TRUE);

	}

	CDialog::OnMouseMove(nFlags, point);
}

//되돌리기
void CDlgTab1::OnClickedBack()
{
	imgStore.pop_back();
	if (imgStore.size() == 1)
	{
		DisplayImage(IDC_PIC, originInput);
		isGrabCutFinsh = false;
		isReadyInput = true;

		GetDlgItem(IDC_Back)->EnableWindow(FALSE);
	}

	else
		DisplayImage(IDC_PIC, imgStore[imgStore.size() - 1]);
}


void CDlgTab1::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.
	Mat howTo = imread("howto.JPG", CV_LOAD_IMAGE_UNCHANGED);
	if (howTo.cols % 8 != 0)
		cv::resize(howTo, howTo, cv::Size(howTo.cols - howTo.cols % 8, howTo.rows), 0, 0, CV_INTER_NN);
	DisplayHowTo(IDC_HOWTO, howTo);

	//그랩컷 끝나기 전엔 버튼 비활성화
	if (!isGrabCutFinsh)
	{
		//뒤로가기버튼
		GetDlgItem(IDC_Back)->EnableWindow(FALSE);
		//DisplayImage(IDC_PIC, inputImg);
	}
}


BOOL CDlgTab1::OnInitDialog()
{
	CDialog::OnInitDialog();
	//ShowWindow(SW_SHOWMAXIMIZED);
	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDlgTab1::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	SetCursor(LoadCursor(NULL, m_bWaitCursor ? IDC_WAIT : IDC_ARROW));
	return TRUE;

	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

//실행방법 띄울거
void CDlgTab1::DisplayHowTo(int IDC_PICTURE_TARGET, Mat targetMat)
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

		/////////////////////
		//dcImageTraget.StretchBlt(100, 100, inputImg.cols-100, inputImg.rows-100, &dcImageTraget, 0, 0, inputImg.cols+100, inputImg.rows+100,SRCCOPY);
		cvReleaseImage(&tempImage);
	}

	delete targetIplImage;
}