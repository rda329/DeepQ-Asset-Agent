//The purpose of this source file is to provide functions that retrieve data
#include "access_data.h"
#include <fstream>
#include <sstream>
#include <iostream>


//This function creates a vector container of StockData structs. One for each row read in the csv.

std::vector<StockData> readCSV(std::string& filename) {
    std::string full_path = "../../../Data/"+filename+".csv";
    std::vector<StockData> data;
    std::ifstream file(full_path);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Error opening file.\n";
        return data;
    }

    // Skip header
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string item;
        StockData row;

        std::getline(ss, item, ','); row.close = std::stod(item);
        std::getline(ss, item, ','); row.high = std::stod(item);
        std::getline(ss, item, ','); row.low = std::stod(item);
        std::getline(ss, item, ','); row.open = std::stod(item);
        std::getline(ss, item, ','); row.volume = std::stod(item);
        std::getline(ss, item, ','); row.annual_meu = std::stod(item);
        std::getline(ss, item, ','); row.annual_sigma = std::stod(item);
        std::getline(ss, item, ','); row.local_return_mean = std::stod(item);
        std::getline(ss, item, ','); row.local_return_variance = std::stod(item);
        std::getline(ss, item, ','); row.date_index = std::stoi(item);
        std::getline(ss, item, ','); row.usa_cpi = std::stod(item);
        std::getline(ss, item, ','); row.log_close_gold_future = std::stod(item);
        std::getline(ss, item, ','); row.unemployement_rate = std::stod(item);
        

        data.push_back(row);
    }

    file.close();
    return data;
}
