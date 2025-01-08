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

#include <deque>
std::deque<QPointF> m_data;

//DynamicLineWidget::DynamicLineWidget(QWidget* parent)
//    : ContentWidget(parent)
//{
//    auto chart = new QChart;
//    chart->setTitle("Dynamic Spline Chart");
//    chart->legend()->hide();
//    chart->setAnimationOptions(QChart::AllAnimations);
//
//    // x축과 y축 객체 초기화
//    m_axisX = new QDateTimeAxis;
//    m_axisY = new QValueAxis;
//
//    // 타이머 연결
//    QObject::connect(&m_timer, &QTimer::timeout, this, &DynamicLineWidget::handleTimeout);
//    m_timer.setInterval(1000);
//
//    // QLineSeries 생성
//    m_series = new QLineSeries;
//    QPen green(Qt::red);
//    green.setWidth(3);
//    m_series->setPen(green);
//
//    // 초기 데이터 포인트 추가 (QDateTime을 밀리초 단위로 변환)
//    m_x = QDateTime::currentDateTime();  // m_x를 현재 시간으로 초기화
//    m_y = QRandomGenerator::global()->bounded(5) - 2.5;  // 무작위로 m_y 값 생성
//    m_series->append(m_x.toMSecsSinceEpoch(), m_y);  // m_x를 밀리초 단위로 변환하여 추가
//
//    chart->addSeries(m_series);
//
//    // X축과 Y축 설정
//    chart->addAxis(m_axisX, Qt::AlignBottom);
//    chart->addAxis(m_axisY, Qt::AlignLeft);
//    m_series->attachAxis(m_axisX);
//    m_series->attachAxis(m_axisY);
//
//    // X축 범위 및 눈금 설정
//    m_axisX->setTickCount(11);
//    m_axisX->setRange(QDateTime::currentDateTime().addSecs(-9), QDateTime::currentDateTime().addSecs(1));  // 현재 시간을 기준으로 10초 범위 설정
//    m_axisX->setFormat("hh:mm:ss");  // MM/dd/ss 형식으로 변경
//    m_axisX->setTitleText("Time");
//
//    // Y축 범위 설정
//    m_axisY->setRange(-5, 10);
//    m_axisY->setTitleText("Y Value");
//
//    // 차트 뷰 생성
//    createDefaultChartView(chart);
//
//    // 타이머 시작
//    m_timer.start();
//}




DynamicLineWidget::DynamicLineWidget(QWidget * parent)
    : ContentWidget(parent)
{
    auto chart = new QChart;
    chart->setTitle("Dynamic Spline Chart");
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::AllAnimations);

    // x축과 y축 객체 초기화
    m_axisX = new QDateTimeAxis;
    m_axisY = new QValueAxis;

    // QLineSeries 생성
    m_series = new QLineSeries;
    QPen pen(Qt::blue);
    pen.setWidth(3);
    m_series->setPen(pen);

    // 초기 사용자 데이터 세트 추가
    initializeData();

    chart->addSeries(m_series);

    // X축과 Y축 설정
    chart->addAxis(m_axisX, Qt::AlignBottom);
    chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);

    // X축 범위 및 눈금 설정
    m_axisX->setTickCount(11);
    QDateTime now = QDateTime::currentDateTime();
    m_axisX->setRange(now.addSecs(-10), now);
    m_axisX->setFormat("hh:mm:ss");
    m_axisX->setTitleText("Time");

    // Y축 범위 설정
    m_axisY->setRange(-10, 10);
    m_axisY->setTitleText("Y Value");

    // 차트 뷰 생성
    createDefaultChartView(chart);

    // 타이머를 사용하여 1초마다 새 데이터 추가
    connect(&m_timer, &QTimer::timeout, this, &DynamicLineWidget::addNewData);
    m_timer.setInterval(1000); // 1초 간격
    m_timer.start();
}

// 초기 데이터 세트를 생성하는 함수
void DynamicLineWidget::initializeData() {
    QDateTime now = QDateTime::currentDateTime();
    for ( int i = -10; i < 0; ++i ) {
        qint64 timestamp = now.addSecs(i).toMSecsSinceEpoch();
        double value = QRandomGenerator::global()->bounded(20) - 10; // -10 ~ 10 사이의 임의 값
        m_data.append(QPointF(timestamp, value));
        m_series->append(timestamp, value);
    }
}

// 새로운 데이터를 추가하는 함수
void DynamicLineWidget::addNewData() {
    // 새 데이터 추가
    QDateTime now = QDateTime::currentDateTime();
    qint64 timestamp = now.toMSecsSinceEpoch();
    double value = QRandomGenerator::global()->bounded(20) - 10;

    m_data.append(QPointF(timestamp, value));
    m_series->append(timestamp, value);

    // 오래된 데이터 제거
    if ( m_data.size() > 20 ) {
        m_data.removeFirst();
        m_series->removePoints(0, 1); // 가장 오래된 데이터 제거
    }

    // X축 및 시리즈 업데이트
    updateSeries(timestamp);
}

// QLineSeries에 데이터를 업데이트하는 함수
void DynamicLineWidget::updateSeries(qint64 newTimestamp) {
    if ( m_data.isEmpty() ) {
        return; // 데이터가 없으면 업데이트하지 않음
    }

    // X축의 현재 범위
    QDateTime oldMinTime = m_axisX->min();
    QDateTime oldMaxTime = m_axisX->max();

    // 새로운 X축 범위 계산
    QDateTime newMaxTime = QDateTime::fromMSecsSinceEpoch(newTimestamp);
    QDateTime newMinTime = newMaxTime.addSecs(-10); // 10초 범위 유지

    // 이전 X축 최대값과 새로운 X축 최대값의 차이로 스크롤 거리 계산
    qreal scrollStep = ( newMaxTime.toMSecsSinceEpoch() - oldMaxTime.toMSecsSinceEpoch() )
        / (double)( oldMaxTime.toMSecsSinceEpoch() - oldMinTime.toMSecsSinceEpoch() )
        * defaultChartView()->chart()->plotArea().width();

    // X축 범위 설정 및 스크롤
    m_axisX->setRange(newMinTime, newMaxTime);
    if ( scrollStep > 0 ) {
        defaultChartView()->chart()->scroll(scrollStep, 0); // 오른쪽으로 부드럽게 스크롤
    }
}


//void DynamicLineWidget::handleTimeout()
//{
//    qreal scroll_step = defaultChartView()->chart()->plotArea().width() / (m_axisX->tickCount() - 1);  // 차트 스크롤 거리
//    qreal x_delta = (m_axisX->max().toSecsSinceEpoch() - m_axisX->min().toSecsSinceEpoch()) / (m_axisX->tickCount() - 1);  // X축의 눈금 간 간격 (초 단위)
//
//    // m_x 값을 시간 기준으로 증가시킴 (초 단위로 증가)
//    m_x = m_x.addSecs(x_delta);  // m_x 값을 현재 시간으로 증가
//
//    //////////////////////////////////////////////////////////////////////////
//    m_y = QRandomGenerator::global()->bounded(5) - 2.5;  // 무작위로 m_y 값 생성
//
//    // QDateTime을 밀리초 단위로 변환하여 데이터를 추가
//    m_series->append(m_x.toMSecsSinceEpoch(), m_y);  // 새로운 데이터 포인트 추가
//
//    // 데이터가 `tickCount()`만큼 추가되면 차트를 수평으로 스크롤
//    if (m_x >= m_axisX->min().addSecs((m_axisX->tickCount()-2) * x_delta)) {
//        defaultChartView()->chart()->scroll(scroll_step, 0);  // 차트 오른쪽으로 scroll_step 만큼 스크롤
//    }
//}