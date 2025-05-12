#include <windows.h>
#include <iostream>
#include <conio.h>
#include <thread>
#include <chrono>

#define FILE_NAME "data.dat"
#define ARRAY_SIZE 30

// Сортування 
void bubbleSort(int* data, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - 1 - i; j++) {
            if (data[j] > data[j + 1]) {
                std::swap(data[j], data[j + 1]);
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

// Очікування натискання пробілу
void waitForSpace() {
    std::cout << "Натисніть ПРОБІЛ для початку сортування..." << std::endl;
    while (true) {
        if (_kbhit() && _getch() == ' ') break;
    }
}

// М’ютекс
HANDLE initMutex() {
    HANDLE hMutex = OpenMutexA(SYNCHRONIZE, FALSE, "Global\\MyDataMutex");
    if (!hMutex) {
        hMutex = CreateMutexA(NULL, FALSE, "Global\\MyDataMutex");
    }
    return hMutex;
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    waitForSpace();  

    // Відкриття файлу
    HANDLE hFile = CreateFileA(FILE_NAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Не вдалося відкрити файл." << std::endl;
        return 1;
    }

    HANDLE hMapping = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
    if (!hMapping) {
        std::cerr << "Не вдалося створити відображення файлу." << std::endl;
        CloseHandle(hFile);
        return 1;
    }

    int* data = (int*)MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (!data) {
        std::cerr << "Не вдалося відобразити файл у пам’яті." << std::endl;
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return 1;
    }

    HANDLE hMutex = initMutex();
    if (!hMutex) {
        std::cerr << "Не вдалося створити або відкрити м’ютекс." << std::endl;
        UnmapViewOfFile(data);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return 1;
    }

    std::cout << "Сортування розпочато..." << std::endl;

    if (WaitForSingleObject(hMutex, INFINITE) == WAIT_OBJECT_0) {
        try {
            bubbleSort(data, ARRAY_SIZE);
        }
        catch (...) {
            std::cerr << "Помилка під час сортування!" << std::endl;
        }
        ReleaseMutex(hMutex);
    }
    else {
        std::cerr << "Не вдалося отримати доступ до м’ютексу." << std::endl;
    }

    std::cout << "Сортування завершено." << std::endl;

    UnmapViewOfFile(data);
    CloseHandle(hMapping);
    CloseHandle(hFile);

    return 0;
}
