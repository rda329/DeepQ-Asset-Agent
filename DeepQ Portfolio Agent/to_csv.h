#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <algorithm>

namespace fs = std::filesystem;

// Original version: Single vector to single column CSV
template <typename T>
bool saveVectorToCSV(
    const std::vector<T>& data,
    const std::string& filename,
    const fs::path& folderPath,
    const std::string& columnName,
    char delimiter = ','
) {
    // Create directory if it doesn't exist
    try {
        fs::create_directories(folderPath);
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return false;
    }

    // Construct full file path
    fs::path fullPath = folderPath / (filename + ".csv");

    // Open file for writing
    std::ofstream outFile(fullPath);
    if (!outFile.is_open()) {
        std::cerr << "Error opening file: " << fullPath << std::endl;
        return false;
    }

    // Write column header
    outFile << columnName << "\n";

    // Write each element in a new row
    for (const auto& item : data) {
        outFile << item << "\n";
    }

    outFile.close();
    return true;
}

// Overloaded version: Map of vectors to multi-column CSV
template <typename T>
bool saveVectorToCSV(
    const std::unordered_map<std::string, std::vector<T>>& data,
    const std::string& filename,
    const fs::path& folderPath,
    char delimiter = ','
) {
    // Check if data is empty
    if (data.empty()) {
        std::cerr << "Error: No data provided" << std::endl;
        return false;
    }

    // Check vector sizes
    size_t expected_size = data.begin()->second.size();
    for (const auto& [key, vec] : data) {
        if (vec.size() != expected_size) {
            std::cerr << "Error: Vector size mismatch for column '" << key << "'" << std::endl;
            return false;
        }
    }

    // Create directory
    try {
        fs::create_directories(folderPath);
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Directory error: " << e.what() << std::endl;
        return false;
    }

    // Create file path
    fs::path fullPath = folderPath / (filename + ".csv");

    // Open file
    std::ofstream outFile(fullPath);
    if (!outFile) {
        std::cerr << "File open error: " << fullPath << std::endl;
        return false;
    }

    // Write headers
    bool first = true;
    for (const auto& [key, _] : data) {
        outFile << (first ? "" : std::string(1, delimiter)) << key;
        first = false;
    }
    outFile << "\n";

    // Write data
    for (size_t i = 0; i < expected_size; ++i) {
        first = true;
        for (const auto& [_, vec] : data) {
            outFile << (first ? "" : std::string(1, delimiter)) << vec[i];
            first = false;
        }
        outFile << "\n";
    }

    return true;
}

// Explicit instantiations for single vector version
template bool saveVectorToCSV<int>(const std::vector<int>&, const std::string&, const fs::path&, const std::string&, char);
template bool saveVectorToCSV<double>(const std::vector<double>&, const std::string&, const fs::path&, const std::string&, char);
template bool saveVectorToCSV<std::string>(const std::vector<std::string>&, const std::string&, const fs::path&, const std::string&, char);

// Explicit instantiations for map version
template bool saveVectorToCSV<double>(const std::unordered_map<std::string, std::vector<double>>&, const std::string&, const fs::path&, char);