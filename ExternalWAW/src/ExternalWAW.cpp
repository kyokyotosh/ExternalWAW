#include "gui.h"
#include "Proc.h"
#include "Common.h"
#include <thread>
#include <iostream>

HANDLE hProcess = nullptr;
uintptr_t fovAddr = 0;
uintptr_t moneyAddr = 0;
uintptr_t healthAddr = 0;

int __stdcall wWinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    PWSTR arguments,
    int commandShow)
{
    // Get ProcID of the target process
    DWORD procId = GetProcId(L"CoDWaW.exe");
    if (procId == 0) {
        std::cout << "Failed to find process ID for CoDWaW.exe" << std::endl;
        return EXIT_FAILURE;
    }

    // GetModuleBaseAddress
    uintptr_t moduleBase = GetModuleBaseAddress(procId, L"CoDWaW.exe");
    if (moduleBase == 0) {
        std::cout << "Failed to find module base address" << std::endl;
        return EXIT_FAILURE;
    }

    // Get Handle To Process
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
    if (!hProcess) {
        std::cout << "Failed to open process!" << std::endl;
        return EXIT_FAILURE;
    }

    // Resolve Base Address of the Player pointer chain
    uintptr_t playerBaseAddr = moduleBase + 0x14ED068;
    uintptr_t moneyBaseAddr = moduleBase + 0x14EF124;
    uintptr_t fovBaseAddr = moduleBase + 0x1DC4F98;
    uintptr_t healthBaseAddr = moduleBase + 0x136C8B8;

    std::vector<unsigned int> moneyOffset = {  };
    moneyAddr = FindDMAAddy(hProcess, moneyBaseAddr, moneyOffset);
    if (moneyAddr == 0) {
        std::cout << "Failed to resolve Money address" << std::endl;
        CloseHandle(hProcess);
        return EXIT_FAILURE;
    }

    std::vector<unsigned int> fovOffset = {}; // Empty for direct address
    fovAddr = FindDMAAddy(hProcess, fovBaseAddr, fovOffset);
    if (fovAddr == 0) {
        std::cout << "Failed to resolve FOV address" << std::endl;
        CloseHandle(hProcess);
        return EXIT_FAILURE;
    }

    std::vector<unsigned int> healthOffset = {}; // Empty for direct address
    healthAddr = FindDMAAddy(hProcess, healthBaseAddr, healthOffset);
    if (healthAddr == 0) {
        std::cout << "Failed to resolve Health address" << std::endl;
        CloseHandle(hProcess);
        return EXIT_FAILURE;
    }

    // Create GUI
    gui::CreateHWindow("Cheat Menu");
    if (!gui::CreateDevice()) {
        std::cout << "Failed to create DirectX device" << std::endl;
        CloseHandle(hProcess);
        return EXIT_FAILURE;
    }
    gui::CreateImGui();

    // Main loop
    while (gui::isRunning) {
        gui::BeginRender();
        gui::Render();
        gui::EndRender();

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    // Destroy GUI
    gui::DestroyImGui();
    gui::DestroyDevice();
    gui::DestroyHWindow();

    // Cleanup process handle
    if (hProcess) {
        CloseHandle(hProcess);
        hProcess = nullptr;
    }

    return EXIT_SUCCESS;
}