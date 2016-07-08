#ifndef __GLOBAL__
#define __GLOBAL__
//#include"pxcprojection.h"

#define WIDTH 640
#define HEIGHT 480
#define NO_HAND 0
#define GESTURE_NONE 1
#define GESTURE_PALM 2
#define GESTURE_FIST 3
#define TWO_HANDS 4
#define GESTURE_SIDE 5

#define ACTION_NONE			0
#define ACTION_STOP			1
#define ACTION_FOLLOW		2
#define ACTION_SHUTDOWN		3
#define ACTION_FORWARD		4
#define ACTION_BACKWARD		5
#define ACTION_TURNLEFT		6
#define ACTION_TURNRIGHT	7

#define PALM_MIN_RATIO  1.2
#define FIST_MIN_RATIO  0.45
#define FIST_MAX_RATIO  1.1
#define FIST_PERCENT    0.77
#define HAND_PERCENT    0.77


#define MAX_COUNT 10
#define SAME_COUNT 4
#define TURN_MIN_SPEED 0.6
#define WARD_MIN_SPEED 1

#endif
