#pragma once
class Character;

class Mario : public GameObject
{
public:
	Mario();
	~Mario() override = default;

	Mario(const Mario& other) = delete;
	Mario(Mario&& other) noexcept = delete;
	Mario& operator=(const Mario& other) = delete;
	Mario& operator=(Mario&& other) noexcept = delete;

	void DrawImGui();

	int GetLives();
	int GetCoins();
	int GetSpecialCoins();
	int GetMoons();

	void TakeDamage();
	void TakeMoon();
	void TakeCoin();
	void TakeSpecialCoin();

	void SetLives(int lives) { m_Lives = lives; }

	void SetStartPosition(XMFLOAT3 position);
	XMFLOAT3 GetMarioLocation();

	void SetOnDieCallback(std::function<void()> callback) { m_OnDieCallback = callback; }

protected:
	void Initialize(const SceneContext&) override;
	void PostInitialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:

	void OnCollision(GameObject* pTriggerObject, GameObject* pOtherObject, PxTriggerAction action);
	void InitializeSounds();
	
	// Callbacks
	std::function<void()> m_OnDieCallback{};
	
	ModelAnimator* pAnimator{};
	ModelComponent* m_pMarioModel{};
	FMOD::Channel* m_pJumpSound = nullptr;
	FMOD::Channel* m_pHealthChannel = nullptr;
	FMOD::Sound* m_pJumpSoundEffect = nullptr;
	FMOD::Sound* m_pDamageSoundEffect = nullptr;
	FMOD::Sound* m_pLastLifeSoundEffect = nullptr;
	FMOD::Sound* m_pDeathSoundEffect = nullptr;

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

	int m_Lives{4};
	int m_Coins{0};
	int m_SpecialCoins{0};
	int m_Moons{0};

	GameObject* m_pVisuals{};
	Character* m_pCharacterController{};
};