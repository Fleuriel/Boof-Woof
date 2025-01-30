struct Null final : public Behaviour
{
	using Behaviour::Behaviour;

	virtual void Init(Entity entity) override
	{
		UNREFERENCED_PARAMETER(entity);
		//std::cout << "Null Init" << std::endl;
	}

	virtual void Update(Entity entity) override
	{
		UNREFERENCED_PARAMETER(entity);
		//std::cout << "Null Update" << std::endl;
	}

	virtual void Destroy(Entity entity) override
	{
		UNREFERENCED_PARAMETER(entity);
		//std::cout << "Null Destroy" << std::endl;
	}

	virtual const char* getBehaviourName() override
	{
		return "Null";
	}
};