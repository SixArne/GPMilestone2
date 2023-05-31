#pragma once
class Character;
class Mario;
class BanzaiBill;
class GameHud;
class Mario;

#include "Utils/LocationWriter.h"


class CapKingdom :
    public GameScene
{
public:
	CapKingdom() : GameScene(L"CapkingdomScene") {}

	~CapKingdom() override;
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

	void OnGameOver();

	void UpdateAudioListeners();

	void CreateMap();
	void CreateFirstIsland();
	void CreateSecondIsland();
	void CreateBridge();
	void CreatePlayer();
	void CreateEnemies();
	void CreateHud();
	void CreatePostProcessEffect();
	void CreateCollectibles();

#ifdef _DEBUG
	void CreateLocationWriter();
#endif // DEBUG
	void CreateLocationReader();


	void UpdateHUDText();
	void UpdatePostProcess();

	Mario* m_pMarioComponent{};

	float m_SinWave{};
	const float m_RadiusMinValue{0.8f};
	float m_Radius{m_RadiusMinValue};


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

	PostProcessingMaterial* m_pPostProcessEffect{};

	float m_ShadowMapScale{ 0.3f };
	XMFLOAT3 m_LightDirection{ -0.577f, -0.577f, 0.577f };
	FMOD_VECTOR m_PrevListenerPosition{};

	GameHud* m_pHud{};

	GameObject* m_dCoin{};

	int m_Lives{3};

#ifdef _DEBUG
	LocationWriter m_LocationWriter;
#endif // DEBUG
	LocationReader m_LocationReader;
};
