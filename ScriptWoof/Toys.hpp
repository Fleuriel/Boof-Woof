struct Toys final : public Behaviour 
{
	// If player touches the toys on the floor, timer will be reduced
	using Behaviour::Behaviour;

	virtual void Init(Entity entity) override
	{
		UNREFERENCED_PARAMETER(entity);
	}

	virtual void Update(Entity entity) override
	{
		if (m_Engine.IsColliding(entity))
		{
			const char* collidingEntityName = m_Engine.GetCollidingEntityName(entity);

			//std::cout << "Scripted Entity is colliding with " << collidingEntityName << std::endl;

			if (std::strcmp(collidingEntityName, "Player") == 0)
			{
				//std::cout << "Player is colliding with toys" << std::endl;

				//Play sound

				m_Engine.getAudioSystem().PlaySoundByFile("ToyTouch.wav", false, "SFX");
			

				//Destroy the toys
				m_Engine.getPhysicsSystem().RemoveBody(entity);
				m_Engine.DestroyEntity(entity);

				//Reduce timer by 10 seconds
				 m_Engine.SetTouched(true);

				double currTimer = m_Engine.GetTimerTiming();
				double newTimer = currTimer - 10.0;
				m_Engine.SetTimerTiming(newTimer);
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