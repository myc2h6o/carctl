#include "Car.h"

void Car::init() {
	hCom = CreateFile(TEXT("COM3"),//COM口
		GENERIC_READ | GENERIC_WRITE, //允许读和写
		0, //独占方式
		NULL,
		OPEN_EXISTING, //打开而不是创建
		0, //同步方式
		NULL);
	if (hCom == (HANDLE)-1)
	{
		printf("打开COM失败!\n");
	}
	else
	{
		printf("COM打开成功！\n");
	}
	SetupComm(hCom, 1024, 1024); //输入缓冲区和输出缓冲区的大小都是1024
	COMMTIMEOUTS TimeOuts;
	//设定读超时
	TimeOuts.ReadIntervalTimeout = 1000;
	TimeOuts.ReadTotalTimeoutMultiplier = 500;
	TimeOuts.ReadTotalTimeoutConstant = 5000;
	//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier = 500;
	TimeOuts.WriteTotalTimeoutConstant = 2000;
	SetCommTimeouts(hCom, &TimeOuts); //设置超时
	DCB dcb;
	GetCommState(hCom, &dcb);
	dcb.BaudRate = 9600; //波特率为9600
	dcb.ByteSize = 8; //每个字节有8位
	dcb.Parity = NOPARITY; //无奇偶校验位
	dcb.StopBits = ONE5STOPBITS; //两个停止位
	SetCommState(hCom, &dcb);
}

void Car::setPersonPositions(const vector<Position> &positions) {
	personPositions = positions;
}

void Car::follow() {
	status = S_FOLLOW;
	//get person position
	if (personPositions.size() == 0) {
		return;
	}
	float x = personPositions[0].x;
	float y = personPositions[0].y;
	if (y < 0) { y = -y; }

	float rotate_r = 0.0;
	//rotate_r
	if (x > 0.0001 || x < -0.0001) { rotate_r = sqrt(x * x + y * y) / 2 / sqrt(1 - y * (y / (x * x + y * y))); }

	int status = C_WALK;
	//spin when rotate_r too small, stop when too close to person
	if (rotate_r > 0 && rotate_r <= CAR_RADIUS) {
		status = C_SPIN;
	}
	else if (x * x + y * y <= MIN_DISTANCE_2) {
		status = C_STOP;
	}
	if (x < 0) { rotate_r = -rotate_r; }

	//output to lower level
	if (status == C_STOP) {
		outputToLower(0x0f);
	}
	else if (status == C_SPIN) {
		unsigned char high_char = speedLevel + 9;
		if (rotate_r < 0) { high_char += 3; }
		unsigned char low_char = 7;
		outputToLower((high_char << 4) | low_char);
	}
	else if (status == C_WALK) {
		float rate = (rotate_r + CAR_RADIUS) / (rotate_r - CAR_RADIUS);
		if (rate < 0) rate = -rate;
		unsigned char rate_index = 0;
		for (; rate_index < N_PULSE_RATE; ++rate_index) {
			if (pulse_rate[rate_index] > rate) break;
		}
		if (rate_index == N_PULSE_RATE) rate_index--;
		else if (pulse_rate[rate_index] - rate >= rate - pulse_rate[rate_index - 1]) rate_index--;

		unsigned char high_char = speedLevel + 1;
		outputToLower((high_char << 4) | rate_index);
	}
}

void Car::stop() {
	status = S_STOP;
	outputToLower(0x0f);
}

void Car::process() {
	if (status == S_FORWARD) {
		vector<Position> positions;
		positions.push_back(Position(0, 500));
		setPersonPositions(positions);
		follow();
	}
	else if (status == S_FOLLOW) {
		follow();
	}
	else {
		stop();
	}
}

void Car::outputToLower(unsigned char raw_speed) {
	PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR); //清空缓冲区
	DWORD wCount;//读取的字节数
	BOOL bReadStat;
	unsigned char output_buffer[1] = { raw_speed };
	//cout << (((int)raw_speed & 0xf0) >> 4) << "  " << ((int)raw_speed & 0xf) << endl;
	bReadStat = WriteFile(hCom, output_buffer, 1, &wCount, NULL);
	if (!bReadStat)
	{
		//printf("write com fail\n");
	}
}