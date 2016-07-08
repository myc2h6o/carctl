#include "Car.h"


int main(int argc, char **argv){
	Car car;
	for (int i = 0; i < argc; ++i) {
		cout << argv[i] << " ";
	}
	cout << endl;
	if ((argc > 1) && (strcmp(argv[1], "FORWARD") == 0)) {
		car.setStatus(S_FORWARD);
	}
	if ((argc > 2)) {
		int initSpeed = 0;
		sscanf_s(argv[2], "%d", &initSpeed);
		car.setSpeed(initSpeed);
	}
	cout << car.getStatus() << " " << car.getSpeed() << endl;

	for (int i = 0; i < 200; ++i) {
		car.run(S_FORWARD);
		Sleep(33);
	}
	car.run(S_STOP);
	cin.get();
}