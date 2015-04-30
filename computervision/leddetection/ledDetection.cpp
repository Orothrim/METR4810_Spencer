#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
#include <stdio.h>
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions

#define DEBUG 1

//Cropped area size.
#define SQUARE_SIZE 200
#define CROPMIDPOINT 100

//Marker relations
#define BLUE_RED 400
#define GREEN_FRONT 700
//Red Marker Position
#define RED_X 0
#define RED_Y 0
//Green Marker Position
#define GREEN_X BLUE_RED
#define GREEN_Y 0
//Blue Marker Position
#define BLUE_X BLUE_RED*0.5
#define BLUE_Y GREEN_FRONT
//Purple Marker Position
#define PURPLE_X BLUE_RED*0.5
#define PURPLE_Y GREEN_FRONT
#define PURPLE_Z 40

#define OPEN_CLOSE 1 //1 uses and opening procedure, 0 uses a closing procedure.
#define YUV_RGB 0 //1 = YUV, 0 = RGB.

//Canny edge detection threshold, it is quite low due to the low 
//brightness of the images.
#define THRESH 60
#define AREA_THRESH 100

#define ELEMENT_TYPE 2
#define ELEMENT_SIZE 4

//Used to access all of the images.
#define ONESCOLUMN 6
#define TENSCOLUMN 5
#define NUMIMAGES 16

//Middle point of images.
#define MIDDLEX 292
#define MIDDLEY 360

//Colours used in image analysis.
#define WHITE 255
#define MANICOLOUR 127

//Size of the images when displayed.
#define WINDOWX 325
#define WINDOWY 400
//First of four image positions.
#define FIRSTPOSX 0
#define FIRSTPOSY 0
//Second of four image positions.
#define SECONDPOSX WINDOWX+5
#define SECONDPOSY 0
//Third of four image positions.
#define THIRDPOSX 0
#define THIRDPOSY WINDOWY+25
//Fourth of four image positions.
#define FOURTHPOSX WINDOWX+5
#define FOURTHPOSY WINDOWY+25


using namespace cv;
using namespace std;

Point brightLoc, mousePos;

char input;
int cnt = 0, i, brightness = 0, contourIndex = 0, intense;

double largestArea, area;

Mat element;
Rect bRect;
Scalar intenseAvg, color[3];

vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
vector<Vec3f> circles;

int USB = open( "/dev/rfcomm0", O_RDWR| O_NOCTTY );

//Function Prototypes

void onMouse(int event, int x, int y, int flags, void* userdata);
void disImage(char* winName, Mat Image, int Position);
int write(char message[10]);

int main(int argc, char** argv) {


	string filename = "led3.jpg";

	//If no argument is given to "./Contours" then MRI1_01.png is used.

	Mat originalImage = imread(filename);

	if(YUV_RGB) {
		color[0] = Scalar(255, 255, 255);
		color[1] = Scalar(0, 0, 255);
		color[2] = Scalar(255, 0, 0);
	}
	else {
		color[0] = Scalar(255, 0, 0);
		color[1] = Scalar(0, 255, 0);
		color[2] = Scalar(0, 0, 255);
	}

	if(originalImage.empty()) {
		//Checks that the file can be opened, if it can't, prints "can not open" 
		//and end the program
		cout << "can not open " << filename << endl;
		return -1;
	}
	if (DEBUG) {cout << "File Loaded\n\r";}

	element = getStructuringElement(MORPH_ELLIPSE , Size( 2*ELEMENT_SIZE + 1, 2*ELEMENT_SIZE+1 ), Point(ELEMENT_SIZE, ELEMENT_SIZE));

	//Creates the image variables used for this project, one is used for each step
	//to facilitate debugging and understanding the code.
	Mat image = originalImage.clone(), threshImage, reducedImage, grayImage, erodedImage, colourImages[3], yuvImage, edgeImage, contoursImage, blueImage, greenImage, redImage;

	if(YUV_RGB) {
		cvtColor(image, yuvImage, CV_BGR2YCrCb);
		split(yuvImage, colourImages);
		intenseAvg = mean(yuvImage);
	}
	else{
		split(image, colourImages);
		intenseAvg = mean(image);
		threshold(colourImages[0], blueImage, intenseAvg[0]*1.2, 255, THRESH_TOZERO);
		threshold(colourImages[1], greenImage, intenseAvg[1]*1.2, 255, THRESH_TOZERO);
		threshold(colourImages[2], redImage, intenseAvg[2]*1.2, 255, THRESH_TOZERO);

		//White lights should be reduced, as the markers are distinct colours.
		colourImages[0] = blueImage - greenImage*0.5 - redImage*0.5; 
		colourImages[1] = greenImage - blueImage*0.5 - redImage*0.5;
		colourImages[2] = redImage - greenImage*0.5 - blueImage*0.5;
	}

	if (DEBUG) {cout << "Colours Managed" << endl;}

	// grayImage = imread(filename, 0);
	for(int k = YUV_RGB; k <= 2; k++) {
		if (DEBUG) {cout << "Loop: " << k << endl;}		
		threshold(colourImages[k], threshImage, intenseAvg[k]*1.2, 255, THRESH_TOZERO);

		if (OPEN_CLOSE) {
			erode(threshImage, erodedImage, element);
			dilate(erodedImage, threshImage, element);
		}
		else {
			dilate(threshImage, erodedImage, element);
			erode(erodedImage, threshImage, element);
		}

		Canny(threshImage, edgeImage, THRESH, 2*THRESH, 3);

		contoursImage = threshImage.clone();
		// contoursImage = edgeImage.clone();

		blur(contoursImage, contoursImage, Size(4,4));

// 		HoughCircles(contoursImage, circles, CV_HOUGH_GRADIENT, 2, edgeImage.rows/4, 200, 100 );

// 		for( size_t j = 0; j < circles.size(); j++ )
// 		{
// 		  Point center(cvRound(circles[j][0]), cvRound(circles[j][1]));
// 		  int radius = cvRound(circles[j][2]);
// 		  // circle center
// //		  circle(originalImage, center, 3, Scalar(0,255,0), -1, 8, 0);
// 		  // circle outline
// 		  circle(originalImage, center, radius, color[k], 3, 8, 0);
// 		}

		findContours(contoursImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

		largestArea = 0;
		area = 0;
		for(int i = 0; i<contours.size(); i++) {
			area = contourArea(contours[i], false);
			if(area > AREA_THRESH) {
				// largestArea = area;
				// contourIndex = i;
				bRect = boundingRect(contours[i]);
				rectangle(originalImage, bRect, color[k]);
			}
		}

		switch(k) {
			case 0:
				disImage((char *)"0 Image", edgeImage, 1);
				break;
			case 1:
				disImage((char *)"1 Image", edgeImage, 2);
				break;
			case 2:
				disImage((char *)"2 Image", edgeImage, 3);
				break;
		}
	}

	if (DEBUG) {cout << "Analysis Complete" << endl;}

	disImage((char *)"Threshold Image", originalImage, 4);

	// disImage((char *)"Red Image", redImage, 1);
	// disImage((char *)"redImage sub", colourImages[2], 2);

	//Pointers later used to go through each pixel in the images
	// uchar* p;
	// uchar* q;
	
	// //Set all pixels in the blurred edge image to white if they are not zero.
	// //The purpose of this is to give a clear binary image for further operations.
	// for( int i = 0; i < grayImage.rows; ++i) {
	// 	p = grayImage.ptr<uchar>(i);
	// 	for (int j = 0; j < grayImage.cols; ++j) {
	// 		if (p[j] > brightness) {
	// 			brightness = p[j];
	// 			brightLoc = Point(i, j);
	// 		}
	// 		if (p[j] > 230) {
	// 			cnt++;
	// 		}
	// 	}
	// }

	// cout << "The brightest Spot in the image has a value of " << brightness << " and values of:" << brightLoc.x << ", " << brightLoc.y << endl;

	// cout << "The count of bright pixels is " << cnt << endl;

	waitKey(0);
}

void disImage(char* winName, Mat Image, int Position) {
	namedWindow(winName, WINDOW_NORMAL);
	imshow(winName, Image);
	resizeWindow(winName, WINDOWX, WINDOWY);
	switch (Position) {
		case 1:
			moveWindow(winName, FIRSTPOSX, FIRSTPOSY);
			break;
		case 2:
			moveWindow(winName, SECONDPOSX, SECONDPOSY);
			break;
		case 3:
			moveWindow(winName, THIRDPOSX, THIRDPOSY);
			break;
		case 4:
			moveWindow(winName, FOURTHPOSX, FOURTHPOSY);
			break;
	}
	
}

void onMouse(int event, int x, int y, int flags, void* userdata) {
	//Event that is attached to a mouse click after "setMouseCallback" occurs.
	if(event == EVENT_LBUTTONDOWN) {
		mousePos.x = x;
		mousePos.y = y;
	}
}

int write(char message[10]) {
	int n_written = 0, spot = 0;

	do {
		n_written = write( USB, &message[spot], 1 );
		spot += n_written;
	} while (message[spot-1] != '\r' && n_written > 0);
}