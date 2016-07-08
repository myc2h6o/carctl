#include "Global.h"
#include <opencv2\opencv.hpp>
int analyse(unsigned short* depth_array, int* x, int *z)
{
	int check[65536] = { 0 };
	printf("in\n");
	for (int i = 0;i != HEIGHT;i++)
	{
		for (int j = 0;j != WIDTH;j++)
		{
			unsigned short tmp = depth_array[i * WIDTH + j];
			check[tmp]++;
		}
	}

	int tot = 0;
	for (int i = 1;i != 65536;i++)
	{
		tot += check[i];

	}
	int high = tot / 200;
	int low = tot / 800;

	int start = 0;
	int end = 0;
	for (int i = 1;i != 65536;i++)
	{
		if (check[i] > high)
		{
			start = i;
			end = start;
			break;
		}
	}
	for (int i = start;i != 0;i--)
	{
		if (check[i] < low)
		{
			start = i;
			break;
		}
	}
	for (int i = end;i != 65536;i++)
	{
		if (check[i] < low)
		{
			end = i;
			break;
		}
	}
	int hand = 0;
	for (int i = 0;i != HEIGHT;i++)
	{
		for (int j = 0;j != WIDTH;j++)
		{
			unsigned short tmp = depth_array[i * WIDTH + j];
			if (tmp > (unsigned short)start && tmp < (unsigned short)end)
			{
				*x = j;
				*z = tmp;
				depth_array[i * WIDTH + j] = 65535;
				hand++;
			}
			else
			{

				depth_array[i * WIDTH + j] = 0;
			}
		}
	}
	int tmp_para = start * start * (hand / 100);

	if (tmp_para < 10000000 || tmp_para>80000000)
	{
		//debug
		memset(depth_array, 0, HEIGHT*WIDTH * 2);
		return -1;
	}


	/*
	int five = 5;
	for (int i = 2;i != HEIGHT-2;i++)
	{
		for (int j = 2;j != WIDTH-2;j++)
		{
			if (depth_array[i * WIDTH + j] != 0 && depth_array[i * WIDTH + j - WIDTH] == 0 && depth_array[i * WIDTH + j - WIDTH -1] == 0 && depth_array[i * WIDTH + j - WIDTH+1] == 0)
			{
				if ((depth_array[i * WIDTH + j + 1] == 0 || depth_array[i * WIDTH + j + 2] == 0 ) && (depth_array[i * WIDTH + j - 1] == 0 || depth_array[i * WIDTH + j - 2] == 0))
				{
					five--;
					printf("%d\n", i);
					for (int tmp_j = 0;tmp_j != WIDTH;tmp_j++)
						depth_array[i * WIDTH + tmp_j] = 35535;
					break;
				}
			}
		}
		if (five == 0)
			break;
	}
	*/


	//printf("%d\n", start * start * (hand/100));


	//imwrite("test.jpg", depth*40);


	printf("out");
	return -1;
}