#include <pybind11/pybind11.h> // pybind11 헤더 포함
#include <pybind11/embed.h>     // Python 임베딩을 위한 헤더
namespace py = pybind11;

int main() {
    py::scoped_interpreter guard{};  // Python 인터프리터 초기화

    // Python 코드 실행
    py::exec(R"(
        import sys
        print(sys.version)
    )");

    return 0;
}