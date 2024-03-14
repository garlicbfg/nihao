#pragma once
#include <string>
#include <vector>



class TablePrinter
{

public:


	void TablePrinter::reset()
	{
		str.clear();
		strLen.clear();
	}


	TablePrinter& addRow(const std::vector<std::string>& row)
	{
		const int size = row.size();
		if (size > strLen.size())
			strLen.resize(size, 0);
		for (int j = 0; j < size; j++)
		{
			const int len = row[j].size();
			if (len> strLen[j])
				strLen[j] = len;
		}
		str.push_back(row);
		return *this;
	}


	/*
	TablePrinter& addRow(const std::vector<double>& row)
	{
		std::vector<std::string> s;
		for (int j = 0; j < row.size(); j++)
			s.push_back(format("%f", row[j]));
		return addRow(s);
	}*/



	std::string getString() const
	{
		std::string ret = "";

		for (int i = 0; i < str.size(); i++)
		{
			for (int j = 0; j < str[i].size(); j++)
			{
				ret.append(str[i][j]);
				ret.append(strLen[j] + 1 - str[i][j].size(), ' ');
			}
			ret.append("\n");
		}

		return ret;
	}

private:
	std::vector<int> strLen;
	std::vector<std::vector<std::string>> str;
};
