#include <pxcsensemanager.h>
#include <pxcsession.h>
#include "util_render.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <opencv2\opencv.hpp>
#include <fstream>
#include "Global.h"
#include "GestureRecognition.h"
#include "Sender.h"
#include "Car.h"
#include <string.h>
#include <time.h>

using namespace cv;
using namespace std;

const int dataLen = WIDTH*HEIGHT * 2;

PXCProjection *projection;
Mat depth;

int main(int argc, char **argv)
{
	Car car;
	if ((argc > 1) && (strcmp(argv[1], "FORWARD") == 0)) {
		car.setStatus(S_FORWARD);
	}
	if ((argc > 2)) {
		int initSpeed = 0;
		sscanf_s(argv[2], "%d", &initSpeed);
		car.setSpeed(initSpeed);
	}
	
	UtilRender*  Rcolor = new UtilRender(L"COLOR");
	UtilRender*  Rdepth = new UtilRender(L"DEPTH");

	PXCSenseManager *psm = PXCSenseManager::CreateInstance();
	if (!psm)
	{
		wprintf_s(L"Unabel to create the PXCSenseManager\n");
		return 1;
	}

	psm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, WIDTH, HEIGHT);
	psm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, WIDTH, HEIGHT);

	if (psm->Init() != PXC_STATUS_NO_ERROR)
	{
		wprintf_s(L"Unable to Init the PXCSenseManager\n");
		return 2;
	}

	projection = psm->QueryCaptureManager()->QueryDevice()->CreateProjection();

	PXCImage *colorIm, *depthIm;
	PXCImage::ImageData depth_data;
	PXCImage::ImageData color_data;
	PXCImage::ImageInfo depth_information;
	PXCImage::ImageInfo color_information;

	//Mat img = Mat(480, 640, CV_8UC1);

	namedWindow("color", 0);
	namedWindow("depth", 0);

	int lastResult = NO_HAND;
	int currResult = GESTURE_NONE;
	int action = ACTION_NONE;
	int dx, dy, dz;
	int currx, lastx, curry, lasty, currz, lastz;
	dx = dy = dz = 0;//lastx = lasty = lastz = 0;
	//int count = 0;
	//int 
	int handPosition[SAME_COUNT][4] = { 0 };
	bool first = true; // 第一帧
	char sendData[dataLen];
	int sameGesture = 0;
	clock_t startClock = clock();
	while (waitKey(1))
	{
		if (psm->AcquireFrame(true) < PXC_STATUS_NO_ERROR) break;

		//Sender::send("s", 1);
		PXCCapture::Sample *sample = psm->QuerySample();

		colorIm = sample->color;
		depthIm = sample->depth;

		colorIm->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB24, &color_data);
		depthIm->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_DEPTH, &depth_data);
		depth_information = sample->depth->QueryInfo();
		color_information = sample->color->QueryInfo();
		//cout << depthIm->PixelFormatToString(depth_data.format);
		//printf("%d",(int)depth_information.width);
		depth = Mat(Size(depth_information.width, depth_information.height), CV_16UC1, (void*)depth_data.planes[0], depth_data.pitches[0] / sizeof(uchar));
		Mat color(Size(color_information.width, color_information.height), CV_8UC3, (void*)color_data.planes[0], color_data.pitches[0] / sizeof(uchar));

		/*
		for (int i = 0; i < dataLen; i += 6)
		{
			ushort row = ((i / 6) / WIDTH);
			ushort col = ((i / 6)) % WIDTH;
			sendData[i] = row & 0xff;
			sendData[i + 1] = (row >> 8) & 0xff;
			sendData[i + 2] = col & 0xff;
			sendData[i + 3] = (col >> 8) & 0xff;
			sendData[i + 4] = depth.data[i / 3];
			sendData[i + 5] = depth.data[i / 3 + 1];
		}
		Sender::send(sendData, dataLen);
		*/
		//medianBlur(depth, depth, 5);
	    currResult = analyse((unsigned short*)depth.data,&currx,&curry,&currz);

		//Sender::send((char *)depth.data, dataLen);
		/*
		switch (currResult)
		{
		case NO_HAND:
			Sender::send("a", 1);
			break;
		case GESTURE_NONE:
			Sender::send("b", 1);
			break;
		case GESTURE_PALM:
			Sender::send("c", 1);
			break;
		case GESTURE_FIST:
			Sender::send("d", 1);
			break;
		case TWO_HANDS:
			Sender::send("e", 1);
			break;
		default:
			break;
		}*/
		if (lastResult == NO_HAND)
			first = true;
		if (first)
		{
			//lastx = currx;
			//lasty = curry;
			//lastz = currz;
			dx = 0;
			first = false;
		}
		clock_t nowClock = clock();
		for (int i = 0;i != SAME_COUNT - 1;i++)
		{
			handPosition[i][0] = handPosition[i + 1][0];
			handPosition[i][1] = handPosition[i + 1][1];
			handPosition[i][2] = handPosition[i + 1][2];
			handPosition[i][3] = handPosition[i + 1][3];
		}
		handPosition[SAME_COUNT - 1][0] = currx;
		handPosition[SAME_COUNT - 1][1] = curry;
		handPosition[SAME_COUNT - 1][2] = currz;
		handPosition[SAME_COUNT - 1][3] = nowClock;
		/*if (count < MAX_COUNT)
		{
			dx += currx - lastx;
			count++;
		}
		else
		{
			dx = 0;
			count = 0;
		}*/
		printf("Res: %d currx: %d, curry: %d currz: %d clock: %d\n", lastResult, currx, curry, currz,nowClock);
		//lastx = currx;
		//lasty = curry;
		//lastz = currz;
		//printf("%d %d %d %d\n", currResult, currx, curry, currz);

		//state change

		if (currResult != NO_HAND)
		{
			if (lastResult == currResult)
				sameGesture++;
			else
				sameGesture = 1;
			if (sameGesture == SAME_COUNT)
			{
				if (currResult == GESTURE_PALM)
					action = ACTION_STOP;
				if (currResult == GESTURE_FIST)
					action = ACTION_FOLLOW;
			}
			if (sameGesture >= SAME_COUNT)
			{

				if (currResult == GESTURE_SIDE)
				{
					//printf("DIF: %d\n", handPosition[SAME_COUNT - 1][0] - handPosition[0][0]);
					//printf("dif %d %d\n", handPosition[SAME_COUNT - 1][0] - handPosition[0][0], handPosition[SAME_COUNT - 1][3] - handPosition[0][3]);
					int dx = handPosition[SAME_COUNT - 1][0] - handPosition[0][0];
					int dt = handPosition[SAME_COUNT - 1][3] - handPosition[0][3];
					if (dx>dt*TURN_MIN_SPEED)
					{
						action = ACTION_TURNLEFT;
					}
					if (dx<-dt*TURN_MIN_SPEED)
					{
						action = ACTION_TURNRIGHT;
					}
				}

				if (currResult == TWO_HANDS)
				{
					int dz = handPosition[SAME_COUNT - 1][2] - handPosition[0][2];
					int dt = handPosition[SAME_COUNT - 1][3] - handPosition[0][3];
					if (dz>dt*TURN_MIN_SPEED)
					{
						action = ACTION_BACKWARD;
					}
					if (dz<-dt*TURN_MIN_SPEED)
					{
						action = ACTION_FORWARD;
					}
				}
			}
			/*
			if (action == ACTION_FOLLOW)
			{
				if (currResult == GESTURE_FIST || currResult == GESTURE_NONE)
					action = ACTION_FOLLOW;
				else
					action = ACTION_STOP;
			}
			else
			{
				if (currResult == GESTURE_PALM)
				{
					if (lastResult == GESTURE_PALM)
						action = ACTION_STOP;
				}
				else if (currResult == GESTURE_FIST)
				{
					if (lastResult == GESTURE_FIST)
						action = ACTION_FOLLOW;
				}
			}*/
		}
		else
		{
			if(lastResult == NO_HAND)
				action = ACTION_NONE;
			sameGesture = 0;
		}

		//car action
		if (action == ACTION_FOLLOW)
		{
			car.run(S_FOLLOW, currx, currz + 200);
			printf("follow\n");
		}
		else if (action == ACTION_STOP || action == ACTION_NONE)
		{
			car.run(S_STOP);
			printf("stop\n");
		}
		else if (action == ACTION_FORWARD)
		{
			car.run(S_FORWARD);
			printf("forward\n");
		}
		else if (action == ACTION_BACKWARD)
		{
			car.run(S_BACKWARD);
			printf("backward\n");
		}
		else if (action == ACTION_TURNLEFT)
		{
			car.run(S_SPIN_LEFT);
			printf("turnleft\n");
		}
		else if (action == ACTION_TURNRIGHT)
		{
			car.run(S_SPIN_RIGHT);
			printf("turnright\n");
		}

		lastResult = currResult;

		depthIm->ReleaseAccess(&depth_data);
		colorIm->ReleaseAccess(&color_data);

		imshow("color", color);
		imshow("depth", depth);
		//if (!renderColor->RenderFrame(sample->color)) break;
		//if (!renderDepth->RenderFrame(sample->depth)) break;

		psm->ReleaseFrame();
		/*Mat cur;
		medianBlur(img, cur, 5);
		cv::imshow("ok1", cur);*/
	}
	projection->Release();
	psm->Release();

}