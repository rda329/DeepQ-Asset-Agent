#include <torch/torch.h>
#include <iostream>
#include "access_data.h"
#include "train_agent.h"
#include "Agent.h"
#include "test_agent.h"
#include "to_csv.h"
#include "compute_average_var.h"


int main() {

	int n_episodes = 10000; //number of simulations
	std::vector<StockData> data = readCSV();

	//DNN architecture & hyperparams
	Hyperparams params;
	params.state_size = 13;
	params.action_size = 3;
	params.h1 = 4;
	params.memory_capacity = 200;
	params.epsilon = 1.0;
	params.epsilon_min = 0.01;
	params.epsilon_decay = 0.995;
	params.learning_rate = 0.001;
	params.target_update_freq = 5;


	double uninvested_cash = 1000.00;

	//Agent Interacting with Env.
	DQAgent AAPL_agent(params);

	
	Asset AAPL("AAPL", 0, 10000); //unspent cash & number of shares owned will be random. set during training of agent
	
	//std::vector<double> return_per_sim;

	//return_per_sim = Train_Agent(AAPL_agent,data, AAPL, 1000, 20, 0.02, "trained_model");

	//make a implemntation function where epsilon is set to 0 at start and loads trained model
	//Dont include the .pt in the model filename
	//Test_Agent("test", AAPL_agent, AAPL, data); //i is harded coded to start at 2 not 0

	//Extracting Data to CSV

	// Save with a custom column name
	/*
	bool success = saveVectorToCSV(
		return_per_sim,
		"returns_training",
		"../../../Data",
		"returns",  // Column name
		','              // Delimiter
	);
	

	if (success) {
		std::cout << "CSV saved successfully!" << std::endl;
	}
	else {
		std::cerr << "Failed to save CSV." << std::endl;
	}
	*/

	//hyper param tuning

	std::vector<double> zero_penalty;
	std::vector<double> neg_penalty;
	std::vector<double> returns_mean;
	std::vector<double> returns_var;
	
	for (int zero_return_penalty = 0; zero_return_penalty < 101; zero_return_penalty++) {
		for (int neg_return_penalty = 0; neg_return_penalty < 101; neg_return_penalty++)
		{
			std::vector<double> returns = Train_Agent(AAPL_agent, data, AAPL, 300, 30, 0.02, "trained_model", zero_return_penalty, neg_return_penalty);
			double mean = computeAverage(returns);
			double variance = calculateVariance(returns);

			zero_penalty.push_back(zero_return_penalty);
			neg_penalty.push_back(neg_return_penalty);
			returns_mean.push_back(mean);
			returns_var.push_back(variance);
			std::cout << "Zero_Return_Penalty " << zero_return_penalty << " Neg. Return Penalty " << neg_return_penalty << std::endl;
		}
	}

	std::unordered_map<std::string, std::vector<double>> data2 = 
	{
	{"Zero_Return_Penalty_Param", zero_penalty},
	{"Neg_Return_Penalty_Param", neg_penalty},
	{"Return Mean", returns_mean},
	{"Return Variance", returns_var}
	};

	bool success = saveVectorToCSV(data2, "Penalty_Param_Tuning", "../../../Data");

	if (success) {
		std::cout << "CSV saved successfully!" << std::endl;
	}
	else {
		std::cerr << "Failed to save CSV." << std::endl;
	}


	return 0;
}

//path to executable C:\Users\rubie\Desktop\DeepQ Portfolio Agent\DeepQ Portfolio Agent\build\Debug\
