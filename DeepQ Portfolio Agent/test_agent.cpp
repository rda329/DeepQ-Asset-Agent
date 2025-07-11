#include "test_agent.h"

void Test_Agent(const std::string saved_model_name, DQAgent& agent, Asset& asset, std::vector<StockData>& data)
{
	agent.LoadModel(saved_model_name); //loads pretrained policy
	agent.setEpsilon(-1); //Sets exploration coefficient to less then 0 for gaurenteed exploitation. 

	for (int i = 2; i < data.size(); i++)
	{
		asset.updateCurrentPrice(data, i);
		asset.updateAssetBalance(); //updates asset balance based on current price of stock
		asset.updateReturnForecast(data, i); //computes the log return based on current price, etc
		asset.updateHoldingNetWorth();

		std::vector<double> current_state = {
				data[i].close,              // definitions of members can be found in access_data.h
				data[i].high,
				data[i].low,
				data[i].open,
				data[i].volume,
				data[i].annual_meu,
				data[i].annual_sigma,
				data[i].local_return_mean,
				data[i].local_return_variance,
				data[i].usa_cpi,
				data[i].log_close_gold_future,
				data[i].unemployement_rate,
				asset.getReturnForecast() //gets the daily log return forecast
		};

		action_amount agent_action = agent.Act(current_state, asset); //the act method checks that there are sufficient shares / cash to buy or sell if not it will buy/sell the most it can

		//Buy / sell / hold shares +/- amount
		asset.Buy_Sell_Hold(agent_action.action, agent_action.amount);

		asset.updateCurrentPrice(data, i); //internally updates the price of the stock
		asset.updateAssetBalance(); //internally updates the asset balance
		asset.updateReturnForecast(data, i); //computes the log return
		asset.updateHoldingNetWorth(); //updates unspent cash + asset balance

		std::cout << "Trading day: " << i << " Holding Net Worth: " << asset.getHoldingNetWorth() << std::endl;
	}
}
