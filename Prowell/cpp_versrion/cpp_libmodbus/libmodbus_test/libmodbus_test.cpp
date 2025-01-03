#include <modbus.h>
#include <iostream>
#include <chrono>
#include <thread>

const int NUM_REGISTERS = 46;  // ���� �������� ����

int main() {
    // Modbus TCP ���� ����
    const char* ip_address = "127.0.0.1";  // ���� IP �ּ�
    int port = 5020;  // Modbus TCP ��Ʈ

    // Modbus TCP Ŭ���̾�Ʈ �ʱ�ȭ
    modbus_t* mb;
    uint16_t reg_buff[NUM_REGISTERS];  // �������͸� ������ �迭
    int rc;

    // modbus_new_tcp �Լ��� TCP ���� ��ü ����
    mb = modbus_new_tcp(ip_address, port);
    if (mb == NULL) {
        std::cerr << "Unable to allocate Modbus context\n";
        return -1;
    }

    // ����� Ȱ��ȭ
    modbus_set_debug(mb, TRUE); // �ۼ��ŵǴ� �޽����� ���

    // ������ ����
    rc = modbus_connect(mb);
    if (rc == -1) {
        std::cerr << "Connection failed: " << modbus_strerror(errno) << "\n";
        modbus_free(mb);
        return -1;
    }

    std::cout << "Connected to Modbus server at " << ip_address << ":" << port << "\n";

    // ���� �ֱ⸶�� �������� ���� �о����
    while (true) {
        rc = modbus_read_input_registers(mb, 0, NUM_REGISTERS, reg_buff);  // �����̺� �ּ� 0���� 3�� �Է� �������� �б�
        if (rc == -1) {
            std::cerr << "Failed to read registers: " << modbus_strerror(errno) << "\n";
        }
        else {
            std::cout << "Read " << NUM_REGISTERS << " registers:\n";
            for (int i = 0; i < NUM_REGISTERS; i++) {
                std::cout << "Register " << i << ": " << reg_buff[i] << "\n";
            }
        }

        // ���� �ֱ⸶�� �б� (1�ʸ��� �б�)
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // ���� ����
    modbus_close(mb);
    modbus_free(mb);

    return 0;
}
