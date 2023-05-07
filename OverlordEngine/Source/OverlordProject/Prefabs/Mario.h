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

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	ModelAnimator* pAnimator{};
	Character* m_pCharacter{};

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
};