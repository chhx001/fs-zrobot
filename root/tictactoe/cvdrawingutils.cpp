/*****************************
Copyright 2011 Rafael Muñoz Salinas. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY Rafael Muñoz Salinas ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Rafael Muñoz Salinas OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of Rafael Muñoz Salinas.
********************************/
#include "cvdrawingutils.h"
using namespace cv;
namespace aruco {
/****
 *
 *
 *
 ****/
void CvDrawingUtils::draw3dAxis(cv::Mat &Image,Marker &m,const CameraParameters &CP)
{

  float size=m.ssize*3;
    Mat objectPoints (4,3,CV_32FC1);
    objectPoints.at<float>(0,0)=0;
    objectPoints.at<float>(0,1)=0;
    objectPoints.at<float>(0,2)=0;
    objectPoints.at<float>(1,0)=size;
    objectPoints.at<float>(1,1)=0;
    objectPoints.at<float>(1,2)=0;
    objectPoints.at<float>(2,0)=0;
    objectPoints.at<float>(2,1)=size;
    objectPoints.at<float>(2,2)=0;
    objectPoints.at<float>(3,0)=0;
    objectPoints.at<float>(3,1)=0;
    objectPoints.at<float>(3,2)=size;

    vector<Point2f> imagePoints;
    cv::projectPoints( objectPoints, m.Rvec,m.Tvec, CP.CameraMatrix,CP.Distorsion,   imagePoints);
//draw lines of different colours
    cv::line(Image,imagePoints[0],imagePoints[1],Scalar(0,0,255,255),1,CV_AA);
    cv::line(Image,imagePoints[0],imagePoints[2],Scalar(0,255,0,255),1,CV_AA);
    cv::line(Image,imagePoints[0],imagePoints[3],Scalar(255,0,0,255),1,CV_AA);
    putText(Image,"x", imagePoints[1],FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,0,255,255),2);
    putText(Image,"y", imagePoints[2],FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,255,0,255),2);
    putText(Image,"z", imagePoints[3],FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255,0,0,255),2);
}

/****
 *
 *
 *
 ****/
void CvDrawingUtils::draw3dCube(cv::Mat &Image,Marker &m,const CameraParameters &CP)
{

	// cell centers
    Mat cellCenters (9,3,CV_32FC1);
    double halfSize=m.ssize/2;
	cellCenters.at<float>(4,0)=m.ssize*2.5;
    cellCenters.at<float>(4,1)=0;
    cellCenters.at<float>(4,2)=0;
    cellCenters.at<float>(0,0)=cellCenters.at<float>(4,0)-m.ssize;
    cellCenters.at<float>(0,1)=m.ssize;
    cellCenters.at<float>(0,2)=0;
    cellCenters.at<float>(1,0)=cellCenters.at<float>(4,0);
    cellCenters.at<float>(1,1)=m.ssize;
    cellCenters.at<float>(1,2)=0;
    cellCenters.at<float>(2,0)=cellCenters.at<float>(4,0)+m.ssize;
    cellCenters.at<float>(2,1)=m.ssize;
    cellCenters.at<float>(2,2)=0;
    cellCenters.at<float>(3,0)=cellCenters.at<float>(4,0)-m.ssize;
    cellCenters.at<float>(3,1)=0;
    cellCenters.at<float>(3,2)=0;
    cellCenters.at<float>(5,0)=cellCenters.at<float>(4,0)+m.ssize;
    cellCenters.at<float>(5,1)=0;
    cellCenters.at<float>(5,2)=0;
    cellCenters.at<float>(6,0)=cellCenters.at<float>(4,0)-m.ssize;
    cellCenters.at<float>(6,1)=-m.ssize;
    cellCenters.at<float>(6,2)=0;
    cellCenters.at<float>(7,0)=cellCenters.at<float>(4,0);
    cellCenters.at<float>(7,1)=-m.ssize;
    cellCenters.at<float>(7,2)=0;
	cellCenters.at<float>(8,0)=cellCenters.at<float>(4,0)+m.ssize;
    cellCenters.at<float>(8,1)=-m.ssize;
    cellCenters.at<float>(8,2)=0;
	//
    vector<Point2f> cellCenterPoints;
    projectPoints( cellCenters, m.Rvec,m.Tvec,  CP.CameraMatrix,CP.Distorsion,   cellCenterPoints);

//	Rect chessboardCellRect[9];
	for(int i=0; i<9; i++){
		m.chessboardCellRect[i].x =  cellCenterPoints[i].x - 15;
		m.chessboardCellRect[i].y =  cellCenterPoints[i].y - 15;
		m.chessboardCellRect[i].width = m.chessboardCellRect[i].height = 30;	
	}

	// board outline
	Mat boardCorners (4,3,CV_32FC1);
	float boardCornerOffSet = m.ssize/4;
	boardCorners.at<float>(0,0)=cellCenters.at<float>(4,0)-m.ssize*3/2-boardCornerOffSet;
	boardCorners.at<float>(0,1)=cellCenters.at<float>(4,1)+m.ssize*3/2+boardCornerOffSet;
	boardCorners.at<float>(0,2)=0;
	boardCorners.at<float>(1,0)=cellCenters.at<float>(4,0)+m.ssize*3/2+boardCornerOffSet;
	boardCorners.at<float>(1,1)=cellCenters.at<float>(4,1)+m.ssize*3/2+boardCornerOffSet;
	boardCorners.at<float>(1,2)=0;
	boardCorners.at<float>(2,0)=cellCenters.at<float>(4,0)+m.ssize*3/2+boardCornerOffSet;
	boardCorners.at<float>(2,1)=cellCenters.at<float>(4,1)-m.ssize*3/2-boardCornerOffSet;
	boardCorners.at<float>(2,2)=0;
	boardCorners.at<float>(3,0)=cellCenters.at<float>(4,0)-m.ssize*3/2-boardCornerOffSet;
	boardCorners.at<float>(3,1)=cellCenters.at<float>(4,1)-m.ssize*3/2-boardCornerOffSet;
	boardCorners.at<float>(3,2)=0;
	//
	vector<Point2f> boardCornerPoints;
    projectPoints( boardCorners, m.Rvec,m.Tvec,  CP.CameraMatrix,CP.Distorsion,   boardCornerPoints);
	// cover to Point(2D int)
		for(int i=0; i<4; i++)
			m.boardVerts[i]= boardCornerPoints[i];

	// Cover Detect Area Corners
	Mat detectCorners (4,3,CV_32FC1);
	float detectOffSet = m.ssize/2;
	detectCorners.at<float>(0,0)=cellCenters.at<float>(4,0)-m.ssize*3/2-boardCornerOffSet;
	detectCorners.at<float>(0,1)=cellCenters.at<float>(4,1)+m.ssize*3/2+boardCornerOffSet;
	detectCorners.at<float>(0,2)=0;
	detectCorners.at<float>(1,0)=cellCenters.at<float>(4,0)+m.ssize*3/2+detectOffSet;
	detectCorners.at<float>(1,1)=cellCenters.at<float>(4,1)+m.ssize*3/2+boardCornerOffSet;
	detectCorners.at<float>(1,2)=0;
	detectCorners.at<float>(2,0)=cellCenters.at<float>(4,0)+m.ssize*3/2+detectOffSet;
	detectCorners.at<float>(2,1)=cellCenters.at<float>(4,1)-m.ssize*3/2-boardCornerOffSet;
	detectCorners.at<float>(2,2)=0;
	detectCorners.at<float>(3,0)=cellCenters.at<float>(4,0)-m.ssize*3/2-boardCornerOffSet;
	detectCorners.at<float>(3,1)=cellCenters.at<float>(4,1)-m.ssize*3/2-boardCornerOffSet;
	detectCorners.at<float>(3,2)=0;
	//
	vector<Point2f> detectCornerPoints;
    projectPoints( detectCorners, m.Rvec,m.Tvec,  CP.CameraMatrix,CP.Distorsion,   detectCornerPoints);
	//cover to Point(2D int)
	for(int i=0; i<4; i++)
		m.detectVerts[i]= detectCornerPoints[i];

		


}


/****
 *
 *
 *
 ****/
void CvDrawingUtils::draw3dAxis(cv::Mat &Image,Board &B,const CameraParameters &CP)
{
Mat objectPoints (4,3,CV_32FC1);
objectPoints.at<float>(0,0)=0;objectPoints.at<float>(0,1)=0;objectPoints.at<float>(0,2)=0;
objectPoints.at<float>(1,0)=2*B[0].ssize;objectPoints.at<float>(1,1)=0;objectPoints.at<float>(1,2)=0;
objectPoints.at<float>(2,0)=0;objectPoints.at<float>(2,1)=2*B[0].ssize;objectPoints.at<float>(2,2)=0;
objectPoints.at<float>(3,0)=0;objectPoints.at<float>(3,1)=0;objectPoints.at<float>(3,2)=2*B[0].ssize;

vector<Point2f> imagePoints;
projectPoints( objectPoints, B.Rvec,B.Tvec, CP.CameraMatrix, CP.Distorsion,   imagePoints);
//draw lines of different colours
cv::line(Image,imagePoints[0],imagePoints[1],Scalar(0,0,255,255),2,CV_AA);
cv::line(Image,imagePoints[0],imagePoints[2],Scalar(0,255,0,255),2,CV_AA);
cv::line(Image,imagePoints[0],imagePoints[3],Scalar(255,0,0,255),2,CV_AA);

putText(Image,"X", imagePoints[1],FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,255,255),2);
putText(Image,"Y", imagePoints[2],FONT_HERSHEY_SIMPLEX, 1, Scalar(0,255,0,255),2);
putText(Image,"Z", imagePoints[3],FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0,255),2);
}


/****
 *
 *
 *
 ****/
void CvDrawingUtils::draw3dCube(cv::Mat &Image,Board &B,const CameraParameters &CP)
{

float cubeSize=B[0].ssize;
float txz=-cubeSize/2;
Mat objectPoints (8,3,CV_32FC1);
objectPoints.at<float>(0,0)=txz;objectPoints.at<float>(0,1)=0;objectPoints.at<float>(0,2)=txz;
objectPoints.at<float>(1,0)=txz+cubeSize;objectPoints.at<float>(1,1)=0;objectPoints.at<float>(1,2)=txz;
objectPoints.at<float>(2,0)=txz+cubeSize;objectPoints.at<float>(2,1)=cubeSize;objectPoints.at<float>(2,2)=txz;
objectPoints.at<float>(3,0)=txz;objectPoints.at<float>(3,1)=cubeSize;objectPoints.at<float>(3,2)=txz;

objectPoints.at<float>(4,0)=txz;objectPoints.at<float>(4,1)=0;objectPoints.at<float>(4,2)=txz+cubeSize;
objectPoints.at<float>(5,0)=txz+cubeSize;objectPoints.at<float>(5,1)=0;objectPoints.at<float>(5,2)=txz+cubeSize;
objectPoints.at<float>(6,0)=txz+cubeSize;objectPoints.at<float>(6,1)=cubeSize;objectPoints.at<float>(6,2)=txz+cubeSize;
objectPoints.at<float>(7,0)=txz;objectPoints.at<float>(7,1)=cubeSize;objectPoints.at<float>(7,2)=txz+cubeSize;

vector<Point2f> imagePoints;
projectPoints( objectPoints,B.Rvec,B.Tvec, CP.CameraMatrix, CP.Distorsion,   imagePoints);
//draw lines of different colours
for(int i=0;i<4;i++)
  cv::line(Image,imagePoints[i],imagePoints[(i+1)%4],Scalar(0,0,255,255),1,CV_AA);

for(int i=0;i<4;i++)
  cv::line(Image,imagePoints[i+4],imagePoints[4+(i+1)%4],Scalar(0,0,255,255),1,CV_AA);

for(int i=0;i<4;i++)
  cv::line(Image,imagePoints[i],imagePoints[i+4],Scalar(0,0,255,255),1,CV_AA);
}

}
