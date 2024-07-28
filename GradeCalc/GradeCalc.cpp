//
// GradeCalc.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "TablePrinter.h"
#include "Utils.h"
#include <vector>
#include <string>


const double m_prob[12] = { 0.5, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.25, 0.20, 0.12, 0.05 };
const double n_prob[12] = { 0.65, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.40, 0.35, 0.27, 0.20 };
const double p_prob[12] = { 0.535, 0.335, 0.335, 0.335, 0.335, 0.335, 0.335, 0.335, 0.285, 0.235, 0.155, 0.085 };
const double n_drag1[12] = { 1, 4, 10, 25, 60, 130, 215, 405, 750, 1370, 2525, 4645 };



void printMainTable()
{
	TablePrinter tp;

	std::vector<std::string> row{ "Lev.", "Prob_M", "Prob_N", "Prob_P" };
	tp.addRow(row);

	for (int i = 0; i < 12; i++)
	{
		std::vector<std::string> row;
		char s[4][64];
		sprintf(s[0], "%i", i + 1);
		sprintf(s[1], "%.3f", m_prob[i]);
		sprintf(s[2], "%.3f", n_prob[i]);
		sprintf(s[3], "%.3f", p_prob[i]);

		for (int j = 0; j < 4; j++)
			row.push_back(std::string{ s[j] });
		tp.addRow(row);
	}
	printf("Main table: probabilities.\n");
	printf("%s", tp.getString().c_str());
}


double getPriceGrade(double price, double prob, double priceFail)
{
	return (price - priceFail*(1 - prob)) / prob;
}


void run(double m, double p, double n, double mir, double d5)
{
	printMainTable();
	printf("\n");
	printf("Using prices: mirazh - %f, podz - %f, neb - %f, mirozd - %f, drag5 - %f\n", m, p, n, mir, d5);
	printf("\n");

	double priceGrade[13];
	priceGrade[0] = 0.;

	TablePrinter tp;
	std::vector<std::string> row{ "Lev.", "Price_M", "Price_N", "Price_P", "Price_Mir", "Price_D", "Best", "Price" };
	tp.addRow(row);

	for (int i = 1; i < 13; i++)
	{
		double priceAlt[5];
		priceAlt[0] = -getPriceGrade(priceGrade[i - 1] + m, m_prob[i - 1], 0);
		priceAlt[1] = -getPriceGrade(priceGrade[i - 1] + m + n, n_prob[i - 1], 0);
		priceAlt[2] = -getPriceGrade(priceGrade[i - 1] + m + p, p_prob[i - 1], i>1 ? priceGrade[i - 2]: 0);
		priceAlt[3] = -getPriceGrade(priceGrade[i - 1] + m + mir, 1 / n_drag1[i - 1], priceGrade[i - 1]);
		priceAlt[4] = -(priceGrade[i - 1] + m + d5 / 60. * n_drag1[i - 1]);

		std::string str = "";
		int k = Utils::maxElement(priceAlt, 5);
		switch (k)
		{
			case 0: str = "Mirazh";
				break;
			case 1: str = "Neb";
				break;
			case 2: str = "Podz";
				break;
			case 3: str = "Mirozd";
				break;
			case 4: str = "Drag";
				break;
			default:
			throw __FUNCTION__;
		}

		priceGrade[i] = -priceAlt[k];

		std::vector<std::string> row;
		char s[8][64];
		sprintf(s[0], "%i", i);
		sprintf(s[1], "%.0f", -priceAlt[0]);
		sprintf(s[2], "%.0f", -priceAlt[1]);
		sprintf(s[3], "%.0f", -priceAlt[2]);
		sprintf(s[4], "%.0f", -priceAlt[3]);
		sprintf(s[5], "%.0f", -priceAlt[4]);
		sprintf(s[6], "%s", str.c_str());
		sprintf(s[7], "%.0f", -priceAlt[k]);

		for (int j = 0; j < 8; j++)
			row.push_back(std::string{ s[j] });
		tp.addRow(row);
	}


	printf("Prices.\n");
	printf("%s", tp.getString().c_str());


}


int main(int argc, char* argv[])
{
	if (argc != 6)
	{
		run(8.19, 900, 900, 900, 50000);
		return -1;
	}
	else
	{
		run(atof(argv[1]), atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]));
	}
	
	return 0;

	_getch();
}

