#include "train_agent.h"

std::vector<double> Train_Agent(DQAgent agent, std::vector<StockData>& data, Asset asset, int n_episodes, int ep_duration, double cash_devaluation_rate, std::string trained_model_file_name, double zero_return_penalty, double neg_return_penalty)
{	
	Asset copy_of_asset = asset;
	
	std::vector<double> return_on_investment;
	int min = 0;
	int max;
	if (data.size() < ep_duration)
	{
		max = data.size() - ep_duration - 2; //-2 because we index at 0 also we need to have a + 1 avaliable date
	}
	else
	{
		max = 0; //avoid negative upper bound
	}
	

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(min, max);
	
	for (int i = 0; i < n_episodes; i++)
	{
		asset = copy_of_asset; //At the start of sim, resets asset to starting condition
		
		//Setting up the starting conditions for each simulation
		double reward = 0;
		bool done = false;

		int day_one = dis(gen); //randomly generating a starting date 

		asset.updateCurrentPrice(data, day_one); 
		asset.updateAssetBalance(); //updates asset balance based on current price of stock
		asset.updateReturnForecast(data, day_one); //computes the log return based on current price, etc
		asset.updateHoldingNetWorth();

		double init_holding_net_worth = asset.getHoldingNetWorth();

		std::vector<double> current_state = {
				data[day_one].close,              // definitions of members can be found in access_data.h
				data[day_one].high,              
				data[day_one].low,
				data[day_one].open,
				data[day_one].volume,
				data[day_one].annual_meu,
				data[day_one].annual_sigma,
				data[day_one].local_return_mean,
				data[day_one].local_return_variance,
				data[day_one].usa_cpi,
				data[day_one].log_close_gold_future,
				data[day_one].unemployement_rate,
				asset.getReturnForecast() //gets the daily log return forecast
		};

		
		//Interacting with env. to get experiences
		for (int a = day_one; a < (ep_duration+day_one-1); a++) //Simulation ends aftrer "ep_duration" trading days 
		{
			//std::cout << "Day one " << day_one << " End of ep: " << ep_duration + day_one << " Total Trading days in data: " << data.size() << std::endl;
			if (done == false)
			{
				action_amount agent_action = agent.Act(current_state, asset); //the act method checks that there are sufficient shares / cash to buy or sell if not it will buy/sell the most it can

				//Buy / sell / hold shares +/- amount
				asset.Buy_Sell_Hold(agent_action.action, agent_action.amount);

				asset.updateCurrentPrice(data, a); //internally updates the price of the stock
				asset.updateAssetBalance(); //internally updates the asset balance
				asset.updateReturnForecast(data, a); //computes the log return
				asset.updateHoldingNetWorth(); //updates unspent cash + asset balance

				//End Condition for simulation, if asset balance reach 0 or neg.
				if (asset.getHoldingNetWorth() <= 0)
				{
					done = true;
					if (asset.getHoldingNetWorth() == 0)
					{
						reward = -1*(zero_return_penalty/100); //Penalty for 0 risk
					}
					else if (asset.getHoldingNetWorth() < 0)
					{
						reward = ((asset.getHoldingNetWorth() - init_holding_net_worth) / init_holding_net_worth) - (neg_return_penalty/100); //penalty for going neg. return - penalty coefficient
					}
				}
				else
				{
					reward = (asset.getHoldingNetWorth() - init_holding_net_worth)/init_holding_net_worth; //reward is equal to return
				}

				asset.updateReturnForecast(data, a + 1); //updates the member val for return forecast for next state
				asset.updateUnspentCash(cash_devaluation_rate); //penalty for unspent cash

				std::vector<double> next_state =
				{
					data[a+1].close,              // definitions of members can be found in access_data.h
					data[a+1].high,
					data[a+1].low,
					data[a+1].open,
					data[a+1].volume,
					data[a+1].annual_meu,
					data[a+1].annual_sigma,
					data[a+1].local_return_mean,
					data[a+1].local_return_variance,
					data[a+1].usa_cpi,
					data[a+1].log_close_gold_future,
					data[a+1].unemployement_rate,
					asset.getReturnForecast() //gets the daily log return forecast
				};

				env_result memory;
				memory.action = agent_action;
				memory.state = current_state;
				memory.next_state = next_state;
				memory.done = done;

				agent.Remember(memory); //stores experience struct in the params member of agent object

				current_state = next_state; //updates current state for next eps				
			}
			//std::cout << "Simulation: " << i << " Episode: " << a-day_one << "/" << ep_duration << " Holding Net Worth: " << asset.getHoldingNetWorth() << "\n" << std::endl;
		}

		return_on_investment.push_back((asset.getHoldingNetWorth() - init_holding_net_worth)/init_holding_net_worth); //for each simulation we save worth net

		Hyperparams params = agent.getParams();
		if (params.memory.size() > 0)
		{
			//Once we are done collecting experiences we train the DNN for better decision making
			agent.Replay(10); // batch size number of experiences needed to train DNN
		}
		

		if (i % 100 == 0)
		{
			//std::cout << "Sim: " << i << " Asset Balance: " << asset.getAssetBalance() << std::endl;
			agent.SaveModel(trained_model_file_name); //periodically saving model
		}
	}

	agent.SaveModel(trained_model_file_name);
	return return_on_investment;
}