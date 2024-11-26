#include "pch.h"
#include <vector>
#include <memory>

#include "Player.hpp"
#include "Treat.hpp"

extern "C"
{
	__declspec(dllexport) void* GetScripts(void* pEI_tmp)
	{
		engine_interface& EP = *reinterpret_cast<engine_interface*>(pEI_tmp);

		// using std::make_unique to manage memory internally
		auto pBehaviours = std::make_unique<std::vector<std::unique_ptr<Behaviour_i>>>();

		// Add all the scripts here
		pBehaviours->emplace_back(std::make_unique<Player>(EP));	// using std::make_unique that automatically allocates and manages memory instead of new
		pBehaviours->emplace_back(std::make_unique<Treat>(EP));	// using std::make_unique that automatically allocates and manages memory instead of new


		// release ownership of pointer and cast to void*
		return pBehaviours.release(); // Return raw pointer as void*, caller is responsible for cleanup (need to delete the vector to free memory by urself)
	}
}

