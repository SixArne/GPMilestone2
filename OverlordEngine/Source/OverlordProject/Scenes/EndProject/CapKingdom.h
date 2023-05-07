#pragma once
class Character;

class CapKingdom :
    public GameScene
{
public:
	CapKingdom() : GameScene(L"ModelTestScene") {}

	~CapKingdom() override = default;
	CapKingdom(const CapKingdom& other) = delete;
	CapKingdom(CapKingdom&& other) noexcept = delete;
	CapKingdom& operator=(const CapKingdom& other) = delete;
	CapKingdom& operator=(CapKingdom&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:
	GameObject* m_pModelGameObject{};
	FMOD::Channel* m_pBackgroundMusic = nullptr;
	FreeCamera* m_pCamera{};
	GameObject* m_pProtagonist{};

	class UberMaterial* m_DebugMaterial{};

	void CreateMap();
	void CreateFirstIsland();
	void CreateSecondIsland();
	void CreateBridge();
	void CreatePlayer();

	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterJump
	};

	Character* m_pCharacter{};
};
