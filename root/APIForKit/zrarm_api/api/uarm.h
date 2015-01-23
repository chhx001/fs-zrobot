#ifndef UARM_H_INCLUDED
#define UARM_H_INCLUDED

#define SERVO_L 0
#define SERVO_R 1
#define SERVO_ROT 2
#define SERVO_HANDROT 3
#define SERVO_HAND 4
#define PUMP_EN 5
#define VALVE_EN 6
#define LIMIT_SW 7

#define LOW 0
#define HIGH 1

#define TRUE 1
#define FALSE 0

/********************basic*******************/

/******************* Port definitions*****************/

#define SERVO_L 0
#define SERVO_R 1
#define SERVO_ROT 2
#define SERVO_HANDROT 3
#define SERVO_HAND 4
#define PUMP_EN 5
#define VALVE_EN 6
#define LIMIT_SW 7

#define LOW 0
#define HIGH 1

/******************** Macro definitions **************/
#define ARM_A                   148    // upper arm
#define ARM_B                   160    // lower arm
#define ARM_2AB                 47360  // 2*A*B
#define ARM_A2                  21904  // A^2
#define ARM_B2                  25600  // B^2
#define ARM_A2B2                47504  // A^2 + B^2
#define ARM_STRETCH_MIN         0
#define ARM_STRETCH_MAX         210
#define ARM_HEIGHT_MIN          -180
#define ARM_HEIGHT_MAX          150
#define ARM_ROTATION_MIN        -90
#define ARM_ROTATION_MAX        90
#define HAND_ROTATION_MIN       -90
#define HAND_ROTATION_MAX       90

#define HAND_ANGLE_OPEN         25	//if hand is assembled
#define HAND_ANGLE_CLOSE        70

#define D150A_SERVO_MIN_PUL     535
#define D150A_SERVO_MAX_PUL     2415
#define D009A_SERVO_MIN_PUL     600
#define D009A_SERVO_MAX_PUL     2550

#define SAMPLING_DEADZONE       2	//Parameters for calibration
#define INIT_POS_L              37	//value , that to be compared . in order to calculate the offset.
#define INIT_POS_R              25
#define CALIBRATION_FLAG		0xEE

#define FIXED_OFFSET_L          30	//
#define FIXED_OFFSET_R          40

#define RAD_TO_DEG              57.2957786
/**************************** for learning *****************************/
#define SERVO_MAX				1020 //AD value. Reference = 1V. For learning Mode.
#define SERVO_MIN				65

#define MEMORY_SERVO_PER		335   //  eeprom: (1024 - 3 - 14)/3=335
#define DATA_FLAG				255
#define BUFFER_OUTPUT			5

double constrain(double x,double a,double b);

typedef struct PosXYZ
{
    double x,y,z;
    int handrot;
} PosXYZ;

typedef struct PosSHR
{
    double s,h;  //stretch height
    int r;         //rotate
    int handrot;
} PosSHR;

typedef struct UArmStatus
{
    PosSHR pos;
    char pump;     //true--on,false--off
    int speed;
} UArmStatus;

typedef struct UArmAttr
{
    unsigned int base_address;
    struct {
        unsigned short pwm_min;
        unsigned short pwm_max;
    } servo[4];
    short heightLst;
    unsigned char gripperRst;
    signed char offsetL;
    signed char offsetR;
} UArmAttr;


typedef struct UArm{

    //methods
    PosSHR (*getPosSHR)(struct UArm*);
	PosXYZ (*getPosXYZ)(struct UArm*);
	void (*getXYZ)(struct UArm*,double *x,double *y,double *z);
	void (*getSHR)(struct UArm*,double *s,double *h,double *r);
	int (*getHandrot)(struct UArm*);


	void (*setSHR)(struct UArm*,double stretch,double height,int rot,int handrot);  //set position by rotate
	void (*setXYZ)(struct UArm*,double x,double y,double z,int handrot); //set position by xyz

    void (*setPosSHR)(struct UArm*,PosSHR shr);
    void (*setPosXYZ)(struct UArm*,PosXYZ xyz);

    char (*ifSwitchOn)(struct UArm*);

    void (*pumpOn)(struct UArm *);
	void (*pumpOff)(struct UArm*);

    void (*init)(struct UArm*);

    UArmAttr attr;
    UArmStatus status;

    //values



} UArm;

UArm getUArm();



#endif // UARM_H_INCLUDED
