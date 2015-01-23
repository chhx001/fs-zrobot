//
//  zrcam.h
//  
//
//  Created by Zrobot on 15/1/8.
//
//

#ifndef _zrcam_h
#define _zrcam_h


#include <opencv2/opencv.hpp>
#include "aruco.h"
#include "cvdrawingutils.h"


#define ZRCAM_SPEED_FAST 1
#define ZRCAM_SPEED_NORM 0

class Zrcam
{
public:
    Zrcam(int cap_num,double markers_size=0.025,int speed = ZRCAM_SPEED_NORM):capture(cap_num)
    {
        this->init(marker_size,speed);
    }
    cv::VideoCapture get_capture();         //get capture
    void init(double marker_size,int speed);              //new a capture and
    cv::Mat get_a_frame();               //get a frame;
    void get_code2d_ids(vector<int> &ids);                    //detect 5 frames,pick 2dcode's id
    void release();                          //release the source
    ~Zrcam()
    {
        this->release();
    }//destructor: do close()
    void set_speed(int speed);
    int get_speed();
    void set_marker_size(double marker_size);
    double get_marker_size();
    void detect_code2d(cv::Mat input_frame);
    bool isvaild();
    
    
    
    
private:
    cv::VideoCapture capture;
    cv::Mat frame;
    double marker_size;                      // Two-dimension Code side length, Unit: Meter(m)
    aruco::CameraParameters cam_param;       // Camera Parameters
    aruco::MarkerDetector mdetector;        // Marker Detector
    vector<aruco::Marker> markers;          // Marker Container
    int speed;                              // process speed: 0-normal; 1-fast
    
    
    
    
    
    
};


#endif
