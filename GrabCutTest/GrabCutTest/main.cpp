#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <functional>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#define findMin(a,b) (a) < (b) ? (a) : (b);
#define findMax(a,b) (a) > (b) ? (a) : (b);

using namespace cv;
using namespace std;

void mouseCallback(int event, int x, int y, int flag, void* param);

Mat allEnergy;
cv::Rect rectangleBox;
bool isDrawingBox = false;
int userBoxCount = 0;
double maxTemp = 0;
double allminTemp = 0;

static double calcBeta(const Mat& img)
{
	double beta = 0;
	for (int y = 0; y < img.rows; y++)
	{
		for (int x = 0; x < img.cols; x++)
		{
			Vec3d color = img.at<Vec3b>(y, x);
			if (x>0) // left
			{
				Vec3d diff = color - (Vec3d)img.at<Vec3b>(y, x - 1);
				beta += diff.dot(diff);
			}
			if (y>0 && x>0) // upleft
			{
				Vec3d diff = color - (Vec3d)img.at<Vec3b>(y - 1, x - 1);
				beta += diff.dot(diff);
			}
			if (y>0) // up
			{
				Vec3d diff = color - (Vec3d)img.at<Vec3b>(y - 1, x);
				beta += diff.dot(diff);
			}
			if (y>0 && x<img.cols - 1) // upright
			{
				Vec3d diff = color - (Vec3d)img.at<Vec3b>(y - 1, x + 1);
				beta += diff.dot(diff);
			}
		}
	}
	if (beta <= std::numeric_limits<double>::epsilon())
		beta = 0;
	else
		beta = 1.f / (2 * beta / (4 * img.cols*img.rows - 3 * img.cols - 3 * img.rows + 2));

	return beta;
}

static void calcNWeights(const Mat& img, Mat& leftW, Mat& upleftW, Mat& upW, Mat& uprightW, double beta, double gamma)
{
	
	const double gammaDivSqrt2 = gamma / std::sqrt(2.0f);
	leftW.create(img.rows, img.cols, CV_64FC1);
	upleftW.create(img.rows, img.cols, CV_64FC1);
	upW.create(img.rows, img.cols, CV_64FC1);
	uprightW.create(img.rows, img.cols, CV_64FC1);
	
	for (int y = 0; y < img.rows; y++)
	{
		for (int x = 0; x < img.cols; x++)
		{
			Vec3d color = img.at<Vec3b>(y, x);
			if (x - 1 >= 0) // left
			{
				Vec3d diff = color - (Vec3d)img.at<Vec3b>(y, x - 1);
				leftW.at<double>(y, x) = gamma * exp(-beta*diff.dot(diff));
				allEnergy.at<double>(y, x) += leftW.at<double>(y, x);
				maxTemp = findMax(maxTemp, allEnergy.at<double>(y, x));
				allminTemp = findMin(allEnergy.at<double>(y, x), allminTemp);
			}
			else
				leftW.at<double>(y, x) = 0;
			if (x - 1 >= 0 && y - 1 >= 0) // upleft
			{
				Vec3d diff = color - (Vec3d)img.at<Vec3b>(y - 1, x - 1);
				upleftW.at<double>(y, x) = gammaDivSqrt2 * exp(-beta*diff.dot(diff));
				allEnergy.at<double>(y, x) += upleftW.at<double>(y, x);
				maxTemp = findMax(maxTemp, allEnergy.at<double>(y, x));
				allminTemp = findMin(allEnergy.at<double>(y, x), allminTemp);
			}
			else
				upleftW.at<double>(y, x) = 0;
			if (y - 1 >= 0) // up
			{
				Vec3d diff = color - (Vec3d)img.at<Vec3b>(y - 1, x);
				upW.at<double>(y, x) = gamma * exp(-beta*diff.dot(diff));
				allEnergy.at<double>(y, x) += upW.at<double>(y, x);
				maxTemp = findMax(maxTemp, allEnergy.at<double>(y, x));
				allminTemp = findMin(allEnergy.at<double>(y, x), allminTemp);
			}
			else
				upW.at<double>(y, x) = 0;
			if (x + 1<img.cols && y - 1 >= 0) // upright
			{
				Vec3d diff = color - (Vec3d)img.at<Vec3b>(y - 1, x + 1);
				uprightW.at<double>(y, x) = gammaDivSqrt2 * exp(-beta*diff.dot(diff));
				allEnergy.at<double>(y, x) += uprightW.at<double>(y, x);
				maxTemp = findMax(maxTemp, allEnergy.at<double>(y, x));
				allminTemp = findMin(allEnergy.at<double>(y, x), allminTemp);
			}
			else
				uprightW.at<double>(y, x) = 0;
		}
	}
}

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
	double minimum = 10000;
	double *th = new double[image.rows * image.cols];
	//double th[348243] = { 0, };
	allminTemp = image.rows * image.cols;
	
	allEnergy.create(image.rows, image.cols, CV_64FC1);
	
	for (int y = 0; y < image.rows; y++)
		for (int x = 0; x < image.cols; x++)
			allEnergy.at<double>(y, x) = 0;

	const double gamma = 100;
	const double beta = calcBeta(image);
	Mat leftW, upleftW, upW, uprightW;
	calcNWeights(image, leftW, upleftW, upW, uprightW, beta, gamma);

	//첫째줄을 돌면서 시작점 찾기
	double startTemp = 0;
	int startPointX = 0;
	for (int x = 0; x < image.cols; x++)
	{
		if (allEnergy.at<double>(0, x) != 0)
		{
			startTemp = findMin(allEnergy.at<double>(0, x), startTemp);
		}

		if (startTemp = allEnergy.at<double>(0, x))	startPointX = x;
	}

	//sort(th, th + (image.rows * image.cols));

	
	//printf("%d   \n%d\n\n", maxTemp < allminTemp ? allminTemp : maxTemp, allminTemp);

	
	for (int y = 0; y < image.rows; y++)
	{
		if (y == 0)
			image.at<Vec3b>(y, startPointX) = 0;

		else
		{
			double minTemp = maxTemp;
			int tmpX = 0;
		//	printf("%d   d오옹아아우ㅏ\n", x);
			for (int x = -3; x < 4; x++)
			{
				if (x + startPointX < 0 || x + startPointX > image.cols-1 || allEnergy.at<double>(y, x + startPointX) == 0) continue;
				minTemp = findMin(allEnergy.at<double>(y, x + startPointX), minTemp)

				if (minTemp == allEnergy.at<double>(y, x + startPointX))	tmpX = x + startPointX;
			}
			startPointX = tmpX;
			//printf("%d   d오옹아아우ㅏ\n", startPointX);
			image.at<Vec3b>(y, startPointX) = 0;
		}
	}

	/*for (int y = 0; y < image.rows; y++)
	{
		for (int x = 0; x < image.cols; x++)
		{
			if (allEnergy.at<double>(y, x) > th[150000])
			{
				printf("%d   %d\n", y, x);
				image.at<Vec3b>(y, x) = 0;
			}
				
		}
	}*/
	
	cv::namedWindow("Foreground");
	cv::imshow("Foreground", image);
	//cv::imwrite("output.jpg", image);

	cv::waitKey(0);

	/*rectangleBox = cvRect(-1, -1, 0, 0);

	// Mat -> IplImage* 형 변환
	IplImage* imageCopy;
	IplImage temp;
	temp = image;
	imageCopy = &temp;

	IplImage* tempForCopy = cvCloneImage(imageCopy);

	cvSetMouseCallback("Original Image", mouseCallback, (void*)imageCopy);*/
	//cvReleaseImage(&imageCopy);
/*
	while (1) {
		cvCopy(imageCopy, tempForCopy);
		if (isDrawingBox) drawBox(tempForCopy, rectangleBox);
		cvShowImage("Original Image", tempForCopy);

		if (cvWaitKey(15) == 27) break;

		if (userBoxCount > 0) break;
	}
	
	cv::Rect rectangle(rectangleBox.x, rectangleBox.y,rectangleBox.width, rectangleBox.height);

	//사각형 안그려진 이미지를 다시 로드
	image = cv::imread("input.jpg");
	cv::Mat result, tempFG, tempPRFG; // 분할 (4자기 가능한 값)
	cv::Mat bgModel, fgModel; // 모델 (초기 사용)
	cv::grabCut(image,    // 입력 영상
		result,    // 분할 결과
		rectangle,   // 전경을 포함하는 직사각형
		bgModel, fgModel, // 모델
		40,     // 반복 횟수
		cv::GC_INIT_WITH_RECT); 

	cv::compare(result, cv::GC_PR_FGD, tempPRFG, cv::CMP_EQ);
	cv::compare(result, cv::GC_FGD, tempFG, cv::CMP_EQ);

	// 전경일 가능성이 있는 화소를 마크한 것을 가져오기
	cv::Mat foreground(image.size(), CV_8UC3, cv::Scalar(255, 255, 255));
	// 결과 영상 생성
	image.copyTo(foreground, tempPRFG);
	image.copyTo(foreground, tempFG);
	// 배경 화소는 복사되지 않음
	cv::namedWindow("Result");
	cv::imshow("Result", result);

	cv::namedWindow("Foreground");
	cv::imshow("Foreground", foreground);
	cv::imwrite("output.jpg",foreground);

	cv::waitKey(0);*/

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