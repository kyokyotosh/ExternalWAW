#include "Proc.h"
#include <tchar.h>

// Function to get the Process ID (PID) of a running process by its name
DWORD GetProcId(const TCHAR* procName) {
    DWORD procId = 0;

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hSnap, &procEntry)) {
            do {
                // Use _tcscmp for case-insensitive comparison
                if (!_tcscmp(procEntry.szExeFile, procName)) {
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
uintptr_t GetModuleBaseAddress(DWORD procId, const TCHAR* modName) {
    uintptr_t modBaseAddr = 0;

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);

        if (Module32First(hSnap, &modEntry)) {
            do {
                // Use _tcscmp for case-insensitive comparison
                if (!_tcscmp(modEntry.szModule, modName)) {
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

    uintptr_t addr = ptr;   // Start with the base pointer (often module base + offset)

    for (unsigned int i = 0; i < offsets.size(); ++i) {
        // Read memory at current address to resolve the pointer
        ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);

        // Add the offset to move deeper into the structure
        addr += offsets[i];
    }

    return addr;  // Return the final resolved address
}