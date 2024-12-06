import socket
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

test_val = 0

try:
    while True:
        try:
            print("클라이언트를 기다리는 중...")
            client_socket, client_address = server_socket.accept()
            print(f"클라이언트 연결됨: {client_address}")

            try:
                while True:
                    #test값 생성
                    if test_val < 254:
                        test_val += 1
                    else:
                        test_val = 0

                    # 요청 메시지 수신
                    request = client_socket.recv(1024)
                    if not request:
                        print("클라이언트가 연결을 종료했습니다.")
                        break

                    print(f"요청 메시지: {request.hex()}")

                    # 간단한 Modbus 응답 생성 (예: Holding Register 값 0x1234 반환)
                    response = b'\x00\x00\x00\x00\x00\x11\xFF\x04\x0E\x00\x9F\x00\xD2\x00\xDD\x01\xB0\x02\x01\x00\xC8\x00'+bytes([test_val])
                    client_socket.sendall(response)
                    print(f"응답 메시지 전송: {response.hex()}")

            except Exception as e:
                print(f"클라이언트 처리 중 오류 발생: {e}")
            finally:
                client_socket.close()
                print("클라이언트 소켓 닫힘")
        except socket.timeout:
            # 타임아웃 발생 시 아무 작업도 하지 않고 다시 대기
            continue
except KeyboardInterrupt:
    print("\n서버 종료")
finally:
    server_socket.close()
    print("서버 소켓 닫힘")
