#pragma once

#include <iostream>
#include <string>
#include "Agent.h"

std::vector<double> Test_Agent(std::string _saved_model_name, DQAgent agent, Asset asset, std::vector<StockData>& data);