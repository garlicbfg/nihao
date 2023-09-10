#include "Utils.h"

#include <algorithm>
#define _USE_MATH_DEFINES
#include "Math.h"
#include <ctime>





int Utils::maxElement(const std::vector<double>& v)
{
	return std::distance(v.begin(), std::max_element(v.begin(), v.end()));
}


int Utils::maxElement(const std::valarray<double>& v)
{
	//calc index of max of result;
	int index = 0;
	double maxresult = v[0];
	for (int i = 1; i < v.size(); i++)
	if (v[i] > maxresult)
	{
		index = i;
		maxresult = v[i];
	}
	return index;
}


int Utils::maxElement(const double* arr, const int len)
{
	//calc index of max of result;
	int index = 0;
	double maxresult = arr[0];
	for (int i = 1; i < len; i++)
	if (arr[i] > maxresult)
	{
		index = i;
		maxresult = arr[i];
	}
	return index;
}


std::pair<int, int> Utils::maxElementCount(const std::valarray<double>& v)
{
	//calc index of max of result;
	int index = 0;
	double maxresult = v[0];
	int copies = 1;
	for (int i = 1; i < v.size(); i++)
	{
		if (v[i] > maxresult)
		{
			index = i;
			maxresult = v[i];
			copies = 1;
		}
		else if (v[i] == maxresult)
			copies++;
	}
	return std::make_pair(index, copies);
}


std::pair<int, int> Utils::maxElementCount(const double* arr, const int len)
{
	//calc index of max of result;
	int index = 0;
	double maxresult = arr[0];
	int copies = 1;
	for (int i = 1; i < len; i++)
	{
		if (arr[i] > maxresult)
		{
			index = i;
			maxresult = arr[i];
			copies = 1;
		}
		else if (arr[i] == maxresult)
			copies++;
	}
	return std::make_pair(index, copies);
}


void Utils::clear(double* arr, const int len)
{
	for (int i = 0; i < len; i++)
		arr[i] = 0;
}


void Utils::fill(double* arr, const int len, double val)
{
	for (int i = 0; i < len; i++)
		arr[i] = val;
}


void Utils::normalize(double* arr, const int len)
{
	double norm = sum(arr, len);
	if (norm)
	for (int i = 0; i < len; i++)
		arr[i] /= norm;

}



void Utils::normalizeSafe(double* arr, const int len)
{
	double norm = sum(arr, len);
	if (norm)
	for (int i = 0; i < len; i++)
		arr[i] /= norm;
	else
		fill(arr, len, 1. / len);
}



bool Utils::isValue(double* arr, const int len, double val)
{
	for (int i = 0; i < len; i++)
	if (arr[i] != val)
		return false;
	return true;
}


void Utils::checkVariableRange(const int tested, const int min, const int max)
{
	if ((tested<min) || (tested>max))
	{
		printf("Wrong label!\n");
		throw __FUNCTION__;
	}
};


std::string Utils::boolString(bool b)
{
	return b ? "true" : "false";
}



void Utils::normalDistribution(double input1, double input2, double& output1, double& output2)
{
	double b = sqrt(-2 * log(1 - input1));
	double a = 2 * M_PI*(1 - input2);
	output1 = b*cos(a);
	output2 = b*sin(a);
}



std::string Utils::now()
{
	time_t t = time(0);
	struct tm * now = localtime(&t);
	char buf[80];
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", now);
	return std::string(buf);
}

