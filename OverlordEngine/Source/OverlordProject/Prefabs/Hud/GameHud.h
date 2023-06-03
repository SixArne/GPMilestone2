#pragma once

#include <vector>

class GameHud final : public GameObject
{
public:
	GameHud() = default;
	~GameHud() override = default;
	GameHud(const GameHud& other) = delete;
	GameHud(GameHud&& other) noexcept = delete;
	GameHud& operator=(const GameHud& other) = delete;
	GameHud& operator=(GameHud&& other) noexcept = delete;

	void SetCoins(int coins);
	void SetLives(int lives);
	void SetSpecialCoins(int score);
	void SetMoons(int moons);

	void Toggle();

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	std::string PadText(int number, int padSize);

	int m_Coins = 0;
	int m_Lives = 3;
	int m_Moons = 0;
	int m_TotalMoons = 5;

	TextureData* m_pCoinTexture = nullptr;
	TextureData* m_pLifeTexture = nullptr;
	TextureData* m_pMoonTexture = nullptr;

	SpriteComponent* m_pHealthSprite{};

	std::vector<SpriteComponent*> m_MoonSprites{};

	GameObject* m_HealthHUD{};
	SpriteFont* m_pFont{};

	InputAction m_pDebug{};
};

