
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

    // m_exampleMap�� Ű�� examples�� ���
    QStringList examples = m_exampleMap.keys();
    std::sort(examples.begin(), examples.end());  // Ű ������� ����

    // 9���� ��ư �߰�
    QHBoxLayout* buttonLayout = new QHBoxLayout;    
    for (int i = 0; i < 9; ++i) {
        QPushButton* button = new QPushButton(tr("Button %1").arg(i + 1), this);
        buttonLayout->addWidget(button);
        // ��Ÿ�� ��Ʈ ����
        // ��Ÿ�� ��Ʈ ����: Windows �⺻ ȸ�� ���� ��輱
        button->setStyleSheet(
            "QPushButton {"
            "    background-color: #f0f0f0;"  // Windows �⺻ ȸ�� ���
            "    border-radius: 10px;"         // �𼭸� �ձ۰�
            "    border: 2px solid #d0d0d0;"  // ������ �⺻ ��輱 �� (ȸ��)
            "    color: black;"                // ���� ��
            "    padding: 10px;"               // ��ư �� ����
            "    font-size: 14px;"             // ���� ũ��
            "}"
            "QPushButton:hover {"
            "    background-color: #e0e0e0;"  // ���콺 ���� �� ���� ����
            "    border: 2px solid #b0b0b0;"  // ���콺 ���� �� ��輱 �� ����
            "}"
            "QPushButton:pressed {"
            "    background-color: #d0d0d0;"  // ������ �� ���� ����
            "    border: 2px solid #a0a0a0;"  // ������ �� ��輱 �� ����
            "}"
        );
        button->setMinimumHeight(50);

        // �� ��ư Ŭ�� �� �׷����� �����ϴ� ���� ����
        connect(button, &QPushButton::clicked, this, [this, i, examples]() {
            QString key = examples[i];  // i��° ������ �ش��ϴ� key ��������
            setActiveExample(m_exampleMap[key]);  // �ش��ϴ� �׷��� ��ü ����
            });
    }

    // ������ 16���� on/off �ؽ�Ʈ ���� �߰�
    QVBoxLayout* textBoxLayout = new QVBoxLayout;
    textBoxLayout->setSpacing(0);                       // �ؽ�Ʈ ���� ���� ������ ����
    textBoxLayout->setContentsMargins(10, 10, 10, 10);  // ���̾ƿ��� ������ ����
    for (int i = 0; i < 16; ++i) {
        QTextEdit* textBox = new QTextEdit(this);
        textBox->setText("Alarm Off");
        textBox->setStyleSheet("background-color: #f0f0f0; border: 1px solid #d0d0d0; border-radius: 5px;");
        textBox->setFixedHeight(26);  // �ؽ�Ʈ ������ ���� ũ�� ����
        textBox->setFixedWidth(100);  // �ؽ�Ʈ ������ ���� ũ�� ����
        textBox->setAlignment(Qt::AlignCenter);
        // �ؽ�Ʈ ������ ũ�� ����
        textBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        // �ؽ�Ʈ ���ڰ� ����� ������ ���� ����
        connect(textBox, &QTextEdit::textChanged, this, [this, textBox]() {
            if (textBox->toPlainText() == "Alarm On") {
                textBox->setStyleSheet("background-color: #FFA500; border: 1px solid #FF8C00; border-radius: 5px;");  // on�� �� ���� ��Ȳ��
            }
            else {
                textBox->setStyleSheet("background-color: #f0f0f0; border: 1px solid #d0d0d0; border-radius: 5px;");  // off�� �� �⺻ ȸ��
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

