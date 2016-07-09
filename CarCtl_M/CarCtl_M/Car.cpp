#include "Car.h"
#include "Sender.h"

void Car::init() {
	hCom = CreateFile(TEXT("COM4"),//COM口
		GENERIC_WRITE, //允许读和写
		0, //独占方式
		NULL,
		OPEN_EXISTING, //打开而不是创建
		0,//synchronize
		NULL);
	if (hCom == (HANDLE)-1)
	{
		printf("打开COM失败!\n");
	}
	else
	{
		printf("COM打开成功！\n");
	}
	SetupComm(hCom, 1, 1024); //输入缓冲区和输出缓冲区的大小都是1024
	COMMTIMEOUTS TimeOuts;
	//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier = 100;
	TimeOuts.WriteTotalTimeoutConstant = 100;
	SetCommTimeouts(hCom, &TimeOuts); //设置超时
	DCB dcb;
	GetCommState(hCom, &dcb);
	dcb.BaudRate = 9600; //波特率为9600
	dcb.ByteSize = 8; //每个字节有8位
	dcb.Parity = NOPARITY; //无奇偶校验位
	dcb.StopBits = ONE5STOPBITS; //两个停止位
	SetCommState(hCom, &dcb);
}

void Car::setSpeed(int speed) {
	if (speed > MAX_SPEED_LEVEL) speedLevel = MAX_SPEED_LEVEL;
	else if (speed < MIN_SPEED_LEVEL) speedLevel = MIN_SPEED_LEVEL;
	else speedLevel = speed;
}

void Car::run(int status, int x, int y) {
	carStatus = status;
	switch (status) {
	case S_STOP:
		stop();
		break;
	case S_FORWARD:
		forward();
		break;
	case S_BACKWARD:
		backward();
		break;
	case S_FOLLOW:
		follow((float)x, (float)y);
		break;
	case S_SPIN_LEFT:
		spinLeft();
		break;
	case S_SPIN_RIGHT:
		spinRight();
		break;
	default:
		stop();
	}
}

void Car::stop() {
	outputToCar(0x0f);
}

void Car::follow(float x, float y) {
	//get person position
	if (y < 0) { y = -y; }

	float rotate_r = 0.0;
	//rotate_r
	if (x > 0.0001 || x < -0.0001) { rotate_r = sqrt(x * x + y * y) / 2 / sqrt(1 - y * (y / (x * x + y * y))); }

	bool doSpin = false;
	//spin when rotate_r too small, stop when too close to person
	if (rotate_r > 0 && rotate_r <= CAR_RADIUS) {
		doSpin = true;
	}
	if (x < 0) { rotate_r = -rotate_r; }

	//output to lower level
	if (doSpin) {
		if (rotate_r > 0) {
			spinRight();
		}
		else {
			spinLeft();
		}
	}
	else {
		float rate = (rotate_r + CAR_RADIUS) / (rotate_r - CAR_RADIUS);
		if (rate < 0) rate = -rate;
		unsigned char rate_index = 0;
		for (; rate_index < N_PULSE_RATE; ++rate_index) {
			if (pulse_rate[rate_index] > rate) break;
		}
		if (rate_index == N_PULSE_RATE) rate_index--;
		else if (pulse_rate[rate_index] - rate >= rate - pulse_rate[rate_index - 1]) rate_index--;

		unsigned char high_char = speedLevel + 1;
		outputToCar((high_char << 4) | rate_index);
	}
}

void Car::forward() {
	outputToCar(((speedLevel + 1) << 4) | 7);
}

void Car::backward() {
	outputToCar(((speedLevel + 5) << 4) | 7);
}

void Car::spinLeft() {
	//anti-clockwise
	unsigned char high_char = speedLevel + 12;
	if (speedLevel == MAX_SPEED_LEVEL) high_char--;
	outputToCar((high_char << 4) | 7);
}

void Car::spinRight() {
	//clockwise
	unsigned char high_char = speedLevel + 9;
	if (speedLevel == MAX_SPEED_LEVEL) high_char--;
	outputToCar((high_char << 4) | 7);
}

void Car::outputToCar(unsigned char raw_speed) {
	PurgeComm(hCom, PURGE_TXCLEAR); //清空缓冲区
	DWORD wCount;//读取的字节数
	BOOL bReadStat;
	unsigned char output_buffer[1] = { raw_speed };
	cout << (((int)raw_speed & 0xf0) >> 4) << "  " << ((int)raw_speed & 0xf) << endl;
	bReadStat = WriteFile(hCom, output_buffer, 1, &wCount, NULL);
	if (!bReadStat)
	{
		char c[2] = { 0, 0 };
		Sender::send(c, 2);

	}
	else {
		char c[2] = { (raw_speed & 0xf0) >> 4, raw_speed & 0xf };
		Sender::send(c, 2);
	}

}