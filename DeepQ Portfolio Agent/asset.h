#pragma once
#include <iostream>
#include "access_data.h"


/* 
Implementation is coded for portfolio of 5 companies
Apple Inc. - AAPL
Microsoft Corp. - MSFT
Johnson & Johnson - JNJ
Exxon Mobil - XOM
Visa Inc - V
*/

//Asset class 
class Asset
{
    //class methods are defined explicitly in asset.cpp
public:
    //Constructors/Destructors
    Asset();
    Asset(const std::string& id, double shares,  double unspent_cash);
    Asset(Asset& asset);

    // Getters
    std::string getId() const;
    const double getCurrentPricePerShare() const;
    const double getNumberOfSharesOwned() const;
    const double getReturnForecast() const;
    const double getPricePerShare(std::vector<StockData>& data, int date_index);
    const double getAssetBalance();
    const double getUnspentCash();
    const double getHoldingNetWorth();

    // Setters
    
    void setUnspentCash(double amount);
    void setNumberOfSharesOwned(double amount);

    //Update member variables as time changes
    void Buy_Sell_Hold(const int action, const double num_shares); //updates number of shares owned based on buy,sell action
    void updateReturnForecast(std::vector<StockData>& data, int date_index);
    void updateCurrentPrice(std::vector<StockData>& data, int date_index);
    void updateAssetBalance(); //sets current value of holding by asset
    void updateUnspentCash(double inflation_rate); //updates unspent cash based on a devaluation rate
    void updateHoldingNetWorth();

private:
    std::string id;
    double unspent_cash;
    double number_of_shares_owned;

    //updated based on time
    double close_price_per_share; 
    double log_return_forecast;
    double asset_balance;
    double holding_net_worth;
};