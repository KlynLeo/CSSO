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
#define E_DON_GO    L"DayDonateGo"
#define E_DON_DONE  L"DayDonateDone"
#define LOG_FILE "C:\\Facultate\\CSSO\\H4\\Reports\\logs.txt"
#define DONATE_FILE "C:\\Facultate\\CSSO\\H4\\Reports\\Summary\\donations.txt"

int main() {
    cout << "donate.exe pornit" << endl << flush;

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
    HANDLE eGo = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, E_DON_GO);
    HANDLE eDone = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, E_DON_DONE);

    while (true) {
        WaitForSingleObject(eGo, INFINITE);
        ResetEvent(eGo);

        string day; ifstream("current_day.txt") >> day;
        if (day == "STOP") { cout << "donate.exe terminat" << endl << flush; break; }

        cout << "[Donate] Ziua: " << day << endl << flush;

        int donated = 0, decs = 0;
        WaitForSingleObject(mV, INFINITE);
        for (int id = 0; id < MAX_PRODUCTS; ++id) {
            if (valab[id] == 0) {
                double donation = prices[id];
                valab[id] = 0xFFFFFFFF;

                WaitForSingleObject(mS, INFINITE);
                for (int sh = 0; sh < MAX_PRODUCTS; ++sh) if (shelves[sh] == id) shelves[sh] = 0xFFFFFFFF;
                ReleaseMutex(mS);

                WaitForSingleObject(mP, INFINITE);
                prices[id] = 0xFFFFFFFF;
                ReleaseMutex(mP);

                WaitForSingleObject(mL, INFINITE);
                double total = 0; ifstream in(DONATE_FILE); if (in.is_open()) in >> total; in.close();
                total += donation;
                ofstream out(DONATE_FILE, ios::trunc); out << total; out.close();
                ReleaseMutex(mL);

                ++donated;
            }
            else if (valab[id] < 0xFFFFFFFF) {
                valab[id]--;
                ++decs;
            }
        }
        ReleaseMutex(mV);

        cout << "[Donate] Donatii: " << donated << ", decrementari: " << decs << " (zi " << day << ")" << endl << flush;
        SetEvent(eDone);
    }
    return 0;
}
