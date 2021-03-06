#include "Hooks.h"
#include "SDK\IClientMode.h"
#include "Utils\Utils.h"
#include "Features\Misc.h"

Misc    g_Misc;
Hooks   g_Hooks;

void Hooks::Init()
{
    Interfaces::Init();                         // Get interfaces
    g_pNetvars = std::make_unique<NetvarTree>();// Get netvars after getting interfaces as we use them

    // D3D Device pointer
    uintptr_t d3dDevice = **(uintptr_t**)(g_Utils.FindSignature("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);

    // VMTHooks
    //g_pHooks->pD3DDevice9Hook = std::make_unique<VMTHook>(reinterpret_cast<void*>(d3dDevice));
    g_Hooks.pClientModeHook = std::make_unique<VMTHook>(g_pClientMode);
    g_Hooks.pClientModeHook->Hook(24, (void*)Hooks::CreateMove); 


   
#ifdef _DEBUG       // Example on how to use external WIN console.
    AllocConsole();                                             // Alloc memory and create console    
    g_Utils.SetConsoleHandle(GetStdHandle(STD_OUTPUT_HANDLE));  // Get and save console handle to future use with WriteToConsole etc.    
    g_Utils.LogToConsole(L"Initialization Succeded \n");        // Log info to console
#endif
}



void Hooks::Restore()
{
    // Unhook every function we hooked and restore original one
    g_Hooks.pClientModeHook->Unhook(24);

#ifdef _DEBUG
    FreeConsole();  // Free allocated memory and remove console
#endif
}



bool __fastcall Hooks::CreateMove(IClientMode* thisptr, void* edx, float sampleInputFrametime, CUserCmd* pCmd)
{
    // Call original createmove before we start screwing with it
    auto oCreateMove = g_Hooks.pClientModeHook->GetOriginal<CreateMove_t>(24);
    oCreateMove(thisptr, sampleInputFrametime, pCmd);

    if (!pCmd || pCmd->command_number == 0)
        return false;

    // Some quick debug log, uncomment to check if everything works
//#ifdef _DEBUG
//    auto pLocal = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());
//    std::wstring str = std::to_wstring(pLocal->GetTeam());
//    g_Utils.LogToConsole(str);
//#endif // _DEBUG

    g_Misc.OnCreateMove(pCmd);

    // call shit
    return false;
}