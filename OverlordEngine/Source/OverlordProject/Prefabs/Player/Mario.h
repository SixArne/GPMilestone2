#pragma once
class Character;

class Mario final: public GameObject
{
public:
	Mario(const SceneContext& context);
	~Mario() override = default;

	Mario(const Mario& other) = delete;
	Mario(Mario&& other) noexcept = delete;
	Mario& operator=(const Mario& other) = delete;
	Mario& operator=(Mario&& other) noexcept = delete;

	int GetLives();
	int GetCoins();
	int GetMoons();

	void TakeDamage();
	void TakeMoon();
	void TakeCoin();

	void SetOnAllMoonsCallback(std::function<void()> callback);
	void SetOnMoonCallback(std::function<void()> callback);

	void SetLives(int lives) { m_Lives = lives; }

	void SetStartPosition(XMFLOAT3 position);
	XMFLOAT3 GetMarioLocation();

	Character* GetCharacterController();
	const GameObject* GetVisuals() { return m_pVisuals; };

	void SetOnDieCallback(std::function<void()> callback) { m_OnDieCallback = callback; }
	void PostInitialize(const SceneContext&) override;

	bool IsDead() { return m_IsDead; };

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:

	void InitializeSounds();
	
	// Callbacks
	std::function<void()> m_OnDieCallback{};
	
	ModelAnimator* pAnimator{};
	ModelComponent* m_pMarioModel{};
	const SceneContext& m_pSceneContext;

	FMOD::Channel* m_pJumpSound = nullptr;
	FMOD::Channel* m_pHealthChannel = nullptr;
	FMOD::Channel* m_pCollectibleChannel = nullptr;

	FMOD::Sound* m_pJumpSoundEffect = nullptr;
	FMOD::Sound* m_pDamageSoundEffect = nullptr;
	FMOD::Sound* m_pLastLifeSoundEffect = nullptr;
	FMOD::Sound* m_pDeathSoundEffect = nullptr;
	FMOD::Sound* m_pCoinSoundEffect = nullptr;
	FMOD::Sound* m_pMoonSoundEffect = nullptr;

	int m_AnimationClipId{ 0 };
	float m_AnimationSpeed{ 1.f };

	std::vector<std::string> m_ClipNames{};
	UINT m_ClipCount{};

	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterJump
	};

	int m_Lives{3};
	int m_Coins{0};
	int m_SpecialCoins{0};
	int m_Moons{0};

	bool m_IsHurt{ false };
	bool m_WasHurt{ false };
	bool m_IsDead{ false };
	bool m_HasRecentlyDied{ false };
	bool m_HasInvincibilityFrames{false};
	bool m_IsPlayingDance{ false };

	float m_InvincibilityTimer{0.f};
	float m_InvincibilityTime{1.f};

	GameObject* m_pVisuals{};
	Character* m_pCharacterController{};

	std::function<void()> m_OnAllMoonsCallback{};
	std::function<void()> m_OnMoonCallback{};
};