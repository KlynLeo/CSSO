#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

#define MAX_PRODUCTS 10000
#define MAP_SHELVES L"MarketShelves"
#define MAP_VALAB   L"MarketValability"
#define MAP_PRICES  L"MarketPrices"
#define M_SHELVES   L"MutexShelves"
#define M_VALAB     L"MutexValability"
#define M_PRICES    L"MutexPrices"
#define M_LOGS      L"MutexLogs"
#define M_ERRORS    L"MutexErrors"
#define E_SELL_GO   L"DaySellGo"
#define E_SELL_DONE L"DaySellDone"
#define SOLD_FILE "C:\\Facultate\\CSSO\\H4\\Reports\\Summary\\sold.txt"
#define LOG_FILE  "C:\\Facultate\\CSSO\\H4\\Reports\\logs.txt"
#define ERR_FILE  "C:\\Facultate\\CSSO\\H4\\Reports\\Summary\\errors.txt"

int main() {
    cout << "sell.exe pornit" << endl << flush;

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
    HANDLE eGo = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, E_SELL_GO);
    HANDLE eDone = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, E_SELL_DONE);

    while (true) {
        WaitForSingleObject(eGo, INFINITE);
        ResetEvent(eGo);

        string day; ifstream("current_day.txt") >> day;
        if (day == "STOP") { cout << "sell.exe terminat" << endl << flush; break; }

        cout << "[Sell] Ziua: " << day << endl << flush;

        ifstream fin("sold/" + day);
        if (!fin.is_open()) {
            cout << "[Sell] Fara vanzari (fisier inexistent): sold/" << day << endl << flush;
            SetEvent(eDone);
            continue;
        }

        string line; int cnt = 0;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            int sh; if (sscanf_s(line.c_str(), "%d", &sh) != 1) continue;

            WaitForSingleObject(mS, INFINITE);
            DWORD id = shelves[sh];
            ReleaseMutex(mS);

            if (id == 0xFFFFFFFF) {
                WaitForSingleObject(mE, INFINITE);
                ofstream(ERR_FILE, ios::app) << "Raft gol la vanzare: " << sh << " (zi " << day << ")\n";
                ReleaseMutex(mE);
                continue;
            }

            WaitForSingleObject(mV, INFINITE); DWORD val = valab[id]; ReleaseMutex(mV);
            if (val == 0) {
                WaitForSingleObject(mE, INFINITE);
                ofstream(ERR_FILE, ios::app) << "Produs expirat: " << id << " (zi " << day << ")\n";
                ReleaseMutex(mE);
                continue;
            }

            WaitForSingleObject(mP, INFINITE); int price = prices[id]; ReleaseMutex(mP);
            double net = (val <= 2) ? price * 0.75 : price;

            WaitForSingleObject(mS, INFINITE);
            WaitForSingleObject(mV, INFINITE);
            WaitForSingleObject(mP, INFINITE);
            shelves[sh] = 0xFFFFFFFF;
            valab[id] = 0xFFFFFFFF;
            prices[id] = 0xFFFFFFFF;
            ReleaseMutex(mP);
            ReleaseMutex(mV);
            ReleaseMutex(mS);

            WaitForSingleObject(mL, INFINITE);
            ofstream lg(LOG_FILE, ios::app);
            lg << "S-a vandut produsul " << id
                << " de pe raftul " << sh
                << " cu " << val << " zile ramase; "
                << "pret intreg " << price << ", net " << net << "." << endl;
            lg.close();

            double total = 0; ifstream in(SOLD_FILE); if (in.is_open()) in >> total; in.close();
            total += net;
            ofstream out(SOLD_FILE, ios::trunc); out << total; out.close();
            ReleaseMutex(mL);

            ++cnt;
        }

        cout << "[Sell] Vanzari efectuate: " << cnt << " (zi " << day << ")" << endl << flush;
        SetEvent(eDone);
    }
    return 0;
}
