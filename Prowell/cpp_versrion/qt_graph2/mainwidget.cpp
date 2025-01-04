
#include "dynamiclinewidget.h"
#include "mainwidget.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QListView>
#include <QModelIndex>
#include <QStringListModel>
#include <QVBoxLayout>
#include <QPushButton>
#include <qtextedit.h>

#include <algorithm>

MainWidget::MainWidget(QWidget *parent): QWidget(parent)
    , m_listView(new QListView(this))
    , m_listModel(new QStringListModel(this))
    , m_contentArea(new QWidget(this))                  {

    for (int i = 0; i < 9; ++i) {
        m_exampleMap.insert(tr("graph %1").arg(i + 1), DynamicLineChart);
    }

    QStringList examples = m_exampleMap.keys();
    std::sort(examples.begin(), examples.end());
    
    //m_listModel->setStringList(examples);
    //m_listView->setModel(m_listModel);
    //m_listView->setCurrentIndex(m_listModel->index(0));
    //m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //connect(m_listView->selectionModel(), &QItemSelectionModel::currentChanged, this,
    //    [this](const QModelIndex& index) {
    //        setActiveExample(m_exampleMap[m_listModel->data(index).toString()]);
    //    }
    //);

    m_contentArea->installEventFilter(this);

    setMinimumSize(800, 500);
    resize(1024, 768);


    setActiveExample(m_exampleMap[examples[0]]);

    setMouseTracking(true);

    qApp->setApplicationDisplayName(tr("Qt Charts Example Gallery"));
}

void MainWidget::resizeEvent(QResizeEvent *)
{
    bool isHorizontal = width() >= height();
    if (!layout() || isHorizontal != m_isHorizontal)
        relayout(isHorizontal);

    if (m_isHorizontal)
        m_listView->setMaximumHeight(QWIDGETSIZE_MAX);
    else
        m_listView->setMaximumHeight(height() / 3);
}

bool MainWidget::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::Resize && object == m_contentArea && m_activeWidget)
        m_activeWidget->resize(m_contentArea->size());
    return QObject::eventFilter(object, event);
}

void MainWidget::setActiveExample(Example example)
{
    // We only keep one example alive at the time to save resources.
    // This also allows resetting the example by switching to another example and back.
    if (m_activeWidget) {
        m_activeWidget->setVisible(false);
        m_activeWidget->deleteLater();
    }

    switch (example) {
    case DynamicLineChart:
        m_activeWidget = new DynamicLineWidget(m_contentArea);
        break;
    }

    m_activeWidget->load();
    m_activeWidget->resize(m_contentArea->size());
    m_activeWidget->setVisible(true);
}

void MainWidget::relayout(bool horizontal)
{
    m_isHorizontal = horizontal;

    delete layout();
    QBoxLayout *layout;

    // m_exampleMap의 키를 examples로 담기
    QStringList examples = m_exampleMap.keys();
    std::sort(examples.begin(), examples.end());  // 키 순서대로 정렬

    // 9개의 버튼 추가
    QHBoxLayout* buttonLayout = new QHBoxLayout;    
    for (int i = 0; i < 9; ++i) {
        QPushButton* button = new QPushButton(tr("Button %1").arg(i + 1), this);
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

        // 각 버튼 클릭 시 그래프를 변경하는 동작 연결
        connect(button, &QPushButton::clicked, this, [this, i, examples]() {
            QString key = examples[i];  // i번째 예제에 해당하는 key 가져오기
            setActiveExample(m_exampleMap[key]);  // 해당하는 그래프 객체 설정
            });
    }

    // 우측에 16개의 on/off 텍스트 상자 추가
    QVBoxLayout* textBoxLayout = new QVBoxLayout;
    textBoxLayout->setSpacing(0);                       // 텍스트 상자 사이 간격을 없앰
    textBoxLayout->setContentsMargins(10, 10, 10, 10);  // 레이아웃의 여백을 없앰
    for (int i = 0; i < 16; ++i) {
        QTextEdit* textBox = new QTextEdit(this);
        textBox->setText("Alarm Off");
        textBox->setStyleSheet("background-color: #f0f0f0; border: 1px solid #d0d0d0; border-radius: 5px;");
        textBox->setFixedHeight(26);  // 텍스트 상자의 세로 크기 설정
        textBox->setFixedWidth(100);  // 텍스트 상자의 세로 크기 설정
        textBox->setAlignment(Qt::AlignCenter);
        // 텍스트 상자의 크기 고정
        textBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        // 텍스트 상자가 변경될 때마다 색상 변경
        connect(textBox, &QTextEdit::textChanged, this, [this, textBox]() {
            if (textBox->toPlainText() == "Alarm On") {
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


    QVBoxLayout* contentLayout = new QVBoxLayout;
    contentLayout->addWidget(m_contentArea);
    contentLayout->addLayout(buttonLayout);

    layout = new QHBoxLayout(this);
    layout->addLayout(contentLayout);
    layout->addLayout(textBoxLayout);

    setLayout(layout);
}

