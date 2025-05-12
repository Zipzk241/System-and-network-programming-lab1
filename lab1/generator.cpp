//#include <windows.h>
//#include <iostream>
//#include <fstream>
//#include <random>
//#include <ctime>
//
//#define FILE_NAME "data.dat"
//#define ARRAY_SIZE 30
//
//// ��������� 
//void generateRandomData(const std::string& fileName, int count) {
//    std::ofstream file(fileName, std::ios::binary);
//    if (!file) {
//        throw std::runtime_error("�� ������� �������� ����.");
//    }
//
//    // ����������� ���������� 
//    std::mt19937 rng(static_cast<unsigned>(time(nullptr)));
//    std::uniform_int_distribution<int> dist(10, 100); // ����� �� 10 �� 100
//
//    // ����� � ����
//    for (int i = 0; i < count; ++i) {
//        int number = dist(rng);
//        file.write(reinterpret_cast<const char*>(&number), sizeof(int));
//    }
//
//    file.close();
//    std::cout << "���� ����������� � " << count << " �������!";
//}
//
//int main() {
//    SetConsoleOutputCP(1251); 
//    SetConsoleCP(1251);
//
//    // ������
//    HANDLE hMutex = CreateMutexA(NULL, FALSE, "Global\\MyDataMutex");
//    if (!hMutex) {
//        std::cerr << "�� ������� �������� mutex!" << std::endl;
//        return 1;
//    }
//
//    if (WaitForSingleObject(hMutex, INFINITE) == WAIT_OBJECT_0) {
//        try {
//            generateRandomData(FILE_NAME, ARRAY_SIZE);
//        }
//        catch (const std::exception& e) {
//            std::cerr << "������� ��� ����� � ����: " << e.what() << std::endl;
//        }
//        ReleaseMutex(hMutex);
//    }
//    else {
//        std::cerr << "�� ������� �������� mutex ��� ������!" << std::endl;
//    }
//
//    CloseHandle(hMutex);
//    return 0;
//}
