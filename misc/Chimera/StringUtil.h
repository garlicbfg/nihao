#pragma once
#include <vector>
#include <string>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>




struct StringUtil
{
	static std::vector<std::string> split(const std::string& input, const std::string& delims)
	{
		std::vector<std::string> ret;
		boost::split(ret, input, boost::is_any_of(delims));
		return ret;
	}
};

