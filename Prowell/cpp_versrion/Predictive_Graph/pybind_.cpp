#include <iostream>
#include <vector>
#include <cmath>

// SARIMA ������ ���� �Լ� ����
std::vector<double> predict_sarima(const std::vector<double>& data, int steps, int seasonal_period = 12) {
    std::vector<double> predictions;

    if (data.size() < seasonal_period) {
        std::cerr << "Insufficient data for SARIMA model" << std::endl;
        return predictions;
    }

    // 1. ����(differencing)
    std::vector<double> differenced_data;
    for (size_t i = 1; i < data.size(); ++i) {
        differenced_data.push_back(data[i] - data[i - 1]);
    }

    // ������ ���� (�������� ���� �� ������ ����)
    std::vector<double> seasonal_differenced_data;
    for (size_t i = seasonal_period; i < data.size(); ++i) {
        seasonal_differenced_data.push_back(data[i] - data[i - seasonal_period]);
    }

    if (seasonal_differenced_data.size() < seasonal_period) {
        std::cerr << "Insufficient seasonal differenced data for SARIMA model" << std::endl;
        return predictions;
    }

    // 2. AR(1), MA(1) �� ������ �� ����
    double phi = 0.5;  // AR(1) ���
    double theta = 0.5;  // MA(1) ���
    double seasonal_phi = 0.6;  // ������ AR ���
    double seasonal_theta = 0.4;  // ������ MA ���
    double last_value = data[data.size() - 1];

    // 3. ���� ����
    for (int i = 0; i < steps; ++i) {
        // �Ϲ� AR(1) + MA(1)
        double ar_term = phi * differenced_data.back();
        double ma_term = theta * differenced_data[differenced_data.size() - 1];

        // ������ AR(1) + MA(1)
        double seasonal_ar_term = seasonal_phi * seasonal_differenced_data.back();
        double seasonal_ma_term = seasonal_theta * seasonal_differenced_data[seasonal_differenced_data.size() - 1];

        // ������ ���
        double prediction = last_value + ar_term + ma_term + seasonal_ar_term + seasonal_ma_term;
        predictions.push_back(prediction);

        // �������� ������Ʈ
        last_value = prediction;
    }

    return predictions;
}
