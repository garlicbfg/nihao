#pragma once
#include <string>
#include <vector>





class Chimera
{
private:
    enum CurrencyType{ gbpusd, eurusd, audusd, usdcad, usdjpy, usdchf};

    struct TransactionData
    {
    public:
        long long int id;
        long long int openTime, closeTime;
        bool buy;
        CurrencyType currency;
        bool usdFirst;
        bool cancelled;
        bool limit;
        double size, slField, tpField, openPrice, closePrice, comission, taxes, swap, profit;
        std::string t;

        std::string getBuySell() const
        {
            return buy ? "buy" : "sell";
        }

        std::string getCurrency() const
        {
            static const std::vector<std::string> names{ "gbpusd", "eurusd", "audusd", "usdcad", "usdjpy", "usdchf" };
            return names[currency];
        }
    };


public:
    void run(const std::string& inputFile);


private:
    TransactionData parseLine(const std::string& s) const;
    void print(const TransactionData& data) const;
    long long int timestamp(const std::string& s) const;
    std::vector<std::vector<int>> makeClusters(std::vector<TransactionData>& tdv) const;
};
