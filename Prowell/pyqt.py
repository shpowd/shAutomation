import sys
import random
import time
# from PyQt5.QtCore import QTimer, Qt
# from PyQt5.QtWidgets import (
#     QApplication, QMainWindow, QVBoxLayout, QHBoxLayout, QGroupBox, QLabel, QWidget, QDesktopWidget, QCheckBox
# )
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *

import pyqtgraph as pg


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        # 기본 UI 설정
        self.setWindowTitle("통합 그래프 및 데이터 모니터링")
        self.setGeometry(100, 100, 1400, 800)
        self.center()

        # 중앙 위젯
        central_widget = QWidget(self)
        self.setCentralWidget(central_widget)

        # 윈도우 레이아웃 설정
        window_layout = QHBoxLayout()
        central_widget.setLayout(window_layout)

        #좌측 컨트롤 영역
        control_layout = QVBoxLayout()
        window_layout.addLayout(control_layout)

        # 메인 레이아웃 설정
        main_layout = QVBoxLayout()
        window_layout.addLayout(main_layout)

        # 상단 그래프 영역
        graph_layout = QHBoxLayout()
        main_layout.addLayout(graph_layout)

        # 하단 데이터 표시 영역
        data_layout = QHBoxLayout()
        main_layout.addLayout(data_layout)

        # 그래프 초기화
        self.init_graphs(graph_layout)

        # 데이터 표시 초기화
        self.init_data_indicators(data_layout)

        # 컨트롤 영역 초기화
        self.init_control(control_layout)

        # 타이머 설정
        self.timer = QTimer()
        self.timer.setInterval(1000)  # 1초 간격으로 갱신
        self.timer.timeout.connect(self.update_data)
        self.timer.start()

        # 데이터를 저장하는 딕셔너리 초기화
        self.graph_data = {key: {"x": [], "y": []} for key in self.graphs.keys()}
        self.max_length = 100  # 최대 데이터 길이

    def center(self):
        qr = self.frameGeometry()
        cp = QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())

    def init_graphs(self, layout):
        """그래프 위젯 초기화 및 설정"""
        self.graphs = {}

        graph_layOut1 = QHBoxLayout()
        # Voltage1 그래프
        self.graphs["Voltage1"] = self.create_graph("Voltage1", "Power (W)")
        graph_layOut1.addWidget(self.graphs["Voltage1"])
        # Current1 그래프
        self.graphs["Current1"] = self.create_graph("Current1", "Energy (Wh)")
        graph_layOut1.addWidget(self.graphs["Current1"])
        # Power1 그래프
        self.graphs["Power1"] = self.create_graph("Power1", "Voltage (V)")
        graph_layOut1.addWidget(self.graphs["Power1"])

        graph_layOut2 = QHBoxLayout()
        # Voltage2 그래프
        self.graphs["Voltage2"] = self.create_graph("Voltage2", "Temperature (℃)")
        graph_layOut2.addWidget(self.graphs["Voltage2"])
        # Current2 그래프
        self.graphs["Current2"] = self.create_graph("Current2", "Temperature (℃)")
        graph_layOut2.addWidget(self.graphs["Current2"])
        # Power2 그래프
        self.graphs["Power2"] = self.create_graph("Power2", "Temperature (℃)")
        graph_layOut2.addWidget(self.graphs["Power2"])

        graph_total_layout = QVBoxLayout()
        graph_total_layout.addLayout(graph_layOut1)
        graph_total_layout.addLayout(graph_layOut2)
        
        
        # 그래프 레이아웃을 포함할 위젯 생성
        graph_widget = QWidget()
        graph_widget.setLayout(graph_total_layout)

        # 메인 레이아웃에 그래프 위젯 추가
        layout.addWidget(graph_widget)

    def create_graph(self, title, ylabel):
        """단일 그래프 생성"""
        graph = pg.PlotWidget()
        graph.setBackground("w")
        graph.setTitle(title, color="#828282", size="12pt")
        graph.setLabel("left", ylabel, color="#828282", size="10pt")
        graph.setLabel("bottom", "Time", color="#828282", size="10pt")
        graph.showGrid(x=True, y=True)
        graph.setYRange(0, 100)  # Y축 범위 설정
        return graph

    def init_data_indicators(self, layout):
        """데이터 표시 위젯 초기화 및 설정"""
        self.data_labels = {}

        # 그룹 박스 및 레이블 생성
        indicators = [
            ("Voltage1", "0 V"),
            ("Current1", "0 A"),
            ("Power1", "0 W"),
            ("Voltage2", "0 V"),
            ("Current2", "0 A"),
            ("Power2", "0 W"),
            ("Temperature", "0 ℃"),
            ("Operating Time", "0 h"),
            ("Inverter Status", "Ready"),
        ]

        for name, initial_value in indicators:
            group_box = QGroupBox(name)
            label = QLabel(initial_value)
            label.setAlignment(Qt.AlignCenter)
            label.setStyleSheet(
                "color:rgb(44, 106, 180); background-color:rgb(250,250,250);"
                "border-style: solid; border-width: 1px; border-color: rgb(200,200,200); border-radius: 5px;"
            )
            font = label.font()
            font.setPointSize(12)
            font.setBold(True)
            label.setFont(font)

            layout.addWidget(group_box)
            group_box_layout = QVBoxLayout()
            group_box.setLayout(group_box_layout)
            group_box_layout.addWidget(label)

            self.data_labels[name] = label

    def init_control(self, layout):
        """데이터 표시 위젯 초기화 및 설정"""
        check_box1 = QCheckBox("test1")
        check_box2 = QCheckBox("test2")
        push_button1 = QPushButton("PLC 연결", self)
        push_button1.setMaximumHeight(200)
        push_button1.setMaximumWidth(200)
        push_button1.setGeometry(200, 150, 100, 40)
        push_button1.setFont(QFont("", 15))
        # push_button1.clicked.connect()

        push_button2 = QPushButton("연결 종료", self)
        push_button2.setMaximumHeight(200)
        push_button2.setMaximumWidth(200)
        push_button2.setGeometry(200, 150, 100, 40)
        push_button2.setFont(QFont("", 15))
        # push_button2.clicked.connect()
        
        leftInnerLayOut = QVBoxLayout()
        leftInnerLayOut.addWidget(check_box1)
        leftInnerLayOut.addWidget(check_box2)
        leftInnerLayOut.addWidget(push_button1)
        leftInnerLayOut.addWidget(push_button2)

        group_box_layout = QVBoxLayout()
        group_box_layout.addLayout(leftInnerLayOut)

        group_box = QGroupBox("Control")
        group_box.setLayout(group_box_layout)
        group_box.setFixedWidth(200)
        layout.addWidget(group_box)

    def update_data(self):
        """실시간 데이터 갱신"""
        # 랜덤 데이터 생성
        Voltage1 = random.uniform(10, 100)
        Current1 = random.uniform(10, 100)
        Power1 = random.uniform(10, 100)
        Voltage2 = random.uniform(100, 240)
        Current2 = random.uniform(5, 20)
        Power2 = random.uniform(10, 100)
        Temperature = random.uniform(10, 100)
        Operating_Time = random.uniform(10, 100)
        Inverter_Status = random.uniform(10, 100)
        status = "Inverter RUN" if Inverter_Status > 50 else "Inverter STOP"

        # 데이터 표시 레이블 갱신
        self.data_labels["Voltage1"].setText(f"{Voltage1:.1f} V")
        self.data_labels["Current1"].setText(f"{Current1:.1f} A")
        self.data_labels["Power1"].setText(f"{Power1:.1f} W")
        self.data_labels["Voltage2"].setText(f"{Voltage2:.1f} V")
        self.data_labels["Current2"].setText(f"{Current2:.1f} A")
        self.data_labels["Power2"].setText(f"{Power2:.1f} W")
        self.data_labels["Temperature"].setText(f"{Temperature:.1f} ℃")
        self.data_labels["Operating Time"].setText(f"{Operating_Time:.1f} Wh")
        self.data_labels["Inverter Status"].setText(status)

        # 상태에 따른 색상 변경
        if status == "Inverter STOP":
            self.data_labels["Inverter Status"].setStyleSheet("color:rgb(240, 5, 50);")
        else:
            self.data_labels["Inverter Status"].setStyleSheet("color:rgb(44, 106, 180);")

        # 그래프 업데이트
        # current_time = time.strftime("%H:%M:%S", time.localtime())
        current_time = time.time()
        self.pen = pg.mkPen(color=(255, 0, 0), width=2, style=Qt.SolidLine)  # 빨간색, 두께 2, 실선

        for key, label in self.data_labels.items():
            # 데이터를 업데이트 (label은 [x, y] 형태라고 가정)
            if key not in self.graph_data:
                self.graph_data[key] = {"x": [], "y": []}

            # 새 데이터 추가
            self.graph_data[key]["x"].append(current_time)
            self.graph_data[key]["y"].append(random.uniform(0, 100))

            # 최대 데이터 길이 제한 (예: 100)
            max_length = 100
            if len(self.graph_data[key]["x"]) > max_length:
                self.graph_data[key]["x"].pop(0)
                self.graph_data[key]["y"].pop(0)

            # 그래프 업데이트
            graph = self.graphs[key]
            graph.plot(
                self.graph_data[key]["x"], 
                self.graph_data[key]["y"], 
                pen="b",  # 선 색상
                symbol="o",  # 점 모양
                symbolBrush="b",  # 점 색상
                symbolSize=5,  # 점 크기
                clear=True  # 기존 데이터를 지우고 다시 그림
            )
        if len(self.graph_data[key]["x"]) > self.max_length:
            self.graph_data[key]["x"].pop(0)
            self.graph_data[key]["y"].pop(0)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
