#pragma once
#include <windows.h>

extern HANDLE hProcess;
extern uintptr_t fovAddr;
extern uintptr_t moneyAddr;
extern uintptr_t healthAddr;
extern uintptr_t recoilAddr;

extern bool isRunning;

bool PatchMemory(HANDLE hProcess, uintptr_t address, BYTE* patch, size_t size);