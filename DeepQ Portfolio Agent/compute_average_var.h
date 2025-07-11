#pragma once

#include <vector>
#include <numeric>
#include <cmath>

// Function to compute average of a vector
double computeAverage(const std::vector<double>& values) {
	if (values.empty()) return 0.0;
	return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

//Computes variance of vector
double calculateVariance(const std::vector<double>& data) 
{
    if (data.empty()) {
        return 0.0; // or you could throw an exception for empty input
    }

    // Calculate the mean (average)
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    double mean = sum / data.size();

    // Calculate the sum of squared differences from the mean
    double squared_diff_sum = 0.0;
    for (double value : data) {
        squared_diff_sum += std::pow(value - mean, 2);
    }

    // Calculate variance (population variance)
    double variance = squared_diff_sum / data.size();

    return variance;
}