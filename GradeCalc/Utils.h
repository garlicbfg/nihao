#pragma once

#include <valarray>
#include <vector>
#include <set>
#include <iostream>



namespace Utils
{
	const std::string streetName[5] { "preflop", "flop", "turn", "river", "all streets"};

	inline double logSafe(double a)
	{
		return (a != 0) ? log(a) : -730;
	}
	int maxElement(const std::vector<double>& v);
	int maxElement(const std::valarray<double>& v);
	int maxElement(const double* arr, const int len);
	std::pair<int, int> maxElementCount(const std::valarray<double>& v);
	std::pair<int, int> maxElementCount(const double* arr, const int len);
	void clear(double* arr, const int len);
	void normalize(double* arr, const int len);
	void normalizeSafe(double* arr, const int len);
	void fill(double* arr, const int len, double val);
	bool isValue(double* arr, const int len, double val);
	void checkVariableRange(const int tested, const int min, const int max);
	std::string boolString(bool b);
	void normalDistribution(double input1, double input2, double& output1, double& output2);

	template< class R>
	inline double sum(R* arr, const int len)
	{
		R norm = 0;
		for (int i = 0; i < len; i++)
			norm += arr[i];
		return norm;
	}


	template< class NUM>
	void makeCDFfromPDF(NUM* cdf, const NUM* pdf, int len)
	{
		cdf[0] = pdf[0];
		for (int i = 1; i < len; i++)
			cdf[i] = cdf[i - 1] + pdf[i];
	}


	template< class T>
	bool isEqual(std::valarray<T> l, std::valarray<T> r)
	{
		if (l.size() != r.size())
			return false;
		for (int i = 0; i < l.size(); i++)
		{
			if (l[i] != r[i])
				return false;
		}
		return true;
	}

	std::string now();
};
