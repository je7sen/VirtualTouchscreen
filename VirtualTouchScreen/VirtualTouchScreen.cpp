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
CvBlobs blobs2;

//Windows
cvNamedWindow("Live",CV_WINDOW_AUTOSIZE);
cvNamedWindow("Threshold Filter",CV_WINDOW_AUTOSIZE);
cvNamedWindow("Threshold Filter2",CV_WINDOW_AUTOSIZE);
cvNamedWindow("Control",CV_WINDOW_AUTOSIZE);
cvNamedWindow("Control_RCE",CV_WINDOW_AUTOSIZE);

//Image Variables
IplImage *frame=cvCreateImage(cvSize(w,h),8,3); //Original Image
IplImage *hsvframe=cvCreateImage(cvSize(w,h),8,3);//Image in HSV color space
IplImage *labelImg=cvCreateImage(cvSize(w,h),IPL_DEPTH_LABEL,1);//Image Variable for blobs
IplImage *threshy=cvCreateImage(cvSize(w,h),8,1); //Threshold image of yellow color
IplImage *labelImg2=cvCreateImage(cvSize(w,h),IPL_DEPTH_LABEL,1);//Image Variable for blobs2
IplImage *threshy2=cvCreateImage(cvSize(w,h),8,1); //Threshold image of skin color
IplImage *threshy3=cvCreateImage(cvSize(w,h),8,1); //Threshold image of combine
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

//Variables for trackbars 2
int h3=70;int s3=62;int v3=0;
int h4=98;int s4=110;int v4=61;

//Variables for time counting
double beginTime=0;
double endTime=0;
double StopTime=0;
double beginTime2=0;
double endTime2=0;
double StopTime2=0;
double beginTime3=0;
double endTime3=0;
double StopTime3=0;

//variable to confirm double click
int clc=0;
bool doubleclick = false;

//variable to confirm long click
bool ToEnableLongClick = false;
bool longclick = false;


//Creating the trackbars
cvCreateTrackbar("Hue_1","Control",&h1,255,0);
cvCreateTrackbar("Hue_2","Control",&h2,255,0);
cvCreateTrackbar("Sat_1","Control",&s1,255,0);
cvCreateTrackbar("Sat_2","Control",&s2,255,0);
cvCreateTrackbar("Val_1","Control",&v1,255,0);
cvCreateTrackbar("Val_2","Control",&v2,255,0);

//Creating the trackbars 2
cvCreateTrackbar("Hue_1","Control_RCE",&h3,255,0);
cvCreateTrackbar("Hue_2","Control_RCE",&h4,255,0);
cvCreateTrackbar("Sat_1","Control_RCE",&s3,255,0);
cvCreateTrackbar("Sat_2","Control_RCE",&s4,255,0);
cvCreateTrackbar("Val_1","Control_RCE",&v3,255,0);
cvCreateTrackbar("Val_2","Control_RCE",&v4,255,0);


while(1)
{
	//Getting the current frame
	IplImage *fram=cvQueryFrame(capture);

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

	//Thresholding the fram for skin colour
	cvInRangeS(hsvframe,cvScalar(h3,s3,v3),cvScalar(h4,s4,v4),threshy2);


	//Filtering the frame
	cvSmooth(threshy,threshy,CV_MEDIAN,7,7);

	//Filtering the frame
	cvSmooth(threshy2,threshy2,CV_MEDIAN,7,7);

	//Finding the blobs
	unsigned int result=cvLabel(threshy,labelImg,blobs);

	//Finding the blobs 2
	unsigned int result2=cvLabel(threshy2,labelImg2,blobs2);

	//Rendering the blobs
	cvRenderBlobs(labelImg,blobs,frame,frame);

	//Rendering the blobs
	cvRenderBlobs(labelImg2,blobs2,frame,frame);

	//-----------------------
	//Copying the yellow thresh image to pass to change picture
	cvCopy(threshy,threcpy);
	//changepicture(threcpy,mhi,orient,frame,timestamp);

	//Filtering the blobs
	cvFilterByArea(blobs,60,500);

	//Filtering the blobs
	cvFilterByArea(blobs2,60,500);

	// Start capturing cursor (found cursor on screen)
	int x = 0;
	int y = 0;


	//time counting to enable click
	beginTime = timestamp;
	beginTime3 = timestamp;
	if(doubleclick)
	{
		beginTime2 = timestamp;
	}

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
		clc = 1;
		
		//beginTime2 = timestamp;
		endTime3 = timestamp;
		cout<<beginTime2<<endl;
		doubleclick = true;
		ToEnableLongClick = true;

	}


	//counting for enable double clicks.
	StopTime2 = beginTime2 - endTime2; 

	//disable double click if time elaspe more than 1.5 second
	if(StopTime2 > 1.5 && doubleclick )
	{
		beginTime2 = 0;
		clc = 0;
		doubleclick = false;
	}

	//Double click event triggered 
	if(doubleclick && StopTime2 >0.3 && endTime2 > 0)
	{
		//Double MouseClick Event
		mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
		cout<<" Double click "<<endl;
		endTime = 0;
		endTime2 = 0;
		endTime3 = 0;
		doubleclick = false;
		ToEnableLongClick = false;
		clc = 0; 
	}

	//counting for long click condition
	StopTime3 = beginTime3 - endTime3;

	//enable long click 
	if(clc = 1 && endTime3 >0 && ToEnableLongClick)
	{
		if(StopTime3 > 1.5)
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
			cout<<" long click "<<endl;
			clc = 0;
			endTime2=0;
			doubleclick = false;
			ToEnableLongClick = false;
			longclick = true;
		}

	}

	//disable long click if time more than 4 second
	if(StopTime3 > 4 && longclick )
	{
		mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
		cout<<" release long click"<<endl;
		clc = 0;
		endTime3=0;
		longclick = false;
	}


	for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
	{

		//reset clc to enable left click again if long click enabled
		if( ToEnableLongClick)
		{
			ToEnableLongClick = false;
			longclick =  false;
			
		}

		

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
		SetCursorPos(x,y);

		//release long mouse click
		mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);



		//record time for first click
		endTime = timestamp;

		//enable time for double click if first click is trigger
		if(doubleclick)
		{
			endTime2 = 0;
			endTime2 = timestamp;
		}


	}

	for (CvBlobs::const_iterator ti=blobs2.begin(); ti!=blobs2.end(); ++ti)
		{
		double moment10_2 = ti->second->m10;
		double moment01_2 = ti->second->m01;
		double area2 = ti->second->area;

		//Variable for holding position
		int x2;
		int y2;

		//Calculating the current position
		x2 = moment10_2/area2;
		y2 = moment01_2/area2;

		//Mapping to the screen coordinates
		int x3=(int)(x2*screenx/w);
		int y3=(int)(y2*screeny/h);

		//trigger right mouse click event
		mouse_event(MOUSEEVENTF_RIGHTDOWN, x, y, 0, 0);
		mouse_event(MOUSEEVENTF_RIGHTUP, x, y, 0, 0);
		cout<<" Right click"<<endl;
		}




	//Showing the images
	cvShowImage("Live",frame);
	cvAnd(threshy,threshy2,threshy3);
	cvShowImage("Threshold Filter",threshy);
	cvShowImage("Threshold Filter2",threshy2);


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