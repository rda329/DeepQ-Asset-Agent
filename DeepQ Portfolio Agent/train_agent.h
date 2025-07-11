#pragma once
#include <vector>
#include "asset.h"
#include "Agent.h"
#include <random>
#include "access_data.h"

std::vector<double> Train_Agent(DQAgent& agent, std::vector<StockData>& data, Asset& asset, int n_episodes, int ep_duration, double cash_devaluation_rate, std::string trained_model_file_name, double zero_return_penalty, double neg_return_penalty);

//zero_return penalty and neg_return_penalty are coded to be passed as positive integers