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

    // x축과 y축 객체 초기화
    m_axisX = new QDateTimeAxis;
    m_axisY = new QValueAxis;

    // 타이머 연결
    QObject::connect(&m_timer, &QTimer::timeout, this, &DynamicLineWidget::handleTimeout);
    m_timer.setInterval(1000);

    // QLineSeries 생성
    m_series = new QLineSeries;
    QPen green(Qt::red);
    green.setWidth(3);
    m_series->setPen(green);

    // 초기 데이터 포인트 추가 (QDateTime을 밀리초 단위로 변환)
    m_x = QDateTime::currentDateTime();  // m_x를 현재 시간으로 초기화
    m_y = QRandomGenerator::global()->bounded(5) - 2.5;  // 무작위로 m_y 값 생성
    m_series->append(m_x.toMSecsSinceEpoch(), m_y);  // m_x를 밀리초 단위로 변환하여 추가

    chart->addSeries(m_series);

    // X축과 Y축 설정
    chart->addAxis(m_axisX, Qt::AlignBottom);
    chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);

    // X축 범위 및 눈금 설정
    m_axisX->setTickCount(11);
    m_axisX->setRange(QDateTime::currentDateTime().addSecs(-9), QDateTime::currentDateTime().addSecs(1));  // 현재 시간을 기준으로 10초 범위 설정
    m_axisX->setFormat("hh:mm:ss");  // MM/dd/ss 형식으로 변경
    m_axisX->setTitleText("Time");

    // Y축 범위 설정
    m_axisY->setRange(-5, 10);
    m_axisY->setTitleText("Y Value");

    // 차트 뷰 생성
    createDefaultChartView(chart);

    // 타이머 시작
    m_timer.start();
}

void DynamicLineWidget::handleTimeout()
{
    qreal scroll_step = defaultChartView()->chart()->plotArea().width() / (m_axisX->tickCount() - 1);  // 차트 스크롤 거리
    qreal x_delta = (m_axisX->max().toSecsSinceEpoch() - m_axisX->min().toSecsSinceEpoch()) / (m_axisX->tickCount() - 1);  // X축의 눈금 간 간격 (초 단위)

    // m_x 값을 시간 기준으로 증가시킴 (초 단위로 증가)
    m_x = m_x.addSecs(x_delta);  // m_x 값을 현재 시간으로 증가

    //////////////////////////////////////////////////////////////////////////
    m_y = QRandomGenerator::global()->bounded(5) - 2.5;  // 무작위로 m_y 값 생성

    // QDateTime을 밀리초 단위로 변환하여 데이터를 추가
    m_series->append(m_x.toMSecsSinceEpoch(), m_y);  // 새로운 데이터 포인트 추가

    // 데이터가 `tickCount()`만큼 추가되면 차트를 수평으로 스크롤
    if (m_x >= m_axisX->min().addSecs((m_axisX->tickCount()-2) * x_delta)) {
        defaultChartView()->chart()->scroll(scroll_step, 0);  // 차트 오른쪽으로 scroll_step 만큼 스크롤
    }
}