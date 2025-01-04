// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef DYNAMICLINEWIDGET_H
#define DYNAMICLINEWIDGET_H

#include "contentwidget.h"

#include <QTimer>
#include <QDateTime>
#include <QDateTimeAxis>

QT_FORWARD_DECLARE_CLASS(QLineSeries)
QT_FORWARD_DECLARE_CLASS(QValueAxis)

class DynamicLineWidget : public ContentWidget
{
    Q_OBJECT
public:
    explicit DynamicLineWidget(QWidget *parent = nullptr);

public slots:
    void handleTimeout();

private:
    QLineSeries *m_series = nullptr;
    QDateTimeAxis *m_axisX = nullptr;
    QValueAxis *m_axisY = nullptr;
    QTimer m_timer;
    QStringList m_titles;
    qreal m_step = 0.;
    QDateTime  m_x;
    qreal m_y = 1;
};

#endif
