#include <stdio.h>	//
#include <Windows.h>//
#include <iostream>//
#include <time.h>//
using namespace std;//
void process() {//
	int n = rand() % 1000 + 500;//
	Sleep(n);//
	cout << " done "<< n << endl;//
}//

#define N_WAIT_MS 1000
clock_t frameClock = clock();
clock_t newFrameClock;




int main() {//
	while (1) {//
		process();//


		newFrameClock = clock();
		if (N_WAIT_MS - newFrameClock + frameClock > 0) {
			Sleep(N_WAIT_MS - newFrameClock + frameClock);
		}
		frameClock = clock();


		cout << frameClock / 1000 << endl;//
	}//
}//