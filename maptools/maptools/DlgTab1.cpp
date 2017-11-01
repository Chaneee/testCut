// DlgTab1.cpp : ���� �����Դϴ�.
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

//����������///////////////////////
bool isDrawingBox = false;// �簢�� ���� �� True : ���콺 ���� ������ �ִ� ���� / false : �� ����
bool isRMouseDown = false; // True : ���콺 ������ ������ �ִ� ���� / false : �� ����
bool isReadyInput = false;
bool isGrabCutFinsh = false; //�׷��� �˰������� ������ �˸�
int backCount = 0;

Mat inputImg, originInput;
vector<Mat> imgStore;
cv::Mat result, tempFG, tempPRFG; // ���� (4�ڱ� ������ ��)

								  //���콺 ������, ����
CPoint mouseStart, mouseEnd;
CPoint originRangeStart, originRangeEnd;
//////////////////////////////////


// CDlgTab1 ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgTab1, CDialog)

CDlgTab1::CDlgTab1(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG1, pParent)
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
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Mat CDlgTab1::doGrabcut(Mat targetMat, int mode)
{
	int grabcutMode;
	if (mode == 0)
		grabcutMode = cv::GC_INIT_WITH_RECT;
	else
		grabcutMode = mode;

	// Mat -> IplImage* �� ��ȯ
	IplImage* imageCopy;
	IplImage temp;
	temp = targetMat;
	imageCopy = &temp;

	IplImage* tempForCopy = cvCloneImage(imageCopy);

	cv::Rect rectangle(mouseStart.x, mouseStart.y, mouseEnd.x, mouseEnd.y);

	//�簢�� �ȱ׷��� �̹����� �ٽ� �ε�


	cv::Mat bgModel, fgModel; // �� (�ʱ� ���)

	cv::grabCut(targetMat,    // �Է� ����
		result,    // ���� ���
		rectangle,   // ������ �����ϴ� ���簢��
		bgModel, fgModel, // ��
		1,     // �ݺ� Ƚ��
		grabcutMode);

	cv::compare(result, cv::GC_PR_FGD, tempPRFG, cv::CMP_EQ);
	cv::compare(result, cv::GC_FGD, tempFG, cv::CMP_EQ);

	// ������ ���ɼ��� �ִ� ȭ�Ҹ� ��ũ�� ���� ��������
	cv::Mat foreground(targetMat.size(), CV_8UC3, cv::Scalar(4, 8, 6, 0));
	// ��� ���� ����
	targetMat.copyTo(foreground, tempPRFG);
	targetMat.copyTo(foreground, tempFG);

	foreground.copyTo(inputImg);

	DisplayImage(IDC_PIC, foreground);
	isGrabCutFinsh = true;


	imgStore.push_back(foreground);
	//backCount = 0; //�ʱ�ȭ
	return foreground;

}

//���÷��� �Լ�
void CDlgTab1::DisplayImage(int IDC_PICTURE_TARGET, Mat targetMat)
{
	/*IplImage* targetIplImage = new IplImage(targetMat);
	if (targetIplImage != nullptr) {
	CWnd* pWnd_ImageTraget = (CWnd*)GetDlgItem(IDC_PICTURE_TARGET);//
	CDC *dc = pWnd_ImageTraget->GetDC();////
	CStatic *staticSize = (CStatic *)GetDlgItem(IDC_PICTURE_TARGET);////
	//CClientDC dcImageTraget(pWnd_ImageTraget);

	CRect rcImageTraget; //
	pWnd_ImageTraget->GetClientRect(&rcImageTraget);
	rcImageTraget.top = 0;
	rcImageTraget.left = 0;
	rcImageTraget.bottom = targetMat.rows;
	rcImageTraget.right = targetMat.cols;
	CString path = _T("C:\\Users\Chan\Desktop\test\GrabCutTest\GrabCutTest\\input.jpg");
	staticSize->GetClientRect(rcImageTraget);////
	m_Img.Load(path);
	m_Img.Draw(dc->m_hDC, 0, 0, m_Img.GetWidth(), m_Img.GetHeight());
	/*
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
	tempImage = cvCreateImage(cvSize(targetIplImage->width/10, targetIplImage->height/10), IPL_DEPTH_8U, 3);
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
	cvReleaseImage(&tempImage);*/
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
	cv::imwrite("output.png", targetMat);

}


// CDlgTab1 �޽��� ó�����Դϴ�.

void CDlgTab1::OnBnClickedMfcbutton1()
{
	
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

}

/*
void CDlgTab1::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}*/

void CDlgTab1::OnBnClickedButton2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	MessageBox(TEXT("Hello??????!"));
}

//�ҷ�����
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

		if (inputImg.cols % 8 != 0)
			cv::resize(inputImg, inputImg, cv::Size(inputImg.cols - inputImg.cols % 8, inputImg.rows), 0, 0, CV_INTER_NN);

		imgStore.push_back(inputImg);
		inputImg.copyTo(originInput);


		DisplayImage(IDC_PIC, inputImg);
		isReadyInput = true;
	}
}


void CDlgTab1::OnBnClickedButton4()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}



//���콺 ���� �̺�Ʈ
void CDlgTab1::OnLButtonDown(UINT nFlags, CPoint point)
{
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
		mouseEnd = point;
		CRect grabRect;
		grabRect.SetRect(mouseStart.x, mouseStart.y, mouseEnd.x, mouseEnd.y);

		CClientDC dc(this);
		CPen pen;
		pen.CreatePen(PS_DOT, 2, RGB(0, 255, 0));    // ������ ���� ����
		CPen* oldPen = dc.SelectObject(&pen);
		CBrush brush;
		brush.CreateStockObject(NULL_BRUSH);    // ���� �귯��
		CBrush* pOldBrush = dc.SelectObject(&brush);
		dc.Rectangle(grabRect);
		dc.SelectObject(pOldBrush);
		if (!isGrabCutFinsh)
		{
			originRangeEnd = point;
			doGrabcut(inputImg, 0); //�׷��� ����
		}

		else if (isGrabCutFinsh)
			doGrabcut(originInput, 2); //�׷��� ����
		isDrawingBox = false;
		isReadyInput = false;
	}

	CDialog::OnLButtonUp(nFlags, point);
}

//���콺 ������ �̺�Ʈ
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
		pen.CreatePen(PS_DASH, 2, RGB(255, 0, 0));    // ������ ���� ����
		CPen* oldPen = dc.SelectObject(&pen);
		CBrush brush;
		brush.CreateStockObject(NULL_BRUSH);    // ���� �귯��
		CBrush* pOldBrush = dc.SelectObject(&brush);
		dc.Rectangle(grabRect);
		dc.SelectObject(pOldBrush);
		//inputImg.at<uchar>(point.x, point.y) = GC_BGD;
		doGrabcut(originInput, 1); //�׷��� ����

		isRMouseDown = false;
	}

	CDialog::OnRButtonUp(nFlags, point);
}

//���콺 �巡��
void CDlgTab1::OnMouseMove(UINT nFlags, CPoint point)
{
	nowMousePos = point;
	CPen mouseMovePen;
	//Invalidate();



	// ���콺 ����ä�� �巡���ϸ� ���� ����
	if (isDrawingBox || isRMouseDown) {
		CRect grabRect;
		CClientDC dc(this);

		CDC* cdc = GetDC();
		cdc->SetROP2(R2_NOT);//�������� ȿ��
		grabRect.SetRect(mouseStart.x, mouseStart.y, oldMousePoint.x, oldMousePoint.y);
		cdc->Rectangle(grabRect);

		//���� �׷���
		grabRect.SetRect(mouseStart.x, mouseStart.y, nowMousePos.x, nowMousePos.y);
		cdc->Rectangle(grabRect);

		oldMousePoint = point;       // ������ �������ش�.
		ReleaseDC(cdc);
		CDialog::OnMouseMove(nFlags, point);

	}

	//�׷��� ���� �� ��ư Ȱ��ȭ
/*	if (isGrabCutFinsh)
	{
		GetDlgItem(IDC_OutputBtn)->EnableWindow(TRUE);
		//�ڷΰ����ư ��Ȱ��
		if (imgStore.size() >= 1)
			GetDlgItem(IDC_Back)->EnableWindow(TRUE);

	}*/

	CDialog::OnMouseMove(nFlags, point);
}

//�ǵ�����
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