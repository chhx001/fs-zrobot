/*
 Project:		Two_Dimension Code Recognition & Location
 Author:		Jerry Peng
 Created on:	Mar 19,2014

 2014-7-5 Modify: set Y Perpendicular to 2D code surface
*/

#include "zrcam.h"
#include <vector>

using namespace std;
using namespace cv;

int main(int argc, const char** argv)
{
     Zrcam zrcam(0);
     vector<int> ids;
     if(!zrcam.isvaild()){
         cout << "capture open failed" << endl;
         return -1;
     }

	while(1){
        
        //cv::imshow("test",zrcam.get_a_frame());
        zrcam.get_code2d_ids(ids);

		for (unsigned int i=0;i<ids.size();i++) {
            cout<<ids[i]<<endl;
        }
        //cv::waitKey(33);

	}

	return 0;
}

