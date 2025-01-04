#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMessageBox>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);  // Qt 애플리케이션 초기화

    QWidget window;  // 기본 창을 생성
    window.setWindowTitle("Qt Basic Window");  // 창 제목 설정
    window.resize(400, 300);  // 창 크기 설정 (너비 400, 높이 300)

    QPushButton* button = new QPushButton("Click Me", &window);  // 버튼 생성
    button->setGeometry(150, 100, 100, 30);  // 버튼 위치 및 크기 설정

    // 버튼 클릭 시 메시지 박스 표시
    QObject::connect(button, &QPushButton::clicked, [&]() {
        QMessageBox::information(&window, "Button Clicked", "You clicked the button!");
        });

    window.show();  // 창을 화면에 표시

    return app.exec();  // 이벤트 루프 시작
}
