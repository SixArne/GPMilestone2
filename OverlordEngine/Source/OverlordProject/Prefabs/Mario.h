#pragma once
class Character;

class Mario : public GameObject
{
public:
	Mario() {};
	~Mario() override = default;

	Mario(const Mario& other) = delete;
	Mario(Mario&& other) noexcept = delete;
	Mario& operator=(const Mario& other) = delete;
	Mario& operator=(Mario&& other) noexcept = delete;

	void DrawImGui();

	int GetLives() { return m_Lives;  };
	int GetCoins() { return m_Coins;  };
	int GetSpecialCoins() { return m_SpecialCoins; };
	int GetMoons() { return m_Moons; };

protected:
	void Initialize(const SceneContext&) override;
	void PostInitialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	void InitializeSounds();
	
	ModelAnimator* pAnimator{};
	Character* m_pCharacter{};
	ModelComponent* m_pMarioModel{};
	FMOD::Channel* m_pJumpSound = nullptr;
	FMOD::Sound* m_pJumpSoundEffect = nullptr;

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

	int m_Lives = 3;
	int m_Coins = 0;
	int m_SpecialCoins = 0;
	int m_Moons = 0;
};