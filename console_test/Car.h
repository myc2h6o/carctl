#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
using namespace std;

const int C_STOP = 0;
const int C_SPIN = 1;
const int C_WALK = 2;
const int S_STOP = 3;
const int S_FORWARD = 4;
const int S_FOLLOW = 5;

const float Pi = 3.1415926536f;
const float CAR_RADIUS = 182.0;
const float MAX_SPEED = 5.0;
const int MIN_DISTANCE_2 = 40000;

const int MIN_SPEED_LEVEL = 0;
const int MAX_SPEED_LEVEL = 2;
const unsigned char N_PULSE_RATE = 15;
//const float pulse_rate[N_PULSE_RATE] = { 0.7071, 0.7489, 0.7908, 0.8326, 0.8745, 0.9163, 0.9444, 1.0, 1.0589,  1.0913, 1.1435, 1.2011, 1.2645, 1.3353, 1.4142 };
const float pulse_rate[N_PULSE_RATE] = { 0.5071, 0.5489, 0.5908, 0.6326, 0.6745, 0.7163, 0.7444, 1.0, 1.2589,  1.2913, 1.3435, 1.4011, 1.4645, 1.5353, 1.6142 };

struct Position {
	float x;
	float y;
	Position(float xx, float yy) :x(xx), y(yy) {}
};

class Car {
public:
	int getStatus() { return status; }
	void setStatus(int s) { status = s; }
	int getSpeedLevel() { return speedLevel; }
	Car() { status = S_STOP; init(); }
	Car(int initial_status) : status(initial_status) { init(); }
	void setPersonPositions(const vector<Position> &positions);
	void follow();
	void stop();
	void process();
	void speedUp() { if (speedLevel < MAX_SPEED_LEVEL) speedLevel++; }
	void speedDown() { if (speedLevel > MIN_SPEED_LEVEL) speedLevel--; }

	vector<Position> personPositions;
	int speedLevel = MIN_SPEED_LEVEL+2;
	int status = S_STOP;
	HANDLE hCom;
	void outputToLower(unsigned char raw_speed);
	void init();
};