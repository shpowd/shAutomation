#ifndef PYBIND_H
#define PYBIND_H

#include <vector>

// ARIMA 예측을 위한 함수 선언
std::vector<double> predict_arima(const std::vector<double>& data, int steps);

#endif // PYBIND_H
