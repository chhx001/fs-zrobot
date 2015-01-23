/*
 Project:		Two_Dimension Code Recognition & Location
 Author:		Jerry Peng
 Created on:	Mar 19,2014
*/


#if !defined CODERECOG2D
#define CODERECOG2D

#include <opencv2/opencv.hpp>
#include "aruco.h"
#include "cvdrawingutils.h"

enum
{
	SPEED_NORM = 0,
	SPEED_FAST = 1
};


class CodeRecog2D{

private:
	double markerSize;					// Two-dimension Code side length, Unit: Meter(m)
	aruco::CameraParameters CamParam;	// Camera Parameters
	aruco::MarkerDetector MDetector;    // Marker Detector
	vector<aruco::Marker> Markers;      // Marker Container
	cv::Mat TheInputImageCopy;          // Copy of input image
	int speed;							// process speed: 0-normal; 1-fast


public:
/******************************************************************
/* Function: Constructors and Destructor
******************************************************************/
CodeRecog2D(double markerSize):markerSize(markerSize){

	//read camera parameter
    CamParam.readFromXMLFile("camera.yml");

	// non-pyrdown
	MDetector.pyrDown(0);
}

CodeRecog2D(double markerSize,int speed):markerSize(markerSize), speed(speed){

	//read camera parameter
    CamParam.readFromXMLFile("camera.yml");

	// non-pyrdown
	MDetector.pyrDown(speed);
}

~CodeRecog2D(){}


/******************************************************************
/* Function: Sets and Gets
******************************************************************/
void SetSpeed(int speed_set){
	speed = speed_set;
	MDetector.pyrDown(speed);
}

int GetSpeed(){
	return speed;
}

void SetMarkerSize(double markerSize_set){
	markerSize= markerSize_set;
}

double GetMarkerSize(){
	return markerSize;
}

vector<aruco::Marker> getMarkers(){
	return Markers;
}


/******************************************************************
/* Function: 
******************************************************************/
void detect(cv::Mat InImage){
	// resize parameter
	CamParam.resize( InImage.size());
	// copy input image to a frame buffer
	InImage.copyTo(TheInputImageCopy);

	// Detect
	MDetector.detect(InImage,Markers,CamParam,markerSize, true);
}

/******************************************************************
/* Function: 
******************************************************************/
void detectAndDraw(cv::Mat InImage){
	// Detect
	detect(InImage);
	for (unsigned int i=0;i<Markers.size();i++) {
		Markers[i].draw(InImage,cv::Scalar(0,0,255),2);
     }
	////draw a XYZ-axis
        if (  CamParam.isValid() && markerSize!=-1)
            for (unsigned int i=0;i<Markers.size();i++) {
                aruco::CvDrawingUtils::draw3dAxis(InImage,Markers[i],CamParam);
				aruco::CvDrawingUtils::draw3dCube(InImage,Markers[i],CamParam);
            }

	

}



} /*CLASS END*/



#endif
