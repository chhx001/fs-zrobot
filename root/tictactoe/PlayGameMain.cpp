/*
 Project:		TicTacToe with 2D Code
 Version:		Beta
 Author:		Jerry Peng
 Created on:	June 11,2014
*/
#include <opencv2/opencv.hpp>
#include "codeRecog2D.h"
#include "TicTacToe.h"
extern "C"
{
	#include "uArmMovements.h"
    #include "xil_io.h"
}
#include <unistd.h>

using namespace cv;
using namespace std;


#define ADDR_BTN 0x41210008
#define ADDR_LED 0x41200008



bool isRobotDraw[9]={false,false,false,false,false,false,false,false,false};
bool RobotTurn = true;
bool reset = false;
bool start = false;



void *listenBtn(void *arg){
    int btn;
    while(1)
    {
        if(zrarm_btn_state(1))
        {
            start = true;
            sleep(1);
        }
    }

}





//main fuction
int main( int argc, char** argv )
{


	// uArm control Position
	Point3i CoinPos;
	CoinPos.x=-20; CoinPos.y=130; CoinPos.z=0;
	Point3i humanPos;
	humanPos.x=-50; humanPos.y=-90; humanPos.z=-20;
	Point3i BoardCell[9];

	BoardCell[0].x = 50; BoardCell[0].y = 40;
	BoardCell[1].x = 50; BoardCell[1].y = -20;
	BoardCell[2].x = 60; BoardCell[2].y = -75;
	BoardCell[3].x = 97; BoardCell[3].y = 45;
	BoardCell[4].x = 108; BoardCell[4].y = -15;
	BoardCell[5].x = 105; BoardCell[5].y = -65;
	BoardCell[6].x = 140; BoardCell[6].y = 50;
	BoardCell[7].x = 147; BoardCell[7].y = -8;
	BoardCell[8].x = 147; BoardCell[8].y = -60;

	//
	for(int i=0; i<9; i++)
		BoardCell[i].z = -0;

    reset = false;
    start = false;

//////*Creat*///////////////////////////////////////////////////
	VideoCapture cap(0);


	if( !cap.isOpened() ){
		cout << "Could not initialize capturing...\n";
        return -1;
	}

	pthread_t tid;
	int err = pthread_create(&tid,NULL,listenBtn,NULL);
	zrarm_init();
	Xil_Out32(ADDR_LED + 4,0x0);
	cout << "cool" << endl;
	// Creat UART Port
//	 sleep(3000);
//////*Initial*/////////////////////////////////////////////////

    while(1){
        Mat frame;
        Mat markerDetectImage;
        vector<aruco::Marker> markers;
		Xil_Out32(ADDR_LED,1);
        while(!start);
		Xil_Out32(ADDR_LED,0);
		printf("start!\n");
        uArmCtrl(CoinPos.x,CoinPos.y,CoinPos.z,0x01);
		//printf("is it sleep?\n");
        //sleep(2);
		//printf("sleep!\n");
//////*Robot first move*////////////////////////////////////////
        srand((int)time(0));
        char _board[9];
        memset(_board, 0, 9);
        int v;
        int state = INPROGRESS;
		bool isClick[9];
        double tagx,tagy,tagz;
        int tagcmd;
        RobotTurn = true;
		CodeRecog2D codeRecog2D(0.043);
    //////*while loop*////////////////////////////////////////
        while(state != WIN && state != LOSE && state != DRAW){
            if(!start)
                break;
            if(RobotTurn){
                v = minimax(_board);
                _board[v] = o;
                state = gameState(_board);
                printf("\ncompute play is:%d\n", v);
				codeRecog2D.lastcell[v] = 0xFFFF;
                PrintBoard(_board);

                // grasp one coin
                uArmCtrl(CoinPos.x,CoinPos.y,CoinPos.z,18);
                sleep(1);
                // place one coin
                uArmCtrl(BoardCell[v].x,BoardCell[v].y,BoardCell[v].z,19);
				sleep(1);
                tagcmd = 0;
    //			while(tagcmd != 9999){
    //			scanf("%lf %lf %lf %d",&tagx,&tagy,&tagz,&tagcmd);
    //			uArmCtrl(tagx,tagy,tagz,tagcmd);
    //			}
    //			sleep(5);
				
				for(int tempi = 0;tempi < 8; ++ tempi)
				{
					cap >> frame;
				}

                if (state == WIN || state == LOSE || state == DRAW)
                    break;

                RobotTurn = false;
            }


            cap>>frame;
            frame.copyTo(markerDetectImage);
            //printf("copyto\n");
            codeRecog2D.detectAndDraw(markerDetectImage);
            //printf("detect\n");
            markers = codeRecog2D.getMarkers();
            printf("get marker\n");
            for(int i=0;i<9;i++){
                isClick[i] = codeRecog2D.GetClickOrNot(i);
                if(isClick[i]) cout<<i<<endl;
            }
            //imshow("markerDetectImage1", markerDetectImage);

            for(int i=0; i<9; i++){
                    if(isClick[i]==true && _board[i]==empty) {
                        _board[i] = Xrobot;
                        RobotTurn = true;
                    }
                }
            state = gameState(_board);
            if(RobotTurn) PrintBoard(_board);


            switch (state)
            {
            case WIN:
                printf("Win!!\n");
                break;
            case LOSE:
                printf("Lose!!\n");
                break;
            case DRAW:
                printf("Draw!!\n");
                break;
            default:
                break;
            }


    //		imshow("raw", frame);


            if(waitKey(33)==27) break;
        } // while_end
        start = false;
	}

	// clear board
/*	for(int i=0; i<9; i++){
		if(_board[i] == o){		// move from boardCell to cion initial position
			// grasp coin
			uArmCtrl(BoardCell[i].x,BoardCell[i].y,BoardCell[i].z,0xA2);
			sleep(3);
			// place coin back
			uArmCtrl(CoinPos.x,CoinPos.y,CoinPos.z,0x13);
		}
		else if(_board[i] == Xrobot){	// move from boardCell to people side
			// grasp coin
			uArmCtrl(BoardCell[i].x,BoardCell[i].y,BoardCell[i].z,0xA2);
			sleep(3);
			// place coin back
			uArmCtrl(humanPos.x,humanPos.y,humanPos.z,0x13);
		}
	}
*/

// } //outer-while End
}
