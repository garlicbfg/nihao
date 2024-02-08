#pragma once

#include <fstream>
#include <vector>
#include <string>





struct FileUtil
{
	static std::vector<std::string> readToLines(const std::string& fileName)
	{
		std::ifstream file(fileName);

		std::vector<std::string> ret;
		std::string s;

		while (std::getline(file, s))
		{
			if (s.length())
			{
				ret.push_back(s);
			}
		}

		return ret;
	}
};