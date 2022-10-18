//////////////////////////////////// VIRTUAL PAINT //////////////////////////////////////

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat img;
Mat imgResize;

vector<vector<int>> newPoints;

vector<vector<int>> myColors

{
	{124, 48, 117, 143, 120, 255},
	{68, 72, 156, 102, 126, 255},
	{0, 62, 0, 35, 255, 255} };

vector<Scalar> myColorValues{
	{255, 0, 255}, // purple
	{0, 255, 0},   // green
	{51, 153, 255} // orange
};

Point getContours(Mat imgDil)
{
	vector<vector<Point>> contours;
	vector<Vec4i> heirarchy;

	findContours(imgDil, contours, heirarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	// drawContours(img, contours, -1, Scalar(255, 0, 255), 10);

	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());

	Point myPoint(0, 0);

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		cout << area << endl;

		string objectType;

		if (area > 1000)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
			cout << conPoly[i].size() << endl;
			boundRect[i] = boundingRect(conPoly[i]);
			myPoint.x = boundRect[i].x + boundRect[i].width / 2;
			myPoint.y = boundRect[i].y;

			drawContours(img, conPoly, i, Scalar(255, 255, 0), 5);

			// rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 4);

			// putText(img, objectType, { boundRect[i].x, boundRect[i].y - 5 }, FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(255, 133, 110), 1);
		}
	}

	return myPoint;
}

vector<vector<int>> findColor(Mat img)
{
	Mat imgHSV;
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	for (int i = 0; i < myColors.size(); i++)
	{
		Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);
		Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]);

		Mat mask;

		inRange(imgHSV, lower, upper, mask);

		// imshow(to_string(i), mask);

		Point myPoint = getContours(mask);

		if (myPoint.x != 0 && myPoint.y != 0)
		{
			newPoints.push_back({ myPoint.x, myPoint.y, i });
		}
	}

	return newPoints;
}

void drawOnCanvas(vector<vector<int>> newPoints, vector<Scalar> myColorValues)
{
	for (int i = 0; i < newPoints.size(); i++)
	{
		circle(img, Point(newPoints[i][0], newPoints[i][1]), 10, myColorValues[newPoints[i][2]], FILLED);
	}
}

void main()
{

	VideoCapture cap(0);

	while (true)
	{
		cap.read(img);
		newPoints = findColor(img);
		drawOnCanvas(newPoints, myColorValues);

		// resize(img, imgResize, Size(), 0.5, 0.5);
		// imshow("Image Resize", imgResize);

		imshow("image", img);

		waitKey(1);
	}
}