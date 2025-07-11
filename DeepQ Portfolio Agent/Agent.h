#pragma once

#include <iostream>
#include "torch/torch.h"
#include <deque>
#include "asset.h"

struct action_amount
{
	int action; //Buy, Sell, Hold
	double amount; //Number of shares
};


//This is a structure storing the results from each episode of training
struct env_result
{
	std::vector<double> state;
	action_amount action;
	double reward;
	std::vector<double> next_state;
	bool done;
};

//These are the hyperparameters of the DNN model for the DQN
struct Hyperparams
{
	int state_size; //number of nuerons in input layer
	int action_size; //number of nuerons in output layer
	int h1; //number of nuerons in hidden layer 1
	std::deque<env_result> memory;
	int memory_capacity; //max number of memories stored
	double gamma; //Discount factor : How much do we value long term rewards constant 
	double epsilon; //Exploration coefficient
	double epsilon_decay;
	double epsilon_min;
	double learning_rate;
	int target_update_freq; //how many steps before the weights of the target are copied from the online policy model 
};

class DQAgent : public torch::nn::Module
{
public:
	DQAgent(Hyperparams params);

	//set epsilon
	void setEpsilon(double epsilon); 

	void BuildModel(); //constructs DNN online and target model 
	void UpdateTargetModel();

	void DQAgent::Fit(torch::Tensor& state, torch::Tensor& target, int epoch); ///Trains model_1
	void Remember(env_result eps_result);
	const action_amount Act(std::vector<double> state, Asset& asset);
	void Replay(int batch_size);

	void DQAgent::SaveModel(const std::string& filename);
	void DQAgent::LoadModel(const std::string& filename);


	const Hyperparams& getParams();

private:
	/*
	current state
	current reward
	current action
	next state
	done : param to let us know if episode has ended
	*/

	Hyperparams params;

	//Model member variables
	torch::nn::Sequential online_model;
	torch::nn::Sequential target_model; 
	std::unique_ptr<torch::optim::Adam> optimizer_ptr;
	torch::nn::MSELoss loss_function;

	int update_counter;
};