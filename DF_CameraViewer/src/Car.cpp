#include "Car.h"

void Car::init() {
	hCom = CreateFile(TEXT("COM3"),//COM��
		GENERIC_READ | GENERIC_WRITE, //�������д
		0, //��ռ��ʽ
		NULL,
		OPEN_EXISTING, //�򿪶����Ǵ���
		0, //ͬ����ʽ
		NULL);
	if (hCom == (HANDLE)-1)
	{
		printf("��COMʧ��!\n");
	}
	else
	{
		printf("COM�򿪳ɹ���\n");
	}
	SetupComm(hCom, 1024, 1024); //���뻺����������������Ĵ�С����1024
	COMMTIMEOUTS TimeOuts;
	//�趨����ʱ
	TimeOuts.ReadIntervalTimeout = 1000;
	TimeOuts.ReadTotalTimeoutMultiplier = 500;
	TimeOuts.ReadTotalTimeoutConstant = 5000;
	//�趨д��ʱ
	TimeOuts.WriteTotalTimeoutMultiplier = 500;
	TimeOuts.WriteTotalTimeoutConstant = 2000;
	SetCommTimeouts(hCom, &TimeOuts); //���ó�ʱ
	DCB dcb;
	GetCommState(hCom, &dcb);
	dcb.BaudRate = 9600; //������Ϊ9600
	dcb.ByteSize = 8; //ÿ���ֽ���8λ
	dcb.Parity = NOPARITY; //����żУ��λ
	dcb.StopBits = ONE5STOPBITS; //����ֹͣλ
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
	PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR); //��ջ�����
	DWORD wCount;//��ȡ���ֽ���
	BOOL bReadStat;
	unsigned char output_buffer[1] = { raw_speed };
	//cout << (((int)raw_speed & 0xf0) >> 4) << "  " << ((int)raw_speed & 0xf) << endl;
	bReadStat = WriteFile(hCom, output_buffer, 1, &wCount, NULL);
	if (!bReadStat)
	{
		//printf("write com fail\n");
	}
}