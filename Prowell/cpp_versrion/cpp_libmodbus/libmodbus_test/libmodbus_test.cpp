#include <modbus.h>
#include <iostream>
#include <chrono>
#include <thread>

const int NUM_REGISTERS = 46;  // 읽을 레지스터 개수

int main() {
    // Modbus TCP 서버 설정
    const char* ip_address = "127.0.0.1";  // 서버 IP 주소
    int port = 5020;  // Modbus TCP 포트

    // Modbus TCP 클라이언트 초기화
    modbus_t* mb;
    uint16_t reg_buff[NUM_REGISTERS];  // 레지스터를 저장할 배열
    int rc;

    // modbus_new_tcp 함수로 TCP 연결 객체 생성
    mb = modbus_new_tcp(ip_address, port);
    if (mb == NULL) {
        std::cerr << "Unable to allocate Modbus context\n";
        return -1;
    }

    // 디버깅 활성화
    modbus_set_debug(mb, TRUE); // 송수신되는 메시지를 출력

    // 서버에 연결
    rc = modbus_connect(mb);
    if (rc == -1) {
        std::cerr << "Connection failed: " << modbus_strerror(errno) << "\n";
        modbus_free(mb);
        return -1;
    }

    std::cout << "Connected to Modbus server at " << ip_address << ":" << port << "\n";

    // 일정 주기마다 레지스터 값을 읽어오기
    while (true) {
        rc = modbus_read_input_registers(mb, 0, NUM_REGISTERS, reg_buff);  // 슬레이브 주소 0에서 3개 입력 레지스터 읽기
        if (rc == -1) {
            std::cerr << "Failed to read registers: " << modbus_strerror(errno) << "\n";
        }
        else {
            std::cout << "Read " << NUM_REGISTERS << " registers:\n";
            for (int i = 0; i < NUM_REGISTERS; i++) {
                std::cout << "Register " << i << ": " << reg_buff[i] << "\n";
            }
        }

        // 일정 주기마다 읽기 (1초마다 읽기)
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // 연결 종료
    modbus_close(mb);
    modbus_free(mb);

    return 0;
}
