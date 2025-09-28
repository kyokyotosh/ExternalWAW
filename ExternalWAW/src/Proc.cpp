#include "Proc.h"
#include <wchar.h>
#include <vector>
#include <string>

// Helper function to convert char* to wchar_t*
std::wstring ConvertToWideString(const char* narrowStr) {
    if (!narrowStr) return std::wstring();
    int len = MultiByteToWideChar(CP_ACP, 0, narrowStr, -1, nullptr, 0);
    if (len == 0) return std::wstring();
    std::vector<wchar_t> buffer(len);
    MultiByteToWideChar(CP_ACP, 0, narrowStr, -1, buffer.data(), len);
    return std::wstring(buffer.data());
}

// Function to get the Process ID (PID) of a running process by its name
DWORD GetProcId(const wchar_t* procName) {
    DWORD procId = 0;

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hSnap, &procEntry)) {
            do {
                // Convert szExeFile (char[256]) to wide string
                std::wstring wideExeFile = ConvertToWideString(procEntry.szExeFile);
                if (wcscmp(wideExeFile.c_str(), procName) == 0) {
                    procId = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
        CloseHandle(hSnap);
    }
    return procId;
}



// Function to get the base address of a loaded module
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName) {
    uintptr_t modBaseAddr = 0;

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);

        if (Module32First(hSnap, &modEntry)) {
            do {
                // Convert szModule (CHAR[260]) to wide string
                std::wstring wideModName = ConvertToWideString(modEntry.szModule);
                if (wcscmp(wideModName.c_str(), modName) == 0) {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
        CloseHandle(hSnap);
    }
    return modBaseAddr;
}



// Function to resolve a Dynamic Memory Address (DMA) using pointer chain
// Example: base_ptr -> [offset1] -> [offset2] -> final address
uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets) {
    uintptr_t addr = ptr;

    for (unsigned int i = 0; i < offsets.size(); ++i) {
        ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
        addr += offsets[i];
    }

    return addr;
}