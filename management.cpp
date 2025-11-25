#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <string>
using namespace std;
namespace fs = std::filesystem;

#define MAX_PRODUCTS 10000
#define MAP_SIZE (MAX_PRODUCTS * sizeof(DWORD))
#define MAP_SHELVES L"MarketShelves"
#define MAP_VALAB L"MarketValability"
#define MAP_PRICES L"MarketPrices"
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
#define LOG_FILE "C:\\Facultate\\CSSO\\H4\\Reports\\logs.txt"
#define SOLD_FILE "C:\\Facultate\\CSSO\\H4\\Reports\\Summary\\sold.txt"
#define DONATE_FILE "C:\\Facultate\\CSSO\\H4\\Reports\\Summary\\donations.txt"
#define ERR_FILE "C:\\Facultate\\CSSO\\H4\\Reports\\Summary\\errors.txt"

static int keyFromName(const string& s) {
    size_t a = s.find('.'), b = s.find('.', a + 1), c = s.find('.', b + 1);
    int y = stoi(s.substr(0, a)), m = stoi(s.substr(a + 1, b - a - 1)), d = stoi(s.substr(b + 1, c - b - 1));
    return y * 10000 + m * 100 + d;
}

int main() {
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
    for (int i = 0; i < MAX_PRODUCTS; i++) shelves[i] = valab[i] = prices[i] = 0xFFFFFFFF;

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

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION piDep, piSell, piDon;

    wchar_t exePath[MAX_PATH]; GetModuleFileNameW(NULL, exePath, MAX_PATH);
    wstring wdir = exePath; size_t pos = wdir.find_last_of(L"\\/");
    wdir = wdir.substr(0, pos);
    wstring depExe = wdir + L"\\deposit.exe";
    wstring sellExe = wdir + L"\\sell.exe";
    wstring donExe = wdir + L"\\donate.exe";
    wchar_t depCmd[MAX_PATH]; wcscpy_s(depCmd, depExe.c_str());
    wchar_t sellCmd[MAX_PATH]; wcscpy_s(sellCmd, sellExe.c_str());
    wchar_t donCmd[MAX_PATH]; wcscpy_s(donCmd, donExe.c_str());
    CreateProcessW(depExe.c_str(), depCmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &piDep);
    CreateProcessW(sellExe.c_str(), sellCmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &piSell);
    CreateProcessW(donExe.c_str(), donCmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &piDon);

    vector<string> days;
    wstring depositFolder = wdir + L"\\deposit";
    for (auto& f : fs::directory_iterator(depositFolder)) if (f.is_regular_file()) days.push_back(f.path().filename().string());
    sort(days.begin(), days.end(), [](const string& a, const string& b) { return keyFromName(a) < keyFromName(b); });

    if (days.empty()) { ofstream("current_day.txt") << "STOP"; return 0; }

    HANDLE eDepGo = OpenEvent(EVENT_MODIFY_STATE, FALSE, E_DEP_GO);
    HANDLE eDepDone = OpenEvent(SYNCHRONIZE, FALSE, E_DEP_DONE);
    HANDLE eSellGo = OpenEvent(EVENT_MODIFY_STATE, FALSE, E_SELL_GO);
    HANDLE eSellDone = OpenEvent(SYNCHRONIZE, FALSE, E_SELL_DONE);
    HANDLE eDonGo = OpenEvent(EVENT_MODIFY_STATE, FALSE, E_DON_GO);
    HANDLE eDonDone = OpenEvent(SYNCHRONIZE, FALSE, E_DON_DONE);

    int cur = 0, total = (int)days.size();
    for (auto& day : days) {
        ++cur;
        cout << "==============================" << endl;
        cout << "Procesam ziua " << cur << "/" << total << ": " << day << endl << flush;
        ofstream("current_day.txt", ios::trunc) << day;

        // === Delimitare în logs.txt pentru început de zi ===
        {
            HANDLE mL = OpenMutex(MUTEX_ALL_ACCESS, FALSE, M_LOGS);
            WaitForSingleObject(mL, INFINITE);
            ofstream lg(LOG_FILE, ios::app);
            lg << "\n==============================\n";
            lg << "=== Incepe ziua " << cur << ": " << day << " ===\n";
            lg << "==============================\n";
            lg.close();
            ReleaseMutex(mL);
        }

        SetEvent(eDepGo);
        WaitForSingleObject(eDepDone, INFINITE);
        ResetEvent(eDepDone);

        SetEvent(eSellGo);
        WaitForSingleObject(eSellDone, INFINITE);
        ResetEvent(eSellDone);

        SetEvent(eDonGo);
        WaitForSingleObject(eDonDone, INFINITE);
        ResetEvent(eDonDone);
        // mică întârziere pentru a permite proceselor să termine scrierile în loguri
        Sleep(100);

        // === Delimitare în logs.txt pentru sfârșit de zi ===
        {
            HANDLE mL = OpenMutex(MUTEX_ALL_ACCESS, FALSE, M_LOGS);
            WaitForSingleObject(mL, INFINITE);
            ofstream lg(LOG_FILE, ios::app);
            lg << "=== Sfarsit ziua " << cur << ": " << day << " ===\n";
            lg << "==============================\n";
            lg.close();
            ReleaseMutex(mL);
        }

        cout << "Ziua " << cur << " procesata complet" << endl << flush;
    }

    ofstream("current_day.txt", ios::trunc) << "STOP";
    SetEvent(eDepGo); SetEvent(eSellGo); SetEvent(eDonGo);

    WaitForSingleObject(piDep.hProcess, INFINITE);
    WaitForSingleObject(piSell.hProcess, INFINITE);
    WaitForSingleObject(piDon.hProcess, INFINITE);

    string sold, don; ifstream(SOLD_FILE) >> sold; ifstream(DONATE_FILE) >> don;
    cout << "==============================" << endl;
    cout << "Sold total: " << sold << endl;
    cout << "Donatii totale: " << don << endl << flush;
    return 0;
}
