#pragma once
class Character;
class Mario;
class BanzaiBill;

class CapKingdom :
    public GameScene
{
public:
	CapKingdom() : GameScene(L"CapkingdomScene") {}

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
	void PostDraw() override;

private:
	GameObject* m_pModelGameObject{};
	FMOD::Channel* m_pBackgroundMusic = nullptr;
	FMOD::Channel* m_pChannel3D = nullptr;

	FMOD::Sound* m_pSound = nullptr;
	FreeCamera* m_pCamera{};
	GameObject* m_pProtagonist{};

	class UberMaterial* m_DebugMaterial{};

	std::vector<BanzaiBill*> m_Bills{};

	void InitSound();

	void UpdateAudioListeners();

	void CreateMap();
	void CreateFirstIsland();
	void CreateSecondIsland();
	void CreateBridge();
	void CreatePlayer();
	void CreateEnemies();

	Character* m_pMario{};

	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterJump
	};


	bool m_DrawShadowMap{ false };
	bool m_HasStartedLevel{ false };

	float m_ShadowMapScale{ 0.3f };
	XMFLOAT3 m_LightDirection{ -0.577f, -0.577f, 0.577f };
	FMOD_VECTOR m_PrevListenerPosition{};
};
