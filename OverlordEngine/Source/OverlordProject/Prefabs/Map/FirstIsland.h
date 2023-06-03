#pragma once
class FirstIsland final :public GameObject
{
public:
	FirstIsland();
	~FirstIsland() override;

	FirstIsland(const FirstIsland& other) = delete;
	FirstIsland(FirstIsland&& other) noexcept = delete;
	FirstIsland& operator=(const FirstIsland& other) = delete;
	FirstIsland& operator=(FirstIsland&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;
};

