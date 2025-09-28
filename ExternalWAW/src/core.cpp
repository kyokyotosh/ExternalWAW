#include <iostream>
#include <vector>
#include <Windows.h>
#include "Proc.h"

int main()
{
    DWORD procId = GetProcId(L"CoDWaW.exe");
    uintptr_t moduleBase = GetModuleBaseAddress(procId, L"CoDWaW.exe");
    HANDLE hProcess = 0;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

    // Player
    uintptr_t playerBaseAddr = moduleBase + 0x14ED068;
    uintptr_t fovBaseAddr = moduleBase + 0x1DC4F98;
    uintptr_t healthBaseAddr = moduleBase + 0x136C8B8;


    std::vector<unsigned int> ammoOffset = {};
    uintptr_t ammoAddr = FindDMAAddy(hProcess, fovBaseAddr, ammoOffset);

    // Read Ammo Value
    float fovValue = 0.0f;
    ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &fovValue, sizeof(fovValue), nullptr);

    // Write To It
    float newFov = 120.0f;
    WriteProcessMemory(hProcess, (BYTE*)ammoAddr, &newFov, sizeof(newFov), nullptr);

    getchar();
    return 0;
}