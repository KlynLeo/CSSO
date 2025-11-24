#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

#define MAX_PRODUCTS 10000
#define MAP_SIZE (MAX_PRODUCTS * sizeof(DWORD))
#define MAP_SHELVES L"MarketShelves"
#define MAP_VALAB L"MarketValability"
#define MAP_PRICES L"ProductPrices"
#define M_SHELVES L"MutexShelves"
#define M_VALAB L"MutexValability"
#define M_PRICES L"MutexPrices"
#define M_LOGS L"MutexLogs"
#define M_ERRORS L"MutexErrors"
#define E_DEP_GO L"DayDepositGo"
#define E_DEP_DONE L"DayDepositDone"
#define E_SELL_GO L"DaySellGo"
#define E_SELL_DONE L"DaySellDone"
#define E_DON_GO L"DayDonateGo"
#define E_DON_DONE L"DayDonateDone"
#define SEM_DAY L"DaySemaphore"
#define LOG_FILE "C:\\Facultate\\CSSO\\H4\\Reports\\logs.txt"
#define SOLD_FILE "C:\\Facultate\\CSSO\\H4\\Reports\\Summary\\sold.txt"
#define DONATE_FILE "C:\\Facultate\\CSSO\\H4\\Reports\\Summary\\donations.txt"
#define ERR_FILE "C:\\Facultate\\CSSO\\H4\\Reports\\Summary\\errors.txt"

int main() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::wstring wdir = exePath;
    size_t pos = wdir.find_last_of(L"\\/");
    if (pos != std::wstring::npos) {
        wdir = wdir.substr(0, pos);
        SetCurrentDirectoryW(wdir.c_str());
    }

    fs::create_directories("deposit");
    fs::create_directories("sold");
    fs::create_directories("C:\\Facultate\\CSSO\\H4\\Reports\\Summary");

    ofstream(LOG_FILE, ios::trunc);
    ofstream(SOLD_FILE, ios::trunc) << "0";
    ofstream(DONATE_FILE, ios::trunc) << "0";
    ofstream(ERR_FILE, ios::trunc);

    HANDLE hS = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MAP_SIZE, MAP_SHELVES);
    HANDLE hV = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MAP_SIZE, MAP_VALAB);
    HANDLE hP = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MAP_SIZE, MAP_PRICES);

    DWORD* shelves = (DWORD*)MapViewOfFile(hS, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    DWORD* valab = (DWORD*)MapViewOfFile(hV, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    DWORD* prices = (DWORD*)MapViewOfFile(hP, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    for (int i = 0; i < MAX_PRODUCTS; i++)
        shelves[i] = valab[i] = prices[i] = 0xFFFFFFFF;

    CreateMutex(NULL, FALSE, M_SHELVES);
    CreateMutex(NULL, FALSE, M_VALAB);
    CreateMutex(NULL, FALSE, M_PRICES);
    CreateMutex(NULL, FALSE, M_LOGS);
    CreateMutex(NULL, FALSE, M_ERRORS);

    CreateEvent(NULL, TRUE, FALSE, E_DEP_GO);
    CreateEvent(NULL, TRUE, FALSE, E_DEP_DONE);
    CreateEvent(NULL, TRUE, FALSE, E_SELL_GO);
    CreateEvent(NULL, TRUE, FALSE, E_SELL_DONE);
    CreateEvent(NULL, TRUE, FALSE, E_DON_GO);
    CreateEvent(NULL, TRUE, FALSE, E_DON_DONE);
    CreateSemaphore(NULL, 1, 1, SEM_DAY);

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION piDep, piSell, piDon;

    CreateProcess(L"C:\\Facultate\\CSSO\\tema4\\Lab4\\bin\\deposit.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &piDep);
    CreateProcess(L"C:\\Facultate\\CSSO\\tema4\\Lab4\\bin\\sell.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &piSell);
    CreateProcess(L"C:\\Facultate\\CSSO\\tema4\\Lab4\\bin\\donate.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &piDon);

    vector<string> days;
    if (fs::exists("deposit")) {
        for (auto& f : fs::directory_iterator("deposit"))
            if (f.is_regular_file()) days.push_back(f.path().filename().string());
        sort(days.begin(), days.end());
    }
    else {
        cout << "Folderul 'deposit' nu exista.\n";
        ofstream("current_day.txt", ios::trunc) << "STOP";
        HANDLE eDepGo = OpenEvent(EVENT_MODIFY_STATE, FALSE, E_DEP_GO);
        HANDLE eSellGo = OpenEvent(EVENT_MODIFY_STATE, FALSE, E_SELL_GO);
        HANDLE eDonGo = OpenEvent(EVENT_MODIFY_STATE, FALSE, E_DON_GO);
        if (eDepGo) SetEvent(eDepGo);
        if (eSellGo) SetEvent(eSellGo);
        if (eDonGo) SetEvent(eDonGo);
        return 1;
    }

    if (days.empty()) {
        cout << "Nu s-au găsit fișiere în folderul 'deposit'. Ieșire curată.\n";
        ofstream("current_day.txt", ios::trunc) << "STOP";
        HANDLE eDepGo = OpenEvent(EVENT_MODIFY_STATE, FALSE, E_DEP_GO);
        HANDLE eSellGo = OpenEvent(EVENT_MODIFY_STATE, FALSE, E_SELL_GO);
        HANDLE eDonGo = OpenEvent(EVENT_MODIFY_STATE, FALSE, E_DON_GO);
        if (eDepGo) SetEvent(eDepGo);
        if (eSellGo) SetEvent(eSellGo);
        if (eDonGo) SetEvent(eDonGo);
        return 0;
    }

    HANDLE eDepGo = OpenEvent(EVENT_MODIFY_STATE, FALSE, E_DEP_GO);
    HANDLE eDepDone = OpenEvent(SYNCHRONIZE, FALSE, E_DEP_DONE);
    HANDLE eSellGo = OpenEvent(EVENT_MODIFY_STATE, FALSE, E_SELL_GO);
    HANDLE eSellDone = OpenEvent(SYNCHRONIZE, FALSE, E_SELL_DONE);
    HANDLE eDonGo = OpenEvent(EVENT_MODIFY_STATE, FALSE, E_DON_GO);
    HANDLE eDonDone = OpenEvent(SYNCHRONIZE, FALSE, E_DON_DONE);

    int totalDays = (int)days.size();
    int currentDay = 0;

    for (auto& day : days) {
        currentDay++;
        cout << "=============================\n";
        cout << "Procesam ziua " << currentDay << " din " << totalDays << ": " << day << endl;

        ofstream("current_day.txt", ios::trunc) << day;

        SetEvent(eDepGo);
        WaitForSingleObject(eDepDone, INFINITE);
        ResetEvent(eDepDone);

        SetEvent(eSellGo);
        WaitForSingleObject(eSellDone, INFINITE);
        ResetEvent(eSellDone);

        SetEvent(eDonGo);
        WaitForSingleObject(eDonDone, INFINITE);
        ResetEvent(eDonDone);

        cout << "Ziua " << currentDay << " procesată complet.\n";
    }

    ofstream("current_day.txt", ios::trunc) << "STOP";
    SetEvent(eDepGo);
    SetEvent(eSellGo);
    SetEvent(eDonGo);

    WaitForSingleObject(piDep.hProcess, INFINITE);
    WaitForSingleObject(piSell.hProcess, INFINITE);
    WaitForSingleObject(piDon.hProcess, INFINITE);

    string sold, donate;
    ifstream(SOLD_FILE) >> sold;
    ifstream(DONATE_FILE) >> donate;

    cout << "=============================\n";
    cout << "Procesele s-au incheiat.\n";
    cout << "Sold total: " << sold << "\nDonatii totale: " << donate << endl;

    return 0;
}
