#include <pybind11/pybind11.h> // pybind11 ��� ����
#include <pybind11/embed.h>     // Python �Ӻ����� ���� ���
namespace py = pybind11;

int main() {
    py::scoped_interpreter guard{};  // Python ���������� �ʱ�ȭ

    // Python �ڵ� ����
    py::exec(R"(
        import sys
        print(sys.version)
    )");

    return 0;
}