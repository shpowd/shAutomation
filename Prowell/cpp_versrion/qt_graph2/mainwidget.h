﻿// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QHash>
#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QListView)
QT_FORWARD_DECLARE_CLASS(QStringListModel)

class ContentWidget;

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *) override;
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    enum Example {
        DynamicLineChart
    };

    void setActiveExample(Example example);
    void relayout(bool horizontal);
    void openSettingWindow();


    QWidget *m_contentArea = nullptr;
    ContentWidget *m_activeWidget = nullptr;
    QHash<QString, Example> m_exampleMap;
    bool m_isHorizontal = false;
};

#endif