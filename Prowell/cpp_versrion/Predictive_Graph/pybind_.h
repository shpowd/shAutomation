#ifndef PYBIND_H
#define PYBIND_H

#include <vector>

// ARIMA ������ ���� �Լ� ����
std::vector<double> predict_arima(const std::vector<double>& data, int steps);

#endif // PYBIND_H
