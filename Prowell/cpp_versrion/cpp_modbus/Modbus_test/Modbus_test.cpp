#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <cstdint>

// Winsock ���̺귯�� ����� ���� ��ũ
#pragma comment(lib, "ws2_32.lib")

#define MODBUS_TCP_PORT 5020  // Modbus TCP�� �⺻ ��Ʈ

// Modbus TCP ��Ŷ ��� ����ü ����
struct ModbusTCPFrame {
    uint16_t transactionId;  // Ʈ����� ID
    uint16_t protocolId;     // �������� ID
    uint16_t length;         // ����
    uint8_t unitId;          // ��ġ ID
    uint8_t functionCode;    // �Լ� �ڵ�
    uint8_t data[4];       // ������ ����
};

int main() {
    // Winsock �ʱ�ȭ
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization failed!" << std::endl;
        return -1;
    }

    // ���� ���� ���� (Modbus ������ IP�� ��Ʈ)
    const char* server_ip = "127.0.0.1";  // Modbus ���� IP
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;              // IPv4 �ּ� �йи�
    serverAddr.sin_port = htons(MODBUS_TCP_PORT); // ��Ʈ ���� (502�� ��Ʈ)

    // inet_pton�� ����Ͽ� IP ���ڿ��� ��Ʈ��ũ ����Ʈ ������ ��ȯ
    if (inet_pton(AF_INET, server_ip, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address or Address not supported" << std::endl;
        WSACleanup();
        return -1;
    }

    // ���� ���� (TCP ����)
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!" << std::endl;
        WSACleanup();
        return -1;
    }

    // Modbus ������ ����
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed!" << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    std::cout << "Connected to Modbus server" << std::endl;

    // Modbus ��û: Read Holding Registers (Function Code 0x03)
    ModbusTCPFrame request;
    request.transactionId = htons(1);       // Ʈ����� ID ���� (1)
    request.protocolId = 0;                 // �������� ID (0�� �⺻��)
    request.length = htons(6);              // ������ ���� ���� (6����Ʈ)
    request.unitId = 1;                     // ���� ID (1�� ��ġ)
    request.functionCode = 0x03;            // ��� �ڵ�: Read Holding Registers (0x03)

    // ���� ���������� ���� �ּ�(0)�� �������� ��(2)
    request.data[0] = 0x00;  // ���� �ּ� (0�� ��������)
    request.data[1] = 0x00;
    request.data[2] = 0x00;  // ���� �������� �� (2��)
    request.data[3] = 0x02;

    // ��û ��Ŷ�� ������ ����
    if (send(sock, (char*)&request, sizeof(request), 0) == SOCKET_ERROR) {
        std::cerr << "Send failed!" << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    // ���� �ޱ�
    ModbusTCPFrame response;
    int recvSize = recv(sock, (char*)&response, sizeof(response), 0);
    if (recvSize == SOCKET_ERROR) {
        std::cerr << "Receive failed!" << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    // ���� �����͸� ��� (���� �����ʹ� 16������ ǥ��)
    if (recvSize > 0) {
        std::cout << "Received data: ";
        for (int i = 0; i < recvSize; i++) {
            std::cout << std::hex << (int)((unsigned char*)&response)[i] << " ";
        }
        std::cout << std::dec << std::endl;  // 10������ ���ư��� ���
    }

    // ���� ���� �� Winsock ����
    closesocket(sock);
    WSACleanup();

    system("pause");
    return 0;
}
