#include "../BoofWoof/Core/Logic/BehaviourInterface.h"
#include <vector>
#include <memory>
#include "Player.hpp"

extern "C"
{
	__declspec(dllexport) void* GetScripts(void* pEI_tmp)
	{
		engine_interface& EP = *reinterpret_cast<engine_interface*>(pEI_tmp);
		std::vector<std::unique_ptr<Behaviour_i>>* pBehaviours = new std::vector<std::unique_ptr<Behaviour_i>>();

		// Add all the scripts here
		pBehaviours->emplace_back(new Player(EP));


		return pBehaviours;
	}
}

