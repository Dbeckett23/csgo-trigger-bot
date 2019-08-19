#include "MemoryManager.h"
#include <iostream>

MemoryManager * Mem;
bool TriggerToggled = false;

// Offsets
#define m_dwLocalPlayer 0xCF2A3C // 
#define m_iTeamNum 0xF4 // 
#define m_iCrossHairID 0xB3AC // 
#define m_dwEntityList 0x4D05AE4 // 
#define m_bDormant 0xED //
#define m_dwForceAttack 0x31371D0 // 

#define m_bIsScoped 0x3910 // To check if aimed in.

// End of offsets

void Trigger();

int main() {
	Mem = new MemoryManager();
	Trigger();
	delete Mem;
	return 0;
}


void Trigger() {
	while (true)
	{
		if (GetAsyncKeyState(VK_MENU)) { // Toggle key
			if (TriggerToggled == false) {
				TriggerToggled = true;
				std::cout << "Trigger bot is now ON" << std::endl;
			}
			else {
				TriggerToggled = false;
				std::cout << "Trigger bot if now OFF" << std::endl;
			}
			Sleep(1000);
		}
		//else {
		//	//std::cout << "TriggerBot now off" << std::endl;
		//	TriggerToggled = false;

		//}
		Sleep(25);


		// If not toggled skip this iteration of the loop
		if (!TriggerToggled) continue;

		//Retrieve player informations
		DWORD LocalPlayer_Base = Mem->Read<DWORD>(Mem->ClientDLL_Base + m_dwLocalPlayer);
		int LocalPlayer_inCross = Mem->Read<int>(LocalPlayer_Base + m_iCrossHairID);
		int LocalPlayer_Team = Mem->Read<int>(LocalPlayer_Base + m_iTeamNum);

		//Retrieve the EntityBase, using dwEntityList
		DWORD Trigger_EntityBase = Mem->Read<DWORD>(Mem->ClientDLL_Base + m_dwEntityList + ((LocalPlayer_inCross - 1) * 0x10));
		int Trigger_EntityTeam = Mem->Read<int>(Trigger_EntityBase + m_iTeamNum);

		//bDormant is a boolean value, the offset is 0xE9
		bool Trigger_EntityDormant = Mem->Read<bool>(Trigger_EntityBase + m_bDormant);

		if ((LocalPlayer_inCross > 0 && LocalPlayer_inCross <= 64) && (Trigger_EntityBase != NULL) && (Trigger_EntityTeam != LocalPlayer_Team) && (!Trigger_EntityDormant))
		{
			//Shoot here
			//External way, you can customize the delays
			Sleep(10); //Delay before shooting
			mouse_event(MOUSEEVENTF_LEFTDOWN, NULL, NULL, NULL, NULL);
			Sleep(5); //Delay between shots
			mouse_event(MOUSEEVENTF_LEFTUP, NULL, NULL, NULL, NULL);
			Sleep(5); //Delay after shooting

			// ***** This Methoid seems detectable my VAC memory *****
			////"Internal-like" way, you can customize delays
			//// Offset for m_dwForceAttack is 0x2EC89E8
			//Sleep(10); //Delay before shooting
			//Mem->Write<int>(Mem->ClientDLL_Base + m_dwForceAttack, 5);
			//Sleep(10); //Delay between shots
			//Mem->Write<int>(Mem->ClientDLL_Base + m_dwForceAttack, 4);
			//Sleep(10); //Delay after shooting
		}


	}
}