struct Treat final : public Behaviour
{
	using Behaviour::Behaviour;

	virtual void Init(Entity entity) override
	{
		UNREFERENCED_PARAMETER(entity);
		//std::cout << "Treat Init" << std::endl;
	}

	virtual void Update(Entity entity) override
	{
		//Check if player is colliding with treat
		if (m_Engine.IsColliding(entity))
		{

			//Get the name of the entity that is colliding with the treat
			const char* collidingEntityName = m_Engine.GetCollidingEntityName(entity);

			std::cout << "Scripted Entity is colliding with " << collidingEntityName << std::endl;

			//Check if the entity is the player
			if (std::strcmp(collidingEntityName, "Player") == 0) // If the entity is the player, destroy the treat
			{
				std::cout << "Player is colliding with treat" << std::endl;

				//Play sound
				m_Engine.getAudioSystem().PlaySound("TreatSound");

				//Destroy the treat
				m_Engine.getPhysicsSystem().RemoveBody(entity);
				m_Engine.DestroyEntity(entity);
			}
		}

	}

	virtual void Destroy(Entity entity) override
	{
		UNREFERENCED_PARAMETER(entity);
		//std::cout << "Treat Destroy" << std::endl;
	}

	virtual const char* getBehaviourName() override
	{
		return "Treat";
	}
};