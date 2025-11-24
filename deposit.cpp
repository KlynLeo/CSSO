#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#define MAX_PRODUCTS 10000
#define MAP_SHELVES L"MarketShelves"
#define MAP_VALAB L"MarketValability"
#define MAP_PRICES L"ProductPrices"
#define M_LOGS L"MutexLogs"
#define E_DEP_GO L"DayDepositGo"
#define E_DEP_DONE L"DayDepositDone"
#define LOG_FILE "C:\\Facultate\\CSSO\\H4\\Reports\\logs.txt"

int main() {
    cout << "deposit.exe pornit\n";

    HANDLE hS = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MAP_SHELVES);
    HANDLE hV = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MAP_VALAB);
    HANDLE hP = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MAP_PRICES);
    DWORD* shelves = (DWORD*)MapViewOfFile(hS, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    DWORD* valab = (DWORD*)MapViewOfFile(hV, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    DWORD* prices = (DWORD*)MapViewOfFile(hP, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    HANDLE mLogs = OpenMutex(MUTEX_ALL_ACCESS, FALSE, M_LOGS);
    HANDLE eGo = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, E_DEP_GO);
    HANDLE eDone = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, E_DEP_DONE);

    while (true) {
        WaitForSingleObject(eGo, INFINITE);
        ResetEvent(eGo);

        string dayFile;
        ifstream("current_day.txt") >> dayFile;
        if (dayFile == "STOP") break;

        cout << "[Deposit] Procesăm fișierul: " << dayFile << endl;
        ifstream fin("deposit/" + dayFile);
        if (!fin.is_open()) { SetEvent(eDone); continue; }

        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            int id, days, shelf, price;
            sscanf_s(line.c_str(), "%d,%d,%d,%d", &id, &days, &shelf, &price);
            shelves[shelf] = id;
            valab[id] = days;
            prices[id] = price;

            WaitForSingleObject(mLogs, INFINITE);
            ofstream log(LOG_FILE, ios::app);
            log << "Added product " << id << " on shelf " << shelf << " (" << days << " zile, " << price << " lei)\n";
            ReleaseMutex(mLogs);
        }

        cout << "[Deposit] Zi procesată complet.\n";
        SetEvent(eDone);
    }

    cout << "deposit.exe terminat\n";
    return 0;
}
