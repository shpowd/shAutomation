#include "qt_graph.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qt_graph w;
    w.show();
    return a.exec();
}
