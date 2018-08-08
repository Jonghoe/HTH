#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<opencv2/opencv.hpp>
#include<Windows.h>
#include"LabelFactory.hpp"
using namespace cv;
using namespace std;




Mat transformMatrix;
bool detecting;
bool finish = false;
Mat input,prevIMG;
class WarpData {
public:
	Point2f corners[4];
	void setCorners(Point2f _corners[4]) {		
		for (int i = 0; i < 4; ++i) {
			corners[i] = _corners[i];
		}
	}
	void getCorners(Point2f dst[4]) {
		for (int i = 0; i < 4; ++i) {
			dst[i] = corners[i];
		}
	}

public:
	Size warpSize;
	Mat warpIMG;
	//Transformation Matrix 구하기
	Mat transformMatrix;
	
};

WarpData warpData;

vector<Point2f> findCorner(const Mat& input,int pixelMin,int pixelMax)
{
	vector<Point2f> retV;
	Mat bgr[3], thresholdIMG_2, thresholdIMG_0;
	split(input, bgr);
	threshold(bgr[2], thresholdIMG_2, 125, 255, CV_THRESH_BINARY_INV);
	threshold(bgr[0], thresholdIMG_0, 125, 255, CV_THRESH_BINARY);
	Mat binary = thresholdIMG_0 & thresholdIMG_2;
	vector<Label> labels = LabelFactory::findLabel(thresholdIMG_2);
	for (int i = 0; i < labels.size(); ++i) {
		if (pixelMin<labels[i].getPixel() && labels[i].getPixel() < pixelMax)
			retV.push_back(labels[i].getCenter());
	}
	return retV;
}

void startWarp();

Rect button1;
Rect button2;
Rect button3;

int histSize[1];

float hranges[2];

const float* ranges[1];

int channels[1];


Mat calculateHistogram(Mat& src)
{

	histSize[0] = 256;

	hranges[0] = 0.0;	//히스토그램 최소 값

	hranges[1] = 255.0; //히스토그램 최대 값

	ranges[0] = hranges;

	channels[0] = 0;

	MatND hist;

	calcHist(&src, 1, channels, Mat(), hist, 1, histSize, ranges);

	return hist;
}

void testHistogram() {
	MatND hist[3],bgr[3];
	Mat img = imread("Surrounding_plate.jpg", IMREAD_COLOR);
	Mat showHIST(Size(256, img.rows*img.cols), CV_8UC3);
	showHIST = Mat::zeros(Size(256, img.rows*img.cols), CV_8UC3);
	split(img, bgr);
	hist[0] = calculateHistogram(bgr[0]);
	hist[1] = calculateHistogram(bgr[1]);
	hist[2] = calculateHistogram(bgr[2]);

	Mat histImg(histSize[0], histSize[0], CV_8UC3, Scalar::all(255));
	
	int hpt = static_cast<int>(0.9*histSize[0]);
	double maxVal = 0; // 최대 빈도수
	double minVal = 0; // 최소 빈도수
	for (int i = 0; i < 3; ++i) {
		double tmpMax;
		minMaxLoc(hist[i], &minVal, &tmpMax, 0, 0);
		if (maxVal < tmpMax)
			maxVal = tmpMax;
	}

	//그래프 그리기
	for (int i = 0; i < 3; ++i) {

		for (int h = 1; h < histSize[0]; h++) {
			float binVal[2] = { hist[i].at<float>(h - 1), hist[i].at<float>(h) };
			int intensity[2] = { static_cast<int> (binVal[0] * hpt / maxVal),static_cast<int> (binVal[1] * hpt / maxVal) };
			if (i == 0) {
				line(histImg, Point(h - 1, histSize[0] - intensity[0]), Point(h, histSize[0] - intensity[2]), Scalar(255, 0, 0), 2);
			}
			if (i == 1) {
				line(histImg, Point(h - 1, histSize[0] - intensity[0]), Point(h, histSize[0] - intensity[2]), Scalar(0, 255, 0), 2);
			}
			if (i == 2) {
				line(histImg, Point(h - 1, histSize[0] - intensity[0]), Point(h, histSize[0] - intensity[2]), Scalar(0, 0, 255), 2);
			}
		}
	}
	imshow("hist", histImg);
}




void onMouse(int e, int x, int y, int flag, void* param);


int main() {
	/*
	*	시작 신호 오면 인식 시작. => 물체가 나타나고 일정 시간동안 움직임이 없을시 warping => warping한 결과로 점수 계산
	*/
	VideoCapture vc(0);
	Mat3b canvas;
	detecting = false;
	vc >> input;
	namedWindow("Test");
	button1 = Rect(0, 0, input.cols / 3, 60);
	button2 = Rect(input.cols/3, 0, input.cols / 3, 60);
	button3 = Rect(input.cols/3*2, 0, input.cols/3, 60);
	finish = false;
	setMouseCallback("Test", onMouse);
	while (!finish && waitKey(10) != 27) {
		vc >> input;
		canvas = Mat3b(button1.height + input.rows, input.cols, Vec3b(0, 0, 0));
		canvas(button1) = Vec3b(200, 200, 200);
		canvas(button2) = Vec3b(200, 200, 200);
		canvas(button3) = Vec3b(200, 200, 200);
		line(canvas, button1.br(), button2.tl(), Scalar(0, 0, 0), 2);
		line(canvas, button2.br(), button3.tl(), Scalar(0, 0, 0), 2);
		input.copyTo(canvas(Rect(0, button1.height, input.cols, input.rows)));
		putText(canvas, "START", Point(button1.width*0.2, button1.height*0.6), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 0));
		putText(canvas, "HISTOGRAM", Point(button1.width+button2.width*0.1, button2.height*0.6), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 0));
		putText(canvas, "END", Point(button2.width+button3.width+button3.width*0.1, button3.height*0.6), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 0));
		
		imshow("Test", canvas);
		prevIMG = input;
	}
}


void startWarp()
{
	//Mat img = input;
	Mat img = imread("Surrounding_plate.jpg", IMREAD_COLOR);

	////Warping 전의 이미지 상의 좌표 
	Point2f corners[4];
	vector<Point2f> corner = findCorner(img, 35, 45);
	for (int i = 0; i < 4; ++i)
		corners[i] = corner[i];
	warpData.setCorners(corners);

	// Warping 이미지
	Size warpSize(500, 500);
	Mat warpIMG(warpSize, img.type());

	//Warping 후의 좌표
	Point2f warpCorners[4];
	warpCorners[0] = Point2f(0, 0);
	warpCorners[1] = Point2f(warpIMG.cols, 0);
	warpCorners[2] = Point2f(0, warpIMG.rows);
	warpCorners[3] = Point2f(warpIMG.cols, warpIMG.rows);

	//Transformation Matrix 구하기
	Mat transformMatrix = getPerspectiveTransform(corners, warpCorners);
	warpData.transformMatrix = transformMatrix;
	warpData.warpSize = warpSize;
	//Warping
	warpPerspective(img, warpIMG, transformMatrix, warpSize);	
	imshow("origin", img);
	imshow("warpIMG", warpIMG);
}


void onMouse(int e, int x, int y, int flag, void* param) {
	Point click(x, y);
	if (e == EVENT_LBUTTONDOWN && button1.contains(click)) {
		startWarp();
	}

	if (e == EVENT_LBUTTONDOWN && button2.contains(click)) {
		testHistogram();
	}


	if (e == EVENT_LBUTTONDOWN && button3.contains(click)) {
		destroyAllWindows();
		finish = true;
	}
}
