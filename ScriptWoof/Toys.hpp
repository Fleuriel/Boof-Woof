struct Toys final : public Behaviour 
{
	// If player touches the toys on the floor, timer will be reduced
	using Behaviour::Behaviour;
	
	bool touchingToy{ false }, minusTen{ false };
	double stunlockTimer = 1.5;	// 1.5 seconds
	double cooldownTimer = 0.0;

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
				// Only play sound the first time the player touches the toy
				if (!touchingToy) 
				{
					m_Engine.getAudioSystem().PlaySoundByFile("ToyTouch.wav", false, "SFX");
					touchingToy = true;
					m_Engine.SetTouched(true);
				}

				// Reduce timer only once per touch
				if (touchingToy && !minusTen) {
					double currTimer = m_Engine.GetTimerTiming();
					double newTimer = currTimer - 10.0;
					m_Engine.SetTimerTiming(newTimer);
					minusTen = true;
				}

				// Apply stunlock timer countdown
				if (stunlockTimer > 0.0) 
				{
					stunlockTimer -= m_Engine.GetDeltaTime();
				}
				else 
				{
					cooldownTimer += m_Engine.GetDeltaTime(); 

					// Cooldown 2 seconds before player can be stunned again
					if (cooldownTimer >= 2.0) 
					{
						touchingToy = false;
						stunlockTimer = 1.5; 
						minusTen = false; 
						cooldownTimer = 0.0; 
					}
				}
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