#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMessageBox>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);  // Qt ���ø����̼� �ʱ�ȭ

    QWidget window;  // �⺻ â�� ����
    window.setWindowTitle("Qt Basic Window");  // â ���� ����
    window.resize(400, 300);  // â ũ�� ���� (�ʺ� 400, ���� 300)

    QPushButton* button = new QPushButton("Click Me", &window);  // ��ư ����
    button->setGeometry(150, 100, 100, 30);  // ��ư ��ġ �� ũ�� ����

    // ��ư Ŭ�� �� �޽��� �ڽ� ǥ��
    QObject::connect(button, &QPushButton::clicked, [&]() {
        QMessageBox::information(&window, "Button Clicked", "You clicked the button!");
        });

    window.show();  // â�� ȭ�鿡 ǥ��

    return app.exec();  // �̺�Ʈ ���� ����
}
