#pragma once

#include <string>
#include <vector>

struct StockData {
    double close; //closing price of asset
    double high; //daily high of asset
    double low; //daily low of asset
    double open; //market open price of asset
    double volume; //daily volume traded of asset
    double annual_meu; //average annual log_daily_return of asset *for GBM log return forecase computation
    double annual_sigma; //average annual log_daily_return of asset *for log return forecast GBM computation
    double local_return_mean; //average daily log return of asset in prior 20 trading days
    double local_return_variance; //variance of daily log return of asset in prior 20 trading days
    double usa_cpi; //monthly US CPI *obtained from FRED data source
    double log_close_gold_future; //log closing price of gold future GC=F
    double unemployement_rate; //monthly unemployement rates in USA
    int date_index; //Trading day index in data set
};

//Implementation found in access_data.cpp
std::vector<StockData> readCSV(std::string& filename); 
