double calculateHeartRateAverage(const std::vector<double>& heartRateData) {
    size_t totalSamples = heartRateData.size();

    if (totalSamples == 0) {
        return 0.0;
    }

    double sum = 0.0;
    for (double heartRate : heartRateData) {
        sum += heartRate;
    }

    double average = sum / totalSamples;

    return average;
}