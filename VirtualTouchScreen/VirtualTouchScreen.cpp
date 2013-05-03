//OpenCV Headers
#include<cv.h>
#include<highgui.h>
//Input-Output
#include<stdio.h>
//Blob Library Headers
#include "cvblob.h"
//Windows Headers
#include<winuser.h>
#include<WinBase.h>

//Time and Windows Routines
#include<time.h>


//Definitions
#define h 240
#define w 320

//NameSpaces
using namespace cvb;
using namespace std;


int main()
{
//--------------------
//Function prototypes
void findxy(IplImage*,int*,int*,IplImage*);     //Function to find X and Y
double detectangle(IplImage*,IplImage*,double,IplImage*,IplImage*);//Angle measure
double findvelocity(IplImage*);//Velocity measure
void changepicture(IplImage*,IplImage*,IplImage*,IplImage*,double);


//Structure to get feed from CAM
CvCapture* capture=cvCreateCameraCapture(0);

//Structure to hold blobs
CvBlobs blobs;

//Windows
cvNamedWindow("Live",CV_WINDOW_AUTOSIZE);
cvNamedWindow("Threshold Filter",CV_WINDOW_AUTOSIZE);
cvNamedWindow("Control",CV_WINDOW_AUTOSIZE);

//Image Variables
IplImage *frame=cvCreateImage(cvSize(w,h),8,3); //Original Image
IplImage *hsvframe=cvCreateImage(cvSize(w,h),8,3);//Image in HSV color space
IplImage *labelImg=cvCreateImage(cvSize(w,h),IPL_DEPTH_LABEL,1);//Image Variable for blobs
IplImage *threshy=cvCreateImage(cvSize(w,h),8,1); //Threshold image of yellow color
//------------------
IplImage *threcpy=cvCreateImage(cvSize(w,h),8,1); //Copy to pass to image velocity finder
IplImage *mhi=cvCreateImage(cvSize(w,h),32,1);    //Motion History Image update
IplImage *orient=cvCreateImage(cvSize(w,h),32,1); //Orientation Image


//Getting the screen information
int screenx = GetSystemMetrics(SM_CXSCREEN);
int screeny = GetSystemMetrics(SM_CYSCREEN);

//Variables for trackbars
int h1=23;int s1=229;int v1=8;
int h2=39;int s2=255;int v2=255;

//Variables for time counting
double beginTime=0;
double endTime=0;
double StopTime=0;
double beginTime2=0;
double endTime2=0;
double StopTime2=0;

//variable to confirm double click
int clc=0;
bool doubleclick = false;

//Creating the trackbars
cvCreateTrackbar("Hue_1","Control",&h1,255,0);
cvCreateTrackbar("Hue_2","Control",&h2,255,0);
cvCreateTrackbar("Sat_1","Control",&s1,255,0);
cvCreateTrackbar("Sat_2","Control",&s2,255,0);
cvCreateTrackbar("Val_1","Control",&v1,255,0);
cvCreateTrackbar("Val_2","Control",&v2,255,0);


while(1)
{
	//Getting the current frame
	IplImage *fram=cvQueryFrame(capture);

	//------------------
	//Getting time in seconds
	double timestamp = (double)clock()/CLOCKS_PER_SEC;

	//If failed to get break the loop
	if(!fram)
	break;

	//Resizing the capture
	cvResize(fram,frame,CV_INTER_LINEAR );

	//Flipping the frame
	cvFlip(frame,frame,1);

	//Changing the color space
	cvCvtColor(frame,hsvframe,CV_BGR2HSV);

	//Thresholding the frame for yellow
	cvInRangeS(hsvframe,cvScalar(h1,s1,v1),cvScalar(h2,s2,v2),threshy);

	
	//Filtering the frame
	cvSmooth(threshy,threshy,CV_MEDIAN,7,7);

	//Finding the blobs
	unsigned int result=cvLabel(threshy,labelImg,blobs);

	//Rendering the blobs
	cvRenderBlobs(labelImg,blobs,frame,frame);

	//-----------------------
	//Copying the yellow thresh image to pass to change picture
	cvCopy(threshy,threcpy);
	//changepicture(threcpy,mhi,orient,frame,timestamp);

	//Filtering the blobs
	cvFilterByArea(blobs,60,500);

	// Start capturing cursor (found cursor on screen)
	bool cursorCaptured = false;
	bool cursorDisappear = false;
	int x = 0;
	int y = 0;
	
	
	//time counting to enable click
	beginTime = timestamp;
	//cout<<"Begin Time :"<<beginTime<<endl;
	//cout<<"End Time :"<<endTime<<endl;
	StopTime = beginTime - endTime;
	
	// Implement left mouse click event
	if(StopTime > 0.3  && endTime >0 && clc < 1 )
	{
		//MouseClick Event
		mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
		cout<<" click "<<endl;
		mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
		endTime=0;
		clc ++;
		beginTime2 = timestamp;
		cout<<beginTime2<<endl;
		doubleclick = true;
	}
	//counting for enable double clicks.
	StopTime2 = endTime2 - beginTime2; 
	
	//disable double click if time elaspe more than 0.6 second
	if(StopTime2 > 0.6)
	{
		beginTime2 = 0;
		clc = 0;
		doubleclick = false;
	}

	//Double click event triggered 
	if(clc == 1 && StopTime2 >0.3 && endTime2 > 0)
	{
		//Double MouseClick Event
		mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
		cout<<" Double click "<<endl;
		endTime=0;
		endTime2 = 0;
		clc = 0; 
	}

	
	for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
	{
		double moment10 = it->second->m10;
		double moment01 = it->second->m01;
		double area = it->second->area;

		//Variable for holding position
		int x1;
		int y1;

		//Calculating the current position
		x1 = moment10/area;
		y1 = moment01/area;

		//Mapping to the screen coordinates
		x=(int)(x1*screenx/w);
		y=(int)(y1*screeny/h);

		//Printing the position information
		cout<<"X-CursorPosition: "<<x<<" Y-CursorPosition: "<<y<<endl;

		//Moving the mouse pointer
		cursorCaptured = true;
		SetCursorPos(x,y);
		endTime = timestamp;
		if(doubleclick)
		{
			endTime2 = timestamp;
		}
	}

	
	StopTime = beginTime - endTime;

	//clear cvBlob
	//blobs.clear();
	cursorDisappear = true;

	
	

	//Showing the images
	cvShowImage("Live",frame);
	cvShowImage("Threshold Filter",threshy);

	//Escape Sequence
	char c=cvWaitKey(33);
	if(c==27)
	break;
}

//Cleanup
cvReleaseCapture(&capture);
cvDestroyAllWindows();
}


/* ||CalculateAngle||
-------------------------

double detectangle(IplImage *thresh,IplImage *mhi,double timestamp,IplImage *orient,IplImage *frame)
{
//Updating the motion history
cvUpdateMotionHistory(thresh,mhi,timestamp,1);

//Calculating motion gradient
cvCalcMotionGradient( mhi,thresh, orient,.5,.05, 3 );

//Rectangle
CvRect rectangle;

//Setting ROI
rectangle=cvRect( 0, 0,w,h);

// select component ROI
cvSetImageROI(frame,rectangle );
cvSetImageROI( mhi,rectangle );
cvSetImageROI( orient,rectangle);
cvSetImageROI(thresh,rectangle);

//Calculating the angle
double angle=cvCalcGlobalOrientation(orient,thresh, mhi, timestamp,1);
angle=360.0-angle;

// cout<<"Angle:"<<angle<<endl;
return angle;
}
*/