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

#define timeDelaySet 20
#define coverDetectThres 500
#define coinDetectThres 50

using namespace std;
using namespace cv;

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

	bool isCover;		// Board Detect Result
	bool isDraw[9];		// Board Detect Result
	bool isDrawDelay[9]; // Board Detect Result
	bool isClick[9]; // Board Detect Result
	int timeDelay;



public:
	int lastcell[9];
/******************************************************************
/* Function: Constructors and Destructor
******************************************************************/
CodeRecog2D(double markerSize):markerSize(markerSize){

	//read camera parameter
    CamParam.readFromXMLFile("/root/tictactoe/camera.yml");

	// non-pyrdown
	MDetector.pyrDown(0);
	// Board state
	isCover = false;
	for(int i=0; i<9; i++){
		isDraw[i] = false;
		isDrawDelay[i] = false;
		isClick[i] = false;
	}
	timeDelay = 0;// timeDelaySet;

	initcell();
}

CodeRecog2D(double markerSize,int speed):markerSize(markerSize), speed(speed){

	//read camera parameter
    CamParam.readFromXMLFile("/root/tictactoe/camera.yml");

	// non-pyrdown
	MDetector.pyrDown(speed);
	// Board state
	isCover = false;
	for(int i=0; i<9; i++){
		isDraw[i] = false;
		isDrawDelay[i] = false;
		isClick[i] = false;
	}
	timeDelay = 0; // timeDelaySet;

	initcell();
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

bool GetCoverOrNot(){
	return isCover;
}

bool GetDrawOrNot(int id){
	return isDraw[id];
}

bool GetClickOrNot(int id){
	return isClick[id];
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
	MDetector.detect(InImage,Markers,CamParam,markerSize, false);
}

void initcell()
{
    for(int i = 0;i < 9; ++ i){
        lastcell[i] = 0;
    }
}

void putcell(int i){
    lastcell[i] = 5000;
}



/******************************************************************
/* Function:
******************************************************************/
void detectAndDraw(cv::Mat InImage){
	// Detect
	static int last_cover = coverDetectThres;
	detect(InImage);

	for (unsigned int i=0;i<Markers.size();i++) {
//		Markers[i].draw(InImage,cv::Scalar(0,0,255),2);
     }
	////draw a XYZ-axis
        if (  CamParam.isValid() && markerSize!=-1)
            for (unsigned int i=0;i<Markers.size();i++) {
//              aruco::CvDrawingUtils::draw3dAxis(InImage,Markers[i],CamParam);

				if(Markers[i].id == 985){
						aruco::CvDrawingUtils::draw3dCube(InImage,Markers[i],CamParam);

//////////////////////////// Image Processing ///////////////////////////////////////////////////////////
						Mat gray;
						cvtColor(InImage, gray, CV_BGR2GRAY);
						Mat BW;
						threshold(gray, BW, 0, 255,  CV_THRESH_OTSU+CV_THRESH_BINARY_INV);
						isCover = false;
						Mat Cover;
						BW.copyTo(Cover);
						Mat  coverDetectMask(Cover.size(), CV_8UC1, Scalar(0));
						fillConvexPoly(coverDetectMask, Markers[i].detectVerts, 4, Scalar(255),8,0);
						Mat coverDetectImage;
						Cover.copyTo(coverDetectImage, coverDetectMask);
                        for(int j=0; j<9; j++)
						{
						    Mat Cell[9];
                            Cell[j] = BW(Markers[i].chessboardCellRect[j]);
                            if(countNonZero(Cell[j]) > lastcell[j] + coinDetectThres){
                                isCover = true;
                                printf("countNonZero(Cell[%d]):%d,lastcell:%d,%d is caught\n",j,countNonZero(Cell[j]),lastcell[j],j);
                            }
						}

						if(timeDelay <= 0 && isCover) timeDelay=timeDelaySet;
						if(timeDelay>0) timeDelay--;
						printf("td:%d\n",timeDelay);

						if(isCover==true && timeDelay == 1){
                            Mat Cell[9];
							// draw detect
							for(int j=0; j<9; j++){
								isDraw[j] = false;
								isClick[j] = false;
							}
							//ROI Rect valid check
							bool roiRectValid = true;
							for(int j=0;j<9;j++){
								if(Markers[i].chessboardCellRect[j].x<0 || Markers[i].chessboardCellRect[j].y<0
									|| Markers[i].chessboardCellRect[j].x + Markers[i].chessboardCellRect[j].width>BW.size().width
									|| Markers[i].chessboardCellRect[j].y + Markers[i].chessboardCellRect[j].height>BW.size().height
									)
									{
									    roiRectValid = false;
									    printf("maker[%d].x:%d .y%d .w%d .h%d\n",j,Markers[i].chessboardCellRect[j].x,Markers[i].chessboardCellRect[j].y,Markers[i].chessboardCellRect[j].width,Markers[i].chessboardCellRect[j].height);
									}

							}

							if(roiRectValid)
								for(int j=0; j<9; j++){
									Cell[j] = BW(Markers[i].chessboardCellRect[j]);
                                    printf("cell[%d]:%d\n",j,countNonZero(Cell[j]));
									if( countNonZero(Cell[j]) > coinDetectThres)
										isDraw[j] = true;
                                    if(countNonZero(Cell[j]) > lastcell[j] + coinDetectThres){
                                        putcell(j);
                                        printf("%d is settled\n",j);
                                        //getchar();
                                    }
									if(isDraw[j]==true && isDrawDelay[j]==false)
										isClick[j] = true;
									else
										isClick[j] = false;
								//	if(isClick[j]) cout<<j<<endl;
									isDrawDelay[j] = isDraw[j];
								}
                            //getchar();
                            last_cover = countNonZero(coverDetectImage);
						}
//////////////////////////// Image Processing end ///////////////////////////////////////////////////////////


				}


            }




}



}; /*CLASS END*/



#endif
