#include <iostream>
#include <vector>
#include <cmath>

// SARIMA 예측을 위한 함수 정의
std::vector<double> predict_sarima(const std::vector<double>& data, int steps, int seasonal_period = 12) {
    std::vector<double> predictions;

    if (data.size() < seasonal_period) {
        std::cerr << "Insufficient data for SARIMA model" << std::endl;
        return predictions;
    }

    // 1. 차분(differencing)
    std::vector<double> differenced_data;
    for (size_t i = 1; i < data.size(); ++i) {
        differenced_data.push_back(data[i] - data[i - 1]);
    }

    // 계절성 차분 (계절성이 있을 때 데이터 차분)
    std::vector<double> seasonal_differenced_data;
    for (size_t i = seasonal_period; i < data.size(); ++i) {
        seasonal_differenced_data.push_back(data[i] - data[i - seasonal_period]);
    }

    if (seasonal_differenced_data.size() < seasonal_period) {
        std::cerr << "Insufficient seasonal differenced data for SARIMA model" << std::endl;
        return predictions;
    }

    // 2. AR(1), MA(1) 및 계절성 모델 설정
    double phi = 0.5;  // AR(1) 계수
    double theta = 0.5;  // MA(1) 계수
    double seasonal_phi = 0.6;  // 계절성 AR 계수
    double seasonal_theta = 0.4;  // 계절성 MA 계수
    double last_value = data[data.size() - 1];

    // 3. 예측 생성
    for (int i = 0; i < steps; ++i) {
        // 일반 AR(1) + MA(1)
        double ar_term = phi * differenced_data.back();
        double ma_term = theta * differenced_data[differenced_data.size() - 1];

        // 계절성 AR(1) + MA(1)
        double seasonal_ar_term = seasonal_phi * seasonal_differenced_data.back();
        double seasonal_ma_term = seasonal_theta * seasonal_differenced_data[seasonal_differenced_data.size() - 1];

        // 예측값 계산
        double prediction = last_value + ar_term + ma_term + seasonal_ar_term + seasonal_ma_term;
        predictions.push_back(prediction);

        // 예측값을 업데이트
        last_value = prediction;
    }

    return predictions;
}
