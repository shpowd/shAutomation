
//#include <pybind11/embed.h>
#include "predictive_graph.h"
#include <QApplication>

//namespace py = pybind11;

int main(int argc, char* argv[]){

    //py::scoped_interpreter guard{}; // Python 인터프리터 전역 초기화

    QApplication a(argc, argv);
    Predictive_Graph w;
    w.show();
    return a.exec();
}