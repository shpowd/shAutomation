// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "dynamiclinewidget.h"

#include <QChart>
#include <QDateTime>
#include <QDateTimeAxis>
#include <QChartView>
#include <QRandomGenerator>
#include <QSplineSeries>
#include <QValueAxis>

DynamicLineWidget::DynamicLineWidget(QWidget* parent)
    : ContentWidget(parent)
{
    auto chart = new QChart;
    chart->setTitle("Dynamic Spline Chart");
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::AllAnimations);

    // x��� y�� ��ü �ʱ�ȭ
    m_axisX = new QDateTimeAxis;
    m_axisY = new QValueAxis;

    // Ÿ�̸� ����
    QObject::connect(&m_timer, &QTimer::timeout, this, &DynamicLineWidget::handleTimeout);
    m_timer.setInterval(1000);

    // QLineSeries ����
    m_series = new QLineSeries;
    QPen green(Qt::red);
    green.setWidth(3);
    m_series->setPen(green);

    // �ʱ� ������ ����Ʈ �߰� (QDateTime�� �и��� ������ ��ȯ)
    m_x = QDateTime::currentDateTime();  // m_x�� ���� �ð����� �ʱ�ȭ
    m_y = QRandomGenerator::global()->bounded(5) - 2.5;  // �������� m_y �� ����
    m_series->append(m_x.toMSecsSinceEpoch(), m_y);  // m_x�� �и��� ������ ��ȯ�Ͽ� �߰�

    chart->addSeries(m_series);

    // X��� Y�� ����
    chart->addAxis(m_axisX, Qt::AlignBottom);
    chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);

    // X�� ���� �� ���� ����
    m_axisX->setTickCount(11);
    m_axisX->setRange(QDateTime::currentDateTime().addSecs(-9), QDateTime::currentDateTime().addSecs(1));  // ���� �ð��� �������� 10�� ���� ����
    m_axisX->setFormat("hh:mm:ss");  // MM/dd/ss �������� ����
    m_axisX->setTitleText("Time");

    // Y�� ���� ����
    m_axisY->setRange(-5, 10);
    m_axisY->setTitleText("Y Value");

    // ��Ʈ �� ����
    createDefaultChartView(chart);

    // Ÿ�̸� ����
    m_timer.start();
}

void DynamicLineWidget::handleTimeout()
{
    qreal scroll_step = defaultChartView()->chart()->plotArea().width() / (m_axisX->tickCount() - 1);  // ��Ʈ ��ũ�� �Ÿ�
    qreal x_delta = (m_axisX->max().toSecsSinceEpoch() - m_axisX->min().toSecsSinceEpoch()) / (m_axisX->tickCount() - 1);  // X���� ���� �� ���� (�� ����)

    // m_x ���� �ð� �������� ������Ŵ (�� ������ ����)
    m_x = m_x.addSecs(x_delta);  // m_x ���� ���� �ð����� ����

    //////////////////////////////////////////////////////////////////////////
    m_y = QRandomGenerator::global()->bounded(5) - 2.5;  // �������� m_y �� ����

    // QDateTime�� �и��� ������ ��ȯ�Ͽ� �����͸� �߰�
    m_series->append(m_x.toMSecsSinceEpoch(), m_y);  // ���ο� ������ ����Ʈ �߰�

    // �����Ͱ� `tickCount()`��ŭ �߰��Ǹ� ��Ʈ�� �������� ��ũ��
    if (m_x >= m_axisX->min().addSecs((m_axisX->tickCount()-2) * x_delta)) {
        defaultChartView()->chart()->scroll(scroll_step, 0);  // ��Ʈ ���������� scroll_step ��ŭ ��ũ��
    }
}