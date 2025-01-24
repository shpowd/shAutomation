#include "predictive_graph.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    Predictive_Graph w;
    w.show();
    return a.exec();
}