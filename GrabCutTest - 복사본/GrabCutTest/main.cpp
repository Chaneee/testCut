#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <functional>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#define findMin(a,b) (a) < (b) ? (a) : (b);
#define findMinn(a,b,c) (a) < (b) < (c) ? (a) : ((b) < (c) ? (b) : ( (c) < (a) ? (c) : (a) ));
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
Mat image;
vector<int> xEnergyVector, yEnergyVector;
vector<int> rowSeam;
vector<int> ev;
vector<int> tmpVec;
vector<int> isVist;

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

static void calcNWeights(const Mat& img, Mat& sobelMat)
{

	int energy = 0;
	int arr[8][2] = { 
			 { -1,-1 }, { -1,0 }, { -1,1 }, 
			 { 0,-1 },			  { 0,1 },  
			 { 1,-1 }, { 1,0 },   { 1,1 } };
		

	for (int y = 0; y < img.rows; y++)
	{
		//printf("%d  \n", y);
		for (int x = 0; x < img.cols; x++)
		{
			for (int i = 0; i < 8; i++)
			{
				if (y + arr[i][0] < 0 || x + arr[i][1] < 0 || y + arr[i][0] >= img.rows || x + arr[i][1] >= img.cols)
					continue;

				energy += pow(sobelMat.at<uchar>(y, x) - sobelMat.at<uchar>(y + arr[i][0], x + arr[i][1]), 2);
			}
				


			/*if(y == 0 || x == 0 || y == img.rows - 1 || x == img.cols - 1)	energy = sobelMat.at<uchar>(y, x);
			else
			{
				for (int i = -1; i < 2; i++)
					for (int j = -1; j < 2; j++)
						energy += pow(sobelMat.at<uchar>(y, x) - sobelMat.at<uchar>(y + i, x + j),2);
				
			}*/
			if (y == 0) xEnergyVector.push_back(energy);
			ev.push_back(energy);
			tmpVec.push_back(1);
			isVist.push_back(energy);

			energy = 0;
		}

		
	}
}

int seamcarving(int startPointX, Mat& sobelMat, int mode, int cutCount)
{
	
	//if (mode==1) printf("%d   \n", startPointX);
	int ret = 0;
	//심카빙
	for (int y = 0; y < image.rows; y++)
	{
		if (startPointX == 0 || startPointX == image.cols - cutCount || y == 0 || y == image.rows) ret = 0;

		else
		{
		//printf("%d   \n", startPointX);
			int minTemp = image.cols * image.rows * 1000;
			int maxTemp = 0;
			int tmpX = 0;
			//mode 0	
		//	if (startPointX - 1 < 0 || startPointX + 1 > image.cols - cutCount - 1) minTemp = ev[y * image.cols + startPointX];
		//	else minTemp = findMinn(ev[y * image.cols + startPointX -1], ev[y * image.cols + startPointX], ev[y * image.cols + startPointX + 1]);
				for (int x = -1; x <= 1; x++)
				{
					
					if (x + startPointX < 0 || x + startPointX > image.cols - cutCount - 1) continue;

					/*if (isVist[y * image.cols + startPointX + x] == 0)
					{
						//tmpX = startPointX - 2;
						continue;
					}*/
					///minTemp = findMin(a.at<uchar>(y, x + startPointX), minTemp)
				/*	if (mode == 0)
					{
						maxTemp = findMax(isVist[y * image.cols + startPointX + x], maxTemp);
						if (maxTemp == isVist[y * image.cols + startPointX + x])	tmpX = x + startPointX;
					}
					else if (mode == 1)
					{*/
						minTemp = findMin(ev[y * image.cols + startPointX + x], minTemp);
						if (minTemp == ev[y * image.cols + startPointX + x])	tmpX = x + startPointX;
				//	}

						/*maxTemp = findMax(ev[y * image.cols + startPointX + x], maxTemp);
						if (maxTemp == ev[y * image.cols + startPointX + x])	tmpX = x + startPointX;*/
				}
				/*if (tmpX = 0)
				{
					for (int x = -2; x < 3; x++)
					{
						tmpX = 0;
						if (x + startPointX < 0 || x + startPointX > image.cols - 1) continue;
						if (isVist[y * image.cols + tmpX] == 0) continue;
						//minTemp = findMin(a.at<uchar>(y, x + startPointX), minTemp)
						minTemp = findMin(ev[y * image.cols + startPointX + x], minTemp);
						if (minTemp == ev[y * image.cols + startPointX + x])	tmpX = x + startPointX;
					}
				}*/
			/*	if (mode == 0)	isVist[y * image.cols + tmpX] = image.cols * image.rows * 100000;
				if (mode == 1)*/	
				//isVist[y * image.cols + tmpX] = image.cols * image.rows * 0;
				ret += minTemp;
				startPointX = tmpX;
				
			
				if (mode == 1)
				{
					//printf("%d**   \n", startPointX);
					image.at<Vec3b>(y, startPointX)[0] = 0;
					image.at<Vec3b>(y, startPointX)[1] = 0;
					image.at<Vec3b>(y, startPointX)[2] = 255;

					for (int c = startPointX; c < image.cols - cutCount; c++)
					{
						if (c == image.cols - cutCount - 1) image.at<Vec3b>(y, c) = image.at<Vec3b>(y, c);
						//printf("%d&&&&&&&&&&&&&   \n", startPointX);
						else
						{
							//a.at<uchar>(y, c) = a.at<uchar>(y, c + 1);
					//		ev[y * image.cols + c] = ev[y * image.cols + c + 1];
					//		image.at<Vec3b>(y, c) = image.at<Vec3b>(y, c + 1);

						if (y == 0)	rowSeam[c] = rowSeam[c + 1];
						}
					}
				}
		}
	}


	return ret;
}

int main()
{
	image = cv::imread("input.jpg");
	cv::namedWindow("Original Image");
	cv::imshow("Original Image", image);

	Mat origin;
	image.copyTo(origin);

	Mat sobelMat(image.size(), CV_8UC3, cv::Scalar(0,0,0));
	cv::cvtColor(image, sobelMat, CV_BGR2GRAY);
	cv::Sobel(sobelMat, sobelMat, CV_8U, 0, 1, 3, 0.6, 128);
	cv::Sobel(sobelMat, sobelMat, CV_8U, 1, 0, 3, 0.6, 128);
	calcNWeights(image, sobelMat);
/*	cv::namedWindow("Foreground");
	cv::imshow("Foreground", sobelMat);
	cv::waitKey(0);*/
	


	isVist.assign(ev.begin(), ev.end());
	for (int x = 0; x < image.cols; x++)	rowSeam.push_back(seamcarving(x, sobelMat, 0, 0));


	vector<int> tempRow;
	tempRow.clear();
	tempRow.assign(rowSeam.begin(), rowSeam.end());
	sort(tempRow.begin(), tempRow.end());

	isVist.assign(ev.begin(), ev.end());
	
	for (int i = 0; i < 300; i++)
	{
		for (int j = 0;j < image.cols;j++)
		{
			if (tempRow[i] == rowSeam[j])
			{
				seamcarving(j, sobelMat, 1, i);
				Rect cutRect(0, 0, origin.cols - (i+1), image.rows);
			//	image = origin(cutRect);
				cv::namedWindow("Foreground");
				cv::imshow("Foreground", image);
				cv::waitKey(1);
			/*	Mat	sobelMat(image.size(), CV_8UC3, cv::Scalar(0, 0, 0));
				cv::cvtColor(image, sobelMat, CV_BGR2GRAY);
				cv::Sobel(sobelMat, sobelMat, CV_8U, 0, 1, 3, 0.7, 128);
				cv::Sobel(sobelMat, sobelMat, CV_8U, 1, 0, 3, 0.7, 128);
				calcNWeights(image, sobelMat);*/
				isVist.assign(ev.begin(), ev.end());
				j = image.cols;
			}
		}
	}
		
	cv::namedWindow("Foreground");
	cv::imshow("Foreground", image);
	cv::waitKey(0);
	
	
	//cv::imwrite("output.jpg", image);

	

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
