
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
		std::cout << "Treat Update" << std::endl;
		//Check if player is colliding with treat
		if (m_Engine.IsColliding(entity))
		{

			//Get the name of the entity that is colliding with the treat
			const char* collidingEntityName = m_Engine.GetCollidingEntityName(entity);

			std::cout << "Scripted Entity is colliding with" << collidingEntityName << std::endl;

			//Check if the entity is the player
			if (strcmp(collidingEntityName, "Player") == 1)
			{
				//Play sound
				m_Engine.getAudioSystem().PlaySound("TreatSound");

				//Destroy the treat
				m_Engine.DestroyEntity(entity);
			}
		}

	}

	virtual void Destroy(Entity entity) override
	{
		UNREFERENCED_PARAMETER(entity);
		//std::cout << "Treat Destroy" << std::endl;
	}
};