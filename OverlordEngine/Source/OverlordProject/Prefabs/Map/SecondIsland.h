#pragma once
class SecondIsland final: public GameObject
{
public:
	SecondIsland() = default;
	~SecondIsland() override = default;

	SecondIsland(const SecondIsland& other) = delete;
	SecondIsland(SecondIsland&& other) noexcept = delete;
	SecondIsland& operator=(const SecondIsland& other) = delete;
	SecondIsland& operator=(SecondIsland&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;
};

