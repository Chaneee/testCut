#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <functional>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#define findMin(a,b) (a) < (b) ? (a) : (b)
#define findMinn(a,b,c) (a) < (b) < (c) ? (a) : ((b) < (c) ? (b) : ( (c) < (a) ? (c) : (a) ));
#define findMax(a,b) (a) > (b) ? (a) : (b)

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
vector<int> tempRow;

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

				energy += sqrt(pow(sobelMat.at<uchar>(y, x) - sobelMat.at<uchar>(y + arr[i][0], x + arr[i][1]), 2));
			}
				
			if (y == 0) xEnergyVector.push_back(energy);
			ev.push_back(energy);
			tmpVec.push_back(1);
			isVist.push_back(energy);

			energy = 0;
		}

		
	}
}

int seamcarving(int startPoint, Mat& sobelMat, int mode, int cutCount)
{
	
	//if (mode==1) printf("%d   \n", startPointX);
	int ret = 0;
	//½ÉÄ«ºù
	if (mode == 0)
	{
		for (int startX = 0; startX < image.cols; startX++)
		{
			if (startX == 0 || startX == image.cols - cutCount || startPoint == 0 || startPoint == image.rows) ret = 0;

			else
			{
				//printf("%d   \n", startPointX);
				int minTemp = image.cols * image.rows * 1000;
				int maxTemp = 0;
				int tmpX = 0;
				for (int x = -1; x <= 1; x++)
				{

					if (x + startX < 0 || x + startX > image.cols - cutCount - 1) continue;

					minTemp = findMin(ev[(startPoint -1) * image.cols + startX + x], minTemp);
					if (minTemp == ev[(startPoint -1) * image.cols + startX + x])	tmpX = x + startX;
					/*maxTemp = findMax(ev[y * image.cols + startPointX + x], maxTemp);
					if (maxTemp == ev[y * image.cols + startPointX + x])	tmpX = x + startPointX;*/
				}

				ev[startPoint * image.cols + tmpX] += minTemp;
				ret += ev[startPoint * image.cols + tmpX];
				//startX = tmpX;
			}
			if (startPoint == image.rows - 1)
				tempRow.push_back(ev[(image.rows - 1) * image.cols + startX]);
		}
	 }

	if (mode == 1)
	{
		for (int y = image.rows-1; y > 0; y--)
		{
			if (startPoint == 0 || startPoint == image.cols - cutCount || y == 0 || y == image.rows) ret = 0;

			else
			{
				//printf("%d   \n", startPointX);
				int minTemp = image.cols * image.rows * 1000;
				int maxTemp = 0;
				int tmpX = 0;
				for (int x = -1; x <= 1; x++)
				{

					if (x + startPoint < 0 || x + startPoint > image.cols - cutCount - 1) continue;

					minTemp = findMin(ev[(y-1) * image.cols + startPoint + x], minTemp);
					if (minTemp == ev[(y-1) * image.cols + startPoint + x])	tmpX = x + startPoint;
					/*maxTemp = findMax(ev[y * image.cols + startPointX + x], maxTemp);
					if (maxTemp == ev[y * image.cols + startPointX + x])	tmpX = x + startPointX;*/
				}

				//ret += minTemp;
				startPoint = tmpX;
			}
			image.at<Vec3b>(y-1, startPoint)[0] = 0;
			image.at<Vec3b>(y-1, startPoint)[1] = 0;
			image.at<Vec3b>(y-1, startPoint)[2] = 255;

			for (int c = startPoint; c < image.cols - cutCount; c++)
			{
				if (c == image.cols - cutCount - 1) image.at<Vec3b>(y-1, c) = image.at<Vec3b>(y-1, c);
				//printf("%d&&&&&&&&&&&&&   \n", startPointX);
				else
				{
					//a.at<uchar>(y, c) = a.at<uchar>(y, c + 1);
			//		ev[(y - 1) * image.cols + c] = ev[(y - 1) * image.cols + c + 1];
			//		image.at<Vec3b>(y - 1, c) = image.at<Vec3b>(y - 1, c + 1);

			//		if (y == image.rows - 1)	ev[(image.rows - 1) * image.cols + c] = ev[(image.rows - 1) * image.cols + c + 1];
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
//	cv::Canny(sobelMat, sobelMat, 20, 20);
	cv::Sobel(sobelMat, sobelMat, CV_8U, 0, 1, 1, 0.9, 255);
	cv::Sobel(sobelMat, sobelMat, CV_8U, 1, 0, 1, 0.9, 1);
	calcNWeights(image, sobelMat);
/**	cv::namedWindow("Foreground");
	cv::imshow("Foreground", sobelMat);
	cv::waitKey(0);*/
	

//int x = 0; x < image.cols; x++
	isVist.assign(ev.begin(), ev.end());
	for (int y = 1; y < image.rows; y++)	seamcarving(y, sobelMat, 0, 0);


/*	vector<int> tempRow;
	tempRow.clear();
	for (int x = 0; x < image.cols; x++)
	{
		tempRow.push_back(ev[(image.rows - 1) * image.cols + x]);
	}*/
//	tempRow.assign(rowSeam.begin(), rowSeam.end());
	std::sort(tempRow.begin(), tempRow.end());
	
	isVist.assign(ev.begin(), ev.end());
	
	for (int i = 0; i < 300; i++)
	{
		for (int j = 0;j < image.cols;j++)
		{
			if (tempRow[i] == ev[(image.rows - 1) * image.cols + j])
			{
				seamcarving(j, sobelMat, 1, i);
				Rect cutRect(0, 0, origin.cols - (i+1), image.rows);
		//		copyMakeBorder(image, image, 0, 0, 0, i+1, BORDER_CONSTANT);
		//		image = origin(cutRect);
				cv::namedWindow("Foreground");
				cv::imshow("Foreground", image);
				cv::waitKey(1);

				isVist.assign(ev.begin(), ev.end());
				j = image.cols;
			}
		}
	}
		
	cv::namedWindow("Foreground");
	cv::imshow("Foreground", image);
	cv::waitKey(0);
	
	return 0;
}
