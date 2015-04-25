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

//Canny edge detection threshold, it is quite low due to the low 
//brightness of the images.
#define THRESH 60

#define ELEMENT_TYPE 2
#define ELEMENT_SIZE 3

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

int i, brightness = 0, contourIndex = 0;
char input;
int cnt = 0;
int intense;
double largestArea;
Mat element, frame;
Rect bRect;
vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
int USB = open( "/dev/rfcomm0", O_RDWR| O_NOCTTY );
int frameNum;

//Function Prototypes

void onMouse(int event, int x, int y, int flags, void* userdata);
void disImage(char* winName, Mat Image, int Position);
int write(char message[10]);

int main(int argc, char** argv) {


	string filename = "led.jpg";

	//If no argument is given to "./Contours" then MRI1_01.png is used.

	Mat originalImage = imread(filename);
	
	if(originalImage.empty()) {
		//Checks that the file can be opened, if it can't, prints "can not open" 
		//and end the program
		cout << "can not open " << filename << endl;
		return -1;
	}
	if (DEBUG) {cout << "File Loaded\n\r";}

	VideoCapture vid("ledvid.mkv");

	//Creates the image variables used for this project, one is used for each step
	//to facilitate debugging and understanding the code.
	Mat image = frame.clone(), threshImage, reducedImage, grayImage, erodedImage;

	for(int j = 0; j < Size(vid); j++) {
		vid >> frame;

		cvtColor( frame, grayImage, CV_BGR2GRAY );

		element = getStructuringElement(MORPH_ELLIPSE , Size( 2*ELEMENT_SIZE + 1, 2*ELEMENT_SIZE+1 ), Point(ELEMENT_SIZE, ELEMENT_SIZE));



		grayImage = imread(filename, 0);

		threshold(grayImage, threshImage, 220, 255, THRESH_TOZERO);

		erode(threshImage, erodedImage, element);
		dilate(erodedImage, threshImage, element);

		findContours(threshImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

		largestArea = 0;
		for(int i=0; i<contours.size(); i++) {
			double area = contourArea(contours[i], false);
			if(area > largestArea) {
				largestArea = area;
				contourIndex = i;
			}
		}

		Scalar color = Scalar(255, 0, 0);
		bRect = boundingRect(contours[contourIndex]);
		rectangle(originalImage, bRect, color);

		disImage((char *)"Threshold Image", originalImage, 4);

		//Pointers later used to go through each pixel in the images
		uchar* p;
		uchar* q;

		//Set all pixels in the blurred edge image to white if they are not zero.
		//The purpose of this is to give a clear binary image for further operations.
		for( int i = 0; i < grayImage.rows; ++i) {
			p = grayImage.ptr<uchar>(i);
			for (int j = 0; j < grayImage.cols; ++j) {
				if (p[j] > brightness) {
					brightness = p[j];
					brightLoc = Point(i, j);
				}
				if (p[j] > 230) {
					cnt++;
				}
			}
		}

		cout << "The brightest Spot in the image has a value of " << brightness << " and values of:" << brightLoc.x << ", " << brightLoc.y << endl;

		cout << "The count of bright pixels is " << cnt << endl;

		waitKey(0);
	}
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