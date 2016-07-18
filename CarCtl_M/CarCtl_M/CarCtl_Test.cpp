#include "Car.h"
#include <conio.h>

int main(int argc, char **argv){
	Car car;
	car.setSpeed(3);
	//car.setSpeed(2);
	//car.setSpeed(1);
	//car.setSpeed(0);
	cin.get();
	for (int i = 0; i < 200; ++i) {
		car.run(S_FORWARD);
		//car.run(S_BACKWARD);
		Sleep(33);
	}
	car.run(S_STOP);

	/*
	car.setSpeed(3);
	cout << "\nq w\ns a d\n\n";
	while (1) {
		if (_kbhit()) {
			char c = _getch();
			switch (c) {
			case 'w':
				car.run(S_FORWARD);
				break;
			case 's':
				car.run(S_BACKWARD);
				break;
			case 'a':
				car.run(S_SPIN_LEFT);
				break;
			case 'd':
				car.run(S_SPIN_RIGHT);
				break;
			case 27:
				return 1;
			default:
				car.run(S_STOP);
				break;
			}
		}
		Sleep(33);
	}
	*/
}