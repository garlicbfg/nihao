#pragma once
#include <vector>
#include <math.h>




struct MeanVarianceEstimator
{
	double val{ 0 }, val2{ 0 }, weight{ 0 };


	void reset()
	{
		val = 0;
		val2 = 0;
		weight = 0;
	}


	void add(const double x)
	{
		val += x;
		val2 += x*x;
		weight += 1.;
	}


	void add(const double x, const double w)
	{
		val += x*w;
		val2 += x*x*w;
		weight += w;
	}


	void add(const std::vector<double>& v)
	{
		for (int i = 0; i < v.size(); i++)
			add(v[i]);
	}


	double mean() const
	{
		return val / weight;
	}


	double variance() const
	{
		double m = mean();

		return (val2 - m * val) / weight;
	}



	double unbiasedVariance() const
	{
		double m = mean();

		return (val2 - m * val) / (weight - 1);
	}



	double standardErrorOfMean() const
	{
		return sqrt(unbiasedVariance() / weight);
	}
};
