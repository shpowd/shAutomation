
# 디스플레이 설정
import sys
import random
import time
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from pyqtgraph import AxisItem
import pyqtgraph as pg


#region
import socket
import struct

HOST = '127.0.0.1' #'192.168.0.100'                  # PLC의 IP 주소,            modbus_sim ip: 127.0.0.1
PORT = 5020                              # Modbus TCP 표준 포트,     modbus_sim port: 5020
COMMUNICATION_INTERVAL = 2              # 통신 주기 (2초)
RUN_DURATION = 10 * 60                  # 총 실행 시간 (10분 = 600초)

# Modbus TCP 요청 메시지 구성
transaction_id = 0                      # 트랜잭션 ID (임의로 설정 가능)
protocol_id = 0                         # 프로토콜 ID (Modbus TCP의 경우 0)
length = 6                              # 길이 (유닛 ID, 함수 코드, 시작 주소, 레지스터 수 의 바이트 합)
################################################################################################################################################
################################################################################################################################################
#####                                                                                                                                    #######
#####                                                                                                                                    #######
unit_id = 55                            # 유닛 ID (Modbus 장치 ID = slave ID, 1 바이트
function_code = 4                       # 함수 코드 (4는 입력 레지스터 읽기, 1 바이트)
start_address = 0                       # 시작 주소 (0부터 시작 , 2 바이트)
register_count = 7                      # 읽을 레지스터 수 (7개의 레지스터 읽기 , 2 바이트)
#####                                                                                                                                    #######
#####                                                                                                                                    #######
################################################################################################################################################
################################################################################################################################################
#endregion


#region 통신 코드
class WorkerThread(QThread):
    # 시그널 정의 (메인 스레드와 통신)
    update_signal = pyqtSignal(list)

    def run(self):
        """백그라운드 작업 실행"""
        # '>HHHBBHH' 형식 문자열을 사용하여 Modbus 요청 메시지 패킹
        request = struct.pack('>HHHBBHH', transaction_id, protocol_id, length, unit_id, function_code, start_address, register_count)

        # 시작 시간 기록
        start_time = time.time()

        # TCP 소켓 생성 및 연결
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:        # socket.AF_INET: IPv4, socket.SOCK_STREAM: TCP
            client_socket.connect((HOST, PORT))                                         # PLC에 연결
            client_socket.sendall(request)                                              # 요청 메시지 전송

            # 응답 메시지 받기
            response = client_socket.recv(1024)  # PLC로부터의 응답 받기

        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
                client_socket.settimeout(6)                                            # 소켓 타임아웃 설정 (6초), connect, send, recieve가 6초 안에 동작안하면 오류 처리
                client_socket.connect((HOST, PORT))
                print("PLC에 연결되었습니다.")

                while True:
                    try:
                        # 요청 메시지 전송
                        client_socket.sendall(request)
                        print("요청 메시지를 전송했습니다.")
                        
                        # 응답 메시지 수신
                        response = client_socket.recv(1024)

                        # Modbus TCP 응답 메시지 분석
                        response_header = struct.unpack('>HHHBBB', response[:9])                    # '>HHHBBB' 형식 문자열을 사용하여 응답 메시지의 헤더 부분 언패킹
                        register_values = struct.unpack('>' + 'H' * register_count, response[9:])   # 나머지 부분을 언패킹하여 레지스터 값들 추출

                        # 결과 출력
                        # print("receive : ", response)  # 받은 전체 응답 메시지
                        # print("header : ", response_header)  # 응답 메시지의 헤더 부분
                        print("Register values :", register_values)  # 추출된 레지스터 값들

                        data = list(register_values)
                        data.append(1)
                        print(data)
                        self.update_signal.emit(data)
                    except socket.timeout:
                        print("응답 대기 시간이 초과되었습니다.")
                    except Exception as e:
                        print(f"통신 중 오류 발생: {e}")

                    time.sleep(COMMUNICATION_INTERVAL)                                   # 통신 주기 동안 대기 (2초) = 데이터 갱신 주기

        except Exception as e:
            print(f"소켓 초기화 중 오류 발생: {e}")

        print("통신을 정상적으로 종료했습니다.")
#endregion



# x축 시간 레이블 포맷 설정
class TimeAxisItem(AxisItem):
    def tickStrings(self, values, scale, spacing):
        return [time.strftime("%H:%M:%S", time.localtime(value)) for value in values]
    
# 디스플레이 윈도우 설정
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.init_thread()
        self.init_timer()

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

        # 데이터를 저장하는 딕셔너리 초기화
        self.graph_data = {key: {"x": [], "y": []} for key in self.graphs.keys()}
        self.max_length = 100  # 최대 데이터 길이

    def init_thread(self):
        print("Thread initializing...")
        self.worker_thread = WorkerThread()
        self.worker_thread.update_signal.connect(self.recieve_data)  # 시그널 연결
        self.worker_thread.start()  # 스레드 시작
        print("Thread started.")

    def init_timer(self):
        print("QTimer initializing...")
        self.timer = QTimer(self)
        self.timer.setInterval(1000)  # 1초마다 호출
        self.timer.timeout.connect(self.update_data)  # UI 갱신 메서드 연결
        self.timer.start()
        print("Timer started.")



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
        self.graphs["Pump1"] = self.create_graph("Pump1", "Current [A]")
        graph_layOut1.addWidget(self.graphs["Pump1"])
        # Current1 그래프
        self.graphs["Pump2"] = self.create_graph("Pump2", "Current [A]")
        graph_layOut1.addWidget(self.graphs["Pump2"])
        # Power1 그래프
        self.graphs["Pump3"] = self.create_graph("Pump3", "Current [A]")
        graph_layOut1.addWidget(self.graphs["Pump3"])

        graph_layOut2 = QHBoxLayout()
        # Voltage2 그래프
        self.graphs["Pump4"] = self.create_graph("Pump4", "Current [A]")
        graph_layOut2.addWidget(self.graphs["Pump4"])
        # Current2 그래프
        self.graphs["Pump5"] = self.create_graph("Pump5", "Current [A]")
        graph_layOut2.addWidget(self.graphs["Pump5"])
        # Power2 그래프
        self.graphs["Pump6"] = self.create_graph("Pump6", "Current [A]")
        graph_layOut2.addWidget(self.graphs["Pump6"])

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
        time_axis = TimeAxisItem(orientation="bottom")
        graph = pg.PlotWidget(axisItems={"bottom": time_axis})
        graph.setBackground("w")
        graph.setTitle(title, color="#828282", size="12pt")
        graph.setLabel("left", ylabel, color="#828282", size="10pt")
        # graph.setLabel("bottom", "Time", color="#828282", size="10pt")
        graph.showGrid(x=True, y=True)
        # graph.setYRange(0, 100)  # Y축 범위 설정
        return graph

    def init_data_indicators(self, layout):
        """데이터 표시 위젯 초기화 및 설정"""
        self.data_labels = {}

        # 그룹 박스 및 레이블 생성
        indicators = [
            ("Pump1", "0 A"),
            ("Pump2", "0 A"),
            ("Pump3", "0 A"),
            ("Pump4", "0 A"),
            ("Pump5", "0 A"),
            ("Pump6", "0 A"),
            # ("Temperature", "0 ℃"),
            # ("Operating Time", "0 h"),
            # ("Inverter Status", "Ready"),
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
        self.raw_data = {
            "Pump1": 0,
            "Pump2": 0,
            "Pump3": 0,
            "Pump4": 0,
            "Pump5": 0,
            "Pump6": 0,
        }

    def init_control(self, layout):
        """데이터 표시 위젯 초기화 및 설정"""
        status_label = QLabel("연결 종료")
        status_label.setAlignment(Qt.AlignCenter)
        status_label.setStyleSheet(
            "color:rgb(44, 106, 180); background-color:rgb(250,250,250);"
            "border-style: solid; border-width: 1px; border-color: rgb(200,200,200); border-radius: 5px;"
        )
        font = status_label.font()
        font.setPointSize(12)
        font.setBold(True)
        status_label.setFont(font)
        status_label.setFixedHeight(50)  # 높이를 50 픽셀로 고정
        self.status_label = status_label

        # check_box1 = QCheckBox("test1")
        # check_box2 = QCheckBox("test2")
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
        # leftInnerLayOut.addWidget(check_box1)
        # leftInnerLayOut.addWidget(check_box2)
        leftInnerLayOut.addWidget(status_label)
        leftInnerLayOut.addWidget(push_button1)
        leftInnerLayOut.addWidget(push_button2)

        group_box_layout = QVBoxLayout()
        group_box_layout.addLayout(leftInnerLayOut)

        group_box = QGroupBox("Control")
        group_box.setLayout(group_box_layout)
        group_box.setFixedWidth(200)
        layout.addWidget(group_box)

    def recieve_data(self, data):
        print("tt")
        """실시간 데이터 수신"""
        self.raw_data = {
            "Pump1": data[0],
            "Pump2": data[1],
            "Pump3": data[2],
            "Pump4": data[3],
            "Pump5": data[4],
            "Pump6": data[5],
        }
        self.status = "연결 중" if data[6] == 1 else "연결 종료"
        print(data[6])

    def update_data(self):
        """실시간 데이터 갱신"""
        for key, value in self.raw_data.items():
            self.data_labels[key].setText(f"{value:.1f} A")
        self.status_label.setText(self.status)
        # 상태에 따른 색상 변경
        if self.status == "연결 종료":
            self.status_label.setStyleSheet("color:rgb(240, 5, 50);")
        else:
            self.status_label.setStyleSheet("color:rgb(44, 106, 180);")

        # 그래프 업데이트
        current_time = time.time()
        readable_time = time.strftime("%Hh:%Mm:%Ss", time.localtime(current_time))
        self.pen = pg.mkPen(color=(255, 0, 0), width=2, style=Qt.SolidLine)  # 파란색, 두께 2, 실선

        for key, label in self.data_labels.items():
            # 데이터를 업데이트 (label은 [x, y] 형태라고 가정)
            if key not in self.graph_data:
                self.graph_data[key] = {"x": [], "y": []}

            # 새 데이터 추가
            self.graph_data[key]["x"].append(current_time)
            self.graph_data[key]["y"].append(self.raw_data[key])

            # 최대 데이터 길이 제한 (예: 100)
            self.max_length = 1000
            if len(self.graph_data[key]["x"]) > self.max_length:
                self.graph_data[key]["x"].pop(0)
                self.graph_data[key]["y"].pop(0)

            # 그래프 업데이트
            graph = self.graphs[key]
            graph.plot(
                self.graph_data[key]["x"], 
                self.graph_data[key]["y"], 
                pen=pg.mkPen(color="b", width=1),
                symbol="o",  # 점 모양
                symbolBrush="b",  # 점 색상
                symbolSize=1,  # 점 크기
                clear=True  # 기존 데이터를 지우고 다시 그림
            )


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
