#pragma once

#include <valarray>
#include <vector>
#include <set>



namespace Utils
{
	template <class T>
	__forceinline T max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}
	
};
