import socket
import struct
import random
import time

# 서버 정보
SERVER_IP = '127.0.0.1'  # 서버 IP 주소
SERVER_PORT = 50200       # 서버 포트


def create_modbus_request(transaction_id, unit_id, start_address, values):
    """
    Modbus 요청 생성 함수 (Write Multiple Registers)
    :param transaction_id: 트랜잭션 ID
    :param unit_id: 유닛 ID
    :param start_address: 시작 주소
    :param values: 레지스터에 작성할 값 목록
    :return: 요청 바이트 문자열
    """
    protocol_id = 0        # Modbus 프로토콜 ID (항상 0)
    function_code = 16     # Write Multiple Registers (기능 코드 16)

    # 데이터 길이 계산
    quantity = len(values)
    byte_count = quantity * 2
    length = 7 + byte_count

    # 데이터 패킹
    data = b''.join(struct.pack('>H', value) for value in values)

    # 요청 패킷 생성
    request = struct.pack('>HHHBBHHB',
                          transaction_id,  # 트랜잭션 ID
                          protocol_id,     # 프로토콜 ID
                          length,          # 길이
                          unit_id,         # 유닛 ID
                          function_code,   # 기능 코드
                          start_address,   # 시작 주소
                          quantity,        # 레지스터 개수
                          byte_count) + data
    return request


def generate_register_values(graph_number):
    """
    홀딩 레지스터 값 생성 함수
    :param graph_number: 그래프 번호 (1~8)
    :return: 생성된 레지스터 값 목록
    """
    return [
        graph_number,                     # 0번 레지스터: 그래프 번호
        random.randint(0, 1024),       # 1번: 전류R (900~1100)
        random.randint(0, 1024),       # 2번: 전류S (900~1100)
        random.randint(0, 1024),       # 3번: 전류T (900~1100)
        random.randint(20, 30),          # 4번: 베어링 온도 (20~30)
        random.randint(20, 30),          # 5번: 권선 온도R (20~30)
        random.randint(20, 30),          # 6번: 권선 온도S (20~30)
        random.randint(20, 30),          # 7번: 권선 온도T (20~30)
        random.randint(5, 15),           # 8번: 진동 (5~15)
        random.randint(0, 65535),         # 9번: 구동시간 (0~65535)
        random.randint(0, 65535),         # 10번: 구동시간 (0~65535)
        random.randint(0, 1024),       # 11번: 전류R (900~1100)
        random.randint(0, 1024),       # 12번: 전류S (900~1100)
        random.randint(0, 1024),       # 13번: 전류T (900~1100)
        random.randint(20, 30),          # 14번: 베어링 온도 (20~30)
        random.randint(20, 30),          # 15번: 권선 온도R (20~30)
        random.randint(20, 30),          # 16번: 권선 온도S (20~30)
        random.randint(20, 30),          # 17번: 권선 온도T (20~30)
        random.randint(5, 15),           # 18번: 진동 (5~15)
        random.randint(0, 65535)         # 19번: 구동시간 (0~65535)
    ]


def main():
    # 소켓 생성 및 서버 연결
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((SERVER_IP, SERVER_PORT))
        print(f"서버에 연결됨: {SERVER_IP}:{SERVER_PORT}")

        transaction_id = 1   # 트랜잭션 ID (요청마다 증가)
        unit_id = 1          # 유닛 ID
        start_address = 0    # 시작 주소
        graph_number = 1     # 초기 그래프 번호

        try:
            while True:
                # 홀딩 레지스터 값 생성
                register_values = generate_register_values(graph_number)

                # Modbus 요청 생성
                request = create_modbus_request(transaction_id, unit_id, start_address, register_values)

                # 요청 전송
                client_socket.sendall(request)
                print(f"요청 메시지 전송: {request.hex()}")
                print(f"보낸 데이터: {register_values}")

                # 응답 수신
                response = client_socket.recv(1024)
                print(f"응답 메시지 수신: {response.hex()}")

                # 그래프 번호 업데이트 (1~8 순환)
                graph_number = (graph_number % 8) + 1

                # 1초 대기
                time.sleep(1)

                # 트랜잭션 ID 증가
                transaction_id += 1

        except KeyboardInterrupt:
            print("\n클라이언트 종료")
        except Exception as e:
            print(f"클라이언트 실행 중 오류 발생: {e}")


if __name__ == "__main__":
    main()
