#include<string.h>
#include<stdio.h>
void erosion(unsigned short* array, int width, int height)
{
	unsigned short* tmp = new unsigned short[width*height];
	memset(tmp, 0, width*height*sizeof(short));

	for (int hi = 1;hi != height-1;hi++)
	{
		for (int wj = 1;wj != width-1; wj++)
		{
			if (array[hi * width + wj])
			{
				tmp[(hi + 1) * width + wj] = 65535;
				tmp[(hi - 1) * width + wj] = 65535;
				tmp[hi * width + wj + 1] = 65535;
				tmp[hi * width + wj - 1] = 65535;
				tmp[hi * width + wj] = 65535;
			}
		}
	}
	memcpy(array, tmp, width*height*sizeof(short));
	delete(tmp);
}

void dilation(unsigned short* array, int width, int height)
{
	unsigned short* tmp = new unsigned short[width*height];
	memset(tmp, 0, width*height*sizeof(unsigned short));
	memcpy(tmp, array, width*height*sizeof(short));
	for (int hi = 1;hi != height - 1;hi++)
	{
		for (int wj = 1;wj != width - 1; wj++)
		{
			if (!array[hi * width + wj])
			{
				tmp[(hi + 1) * width + wj] = 0;
				tmp[(hi - 1) * width + wj] = 0;
				tmp[hi * width + wj + 1] = 0;
				tmp[hi * width + wj - 1] = 0;
				tmp[hi * width + wj] = 0;
			}
		}
	}

	memcpy(array, tmp, width*height*sizeof(short));
	delete(tmp);
}