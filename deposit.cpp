#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#define MAX_PRODUCTS 10000
#define MAP_SHELVES L"MarketShelves"
#define MAP_VALAB   L"MarketValability"
#define MAP_PRICES  L"MarketPrices"
#define M_SHELVES   L"MutexShelves"
#define M_VALAB     L"MutexValability"
#define M_PRICES    L"MutexPrices"
#define M_LOGS      L"MutexLogs"
#define M_ERRORS    L"MutexErrors"
#define E_DEP_GO    L"DayDepositGo"
#define E_DEP_DONE  L"DayDepositDone"
#define LOG_FILE "C:\\Facultate\\CSSO\\H4\\Reports\\logs.txt"
#define ERR_FILE "C:\\Facultate\\CSSO\\H4\\Reports\\Summary\\errors.txt"

int main() {
    HANDLE hS = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MAP_SHELVES);
    HANDLE hV = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MAP_VALAB);
    HANDLE hP = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MAP_PRICES);
    DWORD* shelves = (DWORD*)MapViewOfFile(hS, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    DWORD* valab = (DWORD*)MapViewOfFile(hV, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    DWORD* prices = (DWORD*)MapViewOfFile(hP, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    HANDLE mS = OpenMutex(MUTEX_ALL_ACCESS, FALSE, M_SHELVES);
    HANDLE mV = OpenMutex(MUTEX_ALL_ACCESS, FALSE, M_VALAB);
    HANDLE mP = OpenMutex(MUTEX_ALL_ACCESS, FALSE, M_PRICES);
    HANDLE mL = OpenMutex(MUTEX_ALL_ACCESS, FALSE, M_LOGS);
    HANDLE mE = OpenMutex(MUTEX_ALL_ACCESS, FALSE, M_ERRORS);
    HANDLE eGo = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, E_DEP_GO);
    HANDLE eDone = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, E_DEP_DONE);

    while (true) {
        WaitForSingleObject(eGo, INFINITE);
        ResetEvent(eGo);

        string day; ifstream("current_day.txt") >> day;
        if (day == "STOP") break;

        cout << "[Deposit] Ziua: " << day << endl << flush;

        ifstream fin("deposit/" + day);
        if (!fin.is_open()) { SetEvent(eDone); continue; }

        string line; int adds = 0, rejects = 0;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            int id, dexp, shelf, price;
            if (sscanf_s(line.c_str(), "%d,%d,%d,%d", &id, &dexp, &shelf, &price) != 4) continue;

            WaitForSingleObject(mS, INFINITE);
            bool free = (shelves[shelf] == 0xFFFFFFFF);
            if (free) shelves[shelf] = id;
            ReleaseMutex(mS);

            if (!free) {
                WaitForSingleObject(mE, INFINITE);
                ofstream(ERR_FILE, ios::app) << "Raft ocupat: " << shelf << " (zi " << day << ")\n";
                ReleaseMutex(mE);
                ++rejects;
                continue;
            }

            WaitForSingleObject(mV, INFINITE); valab[id] = dexp; ReleaseMutex(mV);
            WaitForSingleObject(mP, INFINITE); prices[id] = price; ReleaseMutex(mP);

            ++adds;

            WaitForSingleObject(mL, INFINITE);
            ofstream lg(LOG_FILE, ios::app);
            lg << "Am adaugat pe raftul " << shelf
                << " produsul " << id
                << " cu valabilitate " << dexp
                << " zile si pret " << price
                << "." << endl;
            lg.close();
            ReleaseMutex(mL);

        }

        cout << "[Deposit] Done: adaugate=" << adds << ", respinse=" << rejects << endl << flush;
        SetEvent(eDone);
    }
    return 0;
}
