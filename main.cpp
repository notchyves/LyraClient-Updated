#include <iostream>
#include <windows.h>
#include <thread>
#include <wininet.h>
#include "src/Lyra/Client.hpp"
#include "src/Utils/Logger/Logger.hpp"
#include "src/Lyra/Config/Json/json.hpp"
#include "src/Lyra/SDK/SDK.hpp"
#include "src/Lyra/Config/ConfigManager.hpp"
#include "src/Lyra/Module/Modules/Tablist.hpp"
#include "src/Utils/RenderUtils.h"
#include "src/Lyra/Module/Modules/ModMenu.hpp"
#include "src/Lyra/Module/Modules/Notif.hpp"

DWORD WINAPI initialize(HMODULE instance){

    std::chrono::steady_clock::time_point lastBeatTime;

    if (GetConsoleWindow() == nullptr and true) {
        AllocConsole();
        SetConsoleTitleA("Lyra-Debug");
        FILE *out;
        freopen_s(&out, ("CONOUT$"), ("w"), stdout);
    }
    Client::initialize();


    ImGui::ShowDemoWindow();


    std::thread statusThread([&lastBeatTime]() {
        while (true) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastBeatTime);

            if (!Client::disable) {
                if (SDK::clientInstance != nullptr) {
                    if (SDK::clientInstance->getLocalPlayer() != nullptr && SDK::clientInstance->getLocalPlayer()->playerName != "") {
                        if (elapsed >= std::chrono::seconds(60)) {
                            Client::online.clear();
                            Client::onlineUsers.clear();
                            Client::onlineStaff.clear();
                            Client::onlineStaffp.clear();
                            Client::partners.clear();
                            std::string name = SDK::clientInstance->getLocalPlayer()->playerName;
                            //Client::onlineUsers.push_back(Utils::removeColorCodes(name));
                        }
                    }
                }
                Sleep(60);

            }
            else break;
        }
    });

    while (!Client::disable) {
        Sleep(50);
    }
    Client::UnInitialize();
    statusThread.detach();
    FreeLibraryAndExitThread(instance, 1);
}

BOOL APIENTRY DllMain(HMODULE instance, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH: {
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)initialize, instance, 0, 0);
        return TRUE;
        }

    case DLL_PROCESS_DETACH: {
        Logger::info("Saved Config");
        return TRUE;
    }
    }

    return TRUE;
}
