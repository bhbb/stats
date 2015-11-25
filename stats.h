#ifndef __STATS_H__
#define __STATS_H__

#include <iostream>
#include <string.h>
#include <math.h>
#include "util.h"


template<typename T>
double compute_sum(T *data, int start, int length)
{
	double ret = 0;
	int i;

	for(i=start;i<length;i++)
		ret += data[i];

	return ret;
}

template<typename T, typename W>
double compute_weigthed_mean(T *data, W *weigths, int start, int length)
{
	int i;
	double ret = 0;
	double divisor = 0;

	for(i=start;i<length;i++) {
		ret += weigths[i] * data[i];
		divisor += weigths[i];
	}

	return ret/divisor;
}

template<typename T>
double compute_mean(T *data, int start, int length)
{
	int i;
	int *weigths;

	weigths = (int *) xmalloc((length-start)*sizeof(int));
	
	for(i=start;i<length;i++)
		weigths[i] = 1;

	i = compute_weigthed_mean(data, weigths, start, length);

	free(weigths);
	return i;
}

template<typename T>
T compute_median(T *data, int start, int length)
{
	return data[((length-start)/2)+start];
}

template<typename T>
double compute_variance(T *data, int start, int length)
{
	int i;
	double ret = 0;
	T mean = compute_mean(data, start, length);

	for (i=start; i<length; i++) {
		ret += (data[i] - mean) * (data[i] - mean);
	}

	return ret/(length-1);
}

template<typename T>
double compute_standard_deviation(T *data, int start, int length)
{
	return sqrt(compute_variance(data, start, length));
}


#endif
