#pragma once
#include "qt_monitoring.h"

#include <QtWidgets/QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QSpacerItem>
#include <QVector>
#include <QGroupBox>
#include <QTimer>
#include <QIcon>
#include <QModbusTcpClient>
#include <QModbusDataUnit>
#include <QFile>
#include <QTextStream>
#include <QCloseEvent>
#include <QPointer>


constexpr int NUM_REGISTERS = 14;  // 14개의 Holding Register 값
constexpr int NUM_SLAVES = 16;     // 16개의 Modbus 슬레이브 장치

class qt_window : public QWidget{
    Q_OBJECT


public:
    explicit qt_window(QWidget* parent = nullptr);
    ~qt_window();


private:
    // UI 요소 정의
    void initMainUI();                      // UI 구성 메서드
        // "Main" 창
    QPushButton* openSiteSettingButton;     // 현장 설정 버튼
    QTableWidget* mainTableWidget;          // Main Window 전용 테이블
    QPushButton* mainPage1Button;           // Main 1번 페이지 버튼
    QPushButton* mainPage2Button;           // Main 2번 페이지 버튼
    int currentMainPage = 1;                // siteSetupWindow의 현재 페이지

        // "현장 설정" 창
    QWidget* siteSettingWindow;             // 현장 설정 창
    QTableWidget* siteSettingTableWidget;   // 현장 설정 전용 테이블
    QPushButton* siteSetting1Button;        // 현장 설정 1번 페이지 버튼
    QPushButton* siteSetting2Button;        // 현장 설정 1번 페이지 버튼
    int currentSiteSettingpPage = 1;        // siteSetupWindow의 현재 페이지


        // "통신 설정" 창
    QMap<int, QMap<QString, QString>> settings;
    void saveSettingsToCSV();
    void loadSettingsFromCSV();


        // "Monitoring" 창
    QMap<int, QPointer<MonitoringWindow>> monitoringWindows; // 여러 개의 Monitoring 창을 관리할 맵




    // Modbus TCP 클라이언트 및 타이머
    QVector<QModbusTcpClient*> clients;
    QVector<QTimer*> pollTimers;
    QVector<QLineEdit*> ipInputs;
    QVector<QLineEdit*> portInputs;
    QVector<QPushButton*> connectButtons;
    QVector<QPushButton*> disconnectButtons;
    QVector<QVector<QLabel*>> dataDisplays; // 14개 Holding Register 값을 저장하는 2D 벡터
    QVector<QLabel*> statusDisplays;


private slots:

    // UI 관련 슬롯
        // "Main" 창
    void mainWindowDisplayPage(int mainPageIndex);                  // 페이지 전환 함수

    // "현장 설정" 창
    void openSiteSettingWindow();                                   // Main -> siteSetupWindow 슬롯
    void siteSettingWindowDisplayPage(int siteSettingpageIndex);    // "현장 설정" 페이지 전환 함수
    void siteSettingWindowSave();                                   // 셋업 윈도우 설정 저장 함수
    void refreshMainWindow();                                       // siteSetupWindow가 닫힐 때 Main Window 업데이트

    // "통신 설정" 창
    void openCommSettingsWindow(int monitoringIndex); // 통신 설정창 여는 함수


    // "Monitoring" 창
    void openMonitoringWindow(int monitoringIndex); // ✅ Monitoring 창을 여는 함수



    // Modbus 통신 관련 슬롯
    void connectToSlave(int index);
    void disconnectFromSlave(int index);
    void readFromSlave(int index);
    void updateStatus(int index, QModbusDevice::State state);


signals:


protected:


};
