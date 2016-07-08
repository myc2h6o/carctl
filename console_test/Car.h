/*
*high 4 bit:
*0,15  stop
*1-4   forward
*5-8   backward
*9-11  clockwise
*12-14 anti-clockwise
*
*low 4 bit:
*1-6  l>r
*7    1:1
*8-14 l<r
*15   stop
*/
#pragma once
#include <Windows.h>
#include <iostream>
using namespace std;

const int S_STOP = 0;
const int S_FORWARD = 1;
const int S_BACKWARD = 2;
const int S_FOLLOW = 3;
const int S_SPIN_LEFT = 4;
const int S_SPIN_RIGHT = 5;

const float Pi = 3.1415926536f;
const float CAR_RADIUS = 182.0;

const int MIN_SPEED_LEVEL = 0;
const int MAX_SPEED_LEVEL = 3;
const int DEFAULT_SPEED_LEVEL = 1;
const unsigned char N_PULSE_RATE = 15;
const float pulse_rate[N_PULSE_RATE] = { 0.7071, 0.7489, 0.7908, 0.8326, 0.8745, 0.9163, 0.9444, 1.0, 1.0589,  1.0913, 1.1435, 1.2011, 1.2645, 1.3353, 1.4142 };

class Car {
public:
	void run(int status, int x = 0, int y = 0);
	int getStatus() { return carStatus; }
	void setStatus(int status) { carStatus = status; }
	Car() { init(); }
	int getSpeed() { return speedLevel; }
	void setSpeed(int speed);
	void speedUp() { if (speedLevel < MAX_SPEED_LEVEL) speedLevel++; }
	void speedDown() { if (speedLevel > MIN_SPEED_LEVEL) speedLevel--; }

private:
	int speedLevel = DEFAULT_SPEED_LEVEL;
	int carStatus = S_STOP;

	HANDLE hCom;
	void init();
	void stop();
	void follow(float x, float y);
	void forward();
	void backward();
	void spinLeft();
	void spinRight();
	void outputToLower(unsigned char raw_speed);
};