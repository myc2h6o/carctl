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

using namespace cv;
using namespace std;

int main()
{
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

	PXCImage *colorIm, *depthIm;
	PXCImage::ImageData depth_data;
	PXCImage::ImageData color_data;
	PXCImage::ImageInfo depth_information;
	PXCImage::ImageInfo color_information;

	//Mat img = Mat(480, 640, CV_8UC1);

	namedWindow("color", 0);
	namedWindow("depth", 0);

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
		Mat depth(Size(depth_information.width, depth_information.height), CV_16UC1, (void*)depth_data.planes[0], depth_data.pitches[0] / sizeof(uchar));
		Mat color(Size(color_information.width, color_information.height), CV_8UC3, (void*)color_data.planes[0], color_data.pitches[0] / sizeof(uchar));
		uchar *dp = (uchar*)color_data.planes[0];

		int x, z;
		analyse((unsigned short*)depth.data,&x,&z);
		printf("%d %d\n", x, z);

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
	psm->Release();
}