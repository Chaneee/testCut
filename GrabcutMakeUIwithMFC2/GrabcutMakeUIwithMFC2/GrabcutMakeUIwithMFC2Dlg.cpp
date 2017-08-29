
// GrabcutMakeUIwithMFC2Dlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "GrabcutMakeUIwithMFC2.h"
#include "GrabcutMakeUIwithMFC2Dlg.h"
#include "afxdialogex.h"
#include "GrabcutOutput.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
GrabcutOutput *outputDlg; //Output 다이얼로그

//전역변수들///////////////////////
bool isDrawingBox = false;
bool isReadyInput = false;
bool isGrabCutFinsh = false; //그랩컷 알고리즘이 끝남을 알림
int userBoxCount = 0;
Mat inputImg;
//마우스 시작점, 끝점
CPoint mouseStart, mouseEnd;
//////////////////////////////////

//CPen mouseMovePen;

//그랩컷 돌아가는 함수
Mat doGrabcut(Mat targetMat)
{

	// Mat -> IplImage* 형 변환
	IplImage* imageCopy;
	IplImage temp;
	temp = targetMat;
	imageCopy = &temp;

	IplImage* tempForCopy = cvCloneImage(imageCopy);

	cv::Rect rectangle(mouseStart.x, mouseStart.y, mouseEnd.x, mouseEnd.y);

	//사각형 안그려진 이미지를 다시 로드

	cv::Mat result, tempFG, tempPRFG; // 분할 (4자기 가능한 값)
	cv::Mat bgModel, fgModel; // 모델 (초기 사용)

	cv::grabCut(targetMat,    // 입력 영상
		result,    // 분할 결과
		rectangle,   // 전경을 포함하는 직사각형
		bgModel, fgModel, // 모델
		10,     // 반복 횟수
		cv::GC_INIT_WITH_RECT);
	
	cv::compare(result, cv::GC_PR_FGD, tempPRFG, cv::CMP_EQ);
	cv::compare(result, cv::GC_FGD, tempFG, cv::CMP_EQ);

	// 전경일 가능성이 있는 화소를 마크한 것을 가져오기
	cv::Mat foreground(targetMat.size(), CV_8UC3, cv::Scalar(255, 255, 255));
	// 결과 영상 생성
	targetMat.copyTo(foreground, tempPRFG);
	targetMat.copyTo(foreground, tempFG);
	isGrabCutFinsh = true;
	return foreground;
	// 배경 화소는 복사되지 않음
	/*cv::namedWindow("Result");
	cv::imshow("Result", result);

	cv::namedWindow("Foreground");
	cv::imshow("Foreground", foreground);
	cv::imwrite("output.jpg", foreground);

	cv::waitKey(0);*/
}

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGrabcutMakeUIwithMFC2Dlg 대화 상자



CGrabcutMakeUIwithMFC2Dlg::CGrabcutMakeUIwithMFC2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_GRABCUTMAKEUIWITHMFC2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGrabcutMakeUIwithMFC2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Image_open, imgOpen);
	DDX_Control(pDX, IDC_STDisplay, mstDisplay);
}

BEGIN_MESSAGE_MAP(CGrabcutMakeUIwithMFC2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_BN_CLICKED(IDC_BUTTON1, &CGrabcutMakeUIwithMFC2Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_Image_open, &CGrabcutMakeUIwithMFC2Dlg::OnBnClickedImageopen)
	ON_BN_CLICKED(IDC_OutputBtn, &CGrabcutMakeUIwithMFC2Dlg::OnBnClickedOutputbtn)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CGrabcutMakeUIwithMFC2Dlg 메시지 처리기

BOOL CGrabcutMakeUIwithMFC2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CGrabcutMakeUIwithMFC2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CGrabcutMakeUIwithMFC2Dlg::OnPaint()
{
	//그랩컷 끝나기 전엔 버튼 비활성화
	if(!isGrabCutFinsh) GetDlgItem(IDC_OutputBtn)->EnableWindow(FALSE); 
	
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CGrabcutMakeUIwithMFC2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGrabcutMakeUIwithMFC2Dlg::OnBnClickedImageopen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	char openFilter[] = "Image (*.BMP, *.GIF, *.JPG, *.PNG) | *.BMP;*.GIF;*.JPG;*.PNG;*.bmp;*.gif;*.jpg;*.png | All Files(*.*)|*.*||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, openFilter, AfxGetMainWnd());
	if (dlg.DoModal() == IDOK)
	{
		CString cstrImgPath = dlg.GetPathName();
		//AfxMessageBox(cstrImgPath);
		
		inputImg = imread(string(cstrImgPath));

		if (inputImg.cols % 8 != 0) {
			cv::resize(inputImg, inputImg, cv::Size(inputImg.cols - inputImg.cols%8, inputImg.rows), 0, 0, CV_INTER_NN);
		}
		DisplayImage(IDC_PIC, inputImg);
		isReadyInput = true;
	}
}

// 디스플레이 함수
void CGrabcutMakeUIwithMFC2Dlg::DisplayImage(int IDC_PICTURE_TARGET, Mat targetMat)
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
}


void CGrabcutMakeUIwithMFC2Dlg::OnBnClickedOutputbtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	outputDlg = new GrabcutOutput();
	outputDlg->Create(IDD_Output, this);
	outputDlg->ShowWindow(SW_SHOW);	
}

//마우스로 그리기 위한 클래스
void CGrabcutMakeUIwithMFC2Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (isReadyInput) {
		mouseStart = point;
		isDrawingBox = true;
		oldMousePoint = point;
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}

//마우스 떼면 박스 생성
void CGrabcutMakeUIwithMFC2Dlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (isDrawingBox) {
		mouseEnd = point;
		CRect grabRect;
		grabRect.SetRect(mouseStart.x, mouseStart.y, mouseEnd.x, mouseEnd.y);

		CClientDC dc(this);
		CPen pen;
		pen.CreatePen(PS_DOT, 1, RGB(255, 0, 0));    // 빨간색 펜을 생성
		CPen* oldPen = dc.SelectObject(&pen);
		CBrush brush;
		brush.CreateStockObject(NULL_BRUSH);    // 투명 브러시
		CBrush* pOldBrush = dc.SelectObject(&brush);
		dc.Rectangle(grabRect);
		dc.SelectObject(pOldBrush);
		doGrabcut(inputImg); //그랩컷 실행
		isDrawingBox = false;
		isReadyInput = false;
	}
	CDialogEx::OnLButtonUp(nFlags, point);
}

//마우스 움직임
void CGrabcutMakeUIwithMFC2Dlg::OnMouseMove(UINT nFlags, CPoint point)
{
	nowMousePos = point;
	CPen mouseMovePen;
	//Invalidate();
	// 마우스 누른채로 드래그하면 상자 생성

	if (isDrawingBox) {
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
		CDialog::OnMouseMove(nFlags, point);

		/*grabRect.SetRect(mouseStart.x, mouseStart.y, point.x, point.y);
		
		mouseMovePen.CreatePen(PS_DOT, 1, RGB(255, 0, 0));    // 빨간색 펜을 생성
		CPen* oldPen = dc.SelectObject(&mouseMovePen);
		CBrush brush;
		brush.CreateStockObject(NULL_BRUSH);    // 투명 브러시
		CBrush* pOldBrush = dc.SelectObject(&brush);
		dc.Rectangle(grabRect);
		dc.SelectObject(pOldBrush);
		DeleteObject(grabRect);*/
	}

	//그랩컷 종료 후 버튼 활성화
	if (isGrabCutFinsh) GetDlgItem(IDC_OutputBtn)->EnableWindow(TRUE);
	CDialogEx::OnMouseMove(nFlags, point);
}
