#include "../BoofWoof/Core/Logic/Behaviour.h"

struct Player final : public Behaviour
{
	using Behaviour::Behaviour;

	virtual void Update(Entity entity) override
	{	

	}

	virtual const char* getBehaviourName() override
	{
		return "Player";
	}
};