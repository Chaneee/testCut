// GrabcutOutput.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "GrabcutMakeUIwithMFC2.h"
#include "GrabcutMakeUIwithMFC2Dlg.h"
#include "GrabcutOutput.h"
#include "afxdialogex.h"

//����������////////////////////
Mat BGimg;
Mat originGrabImg, GrabCutImg;
CPoint mouseMovePoint;
CPoint nowCutImgPoint=0;
bool isLeftMouseDown = false; // True : ���콺 ���� ������ �ִ� ���� / false : �� ����
//Rect btnRange = (,);

///////////////////////////////

// GrabcutOutput ��ȭ �����Դϴ�.

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


// GrabcutOutput �޽��� ó�����Դϴ�.


void GrabcutOutput::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	
	CDialog::OnRButtonDblClk(nFlags, point);
}


void GrabcutOutput::OnStnClickedOutput()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

//�̹��� ���÷��̸� ���� �Լ� : ���
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

//�̹��� ���÷��̸� ���� �Լ� : (mode=0 : �ٿ��ֱ�, mode=1 : �̵�)
void GrabcutOutput::DisplayPasteGrabcut(int IDC_PICTURE_TARGET, Mat targetMat, int mode)
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
			::StretchDIBits(dcImageTraget.GetSafeHdc(), rcImageTraget.left + BGimg.cols, rcImageTraget.top, rcImageTraget.right, rcImageTraget.bottom,
				0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
			cvReleaseImage(&tempImage);
			
		}

		else if (mode == 1)
		{
			DisplayOutput(IDC_OUTPUT, BGimg);
			::StretchDIBits(dcImageTraget.GetSafeHdc(), mouseMovePoint.x - GrabCutImg.cols / 2, mouseMovePoint.y - GrabCutImg.rows / 2,
				rcImageTraget.right, rcImageTraget.bottom ,
				0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
			cvReleaseImage(&tempImage);
			

		}
		
		//if (isLeftMouseDown)
		//	dcImageTraget.StretchBlt(0, 0, GrabCutImg.cols-100, GrabCutImg.rows-100, &dcImageTraget, 0, 0, GrabCutImg.cols, GrabCutImg.rows,SRCCOPY);
			
	}
}

//�� �̹����� �ռ��ϴ� �Լ�
void GrabcutOutput::Compose(Mat BGMat, Mat originMat, int target_X, int target_Y, int mode)
{
	//mode 0 : �׳� �ռ�(���κ� ���ٺ��̱⸸)

	for (int i = 0;i < originMat.cols;i++)
	{
		for (int j = 0;j < originMat.rows;j++)
		{
			if (originMat.at<Vec3b>(j, i)[0] == 4 && originMat.at<Vec3b>(j, i)[1] == 8 && originMat.at<Vec3b>(j, i)[2] == 6)
			{
				GrabCutImg.at<Vec3b>(j, i) = BGMat.at<Vec3b>(j + target_Y, i + target_X);		

			}
				

		}
	}
	
	


	
}


void GrabcutOutput::OnBnClickedCallbackground()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	char openFilter[] = "Image (*.BMP, *.GIF, *.JPG, *.PNG) | *.BMP;*.GIF;*.JPG;*.PNG;*.bmp;*.gif;*.jpg;*.png | All Files(*.*)|*.*||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, openFilter, AfxGetMainWnd());
	if (dlg.DoModal() == IDOK)
	{
		CString cstrImgPath = dlg.GetPathName();
		//AfxMessageBox(cstrImgPath);

		BGimg = imread(string(cstrImgPath));
		DisplayOutput(IDC_OUTPUT, BGimg);
		nowCutImgPoint.x = BGimg.cols + GrabCutImg.cols / 2;
		nowCutImgPoint.y = GrabCutImg.rows / 2;
	}
}


void GrabcutOutput::OnBnClickedPastegrab()
{//GrabCutImg
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	GrabCutImg = imread("output.png", CV_LOAD_IMAGE_UNCHANGED);
	GrabCutImg.copyTo(originGrabImg);
	DisplayPasteGrabcut(IDC_GRABIMG, GrabCutImg, 0);

}


void GrabcutOutput::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ((point.x <= nowCutImgPoint.x + GrabCutImg.cols / 2 && point.x >= nowCutImgPoint.x - GrabCutImg.cols / 2) 
		&& (point.y <= nowCutImgPoint.y + GrabCutImg.rows / 2 && point.y >= nowCutImgPoint.y - GrabCutImg.rows / 2))
	//{
	//	SetCapture();
		isLeftMouseDown = true;
	//	RedrawWindow();
	//}

	
	CDialog::OnLButtonDown(nFlags, point);
}


void GrabcutOutput::OnLButtonUp(UINT nFlags, CPoint point)
{

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (isLeftMouseDown)
	{
		Compose(BGimg, originGrabImg, mouseMovePoint.x - GrabCutImg.cols / 2, mouseMovePoint.y - GrabCutImg.rows / 2, 1);
		RedrawWindow();
		DisplayPasteGrabcut(IDC_GRABIMG, GrabCutImg, 1);
		
		nowCutImgPoint = point;
		isLeftMouseDown = false;

		//RedrawWindow();
	}
	
	CDialog::OnLButtonUp(nFlags, point);
}


void GrabcutOutput::OnMouseMove(UINT nFlags, CPoint point)
{

	if (isLeftMouseDown)
	{
		//RedrawWindow();
		mouseMovePoint = point;
		DisplayPasteGrabcut(IDC_GRABIMG, GrabCutImg, 1);
		
	}
	
	CDialog::OnMouseMove(nFlags, point);
}


void GrabcutOutput::OnStnClickedGrabimg()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


BOOL GrabcutOutput::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	ShowWindow(SW_SHOWMAXIMIZED);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
