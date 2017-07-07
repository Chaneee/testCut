#include <iostream>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>

void mouseCallback(int event, int x, int y, int flag, void* param);

cv::Rect rectangleBox;
bool isDrawingBox = false;
int userBoxCount = 0;

void drawBox(IplImage* img, CvRect rectangle) {
	cvRectangle(img, cvPoint(rectangle.x, rectangle.y),
				cvPoint(rectangle.x + rectangle.width, rectangle.y + rectangle.height),
				cvScalar(0xff, 0x00, 0x00));

}

int main()
{
	cv::Mat image = cv::imread("input.jpg");
	cv::namedWindow("Original Image");
	cv::imshow("Original Image", image);

	
	rectangleBox = cvRect(-1, -1, 0, 0);

	// Mat -> IplImage* �� ��ȯ
	IplImage* imageCopy;
	IplImage temp;
	temp = image;
	imageCopy = &temp;

	IplImage* tempForCopy = cvCloneImage(imageCopy);

	cvSetMouseCallback("Original Image", mouseCallback, (void*)imageCopy);
	//cvReleaseImage(&imageCopy);

	while (1) {
		cvCopy(imageCopy, tempForCopy);
		if (isDrawingBox) drawBox(tempForCopy, rectangleBox);
		cvShowImage("Original Image", tempForCopy);

		if (cvWaitKey(15) == 27) break;

		if (userBoxCount > 0) break;
	}
	cv::Rect rectangle(rectangleBox.x, rectangleBox.y,rectangleBox.width, rectangleBox.height);
	
	//�簢�� �ȱ׷��� �̹����� �ٽ� �ε�
	image = cv::imread("input.jpg");
	cv::Mat result; // ���� (4�ڱ� ������ ��)
	cv::Mat bgModel, fgModel; // �� (�ʱ� ���)
	cv::grabCut(image,    // �Է� ����
		result,    // ���� ���
		rectangle,   // ������ �����ϴ� ���簢��
		bgModel, fgModel, // ��
		15,     // �ݺ� Ƚ��
		cv::GC_INIT_WITH_RECT); 

	cv::compare(result, cv::GC_PR_FGD, result, cv::CMP_EQ);

	// ������ ���ɼ��� �ִ� ȭ�Ҹ� ��ũ�� ���� ��������
	cv::Mat foreground(image.size(), CV_8UC3, cv::Scalar(255, 255, 255));
	// ��� ���� ����
	image.copyTo(foreground, result);
	// ��� ȭ�Ҵ� ������� ����
	cv::namedWindow("Result");
	cv::imshow("Result", result);

	cv::namedWindow("Foreground");
	cv::imshow("Foreground", foreground);

	cv::waitKey(0);

	return 0;
}

void mouseCallback(int event, int x, int y, int flags, void* param) {
	IplImage* image = (IplImage*)param;

	switch (event) {
	case CV_EVENT_MOUSEMOVE: {if (isDrawingBox) {
		rectangleBox.width = x - rectangleBox.x;
		rectangleBox.height = y - rectangleBox.y;
	}
	}
							 break;
	case CV_EVENT_LBUTTONDOWN: {
		isDrawingBox = true;
		rectangleBox = cvRect(x, y, 0, 0);
	}
							   break;
	case CV_EVENT_LBUTTONUP: {
		isDrawingBox = false;
		if (rectangleBox.width < 0) {
			rectangleBox.x += rectangleBox.width;
			rectangleBox.width *= -1;
		}
		if (rectangleBox.height < 0) {
			rectangleBox.y += rectangleBox.height;
			rectangleBox.height *= -1;
		}
		drawBox(image, rectangleBox);
		userBoxCount++;
	}
							 break;
	}
}