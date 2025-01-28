#ifndef PYBIND_H
#define PYBIND_H

#include <vector>

// SARIMA ������ ���� �Լ� ����
std::vector<double> predict_sarima(const std::vector<double>& data, int steps, int seasonal_period = 12);

#endif // PYBIND_H
