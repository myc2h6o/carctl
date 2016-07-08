
#include <pxcsession.h>
#include <pxcsensemanager.h>
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

using namespace cv;
using namespace std;

const int dataLen = WIDTH*HEIGHT * 2;

PXCProjection *projection;
Mat depth;

int main(int argc, char **argv)
{
	int initialStatus = S_STOP;
	if (argc > 1 && strcmp(argv[1], "FORWARD") == 0) { initialStatus = S_FORWARD; }
	Car car(initialStatus);
	cout << car.getStatus();

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

	int lastResult = GESTURE_NONE;
	int currResult = GESTURE_NONE;
	int result = currResult;
	int action = ACTION_NONE;
	int dx, dy, dz;
	int currx, lastx, curry, lasty, currz, lastz;
	dx = dy = dz = lastx = lasty = lastz = 0;
	int count = 0;
	int gestureCount = 0;
	bool first = true; // 第一帧
	char sendData[dataLen];
	int actionCounter = 0;
	while (waitKey(1))
	{
		if (psm->AcquireFrame(true) < PXC_STATUS_NO_ERROR) break;

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

		Sender::send((char *)depth.data, dataLen);
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
		currResult = analyse((unsigned short*)depth.data, &currx, &curry, &currz);
		if (lastResult == NO_HAND)
			first = true;
		if (first)
		{
			lastx = currx;
			lasty = curry;
			lastz = currz;
			dx = 0;
			first = false;
		}
		if (count < MAX_COUNT)
		{
			dx += currx - lastx;
			count++;
		}
		else
		{
			dx = 0;
			count = 0;
		}
		printf("Res: %d currx: %d, lastx: %d dx: %d\n", lastResult, currx, lastx, dx);
		lastx = currx;
		lasty = curry;
		lastz = currz;
		//printf("%d %d %d %d\n", currResult, currx, curry, currz);
		if (currResult == lastResult)
			gestureCount++;
		else
			gestureCount = 0;

		if (gestureCount >= 4)
		{
			result = currResult;
			gestureCount = 0;
		}

		if (result != 0)
		{
			if (action == ACTION_FOLLOW)
			{
				if (result == GESTURE_FIST || result == GESTURE_NONE)
					action = ACTION_FOLLOW;
				else
					action = ACTION_STOP;
			}
			else
			{
				if (result == GESTURE_PALM)
				{
					action = ACTION_STOP;
				}
				else if (result == GESTURE_FIST)
				{
					action = ACTION_FOLLOW;
				}
			}
		}
		else
		{
			if (result == NO_HAND)
				action = ACTION_NONE;
		}
		if (action == ACTION_FOLLOW)
		{
			vector<Position> pos;
			pos.push_back(Position(currx, currz + 200));
			car.setStatus(S_FOLLOW);
			car.setPersonPositions(pos);
			car.follow();
			printf("follow\n");
		}
		else if (action == ACTION_STOP || action == ACTION_NONE)
		{
			printf("stop\n");
			car.stop();
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