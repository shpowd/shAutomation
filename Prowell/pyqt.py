import sys
import random
import time
from PyQt5.QtCore import QTimer, Qt
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QVBoxLayout, QHBoxLayout, QGroupBox, QLabel, QWidget
)
import pyqtgraph as pg


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        # 기본 UI 설정
        self.setWindowTitle("통합 그래프 및 데이터 모니터링")
        self.setGeometry(100, 100, 1400, 800)

        # 중앙 위젯
        central_widget = QWidget(self)
        self.setCentralWidget(central_widget)

        # 레이아웃 설정
        main_layout = QVBoxLayout()
        central_widget.setLayout(main_layout)

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

        # 타이머 설정
        self.timer = QTimer()
        self.timer.setInterval(1000)  # 1초 간격으로 갱신
        self.timer.timeout.connect(self.update_data)
        self.timer.start()

    def init_graphs(self, layout):
        """그래프 위젯 초기화 및 설정"""
        self.graphs = {}

        # Power 그래프
        self.graphs["Power"] = self.create_graph("Generating Power", "Power (W)")
        layout.addWidget(self.graphs["Power"])

        # Energy 그래프
        self.graphs["Energy"] = self.create_graph("Generating Energy", "Energy (Wh)")
        layout.addWidget(self.graphs["Energy"])

        # Voltage 그래프
        self.graphs["Voltage"] = self.create_graph("Generating Voltage", "Voltage (V)")
        layout.addWidget(self.graphs["Voltage"])

        # Temperature 그래프
        self.graphs["Temperature"] = self.create_graph("Inverter Temperature", "Temperature (℃)")
        layout.addWidget(self.graphs["Temperature"])

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
            ("Solar Voltage", "0 V"),
            ("Solar Current", "0 A"),
            ("Solar Power", "0 W"),
            ("Line Voltage", "0 V"),
            ("Line Current", "0 A"),
            ("Line Power", "0 W"),
            ("Temperature", "0 ℃"),
            ("Energy Today", "0 Wh"),
            ("Lifetime Energy", "0 kWh"),
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

    def update_data(self):
        """실시간 데이터 갱신"""
        # 랜덤 데이터 생성
        solar_voltage = random.uniform(10, 100)
        solar_current = random.uniform(1, 10)
        solar_power = solar_voltage * solar_current
        line_voltage = random.uniform(100, 240)
        line_current = random.uniform(5, 20)
        line_power = line_voltage * line_current
        temperature = random.uniform(20, 80)
        energy_today = random.uniform(0, 1000)
        lifetime_energy = random.uniform(1000, 5000)
        status = "Inverter RUN" if random.random() > 0.5 else "Inverter STOP"

        # 데이터 표시 레이블 갱신
        self.data_labels["Solar Voltage"].setText(f"{solar_voltage:.1f} V")
        self.data_labels["Solar Current"].setText(f"{solar_current:.1f} A")
        self.data_labels["Solar Power"].setText(f"{solar_power:.1f} W")
        self.data_labels["Line Voltage"].setText(f"{line_voltage:.1f} V")
        self.data_labels["Line Current"].setText(f"{line_current:.1f} A")
        self.data_labels["Line Power"].setText(f"{line_power:.1f} W")
        self.data_labels["Temperature"].setText(f"{temperature:.1f} ℃")
        self.data_labels["Energy Today"].setText(f"{energy_today:.1f} Wh")
        self.data_labels["Lifetime Energy"].setText(f"{lifetime_energy:.1f} kWh")
        self.data_labels["Inverter Status"].setText(status)

        # 상태에 따른 색상 변경
        if status == "Inverter STOP":
            self.data_labels["Inverter Status"].setStyleSheet("color:rgb(240, 5, 50);")
        else:
            self.data_labels["Inverter Status"].setStyleSheet("color:rgb(44, 106, 180);")

        # 그래프 업데이트
        current_time = time.strftime("%H:%M:%S", time.localtime())
        for key, graph in self.graphs.items():
            graph.plot([current_time], [random.uniform(0, 100)], pen=None, symbol="o", clear=False)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
