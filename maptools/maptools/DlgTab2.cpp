// DlgTab2.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "maptools.h"
#include "DlgTab2.h"
#include "afxdialogex.h"

//����������////////////////////
Mat BGimg;
Mat originGrabImg, GrabCutImg;
Mat canny;
CPoint mouseMovePoint;
CPoint nowCutImgPoint = 0;
Mat originGrab;
bool isLeftMouseDown = false; // True : ���콺 ���� ������ �ִ� ���� / false : �� ����
bool isStartEdit = false; //OnPaint�κп��� GrabCutImg�� �θ��� ������ ����ϴ� ����
							  
///////////////////////////////

// CDlgTab2 ��ȭ �����Դϴ�.

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

//�̹��� ���÷��̸� ���� �Լ� : ���
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

//�̹��� ���÷��̸� ���� �Լ� : (mode=0 : �ٿ��ֱ�, mode=1 : �̵�)
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
		else if (targetIplImage->nChannels == 3 || targetIplImage->nChannels == 4)
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
			//DisplayPasteGrabcut(IDC_Paste, GrabCutImg, 0);
			::StretchDIBits(dcImageTraget.GetSafeHdc(), mouseMovePoint.x - GrabCutImg.cols / 2, mouseMovePoint.y - GrabCutImg.rows / 2,
				rcImageTraget.right, rcImageTraget.bottom,
				0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
			cvReleaseImage(&tempImage);


		}

		//if (isLeftMouseDown)
		//	dcImageTraget.StretchBlt(0, 0, GrabCutImg.cols-100, GrabCutImg.rows-100, &dcImageTraget, 0, 0, GrabCutImg.cols, GrabCutImg.rows,SRCCOPY);

	}
}

//��� ���ϴ� �Լ�
int CDlgTab2::kMeanss(int clusterMemberCount, int sum, int mode)
{
	int A;

	
	A = sum / clusterMemberCount;


	return A;
}

//RGB���� YCbCr������. 0 = Y, 1 = Cb, 2 = Cr
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

//�� �̹����� �ռ��ϴ� �Լ�
void CDlgTab2::Compose(Mat BGMat, Mat originMat, int target_X, int target_Y, int mode)
{
	int sum[3] = { 0, 0, 0}; // 0:Y, 1:Cb, 2:Cr
	vector<int> inputY, inputCb, inputCr;
	int avrY, avrCb, avrCr;
	int clusterMemberCount = 0;

	//mode 0 : �׳� �ռ�(���κ� ���ٺ��̱�)
	for (int i = 0;i < originMat.cols;i++)
	{
		for (int j = 0;j < originMat.rows;j++)
		{
			if (originMat.at<Vec3b>(j, i) == originGrab.at<Vec3b>(j, i) * 0.2)
					GrabCutImg.at<Vec3b>(j, i) = BGMat.at<Vec3b>(j + target_Y, i + target_X);
	
			else /*if (canny.at<uchar>(j, i) == 255)*/
			{
				sum[0] += RGBtoYCbCr(BGMat, 0, j, i);
				sum[1] += RGBtoYCbCr(BGMat, 1, j, i);
				sum[2] += RGBtoYCbCr(BGMat, 2, j, i);
				inputY.push_back(RGBtoYCbCr(originMat, 0, j, i));
				inputCb.push_back(RGBtoYCbCr(originMat, 1, j, i));
				inputCr.push_back(RGBtoYCbCr(originMat, 2, j, i));
				clusterMemberCount++;
			/*	GrabCutImg.at<Vec3b>(j, i)[0] = BGMat.at<Vec3b>(j + target_Y, i + target_X)[0] * 1 / 2 + originMat.at<Vec3b>(j, i)[0] * 1 / 2;
				GrabCutImg.at<Vec3b>(j, i)[1] = BGMat.at<Vec3b>(j + target_Y, i + target_X)[1] * 1 / 2 + originMat.at<Vec3b>(j, i)[1] * 1 / 2;
				//GrabCutImg.at<Vec3b>(j, i)[1] = /*BGMat.at<Vec3b>(j + target_Y, i + target_X)[1] / 200 * originMat.at<Vec3b>(j, i)[1];
				GrabCutImg.at<Vec3b>(j, i)[2] = BGMat.at<Vec3b>(j + target_Y, i + target_X)[2] * 1 / 2 + originMat.at<Vec3b>(j, i)[2] * 1 / 2;
			*/

			}
		}
	}
	
	//������ġ�� YCbCr ��ձ���
	avrY = sum[0] / clusterMemberCount;
	avrCb = sum[1] / clusterMemberCount;
	avrCr = sum[2] / clusterMemberCount;
	int t = 0;
	for (int i = 0;i < originMat.cols;i++)
	{
		for (int j = 0;j < originMat.rows;j++)
		{
			if (originMat.at<Vec3b>(j, i) != originGrab.at<Vec3b>(j, i) * 0.2)
			{

				GrabCutImg.at<Vec3b>(j, i)[0] = (avrY/2 + (2*inputY[t]/4)) + (1.772 * ((2*inputCb[t]/4) + avrCb/2));
				GrabCutImg.at<Vec3b>(j, i)[1] = (avrY/2 + (2 * inputY[t] / 4)) - (0.34414 * ((2 * inputCb[t] / 4) + avrCb/2)) - (0.71414 * ((2 * inputCr[t] / 4) + avrCr/2));
				GrabCutImg.at<Vec3b>(j, i)[2] = (avrY/2 + (2 * inputY[t] / 4)) + (1.402 * ((2 * inputCr[t] / 4) + avrCr/2));

				t++;
				if (t > clusterMemberCount) break;
			}
		}
	}

}

// CDlgTab2 �޽��� ó�����Դϴ�.

void CDlgTab2::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
					   // �׸��� �޽����� ���ؼ��� CDialog::OnPaint()��(��) ȣ������ ���ʽÿ�.
	if (!isStartEdit)
	{
		originGrab = imread("originInput.png", CV_LOAD_IMAGE_UNCHANGED);
		GrabCutImg = imread("output.png", CV_LOAD_IMAGE_UNCHANGED);
		if (GrabCutImg.cols % 8 != 0)
			cv::resize(GrabCutImg, GrabCutImg, cv::Size(GrabCutImg.cols - GrabCutImg.cols % 8, GrabCutImg.rows), 0, 0, CV_INTER_NN);
		if (originGrab.cols % 8 != 0)
			cv::resize(originGrab, originGrab, cv::Size(originGrab.cols - originGrab.cols % 8, originGrab.rows), 0, 0, CV_INTER_NN);
		GrabCutImg.copyTo(originGrabImg);
	//	GrabCutImg.copyTo(canny);
	//	cv::cvtColor(GrabCutImg, canny, CV_BGR2GRAY);
	//	cv::Canny(canny, canny, 200, 200);
		//EdgePosition();
		DisplayPasteGrabcut(IDC_Paste, GrabCutImg, 0);
	}
}

void CDlgTab2::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}


void CDlgTab2::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}


void CDlgTab2::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}


void CDlgTab2::OnNMCustomdrawSlider3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
		isLeftMouseDown = false;
		Compose(BGimg, originGrabImg, point.x - GrabCutImg.cols / 2, point.y - GrabCutImg.rows / 2, 1);
		RedrawWindow();

		//cv::namedWindow("Original Image");
		//cv::imshow("Original Image", GrabCutImg);
		DisplayPasteGrabcut(IDC_Back, GrabCutImg, 1);

		nowCutImgPoint = point;
		

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

//��� �ҷ�����
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
		
		//Fix me:
		nowCutImgPoint.x = BGimg.cols + GrabCutImg.cols / 2;
		nowCutImgPoint.y = GrabCutImg.rows / 2;
	}
	isStartEdit = true;
}

