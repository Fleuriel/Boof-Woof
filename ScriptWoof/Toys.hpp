struct Toys final : public Behaviour 
{
	// If player touches the toys on the floor, timer will be reduced
	using Behaviour::Behaviour;
	
	//bool touchingPlayer{ false }, minusTen{ false }, cooldownActive{ false };
	//double stunlockTimer = 2.0;	// 2.0 seconds
	//double cooldownTimer = 0.0;
	//bool firstTouchProcessed = true;

	virtual void Init(Entity entity) override
	{
		UNREFERENCED_PARAMETER(entity);
	}

	virtual void Update(Entity entity) override
	{
		if (m_Engine.IsColliding(entity))
		{
			const char* collidingEntityName = m_Engine.GetCollidingEntityName(entity);

			if (std::strcmp(collidingEntityName, "Player") == 0)
			{

			}
		}	
	}

	virtual void Destroy(Entity entity) override
	{
		UNREFERENCED_PARAMETER(entity);
	}

	virtual const char* getBehaviourName() override
	{
		return "Toys";
	}
};