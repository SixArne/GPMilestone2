#include "stdafx.h"
#include "GameHud.h"

void GameHud::OnImGui()
{
}

void GameHud::SetCoins(int coins)
{
	m_Coins = coins;
}

void GameHud::SetLives(int lives)
{
	switch (lives)
	{
	case 3:
		{
			m_pHealthSprite->SetTexture(L"Textures/Hud/hud_3_lives.png");
			m_Lives = lives;
		}
		break;
	case 2:
		{
			m_pHealthSprite->SetTexture(L"Textures/Hud/hud_2_lives.png");
			m_Lives = lives;
		}
		break;
	case 1:
		{
			m_pHealthSprite->SetTexture(L"Textures/Hud/hud_1_life.png");
			m_Lives = lives;
		}
		break;
	default:
		break;
	}
}

void GameHud::SetSpecialCoins(int score)
{
	m_SpecialCoins = score;
}

void GameHud::SetMoons(int moons)
{
	for (int i = 0; i < m_TotalMoons; ++i)
	{
		if (i < moons)
		{
			m_MoonSprites[i]->SetTexture(L"Textures/Hud/hud_moon_filled.png");
		}
		else
		{
			m_MoonSprites[i]->SetTexture(L"Textures/Hud/hud_moon_empty.png");
		}
	}
}

void GameHud::Initialize(const SceneContext& sceneContext)
{
	////////////////////////////////////////////////////////////////////
	// Health indicator
	////////////////////////////////////////////////////////////////////
	m_HealthHUD = new GameObject();
	m_pHealthSprite = m_HealthHUD->AddComponent(new SpriteComponent(L"Textures/Hud/hud_3_lives.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
	AddChild(m_HealthHUD);

	m_HealthHUD->GetTransform()->Translate(sceneContext.windowWidth - m_pHealthSprite->GetDimensions().x, 80, .9f);
	m_HealthHUD->GetTransform()->Scale(0.5f, 0.5f, 0.5f);


	auto coinSprite = new GameObject();
	coinSprite->AddComponent(new SpriteComponent(L"Textures/Hud/hud_coin_counter.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
	AddChild(coinSprite);

	coinSprite->GetTransform()->Translate(75, 75, .9f);
	coinSprite->GetTransform()->Scale(0.5f, 0.5f, 0.5f);


	auto specialCoinSprite = new GameObject();
	specialCoinSprite->AddComponent(new SpriteComponent(L"Textures/Hud/hud_regional_counter.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
	AddChild(specialCoinSprite);

	specialCoinSprite->GetTransform()->Translate(200, 75, .9f);
	specialCoinSprite->GetTransform()->Scale(0.5f, 0.5f, 0.5f);

	
	auto seperator = new GameObject();
	seperator->AddComponent(new SpriteComponent(L"Textures/Hud/hud_divider.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
	AddChild(seperator);

	seperator->GetTransform()->Translate(160, 110, .9f);
	seperator->GetTransform()->Scale(0.6f, 0.6f, 0.6f);


	auto moonRow = new GameObject();
	for (int i{}; i < m_TotalMoons; ++i)
	{
		auto moonSprite = new GameObject();
		auto moonSpriteComponent = moonSprite->AddComponent(new SpriteComponent(L"Textures/Hud/hud_moon_empty.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
		m_MoonSprites.emplace_back(moonSpriteComponent);

		moonSprite->GetTransform()->Translate(70.f + (i * 24.f), 140.f, 0.9f);
		moonSprite->GetTransform()->Scale(0.5f, 0.5f, 0.5f);

		moonRow->AddChild(moonSprite);
	}

	AddChild(moonRow);

	////////////////////////////////////////////////////////////////////
	// Health text
	////////////////////////////////////////////////////////////////////
	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt");

}

void GameHud::Update(const SceneContext& sceneContext)
{
	// TODO: Make seperate prefab of this
	XMFLOAT2 livesPosition{ sceneContext.windowWidth - 122, 65 };
	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(std::to_string(m_Lives)), livesPosition, XMFLOAT4{ 0,0,0,1 });

	XMFLOAT2 coinsPosition{ 100, 60 };
	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(PadText(m_Coins, 4)), coinsPosition, XMFLOAT4{ 1,1,1,1 });

	XMFLOAT2 specialCoinsPosition{ 220, 60 };
	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(PadText(m_SpecialCoins, 3)), specialCoinsPosition, XMFLOAT4{ 1,1,1,1 });
}

std::string GameHud::PadText(int number, int padSize)
{
	std::ostringstream ss;
	ss << std::setfill('0') << std::setw(padSize) << number;
	return ss.str();
}
