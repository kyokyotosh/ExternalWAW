#include "Proc.h"


bool PatchMemory(HANDLE hProcess, uintptr_t address, BYTE* patch, size_t size) {
    DWORD oldProtect;
    if (!VirtualProtectEx(hProcess, (LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return false;
    }
    if (!WriteProcessMemory(hProcess, (LPVOID)address, patch, size, nullptr)) {
        VirtualProtectEx(hProcess, (LPVOID)address, size, oldProtect, &oldProtect);
        return false;
    }
    VirtualProtectEx(hProcess, (LPVOID)address, size, oldProtect, &oldProtect);
    return true;
}