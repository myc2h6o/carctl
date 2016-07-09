#include "Car.h"


int main(int argc, char **argv){
	Car car;
	if ((argc > 1) && (strcmp(argv[1], "FORWARD") == 0)) {
		car.setStatus(S_FORWARD);
	}
	if ((argc > 2)) {
		int initSpeed = 0;
		sscanf_s(argv[2], "%d", &initSpeed);
		car.setSpeed(initSpeed);
	}
	if (car.getSpeed() == 0) {
		for (int i = 0; i < 100; ++i) {
			car.run(S_BACKWARD);
			Sleep(10);
		}
		car.run(S_STOP);
	}
	else if (car.getSpeed() == 1) {
		for (int i = 0; i < 20; ++i) {
			//DEFAULT
			//car.run(S_SPIN_LEFT);
			//car.run(S_SPIN_RIGHT);
			car.run(S_BACKWARD);
			//car.run(S_FORWARD);
			Sleep(10);
		}
		for (int i = 0; i < 60; ++i) {
			//DEFAULT
			//car.run(S_SPIN_LEFT);
			//car.run(S_SPIN_RIGHT);
			car.run(S_FORWARD);
			//car.run(S_FORWARD);
			Sleep(10);
		}
	}
	else if (car.getSpeed() == 2) {
		for (int i = 0; i < 100; ++i) {
			car.run(S_SPIN_LEFT);
			Sleep(10);
		}
		car.run(S_STOP);
	}
	else if (car.getSpeed() == 3) {
		for (int i = 0; i < 100; ++i) {
			car.run(S_SPIN_RIGHT);
			Sleep(33);
		}
		car.run(S_STOP);
	}
}