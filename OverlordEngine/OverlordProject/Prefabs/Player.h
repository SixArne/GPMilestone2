#pragma once
class Player : public GameObject
{
public:
	Player();
	~Player() override = default;
	Player(const Player& other) = delete;
	Player(Player&& other) noexcept = delete;
	Player& operator=(const Player& other) = delete;
	Player& operator=(Player&& other) noexcept = delete;

	FreeCamera* GetCamera() const { return m_pCamera; }


protected:
	void Initialize(const SceneContext&) override;


	void PostInitialize(const SceneContext&) override;


	void Draw(const SceneContext&) override;


	void Update(const SceneContext&) override;


	virtual void OnImGui(const SceneContext&) override;

private:
	FreeCamera* m_pCamera{};
	float m_MoveSpeed{ 2.f };

};

