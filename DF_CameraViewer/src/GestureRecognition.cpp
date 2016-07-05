#include "Global.h"
#include"pxcprojection.h"
#include <vector>
#include <opencv2\opencv.hpp>
//#include <algorithm>
using namespace cv;
using namespace std;

extern Mat depth;
extern PXCProjection *projection;
int analyse(unsigned short* depth_array, int* x, int* y, int *z)
{
	//�Ƚ��лҶ�ֱ��ͼ���㣬check�����ǻҶ�ֱ��ͼ��Ϣ
	int tmpx, tmpy, tmpz;
	int rowWith[HEIGHT] = { 0 };
	int check[65536] = { 0 };
	//printf("in\n");
	for (int i = 0; i != HEIGHT; i++)
	{
		for (int j = 0; j != WIDTH; j++)
		{
			unsigned short tmp = depth_array[i * WIDTH + j];
			check[tmp]++;
		}
	}
	//Ȼ�����ֱ��ͼ����˫��ֵ�������ֲ�
	int tot = 0;
	for (int i = 1; i != 65536; i++)
	{
		tot += check[i];
	}
	int high = tot / 200;
	int low = tot / 700;

	int start = 0;
	int end = 0;
	for (int i = 1; i != 65536; i++)
	{
		if (check[i] > high)
		{
			start = i;
			end = start;
			break;
		}
	}
	for (int i = start; i != 0; i--)
	{
		if (check[i] < low)
		{
			start = i;
			break;
		}
	}
	for (int i = end; i != 65536; i++)
	{
		if (check[i] < low)
		{
			end = i;
			break;
		}
	}
	//��ȡ���ֵ���ֵ�󣬸���start��end�������ֲ�
	int mid_tip = true;
	int maxrow = 0;
	int currow = 0;
	int hand = 0;
	int top = 0;
	for (int i = 0; i != HEIGHT; i++)
	{
		currow = 0;
		for (int j = 0; j != WIDTH; j++)
		{
			unsigned short tmp = depth_array[i * WIDTH + j];
			if (tmp > (unsigned short)start && tmp < (unsigned short)end)
			{
				if (top == 0)
					top = i;
				if (mid_tip)
				{
					tmpx = j;
					tmpy = i;
					tmpz = tmp;
					mid_tip = false;
				}
				depth_array[i * WIDTH + j] = 65535;
				currow++;
				hand++;
			}
			else
			{
				depth_array[i * WIDTH + j] = 0;
			}
		}
		// ��¼����һ��
		rowWith[i] = currow;
		if (maxrow < currow)
			maxrow = currow;
	}
	//������󣬼����ܹ��ĵ㣬�;���ƽ���˻�Ϊ��ֵ���ų���
	int tmp_para = start * start * (hand / 100);

	if (tmp_para < 6000000 || tmp_para>50000000)
	{
		//debug
		*x = *y = *z = 0;
		memset(depth_array, 0, HEIGHT*WIDTH * 2);
		return NO_HAND;
	}

	PXCPoint3DF32 unzPoint;
	PXCPoint3DF32 pos3D;
	unzPoint.x = tmpx;
	unzPoint.y = tmpy;
	unzPoint.z = tmpz;
	// ת��Ϊ��ʵ��������
	projection->ProjectDepthToCamera(1, &unzPoint, &pos3D);

	*x = pos3D.x;
	*y = pos3D.y;
	*z = pos3D.z;
	int five = 5;
	int five_y[5];
	int five_x[5];
	//�����ֵķ����Ǵ��϶�����5������Ҫ��ĵ㡣
	for (int i = 10; i != HEIGHT - 10; i++)
	{
		for (int j = 10; j != WIDTH - 10; j++)
		{
			if (depth_array[i * WIDTH + j] != 0 && depth_array[i * WIDTH + j + WIDTH] != 0)
			{
				/*
				if ((depth_array[i * WIDTH + j + 1] == 0 || depth_array[i * WIDTH + j + 2] == 0 || depth_array[i * WIDTH + j + 3] == 0 || depth_array[i * WIDTH + j + 4] == 0 || depth_array[i * WIDTH + j + 5] == 0 || depth_array[i * WIDTH + j + 6] == 0) && (depth_array[i * WIDTH + j - 1] == 0 || depth_array[i * WIDTH + j - 2] == 0 || depth_array[i * WIDTH + j - 3] == 0 || depth_array[i * WIDTH + j - 4] == 0 || depth_array[i * WIDTH + j - 5] == 0 || depth_array[i * WIDTH + j - 6] == 0))
				{

				}
				else
				{
				//break;
				}*/
				int flag = 0;
				for (int test = -5; test <= 5; test++)
				{
					if (depth_array[i * WIDTH + j - WIDTH + test] != 0)
					{
						flag = 1;
						break;
					}
				}
				if (flag)
					continue;

				//����·�����
				int tot_check = 0;
				for (int checki = 0; checki != 5; checki++)
				{
					for (int checkj = -5; checkj != 5; checkj++)
					{
						if (depth_array[(i + checki) * WIDTH + (checkj + j) + 1])
						{
							tot_check++;
						}
					}
				}
				//printf("TOTCHECK %d\n", tot_check);
				if (tot_check < 15 || tot_check>45)
					break;
				five_x[5 - five] = j;
				five_y[5 - five] = i;
				five--;
				break;
			}
		}
		if (five == 0)
			break;
	}
	//int tolerance = 0;
	//���֮ǰ��ȡ��5�����Ƿ����Ҫ��
	if (five != 0)
	{
		for (int i = 0; i != 4; i++)
		{
			for (int j = i + 1; j != 5; j++)
			{
				//����֮�䲻��̫��
				if (five_x[i] - five_x[j] < 5 && five_x[i] - five_x[j] > -5)
				{
					five = 5;
					break;
				}
			}
		}
		//���Ϻ�����֮��ҲҪ��һ��������
		if ((five_y[4] - five_y[0]) * tmpz < 10000 || (five_y[4] - five_y[0]) * tmpz >80000)
		{
			five = 5;
		}
	}

	int state = 0;
	int last = 0;
	int bottom;
	//���ͻ��㡣
	for (int hi = 0; hi != HEIGHT; hi += 3)
	{
		int left = -1, right = -1;
		for (int wj = 0; wj != WIDTH; wj++)
		{
			if (depth_array[hi * WIDTH + wj])
			{
				left = wj;
				break;
			}
		}
		for (int wj = WIDTH - 1; wj >= 0; wj--)
		{
			if (depth_array[hi * WIDTH + wj])
			{
				right = wj;
				break;
			}
		}
		//printf("diff %d %d %d\n", right - left, last,right - left - last);
		if (right - left - last <= -4)
		{
			state++;
			if (state == 3 && rowWith[hi] < 0.8 * maxrow)
			{
				for (int tm = 0; tm != WIDTH; tm++)
				{
					depth_array[hi * WIDTH + tm] = 35535;
				}
				bottom = hi;
				break;
			}
		}
		else
			state = 0;
		last = right - left;
	}
	int maxheight = (bottom - top);
	int area = maxheight * maxrow;
	float lwratio = (float)maxheight / maxrow;
	float handPercent = (float)hand / area;
	//printf("area: %d\tmaxheight: %d\tmaxrow: %d\thand: %d\n %f %f\n", area, maxheight, maxrow, hand, lwratio, handPercent);

	//������ϣ����������
	if (five == 0 && lwratio > PALM_MIN_RATIO)
	{
		return GESTURE_PALM;
	}
	//if (state == 3)
	{
		//��⵽ͻ��㣬�Ϳ��Կ����Χ���������ռ��[todo]
	    if (lwratio < FIST_MAX_RATIO && lwratio > FIST_MIN_RATIO && handPercent > FIST_PERCENT)
		{
			//printf("fist!!!!\n");
			return GESTURE_FIST;
		}
	}

	//printf("nogesture");
	return GESTURE_NONE;
}