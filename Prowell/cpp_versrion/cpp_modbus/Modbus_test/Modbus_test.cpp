#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <cstdint>

// Winsock 라이브러리 사용을 위한 링크
#pragma comment(lib, "ws2_32.lib")

#define MODBUS_TCP_PORT 5020  // Modbus TCP의 기본 포트

// Modbus TCP 패킷 헤더 구조체 정의
struct ModbusTCPFrame {
    uint16_t transactionId;  // 트랜잭션 ID
    uint16_t protocolId;     // 프로토콜 ID
    uint16_t length;         // 길이
    uint8_t unitId;          // 장치 ID
    uint8_t functionCode;    // 함수 코드
    uint8_t data[4];       // 데이터 영역
};

int main() {
    // Winsock 초기화
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization failed!" << std::endl;
        return -1;
    }

    // 서버 정보 설정 (Modbus 서버의 IP와 포트)
    const char* server_ip = "127.0.0.1";  // Modbus 서버 IP
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;              // IPv4 주소 패밀리
    serverAddr.sin_port = htons(MODBUS_TCP_PORT); // 포트 설정 (502번 포트)

    // inet_pton을 사용하여 IP 문자열을 네트워크 바이트 순서로 변환
    if (inet_pton(AF_INET, server_ip, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address or Address not supported" << std::endl;
        WSACleanup();
        return -1;
    }

    // 소켓 생성 (TCP 소켓)
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!" << std::endl;
        WSACleanup();
        return -1;
    }

    // Modbus 서버와 연결
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed!" << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    std::cout << "Connected to Modbus server" << std::endl;

    // Modbus 요청: Read Holding Registers (Function Code 0x03)
    ModbusTCPFrame request;
    request.transactionId = htons(1);       // 트랜잭션 ID 설정 (1)
    request.protocolId = 0;                 // 프로토콜 ID (0은 기본값)
    request.length = htons(6);              // 데이터 길이 설정 (6바이트)
    request.unitId = 1;                     // 유닛 ID (1번 장치)
    request.functionCode = 0x03;            // 기능 코드: Read Holding Registers (0x03)

    // 읽을 레지스터의 시작 주소(0)와 레지스터 수(2)
    request.data[0] = 0x00;  // 시작 주소 (0번 레지스터)
    request.data[1] = 0x00;
    request.data[2] = 0x00;  // 읽을 레지스터 수 (2개)
    request.data[3] = 0x02;

    // 요청 패킷을 서버로 전송
    if (send(sock, (char*)&request, sizeof(request), 0) == SOCKET_ERROR) {
        std::cerr << "Send failed!" << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    // 응답 받기
    ModbusTCPFrame response;
    int recvSize = recv(sock, (char*)&response, sizeof(response), 0);
    if (recvSize == SOCKET_ERROR) {
        std::cerr << "Receive failed!" << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    // 받은 데이터를 출력 (응답 데이터는 16진수로 표시)
    if (recvSize > 0) {
        std::cout << "Received data: ";
        for (int i = 0; i < recvSize; i++) {
            std::cout << std::hex << (int)((unsigned char*)&response)[i] << " ";
        }
        std::cout << std::dec << std::endl;  // 10진수로 돌아가서 출력
    }

    // 소켓 종료 및 Winsock 정리
    closesocket(sock);
    WSACleanup();

    system("pause");
    return 0;
}
