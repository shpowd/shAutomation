
#include "dynamiclinewidget.h"
#include "mainwidget.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QListView>
#include <QModelIndex>
#include <QStringListModel>
#include <QVBoxLayout>
#include <QPushButton>
#include <Qlabel.h>
#include <QScrollBar.h>
#include <qtextedit.h>
#include <qdialog.h>
#include <qcheckbox.h>

#include <algorithm>

MainWidget::MainWidget(QWidget *parent): QWidget(parent)
    , m_contentArea(new QWidget(this)){

    //키와, 그래프 종류 리스트 맵핑
    for (int i = 0; i < 9; ++i) {
        m_chartListMap.insert(tr("graph %1").arg(i + 1), DynamicLineChart);
    }

    //키로 정렬
    m_charts = m_chartListMap.keys();
    std::sort(m_charts.begin(), m_charts.end());
   
    m_contentArea->installEventFilter(this);

    setMinimumSize(800, 500);
    resize(1024, 768);

    //차트 0번 시작
    setActiveChart(m_chartListMap[ m_charts[0] ]);

    setMouseTracking(true);

    qApp->setApplicationDisplayName(tr("Qt Charts Example Gallery"));
}

void MainWidget::resizeEvent(QResizeEvent *)
{
    bool isHorizontal = width() >= height();
    if (!layout() || isHorizontal != m_isHorizontal)
        relayout(isHorizontal);


}

bool MainWidget::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::Resize && object == m_contentArea && m_activeWidget)
        m_activeWidget->resize(m_contentArea->size());
    return QObject::eventFilter(object, event);
}

void MainWidget::setActiveChart(chartlist chart){
    // We only keep one example alive at the time to save resources.
    // This also allows resetting the example by switching to another example and back.
    if (m_activeWidget) {
        m_activeWidget->setVisible(false);
        m_activeWidget->deleteLater();
    }

    switch ( chart ) {
    case DynamicLineChart:
        m_activeWidget = new DynamicLineWidget(m_contentArea);
        break;
    }

    m_activeWidget->load();
    m_activeWidget->resize(m_contentArea->size());
    m_activeWidget->setVisible(true);
}

void MainWidget::relayout(bool horizontal){
    m_isHorizontal = horizontal;

    delete layout();
    QBoxLayout *layout;

    // m_exampleMap의 키를 examples로 담기
    //m_charts = m_chartListMap.keys();
    //std::sort(m_charts.begin(), charts.end());  // 키 순서대로 정렬

    // 9개의 버튼 추가
    QHBoxLayout* buttonLayout = new QHBoxLayout;    
    for (int i = 0; i < 9; ++i) {
        QPushButton* button = new QPushButton(tr("버튼 %1").arg(i + 1), this);
        buttonLayout->addWidget(button);
        // 스타일 시트 적용
        // 스타일 시트 적용: Windows 기본 회색 배경과 경계선
        button->setStyleSheet(
            "QPushButton {"
            "    background-color: #f0f0f0;"  // Windows 기본 회색 배경
            "    border-radius: 10px;"         // 모서리 둥글게
            "    border: 2px solid #d0d0d0;"  // 윈도우 기본 경계선 색 (회색)
            "    color: black;"                // 글자 색
            "    padding: 10px;"               // 버튼 내 여백
            "    font-size: 14px;"             // 글자 크기
            "}"
            "QPushButton:hover {"
            "    background-color: #e0e0e0;"  // 마우스 오버 시 배경색 변경
            "    border: 2px solid #b0b0b0;"  // 마우스 오버 시 경계선 색 변경
            "}"
            "QPushButton:pressed {"
            "    background-color: #d0d0d0;"  // 눌렀을 때 배경색 변경
            "    border: 2px solid #a0a0a0;"  // 눌렀을 때 경계선 색 변경
            "}"
        );
        button->setMinimumHeight(50);
        button->setMinimumWidth(80);

        // 각 버튼 클릭 시 그래프를 변경하는 동작 연결
        connect(button, &QPushButton::clicked, this, [this, i]() {
            QString key = m_charts[i];  // i번째 예제에 해당하는 key 가져오기
            setActiveChart(m_chartListMap[ m_charts[ i ] ] );  // 해당하는 그래프 객체 설정
            });
    }

    // 우측에 16개의 on/off 텍스트 상자 추가
    QVBoxLayout* textBoxLayout = new QVBoxLayout;
    textBoxLayout->setSpacing(0);                       // 텍스트 상자 사이 간격을 없앰
    textBoxLayout->setContentsMargins(10, 10, 10, 10);  // 레이아웃의 여백을 없앰
    for (int i = 0; i < 16; ++i) {
        QTextEdit* textBox = new QTextEdit(this);
        textBox->setText("경보 Off");
        textBox->setStyleSheet("background-color: #f0f0f0; border: 1px solid #d0d0d0; border-radius: 5px;");
        textBox->setFixedHeight(35);  // 텍스트 상자의 세로 크기 설정
        textBox->setMinimumWidth(100);  // 텍스트 상자의 세로 크기 설정
        textBox->setAlignment(Qt::AlignCenter);
        // 텍스트 상자의 크기 고정
        textBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        // 텍스트 상자가 변경될 때마다 색상 변경
        connect(textBox, &QTextEdit::textChanged, this, [this, textBox]() {
            if (textBox->toPlainText() == "경보 On") {
                textBox->setStyleSheet("background-color: #FFA500; border: 1px solid #FF8C00; border-radius: 5px;");  // on일 때 밝은 주황색
            }
            else {
                textBox->setStyleSheet("background-color: #f0f0f0; border: 1px solid #d0d0d0; border-radius: 5px;");  // off일 때 기본 회색
            }
            });

        textBoxLayout->addWidget(textBox);
    }
    QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    textBoxLayout->addSpacerItem(spacer);

    // PNG 이미지 추가 (QPushButton으로 대체)
    QPushButton* imageButton = new QPushButton(this);
    QPixmap pixmap("./src/image.png");  // 이미지 경로
    imageButton->setIcon(QIcon(pixmap));  // 이미지를 아이콘으로 설정
    imageButton->setIconSize(QSize(200, 100));  // 이미지 크기 설정
    imageButton->setStyleSheet("border: none;");  // 버튼 경계선 없애기
    connect(imageButton, &QPushButton::clicked, this, &MainWidget::openSettingWindow);  // 클릭 시 설정 창 호출

    textBoxLayout->addWidget(imageButton);  // 이미지 버튼 추가


    QVBoxLayout* contentLayout = new QVBoxLayout;
    contentLayout->addWidget(m_contentArea);
    contentLayout->addLayout(buttonLayout);

    layout = new QHBoxLayout(this);
    layout->addLayout(contentLayout);
    layout->addLayout(textBoxLayout);

    setLayout(layout);
}













void MainWidget::openSettingWindow() {
    QDialog* settingDialog = new QDialog(this);
    settingDialog->setWindowTitle("Settings");
    
    QHBoxLayout* mainLayout = new QHBoxLayout(settingDialog);

    //버튼 추가 좌측
    QVBoxLayout* buttonLayout = new QVBoxLayout;
    QHBoxLayout* topButtonLayout = new QHBoxLayout;
    QHBoxLayout* bottomButtonLayout = new QHBoxLayout;
    for (int i = 0; i < 8; ++i) {
        QPushButton* button = new QPushButton(tr("그래프 %1").arg(i + 1), settingDialog);
        button->setMinimumSize(150, 150);  // 가로 100px, 세로 50px
        button->setStyleSheet(
            "QPushButton {"
            "    background-color: #f0f0f0;"  // Windows 기본 회색 배경
            "    border-radius: 10px;"         // 모서리 둥글게
            "    border: 2px solid #d0d0d0;"  // 윈도우 기본 경계선 색 (회색)
            "    color: black;"                // 글자 색
            "    padding: 10px;"               // 버튼 내 여백
            "    font-size: 14px;"             // 글자 크기
            "}"
            "QPushButton:hover {"
            "    background-color: #e0e0e0;"  // 마우스 오버 시 배경색 변경
            "    border: 2px solid #b0b0b0;"  // 마우스 오버 시 경계선 색 변경
            "}");  // 클릭 시 스타일 시트
        if (i < 4) {
            topButtonLayout->addWidget(button);
        }
        else if (4 <= i && i < 8) {
            bottomButtonLayout->addWidget(button);
        }
    }
    buttonLayout->addLayout(topButtonLayout);
    buttonLayout->addLayout(bottomButtonLayout);


    // 우측에 텍스트 상자 배치 (모드버스 프로토콜 설정)
    QVBoxLayout* textLayout = new QVBoxLayout;

    // Modbus ID 설정
    QLabel* protocolLabel = new QLabel("Modbus ID:", settingDialog);
    protocolLabel->setStyleSheet("font-weight: bold;");  // 레이블 스타일 설정
    textLayout->addWidget(protocolLabel);

    QTextEdit* protocolSettings = new QTextEdit(settingDialog);
    protocolSettings->setText("1");
    protocolSettings->setStyleSheet("background-color: #f0f0f0; border: 1px solid #d0d0d0; border-radius: 5px;");
    protocolSettings->setFixedHeight(100);
    textLayout->addWidget(protocolSettings);

    // IP 설정
    QLabel* addressLabel = new QLabel("Ethernet IP:", settingDialog);
    addressLabel->setStyleSheet("font-weight: bold;");
    textLayout->addWidget(addressLabel);

    QTextEdit* addressSettings = new QTextEdit(settingDialog);
    addressSettings->setText("192.168.0.100");
    addressSettings->setStyleSheet("background-color: #f0f0f0; border: 1px solid #d0d0d0; border-radius: 5px;");
    addressSettings->setFixedHeight(100);
    textLayout->addWidget(addressSettings);

    // 그래프 time 설정
    QLabel* graphTimeLabel = new QLabel("Graph Display Time: 1 Minute", settingDialog);
    graphTimeLabel->setStyleSheet("font-weight: bold;");
    textLayout->addWidget(graphTimeLabel);

    // QScrollBar를 사용하여 1분, 1시간, 1일, 1주일을 설정
    QScrollBar * graphTimeScrollBar = new QScrollBar(Qt::Horizontal, settingDialog);
    graphTimeScrollBar->setMinimum(0);
    graphTimeScrollBar->setMaximum(3);          // 4가지 옵션 (0: 1분, 1: 1시간, 2: 1일, 3: 1주일)
    graphTimeScrollBar->setPageStep(1);         // 한 번에 이동할 수 있는 범위
    graphTimeScrollBar->setStyleSheet(
        "QScrollBar:horizontal {"
        "    height: 40px;"                     // 스크롤바의 높이를 40px로 설정
        "    background-color: #d0d0d0;"        // 배경색을 회색빛으로 설정
        "    border: 2px solid #d0d0d0;"        // 테두리 색상 설정
        "    border-radius: 6px;"               // 둥근 모서리
        "}"
    );
    // 스크롤바 값 변경 시 텍스트 업데이트
    connect(graphTimeScrollBar, &QScrollBar::valueChanged, this, [ this, graphTimeScrollBar, graphTimeLabel ] () {
        int value = graphTimeScrollBar->value();
        QString text;
        switch ( value ) {
        case 0: text = "Graph Display Time: 1 Minute"; break;
        case 1: text = "Graph Display Time: 1 Hour"; break;
        case 2: text = "Graph Display Time: 1 Day"; break;
        case 3: text = "Graph Display Time: 1 Week"; break;
        }
        graphTimeLabel->setText(text);  // 하나의 QLabel에서 텍스트 업데이트
        });
    textLayout->addWidget(graphTimeScrollBar);


    // DB 저장 time 설정
    QLabel* saveTimeLabel = new QLabel("DB Save Frequency: 1 Minute", settingDialog);
    saveTimeLabel->setStyleSheet("font-weight: bold;");
    textLayout->addWidget(saveTimeLabel);

    // QScrollBar를 사용하여 1분, 1시간, 1일, 1주일을 설정
    QScrollBar* saveTimeScrollBar = new QScrollBar(Qt::Horizontal, settingDialog);
    saveTimeScrollBar->setMinimum(0);
    saveTimeScrollBar->setMaximum(3);  // 4가지 옵션 (0: 1분, 1: 1시간, 2: 1일, 3: 1주일)
    saveTimeScrollBar->setPageStep(1);  // 한 번에 이동할 수 있는 범위
    saveTimeScrollBar->setStyleSheet(
        "QScrollBar:horizontal {"
        "    height: 40px;"  // 스크롤바의 높이를 40px로 설정
        "    background-color: #d0d0d0;"  // 배경색을 회색빛으로 설정
        "    border: 2px solid #d0d0d0;"  // 테두리 색상 설정
        "    border-radius: 6px;"          // 둥근 모서리
        "}"
    );
    // 스크롤바 값 변경 시 텍스트 업데이트
    connect(saveTimeScrollBar, &QScrollBar::valueChanged, this, [this, saveTimeScrollBar, saveTimeLabel]() {
        int value = saveTimeScrollBar->value();
        QString text;
        switch (value) {
        case 0: text = "DB Save Frequency: 1 Minute"; break;
        case 1: text = "DB Save Frequency: 1 Hour"; break;
        case 2: text = "DB Save Frequency: 1 Day"; break;
        case 3: text = "DB Save Frequency: 1 Week"; break;
        }
        saveTimeLabel->setText(text);  // 하나의 QLabel에서 텍스트 업데이트
        });
    textLayout->addWidget(saveTimeScrollBar);



    // 저장 버튼
    QPushButton* saveButton = new QPushButton("Save", settingDialog);
    connect(saveButton, &QPushButton::clicked, settingDialog, &QDialog::accept);  // 저장 후 창 닫기
    textLayout->addWidget(saveButton);

    // 좌측 버튼 레이아웃과 우측 텍스트 레이아웃을 mainLayout에 추가
    mainLayout->addLayout(buttonLayout, 1);  // 버튼 레이아웃은 비율을 1로 설정
    mainLayout->addLayout(textLayout, 2);    // 텍스트 레이아웃은 비율을 2로 설정

    settingDialog->setLayout(mainLayout);  // 대화 상자의 레이아웃 설정

    settingDialog->exec();  // 대화 상자 실행
}


