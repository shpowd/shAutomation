import socket
import struct
import time

HOST = '192.168.0.100'                  # PLC의 IP 주소,            modbus_sim ip: 127.0.0.1
PORT = 502                              # Modbus TCP 표준 포트,     modbus_sim port: 5020

#설정
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

            except socket.timeout:
                print("응답 대기 시간이 초과되었습니다.")
            except Exception as e:
                print(f"통신 중 오류 발생: {e}")

            time.sleep(COMMUNICATION_INTERVAL)                                   # 통신 주기 동안 대기 (2초) = 데이터 갱신 주기

except Exception as e:
    print(f"소켓 초기화 중 오류 발생: {e}")

print("통신을 정상적으로 종료했습니다.")


