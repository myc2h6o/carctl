#include "Car.h"


int main(int argc, char *argv[]){
	Car car;
	vector<Position> positions;
	positions.push_back(Position(1, 120));
	car.setPersonPositions(positions);
	for (int i = 0; i < 200; ++i) {
		Sleep(33);
		car.follow();
	}
	car.stop();
	system("pause");
}