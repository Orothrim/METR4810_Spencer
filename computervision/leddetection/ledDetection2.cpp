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

#define ELEMENT_TYPE 0
#define OPENING_SIZE 3
#define CLOSING_SIZE 3

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
#define WINDOWX 500
#define WINDOWY 400

using namespace cv;
using namespace std;

Point brightLoc, mousePos;

char input;
int cnt = 0, i, brightness = 0, contourIndex = 0, intense, eleType;

double largestArea, area;

Mat openElement, closeElement;
Rect bRect;
Scalar intenseAvg, yuvColour, color[3], yuvAvg;

vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
vector<Vec3f> circles;

int USB = open( "/dev/rfcomm0", O_RDWR| O_NOCTTY );

//Function Prototypes

void onMouse(int event, int x, int y, int flags, void* userdata);
void disImage(char* winName, Mat Image, int Position);
int write(char message[10]);

int main(int argc, char** argv) {


	string filename = "led4.jpg";

	//If no argument is given to "./Contours" then MRI1_01.png is used.

	Mat originalImage = imread(filename);

	
	yuvColour = Scalar(255, 255, 255);
	color[0] = Scalar(255, 0, 0);
	color[1] = Scalar(0, 255, 0);
	color[2] = Scalar(0, 0, 255);

	if(originalImage.empty()) {
		//Checks that the file can be opened, if it can't, prints "can not open" 
		//and end the program
		cout << "can not open " << filename << endl;
		return -1;
	}
	if (DEBUG) {cout << "File Loaded\n\r";}

	if( ELEMENT_TYPE == 0 ){ eleType = MORPH_RECT; }
	else if( ELEMENT_TYPE == 1 ){ eleType = MORPH_CROSS; }
	else if( ELEMENT_TYPE == 2) { eleType = MORPH_ELLIPSE; }

	openElement = getStructuringElement(eleType , Size( 2*OPENING_SIZE + 1, 2*OPENING_SIZE+1 ), Point(OPENING_SIZE, OPENING_SIZE));
	closeElement = getStructuringElement(eleType , Size( 2*CLOSING_SIZE + 1, 2*CLOSING_SIZE+1 ), Point(CLOSING_SIZE, CLOSING_SIZE));

	if (DEBUG) {cout << "Elements Created\n\r";}

	//Creates the image variables used for this project, one is used for each step
	//to facilitate debugging and understanding the code.
	Mat image = originalImage.clone(), threshImage, reducedImage, grayImage, erodedImage, colourImages[3], yuvImage, yuvSplit[3], edgeImage, contoursImage, blueImage, greenImage, redImage, brightImage;

	if (DEBUG) {cout << "Variables Created\n\r";}

	cvtColor(image, yuvImage, CV_BGR2YCrCb);
	split(yuvImage, yuvSplit);
	yuvAvg = mean(yuvImage);

	threshold(yuvSplit[0], brightImage, yuvAvg[0]*1.5, 255, THRESH_TOZERO);
	threshold(yuvSplit[1], redImage, yuvAvg[1]*1.1, 255, THRESH_TOZERO);
	threshold(yuvSplit[2], blueImage, yuvAvg[2]*1.1, 255, THRESH_TOZERO);

	dilate(brightImage, brightImage, closeElement);
	// subtract(brightDImage, brightImage, borderImage);

	disImage((char *)"Bright Image", brightImage, 6);

	split(image, colourImages);

	// blueImage = colourImages[0].clone();
	greenImage = colourImages[1].clone();
	// redImage = colourImages[2].clone();

	disImage((char *)"Yuv Blue Image", blueImage, 1);
	disImage((char *)"Green Image", greenImage, 2);
	disImage((char *)"Yuv Red Image", redImage, 3);

	//White lights should be reduced, as the markers are distinct colours.
	colourImages[0] = blueImage;// - greenImage*0.3 - redImage*0.3;
	colourImages[1] = greenImage;// - blueImage*0.3 - redImage*0.3;
	colourImages[2] = redImage;// - greenImage*0.3 - blueImage*0.3;
	
	bitwise_and(blueImage, brightImage, colourImages[0]);
	bitwise_and(colourImages[1], brightImage, colourImages[1]);
	bitwise_and(redImage, brightImage, colourImages[2]);

	disImage((char *)"Blue Image 2", colourImages[0], 1);
	disImage((char *)"Green Image 2", colourImages[1], 2);
	disImage((char *)"Red Image 2", colourImages[2], 3);

	if (DEBUG) {cout << "Colours Managed" << endl;}

	for(int k = 0; k <= 2; k++) {
		if (DEBUG) {cout << "Loop: " << k << endl;}

		intenseAvg = mean(colourImages[k]);

		threshold(colourImages[k], threshImage, intenseAvg[0]*1.2, 255, THRESH_BINARY);
	
		erode(colourImages[k], erodedImage, openElement);
		dilate(erodedImage, threshImage, openElement);

		// Canny(threshImage, edgeImage, THRESH, 2*THRESH, 3);

		contoursImage = threshImage.clone();
		// contoursImage = edgeImage.clone();

		blur(contoursImage, contoursImage, Size(4,4));

		findContours(contoursImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

		largestArea = 0;
		area = 0;
		contourIndex = 0;
		for(int i = 0; i<contours.size(); i++) {
			area = contourArea(contours[i], false);
			if(area > AREA_THRESH) {
				largestArea = area;
				contourIndex = i;
				bRect = boundingRect(contours[i]);
				rectangle(originalImage, bRect, color[k]);
			}
		}

		switch(k) {
			case 0:
				disImage((char *)"Blue Image 3", threshImage, 1);
				break;
			case 1:
				disImage((char *)"Green Image 3", threshImage, 2);
				break;
			case 2:
				disImage((char *)"Red Image 3", threshImage, 3);
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
			moveWindow(winName, 0, 0);
			break;
		case 2:
			moveWindow(winName, WINDOWX+5, 0);
			break;
		case 3:
			moveWindow(winName, (WINDOWX*2)+10, 0);
			break;
		case 4:
			moveWindow(winName, 0, WINDOWY+25);
			break;
		case 5:
			moveWindow(winName, WINDOWX+5, WINDOWY+25);
			break;
		case 6:
			moveWindow(winName, (WINDOWX*2)+10, WINDOWY+25);
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