#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#define MAX_PRODUCTS 10000
#define MAP_SHELVES L"MarketShelves"
#define MAP_VALAB L"MarketValability"
#define MAP_PRICES L"ProductPrices"
#define M_SHELVES L"MutexShelves"
#define M_VALAB L"MutexValability"
#define M_PRICES L"MutexPrices"
#define M_LOGS L"MutexLogs"
#define M_ERRORS L"MutexErrors"
#define E_SELL_GO L"DaySellGo"
#define E_SELL_DONE L"DaySellDone"
#define E_DON_GO L"DayDonateGo"
#define SEM_DAY L"DaySemaphore"
#define LOG_FILE "C:\\Facultate\\CSSO\\H4\\Reports\\logs.txt"
#define SOLD_FILE "C:\\Facultate\\CSSO\\H4\\Reports\\Summary\\sold.txt"
#define ERR_FILE "C:\\Facultate\\CSSO\\H4\\Reports\\Summary\\errors.txt"

int main() {
    cout << "sell.exe pornit\n";

    HANDLE hS = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MAP_SHELVES);
    HANDLE hV = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MAP_VALAB);
    HANDLE hP = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MAP_PRICES);
    DWORD* shelves = (DWORD*)MapViewOfFile(hS, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    DWORD* valab = (DWORD*)MapViewOfFile(hV, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    DWORD* prices = (DWORD*)MapViewOfFile(hP, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    HANDLE mS = OpenMutex(MUTEX_ALL_ACCESS, FALSE, M_SHELVES);
    HANDLE mV = OpenMutex(MUTEX_ALL_ACCESS, FALSE, M_VALAB);
    HANDLE mP = OpenMutex(MUTEX_ALL_ACCESS, FALSE, M_PRICES);
    HANDLE mLog = OpenMutex(MUTEX_ALL_ACCESS, FALSE, M_LOGS);
    HANDLE mErr = OpenMutex(MUTEX_ALL_ACCESS, FALSE, M_ERRORS);

    HANDLE eGo = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, E_SELL_GO);
    HANDLE eDone = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, E_SELL_DONE);
    HANDLE eDonGo = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, E_DON_GO);
    HANDLE semDay = OpenSemaphore(SEMAPHORE_MODIFY_STATE | SYNCHRONIZE, FALSE, SEM_DAY);

    while (true) {
        WaitForSingleObject(eGo, INFINITE);
        ResetEvent(eGo);

        string dayFile;
        ifstream("current_day.txt") >> dayFile;
        if (dayFile == "STOP") break;

        cout << "[Sell] Procesăm fișierul: " << dayFile << endl;
        ifstream fin("sold/" + dayFile);
        if (!fin.is_open()) { SetEvent(eDone); continue; }

        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            int sh;
            sscanf_s(line.c_str(), "%d", &sh);

            WaitForSingleObject(mS, INFINITE);
            DWORD id = shelves[sh];
            ReleaseMutex(mS);

            if (id == 0xFFFFFFFF) {
                WaitForSingleObject(mErr, INFINITE);
                ofstream(ERR_FILE, ios::app) << "Raft gol la vânzare: " << sh << "\n";
                ReleaseMutex(mErr);
                continue;
            }

            WaitForSingleObject(mV, INFINITE);
            DWORD oldVal = valab[id];
            ReleaseMutex(mV);

            if (oldVal == 0) {
                WaitForSingleObject(mErr, INFINITE);
                ofstream(ERR_FILE, ios::app) << "Produs expirat: " << id << " de pe raft " << sh << "\n";
                ReleaseMutex(mErr);
                continue;
            }

            WaitForSingleObject(mP, INFINITE);
            int price = prices[id];
            ReleaseMutex(mP);

            double net = (oldVal <= 2) ? price * 0.75 : price;

            WaitForSingleObject(mS, INFINITE);
            WaitForSingleObject(mV, INFINITE);
            WaitForSingleObject(mP, INFINITE);
            shelves[sh] = 0xFFFFFFFF;
            valab[id] = 0xFFFFFFFF;
            prices[id] = 0xFFFFFFFF;
            ReleaseMutex(mP);
            ReleaseMutex(mV);
            ReleaseMutex(mS);

            WaitForSingleObject(mLog, INFINITE);
            ofstream lg(LOG_FILE, ios::app);
            lg << "S-a vandut produsul " << id << " de pe raft " << sh << " (pret " << price << ", net " << net << ")\n";
            double total = 0;
            ifstream in(SOLD_FILE); if (in.is_open()) in >> total; in.close();
            total += net;
            ofstream out(SOLD_FILE, ios::trunc); out << total; out.close();
            ReleaseMutex(mLog);
        }

        cout << "[Sell] Zi procesată complet.\n";
        WaitForSingleObject(semDay, INFINITE);
        SetEvent(eDone);
        ReleaseSemaphore(semDay, 1, NULL);
        SetEvent(eDonGo);
    }

    cout << "sell.exe terminat\n";
    return 0;
}
