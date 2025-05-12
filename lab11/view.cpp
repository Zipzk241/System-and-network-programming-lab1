#include <windows.h>
#include <iostream>

#define FILE_NAME "data.dat"
#define ARRAY_SIZE 30
#define TIMER_ID 1
#define TIMER_INTERVAL 500 

int* data = nullptr;      // Вказівник на відображені дані
HWND hwndGlobal = NULL;   

// відображення файлу в пам’ять
bool mapDataFile() {
    HANDLE hFile = CreateFileA(FILE_NAME, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) return false;

    HANDLE hMapping = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (!hMapping) {
        CloseHandle(hFile);
        return false;
    }

    data = (int*)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
    CloseHandle(hFile);
    CloseHandle(hMapping);

    return data != nullptr;
}

// Вивід зірочок 
void drawArrayBars(HDC hdc, RECT& rect) {
    HANDLE hMutex = OpenMutexA(SYNCHRONIZE, FALSE, "Global\\MyDataMutex");
    if (!hMutex) {
        MessageBoxA(NULL, "Неможливо відкрити mutex для читання", "Помилка", MB_ICONERROR);
        exit(1);
    }

    FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1)); 

    if (WaitForSingleObject(hMutex, 100) == WAIT_OBJECT_0) {
        int y = 10;
        try {
            for (int i = 0; i < ARRAY_SIZE; ++i) {
                int stars = data[i];
                std::string line(stars, '*'); // формуємо рядок зірочок
                TextOutA(hdc, 10, y, line.c_str(), (int)line.length());
                y += 20;
            }
        }
        catch (...) {
            TextOutA(hdc, 10, 10, "Помилка читання з пам’яті", 27);
        }
        ReleaseMutex(hMutex);
    }
    else {
        TextOutA(hdc, 10, 10, "Очікування доступу до даних...", 32);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        SetTimer(hwnd, TIMER_ID, TIMER_INTERVAL, NULL); 
        return 0;

    case WM_TIMER:
        InvalidateRect(hwnd, NULL, TRUE); 
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        drawArrayBars(hdc, ps.rcPaint);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        if (data) UnmapViewOfFile(data); 
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Точка входу 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    if (!mapDataFile()) {
        MessageBoxA(NULL, "Не вдалося відкрити або відобразити файл", "Помилка", MB_ICONERROR);
        return 1;
    }

    const char CLASS_NAME[] = "DataViewerClass";

    WNDCLASSA wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassA(&wc);

    // Створення вікна
    HWND hwnd = CreateWindowExA(
        0, CLASS_NAME, "Візуалізація даних", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 700,
        NULL, NULL, hInstance, NULL);

    if (!hwnd) return 1;

    hwndGlobal = hwnd;
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Головний цикл повідомлень  
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
