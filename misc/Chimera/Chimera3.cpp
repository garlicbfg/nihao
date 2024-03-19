#include "Chimera.h"
#include "FileUtil.h"
#include "StringUtil.h"
#include "Assert.h"
#include "MeanVarianceEstimator.h"
#include <sstream>
#include <iomanip>
#define _CRT_SECURE_NO_WARNINGS





void Chimera::run(const std::string& inputFile)
{
    std::vector<std::string> lines = FileUtil::readToLines(inputFile);

    std::vector<TransactionData> tdv;

    long long int minTimeStamp = LLONG_MAX;

    for (int i = 0; i < lines.size(); i++)
    {
        TransactionData td = parseLine(lines[i]);

        minTimeStamp = min(minTimeStamp, td.openTime);

        if (!td.cancelled)
            tdv.push_back(td);
    }

    auto func = [](const TransactionData& i, const TransactionData& j) { return (i.openTime < j.openTime); };
    std::sort(tdv.begin(), tdv.end(), func);

    long long int lastTimeStamp = -1;
    for (int i = 0; i < tdv.size(); i++)
    {
        tdv[i].openTime -= minTimeStamp;
        tdv[i].closeTime -= minTimeStamp;

        DBG(tdv[i].closeTime > tdv[i].openTime);
        DBG(tdv[i].openTime > lastTimeStamp);

        lastTimeStamp = tdv[i].openTime;
    }

    double sum = 0;
    for (int i = 0; i < tdv.size(); i++)
    {
        print(tdv[i]);
        sum += tdv[i].profit;
    }

    printf("total: %f\n", sum);

    FILE* fs = std::fopen("a.txt", "wt");
    FILE* f1 = std::fopen("a1.txt", "wt");
    FILE* f2 = std::fopen("a2.txt", "wt");
    for (int i = 0; i < tdv.size(); i++)
    {
        bool type2 = tdv[i].buy ^ tdv[i].usdFirst;
        fprintf(fs, "%f %f %d %f\n", tdv[i].openTime / 864., tdv[i].closeTime / 864., type2, tdv[i].profit);
        if(type2)
            fprintf(f1, "%f %f %d %f \n", tdv[i].openTime / 864., tdv[i].closeTime / 864., type2, tdv[i].profit);
        else
            fprintf(f2, "%f %f %d %f\n", tdv[i].openTime / 864., tdv[i].closeTime / 864., type2, tdv[i].profit);
    }
    std::fclose(fs);
    std::fclose(f1);
    std::fclose(f2);

    std::vector<TransactionData> tdv1, tdv2;
    for (int i = 0; i < tdv.size(); i++)
    {
        bool type2 = tdv[i].buy ^ tdv[i].usdFirst;
        if (type2)
            tdv1.push_back(tdv[i]);
        else
            tdv2.push_back(tdv[i]);
    }

    std::vector<std::vector<int>> indexes1 = makeClusters(tdv1);
    std::vector<std::vector<int>> indexes2 = makeClusters(tdv2);

    double totalProfit = 0;
    double totalNormProfit = 0;

    MeanVarianceEstimator est1, est2;

    printf("clusters: %d and %d.\n", indexes1.size(), indexes2.size());
    for (int i = 0; i < indexes1.size(); i++)
    {
        double sum = 0;
        double dealSize = 0;
        for (int j = 0; j < indexes1[i].size(); j++)
        {
            const TransactionData& curTd = tdv1[indexes1[i][j]];
            sum += curTd.profit;
            dealSize += curTd.size;
        }

        dealSize /= 0.01;
        printf("cluster A-%d. size %d. profit %f. normalized %f.\n", i, indexes1[i].size(), sum, sum / dealSize);

        totalProfit += sum;
        totalNormProfit += sum / dealSize;
        est1.add(sum);
        est2.add(sum / dealSize);
    }

    for (int i = 0; i < indexes2.size(); i++)
    {
        double sum = 0;
        double dealSize = 0;
        for (int j = 0; j < indexes2[i].size(); j++)
        {
            const TransactionData& curTd = tdv2[indexes2[i][j]];
            sum += curTd.profit;
            dealSize += curTd.size;
        }

        dealSize /= 0.01;
        printf("cluster B-%d. size %d. profit %f. normalized %f.\n", i, indexes2[i].size(), sum, sum / dealSize);

        totalProfit += sum;
        totalNormProfit += sum / dealSize;
        est1.add(sum);
        est2.add(sum / dealSize);
    }

    printf("Total profit %f. Total normalized profit %f\n", totalProfit, totalNormProfit);
    printf("E1 mean %f, variance %f, sem %f\n", est1.mean(), est1.unbiasedVariance(), est1.standardErrorOfMean());
    printf("E2 mean %f, variance %f, sem %f\n", est2.mean(), est2.unbiasedVariance(), est2.standardErrorOfMean());
}



Chimera::TransactionData Chimera::parseLine(const std::string& s) const
{
    std::vector<std::string> tokens = StringUtil::split(s, " \t");

    TransactionData data;

    data.id = std::stoll(tokens[0]);
    data.openTime = timestamp(tokens[1] + "-" + tokens[2]);

    if (tokens[3] == "buy")
        data.buy = true;
    else if (tokens[3] == "sell")
        data.buy = false;
    else
        throw __FUNCTION__;

    if (tokens[4] == "limit")
    {
        tokens.erase(tokens.begin() + 4);
        data.limit = true;
    }
    else
    {
        data.limit = false;
    }
    data.size = stod(tokens[4]);

    if (tokens[5] == "gbpusd")
    {
        data.currency = gbpusd;
        data.usdFirst = false;
    }
    else if (tokens[5] == "eurusd")
    {
        data.currency = eurusd;
        data.usdFirst = false;
    }
    else if (tokens[5] == "audusd")
    {
        data.currency = audusd;
        data.usdFirst = false;
    }
    else if (tokens[5] == "usdcad")
    {
        data.currency = usdcad;
        data.usdFirst = true;
    }
    else if (tokens[5] == "usdjpy")
    {
        data.currency = usdjpy;
        data.usdFirst = true;
    }
    else if (tokens[5] == "usdchf")
    {
        data.currency = usdchf;
        data.usdFirst = true;
    }
    else
    {
        throw __FUNCTION__;
    }

    data.openPrice= stod(tokens[6]);
    data.slField = stod(tokens[7]);
    data.tpField = stod(tokens[8]);
    data.closeTime = timestamp(tokens[9] + "-" + tokens[10]);
    data.closePrice = stod(tokens[11]);
    if (tokens[12] == "cancelled")
    {
        data.cancelled = true;
        data.comission = 0;
        data.taxes = 0;
        data.swap = 0;
        data.profit = 0;
    }
    else
    {
        data.cancelled = false;
        data.comission = stod(tokens[12]);
        data.taxes = stod(tokens[13]);
        data.swap = stod(tokens[14]);
        data.profit = stod(tokens[15]);
    }

    return data;
}



void Chimera::print(const TransactionData& data) const
{
    bool type2 = data.buy ^ data.usdFirst;



    printf("id: %lli, ts: %llu - %llu, %s %s, type2: %d, profit: %.2f\n",
        data.id, data.openTime / 864, data.closeTime / 864, data.getBuySell().c_str(),
        data.getCurrency().c_str(), type2, data.profit);
}



long long int Chimera::timestamp(const std::string& s) const
{
    std::tm t{};
    std::istringstream ss(s);
    ss >> std::get_time(&t, "%Y.%m.%d-%T");
    if (ss.fail())
        throw __FUNCTION__;
    return mktime(&t) - _timezone;
}



std::vector<std::vector<int>> Chimera::makeClusters(std::vector<TransactionData>& tdv) const
{
    std::vector<std::vector<int>> clusters;
    std::vector<long long int> minTimeStamp;
    std::vector<long long int> maxTimeStamp;
    
    for (int i = 0; i < tdv.size(); i++)
    {
         int currentclusterIndex = clusters.size() - 1;
        if (currentclusterIndex >= 0 && tdv[i].openTime <= maxTimeStamp[currentclusterIndex])
        {
            clusters[currentclusterIndex].push_back(i);
            maxTimeStamp[currentclusterIndex] = max(maxTimeStamp[currentclusterIndex], tdv[i].closeTime);
        }
        else
        {
            std::vector<int> indexes{ i };
            clusters.push_back(indexes);
            minTimeStamp.push_back(tdv[i].openTime);
            maxTimeStamp.push_back(tdv[i].closeTime);
        }
    }

    return clusters;
}



std::pair<int, int> Chimera::test() const
{
	return std::make_pair<int,int>(s.size(), s.size()/2);
}