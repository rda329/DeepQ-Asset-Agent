#include "asset.h"
#include "cmath"
#include <random>


//Constructor
Asset::Asset()
{
}

Asset::Asset(const std::string& id, double shares, double unspent_cash)
    : id(id), close_price_per_share(0.0), number_of_shares_owned(shares), unspent_cash(unspent_cash), 
    log_return_forecast(0.0), asset_balance(0.0), holding_net_worth(0.0)
{
}

Asset::Asset(Asset& asset)
{
    id = asset.id;
    close_price_per_share = asset.close_price_per_share;
    number_of_shares_owned = asset.number_of_shares_owned;
    log_return_forecast = asset.log_return_forecast;
    unspent_cash = asset.unspent_cash;
    holding_net_worth = asset.holding_net_worth;
    asset_balance = asset.asset_balance;

}

// Getters
std::string Asset::getId() const
{
    return id;
}

const double Asset::getCurrentPricePerShare() const
{
    return close_price_per_share;
}

const double Asset::getNumberOfSharesOwned() const
{
    return number_of_shares_owned;
}

const double Asset::getReturnForecast() const
{
    return log_return_forecast;
}

const double Asset::getPricePerShare(std::vector<StockData>& data, int date_index)
{
    StockData row = data[date_index];
    

    return row.close;
}


const double Asset::getAssetBalance()
{
    return asset_balance;
}

const double Asset::getUnspentCash()
{
    return unspent_cash;
}

const double Asset::getHoldingNetWorth()
{
    return holding_net_worth;
}

// Setters
void Asset::setNumberOfSharesOwned(double amount)
{
    this->number_of_shares_owned = amount;
}

void Asset::setUnspentCash(double amount)
{
    this->unspent_cash = amount;
}

//Updates
void Asset::Buy_Sell_Hold(const int action, const double num_shares = 0)
{
    if (action == 1) //Buy action
    {
        this->number_of_shares_owned += num_shares;

    }
    else if (action == 2) //Sell action
    {
        this->number_of_shares_owned -= num_shares;
    }
    else
    {
        return;
    }
}

void Asset::updateCurrentPrice(std::vector<StockData>& data, int date_index)
{
    close_price_per_share = this->getPricePerShare(data,date_index);
}

//Method updates return log return forecast +1 day 
void Asset::updateReturnForecast(std::vector<StockData>& data, int date_index)
{
    StockData row = data[date_index];

    double annual_meu;
    double annual_sigma;

    annual_meu = row.annual_meu;
    annual_sigma = row.annual_sigma;

    // === Forecast log return ===
    double delta_t = 1.0 / 252.0;  // daily step
    static std::default_random_engine generator;
    static std::normal_distribution<double> distribution(0.0, 1.0);
    double Z = distribution(generator);

    //logathrimic SDE
    double log_return = (annual_meu - 0.5 * annual_sigma * annual_sigma) * delta_t
        + annual_sigma * std::sqrt(delta_t) * Z;

    this->log_return_forecast = log_return;
}

void Asset::updateAssetBalance()
{
    this->asset_balance = close_price_per_share * number_of_shares_owned;
}

void Asset::updateUnspentCash(double devaluation_rate) //expects as decimal
{
    unspent_cash *= (1 - devaluation_rate);
}

void Asset::updateHoldingNetWorth()
{
    this->holding_net_worth = unspent_cash + asset_balance;
}