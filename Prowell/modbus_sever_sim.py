import socket
import struct
import random
import time

# 서버 설정
HOST = '127.0.0.1'  # localhost
PORT = 5020         # Modbus TCP 테스트용 포트

# 소켓 생성 및 바인딩
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.settimeout(1.0)  # 1초 타임아웃 설정
server_socket.bind((HOST, PORT))
server_socket.listen(1)  # 한 번에 하나의 클라이언트만 처리

print(f"Modbus 서버 실행 중... ({HOST}:{PORT})")

# 46개의 레지스터 값 초기화
# register_values = [i + 100 for i in range(46)]  # 기본 값: 100 ~ 145
random_indices = [4, 11, 18, 25, 32, 39]  # 0 기반 인덱스 (5번째, 12번째, ...)
register_values = [1, 23]

def update_random_registers():
    """랜덤 값을 특정 레지스터에 업데이트"""
    for idx in random_indices:
        register_values[idx] = random.randint(0, 50)  # 0~65535 범위의 랜덤 값

try:
    while True:
        try:
            print("클라이언트를 기다리는 중...")
            client_socket, client_address = server_socket.accept()
            print(f"클라이언트 연결됨: {client_address}")

            try:
                while True:
                    # 특정 레지스터 값 업데이트
                    # update_random_registers()

                    # 요청 메시지 수신
                    request = client_socket.recv(1024)
                    if not request:
                        print("클라이언트가 연결을 종료했습니다.")
                        break

                    print(f"요청 메시지: {request.hex()}")

                    # Modbus 응답 생성
                    # Modbus TCP 헤더: Transaction ID(2) + Protocol ID(2) + Length(2) + Unit ID(1)
                    transaction_id = request[:2]
                    protocol_id = request[2:4]
                    length = struct.pack('>H', 3 + len(register_values) * 2)
                    unit_id = request[6:7]

                    # Modbus PDU: Function Code(1) + Byte Count(1) + Data(N)
                    function_code = b'\x04'  # Read Holding Registers
                    byte_count = struct.pack('B', len(register_values) * 2)
                    data = b''.join(struct.pack('>H', value) for value in register_values)

                    # 전체 응답 메시지 조립
                    response = transaction_id + protocol_id + length + unit_id + function_code + byte_count + data

                    # 응답 메시지 전송
                    client_socket.sendall(response)
                    print(f"응답 메시지 전송: {response.hex()}")

            except Exception as e:
                print(f"클라이언트 처리 중 오류 발생: {e}")
            finally:
                client_socket.close()
                print("클라이언트 소켓 닫힘")
        except socket.timeout:
            # 타임아웃 발생 시 다시 대기
            continue
except KeyboardInterrupt:
    print("\n서버 종료")
finally:
    server_socket.close()
    print("서버 소켓 닫힘")