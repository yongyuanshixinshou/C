#include "common.h"

static struct timeval start, finish;

void Start()
{
	gettimeofday(&start, NULL);	
}

void ReStart()
{
	gettimeofday(&start, NULL);
}

void Stop()
{
	gettimeofday(&finish, NULL);
}

float GetTimeSpan()
{
	float t = 1000000 * (finish.tv_sec - start.tv_sec) + finish.tv_usec - start.tv_usec;
	t /= 1000000;
	return t;
}
