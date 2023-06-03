#pragma once
class Character;
class Mario;
class BanzaiBill;
class GameHud;
class Mario;
class PauseMenu;

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

	void RemoveBulletBill(BanzaiBill* bill);
	void AddBulletBill(BanzaiBill* bill);

protected:
	void Initialize() override;
	void Update() override;
	void OnGUI() override;
	void PostDraw() override;

	void OnSceneActivated() override;
	void OnSceneDeactivated() override;

private:
	FMOD::Channel* m_pChannelBackgroundMusic = nullptr;
	FMOD::Channel* m_pChannel3DBills = nullptr;
	FMOD::Channel* m_pChannel3DExplosion = nullptr;

	FMOD::Sound* m_pLevelMusic = nullptr;
	FMOD::Sound* m_pRocketSound = nullptr;
	FMOD::Sound* m_pExplosionSound = nullptr;
	FMOD::Sound* m_pWonSound = nullptr;

	// List of bills for audio listeners
	std::vector<BanzaiBill*> m_Bills{};

	// Create methods
	void InitSound();
	void CreateMap();
	void CreatePlayer();
	void CreateEnemies();
	void CreateHud();
	void CreatePauseMenu();
	void CreatePostProcessEffect();
	void CreateCollectibles();
	void CreateSkyBox();
#ifdef _DEBUG
	void CreateLocationWriter();
#endif // DEBUG

	void ClearAudio();
	void OnGameOver();

	void UpdateHUDText();
	void UpdatePostProcess();
	void UpdateAudioListeners();

	// Moon callbacks
	void OnAllMoonsCollected();
	void OnMoonCollected();

	Mario* m_pMario{};
	PauseMenu* m_pPauseMenu{};

	// Post-Process variables
	const float m_RadiusMinValue{0.8f};

	float m_SinWave{};
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
	bool m_IsPaused{false};
	bool m_IsInitialized{ true };

	PostProcessingMaterial* m_pPostProcessEffect{};

	float m_ShadowMapScale{ 0.3f };
	XMFLOAT3 m_LightDirection{ -0.577f, -0.577f, 0.577f };
	FMOD_VECTOR m_PrevListenerPosition{};

	GameHud* m_pHud{};

	std::string m_FileToSaveTo{};

#ifdef _DEBUG
	LocationWriter m_LocationWriter;
#endif // DEBUG
};
