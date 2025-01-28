#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include "pybind_.h"
#include <vector>
#include <iostream>

namespace py = pybind11;

// 전역 Python 인터프리터 초기화
static py::scoped_interpreter guard{};
static py::module prediction_module = py::module::import("prediction");
static py::object arima_predict = prediction_module.attr("arima_predict");

std::vector<double> predict_arima(const std::vector<double>& data, int steps) {
    try {
        if (prediction_module.is_none() || arima_predict.is_none()) {
            throw std::runtime_error("Python module or function is not properly initialized.");
        }

        return py::cast<std::vector<double>>(arima_predict(data, steps));
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
        throw;
    }
}
