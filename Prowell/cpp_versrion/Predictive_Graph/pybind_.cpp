#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <iostream>
#include <string>

namespace py = pybind11;

int test() {
    py::scoped_interpreter guard{};  // Python ���������� �ʱ�ȭ


    // Python �ڵ� ����
    py::exec(R"(
        import sys
        print("Hello from Python!")
        print("This is a test.", file=sys.stderr)
    )");



    return 0;
}
