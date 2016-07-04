#include "Car.h"


int main(int argc, char **argv){
	int initialStatus = S_STOP;
	if (argc > 1 && strcmp(argv[1], "FORWARD") == 0) { initialStatus = S_FORWARD }
	Car car(initialStatus);
	cout << car.getStatus();
	int x;
	int y;
	vector<Position> positions;
	while (1) {
		cout << "x, y:";
		cin >> x;
		cin >> y;
		if (x == 0 && y == 0) break;
		positions.clear();
		positions.push_back(Position(x, y));
		car.setPersonPositions(positions);
		for (int i = 0; i < 200; ++i) {
			Sleep(33);
			car.follow();
		}

	}
	while(1) {
		cout << "0 to quit:";
		cin >> x;
		if (x == 0) break;
		for (int i = 0; i < 200; ++i) {
			Sleep(33);
			car.stop();
		}
	}
}