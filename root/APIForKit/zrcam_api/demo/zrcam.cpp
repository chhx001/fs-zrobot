//
//  Zrcam.cpp
//  
//
//  Created by Zrobot on 15/1/8.
//
//

#include <stdio.h>
#include "Zrcam.h"

void Zrcam::init(double marker_size,int speed)
{
    this->set_marker_size(marker_size);
    this->cam_param.readFromXMLFile("camera.yml");
    this->set_speed(speed);
}

/*Zrcam::Zrcam(int cap_num,double marker_size = 0.025,int speed = 0):capture(cap_num)
{
    this->init(marker_size,speed);
    this->mor.pyrDown(speed);
}*/

bool Zrcam::isvaild()
{
    if(!this->capture.isOpened()){
        return false;
    }
    return true;
}

void Zrcam::set_speed(int speed)
{
    this->speed = speed;
    this->mdetector.pyrDown(speed);
}

void Zrcam::set_marker_size(double marker_size)
{
    this->marker_size = marker_size;
}

int Zrcam::get_speed()
{
    return this->speed;
}

double Zrcam::get_marker_size()
{
    return this->speed;
}

cv::VideoCapture Zrcam::get_capture()
{
    return this->capture;
}

cv::Mat Zrcam::get_a_frame()
{
    capture >> this->frame;
    return this->frame;
}

void Zrcam::detect_code2d(cv::Mat input_frame)
{
    cam_param.resize(input_frame.size());
    input_frame.copyTo(input_frame);
    mdetector.detect(input_frame,this->markers,this->cam_param,this->marker_size,true);
}

void Zrcam::get_code2d_ids(vector<int> &ids)
{
    cv::Mat temp_frame;
    ids.clear();
    for(int i = 5;i > 0; -- i){
        this->capture >> temp_frame;
        this->detect_code2d(temp_frame);
        if(0 < this->markers.size()){
            break;
        }
        cv::waitKey(33);
    }
    for(int i = 0;i < markers.size(); ++ i){
        ids.push_back(markers[i].id);
    }
}

void Zrcam::release()
{
    this->frame.release();
    this->capture.release();
}

/*Zrcam::~Zrcam()
{
    this->release();
}*/


