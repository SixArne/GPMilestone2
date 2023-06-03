#include "stdafx.h"
#include <array>
#include "CapKingdom.h"

#include "Materials/DiffuseMaterial.h"
#include "Materials/ColorMaterial.h"
#include "Materials/UberMaterial.h"
#include "Materials/Post/PostVignette.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

#include "Utils/ClipboardUtil.h"
#include <Utils/TimerManager.h>
#include "Utils/LocationWriter.h"

#include "Prefabs/Hud/GameHud.h"
#include "Prefabs/Player/Mario.h"
#include "Prefabs/Enemies/BanzaiBill.h"
#include "Prefabs/Player/Character.h"
#include "Prefabs/Collectibles/Coin.h"
#include "Prefabs/Collectibles/Moon.h"
#include "Prefabs/SkyBox/SkyBox.h"
#include "Prefabs/PauseMenu/PauseMenu.h"
#include "Prefabs/Enemies/BanzaiBillLauncher.h"
#include "Prefabs/Map/FirstIsland.h"
#include "Prefabs/Map/SecondIsland.h"

CapKingdom::~CapKingdom()
{
	// Clean up timerManager singleton
	if (TimerManager::Get() != nullptr)
	{
		TimerManager::Destroy();
	}
}

// Bullet will remove itself to audio listener list
void CapKingdom::RemoveBulletBill(BanzaiBill* bill)
{
	auto it = std::find(begin(m_Bills), end(m_Bills), bill);
	m_Bills.erase(it);
}

// Bullet will add itself to audio listener list
void CapKingdom::AddBulletBill(BanzaiBill* bill)
{
	m_Bills.push_back(bill);
} 

void CapKingdom::Initialize()
{
	// Create timerManager singleton
	TimerManager::Create();

	if (m_IsInitialized)
	{
		auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
		m_SceneContext.pInput->AddInputAction(inputAction);

		inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
		m_SceneContext.pInput->AddInputAction(inputAction);

		inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
		m_SceneContext.pInput->AddInputAction(inputAction);

		inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
		m_SceneContext.pInput->AddInputAction(inputAction);

		inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
		m_SceneContext.pInput->AddInputAction(inputAction);

		inputAction = InputAction(PauseMenu::PauseMenuOptions::Open, InputState::pressed, VK_ESCAPE, -1, XINPUT_GAMEPAD_Y);
		m_SceneContext.pInput->AddInputAction(inputAction);

		m_SceneContext.settings.enableOnGUI = true;
		m_SceneContext.settings.drawPhysXDebug = false;
		m_SceneContext.settings.drawGrid = false;
		m_SceneContext.useDeferredRendering = false;

		m_SceneContext.pLights->SetDirectionalLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });
		m_SceneContext.pInput->AddInputAction(InputAction(0, InputState::pressed, VK_DELETE));

		// Debug writing to save file
#ifdef _DEBUG
		inputAction = InputAction(1000, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_B);
		m_SceneContext.pInput->AddInputAction(inputAction);
#endif // _DEBUG
	}

	CreateMap();
	CreatePlayer();
	CreatePostProcessEffect();
	CreateHud();
	CreateSkyBox();
	CreatePauseMenu();
	CreateCollectibles();
	CreateEnemies();

	// Create debug save file writer
#ifdef _DEBUG
	CreateLocationWriter();
#endif // _DEBUG
}

void CapKingdom::Update()
{
	// Move light with mario to make sure that shadowmap follows
	const auto pCameraTransform = m_pMario->GetCharacterController()->GetTransform();
	auto oldDirection = m_SceneContext.pLights->GetDirectionalLight().direction;
	m_SceneContext.pLights->SetDirectionalLight(pCameraTransform->GetWorldPosition(), XMFLOAT3{oldDirection.x, oldDirection.y, oldDirection.z});

	// Menu open logic
	if (m_SceneContext.pInput->IsActionTriggered(PauseMenu::PauseMenuOptions::Open))
	{
		// Pause menu will show and hud will hide first time
		m_pPauseMenu->Toggle(); //  default is false
		m_pHud->Toggle(); // default is true

		if (m_pPauseMenu->IsActive())
		{
			m_SceneContext.pGameTime->Stop();
			m_pChannelBackgroundMusic->setPaused(true);
			m_pChannel3DBills->setPaused(true);
			m_IsPaused = true;
		}
		else
		{
			m_SceneContext.pGameTime->Start();
			m_pChannelBackgroundMusic->setPaused(false);
			m_pChannel3DBills->setPaused(false);
			m_IsPaused = false;
		}
	}

	// Prevent level update if game is paused
	if (m_IsPaused) return;

	// Location writing logic.
#ifdef _DEBUG
	if (m_SceneContext.pInput->IsActionTriggered(1000))
	{
		std::cout << "writing location data" << std::endl;
		m_LocationWriter.WriteLocation(m_pMario->GetCharacterController()->GetTransform()->GetWorldPosition());

		auto q = m_pMario->GetVisuals()->GetTransform()->GetWorldRotation();
		XMFLOAT3 angles{};


		// pitch (y-axis rotation)
		float sinp = +2.0f * (q.w * q.y - q.z * q.x);
		if (fabs(sinp) >= 1)
			angles.y = copysign(3.14159f / 2.f, sinp); // use 90 degrees if out of range
		else
			angles.y = asin(sinp);

		angles.y *= (180.f / 3.14159f);

		m_LocationWriter.WriteRotation(angles);
	}
#endif // _DEBUG

	// Start the audio if it hasn't started yet
	if (!m_HasStartedLevel)
	{
		InitSound();
		m_HasStartedLevel = true;
	}

	UpdateAudioListeners(); // Will be updating bills
	UpdateHUDText(); // Will be updating coins, moons and lives
	UpdatePostProcess(); // Will be updating the low health vignette

	// Update the TimerManager.
	TimerManager::Get()->Update(m_SceneContext.pGameTime->GetElapsedReal());
}

void CapKingdom::PostDraw()
{
	if (m_DrawShadowMap)
	{
		ShadowMapRenderer::Get()->Debug_DrawDepthSRV({ m_SceneContext.windowWidth - 10.f, 10.f }, { m_ShadowMapScale, m_ShadowMapScale }, { 1.f,0.f });
	}
}

void CapKingdom::OnSceneActivated()
{
	// This will only run the 2nd time the scene is re-activated.
	if (!m_IsInitialized)
	{
		Initialize();
		m_pMario->PostInitialize(m_SceneContext);
	}
}

void CapKingdom::OnSceneDeactivated()
{
	// Cleanup scene
	m_SceneContext.pCamera->SetActive(false);

	for (size_t i{m_pChildren.size() - 1}; i >= 1; --i)
	{
		RemoveChild(m_pChildren[i], true);
	}

	m_Bills.clear();
	ClearAudio();

	m_IsInitialized = false;
	m_HasStartedLevel = false;
	m_IsPaused = false;

	TimerManager::Destroy();
}

void CapKingdom::OnGUI()
{
	GameScene::OnGUI();

	ImGui::DragFloat3("direction", &m_LightDirection.x, 0.1f, -1.f, 1.f);
	m_SceneContext.pLights->SetDirectionalLight(m_pMario->GetTransform()->GetWorldPosition(), m_LightDirection);

	ImGui::Checkbox("Draw ShadowMap", &m_DrawShadowMap);
	ImGui::SliderFloat("ShadowMap Scale", &m_ShadowMapScale, 0.f, 1.f);

	// Debug file chooser for data
#ifdef _DEBUG
	ImGui::InputText("Input", m_FileToSaveTo.data(), ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::Text("using %s", m_FileToSaveTo.data());
	if (ImGui::Button("use file", ImVec2{ 100, 50 }))
	{
		std::string newFileName = std::format("spawndata/{}.txt", m_FileToSaveTo.c_str());

		m_LocationWriter.SetFile(newFileName);
		std::cout << "Now writing to: "<< newFileName << std::endl;
	}
#endif
}

void CapKingdom::InitSound()
{
	// Init the background music
	SoundManager::Get()->GetSystem()->createStream("Resources/Sound/level_theme.mp3", FMOD_DEFAULT, nullptr, &m_pLevelMusic);
	m_pLevelMusic->setMode(FMOD_LOOP_NORMAL);
	m_pLevelMusic->set3DMinMaxDistance(0.f, 100.f);

	// Play background music
	SoundManager::Get()->GetSystem()->playSound(m_pLevelMusic, nullptr, false, &m_pChannelBackgroundMusic);
	m_pChannelBackgroundMusic->setVolume(0.1f);

	// Init the 3D sounds
	auto result = SoundManager::Get()->GetSystem()->createStream("Resources/Sound/rocket.mp3", FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &m_pRocketSound);
	result = SoundManager::Get()->GetSystem()->createStream("Resources/Sound/explosion.mp3", FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &m_pExplosionSound);
	result = SoundManager::Get()->GetSystem()->createStream("Resources/Sound/won.mp3", FMOD_DEFAULT, 0, &m_pWonSound);

	// Start the 3D audio players
	result = SoundManager::Get()->GetSystem()->playSound(m_pRocketSound, nullptr, false, &m_pChannel3DBills);
	result = SoundManager::Get()->GetSystem()->playSound(m_pExplosionSound, nullptr, false, &m_pChannel3DExplosion);

	// Configure 3D audio players
	m_pChannel3DBills->setMode(FMOD_LOOP_NORMAL);
	m_pChannel3DBills->setVolume(0.3f);
	m_pChannel3DBills->set3DMinMaxDistance(1.f, 200.f);

	m_pChannel3DExplosion->setMode(FMOD_LOOP_OFF);
	m_pChannel3DExplosion->setVolume(0.3f);
	m_pChannel3DExplosion->set3DMinMaxDistance(1.f, 50.f);
}

void CapKingdom::OnGameOver()
{
	// Exit level after 3 seconds.
	TimerManager::Get()->CreateTimer(3.f, [this]() {
		m_pPostProcessEffect->SetIsEnabled(false);
		SceneManager::Get()->PreviousScene();
	});
}

inline FMOD_VECTOR ToFMod(XMFLOAT3 v)
{
	return FMOD_VECTOR{ v.x, v.y, v.z };
}

inline FMOD_VECTOR ToFMod(PxVec3 v)
{
	return FMOD_VECTOR{ v.x, v.y, v.z };
}

void CapKingdom::UpdateAudioListeners()
{
	auto playerPosition = m_pMario->GetCharacterController();

	auto pos = ToFMod(playerPosition->GetTransform()->GetWorldPosition());
	auto forward = ToFMod(playerPosition->GetTransform()->GetForward());
	auto up = ToFMod(playerPosition->GetTransform()->GetUp());

	FMOD_VECTOR vel{};
	auto dt = m_SceneContext.pGameTime->GetElapsed();
	vel.x = (pos.x - m_PrevListenerPosition.x) / dt;
	vel.y = (pos.y - m_PrevListenerPosition.y) / dt;
	vel.z = (pos.z - m_PrevListenerPosition.z) / dt;
	m_PrevListenerPosition = pos;

	SoundManager::Get()->GetSystem()->set3DListenerAttributes(0, &pos, &vel, &forward, &up);

	// Mute bill audio when no bills are flying
	if (m_Bills.empty())
	{
		m_pChannel3DBills->setPaused(true);
	}
	else
	{
		m_pChannel3DBills->setPaused(false);
	}

	// Update bill audio
	for (auto bill : m_Bills)
	{
		auto spherePos = ToFMod(bill->GetTransform()->GetWorldPosition());
		auto sphereVelocity = ToFMod(bill->GetRigidBody()->GetPxRigidActor()->is<PxRigidDynamic>()->getLinearVelocity());

		m_pChannel3DBills->set3DAttributes(&spherePos, &sphereVelocity);

		if (bill->IsMarkedForDestruction())
		{
			SoundManager::Get()->GetSystem()->playSound(m_pExplosionSound, nullptr, false, &m_pChannel3DExplosion);
		}
	}
}

void CapKingdom::CreateMap()
{
	AddChild(new FirstIsland());
	AddChild(new SecondIsland());
}

void CapKingdom::CreatePlayer()
{
	m_pMario = AddChild(new Mario(m_SceneContext));
	m_pMario->SetLives(3);

	m_pMario->SetStartPosition(XMFLOAT3{ -560, 0, -553 });

	m_pMario->SetOnDieCallback([this]() 
		{
			OnGameOver();
		});

	m_pMario->SetOnAllMoonsCallback([this]() {

			OnAllMoonsCollected();
		});

	m_pMario->SetOnMoonCallback([this]() {

		OnMoonCollected();
		
	});
}

void CapKingdom::CreateEnemies()
{
	// Read location and rotation data from files.
	auto billReader = LocationReader("spawndata/bills.txt");
	auto locations = billReader.ReadLocations();
	auto rotations = billReader.ReadRotations();

	std::vector<BanzaiBillLauncher*> launchers{};

	for (auto loc : locations)
	{
		auto billLauncher = AddChild(new BanzaiBillLauncher(m_pMario));
		billLauncher->GetTransform()->Translate(loc.x, loc.y - 7.f, loc.z);

		// Add for later rotation
		launchers.push_back(billLauncher);
	}

	// Apply rotations
	for (int i{}; i < rotations.size(); i++)
	{
		launchers[i]->GetTransform()->Rotate(rotations[i].x, rotations[i].y + 180.f, rotations[i].z);
	}
}

void CapKingdom::CreateHud()
{
	m_pHud = AddChild(new GameHud());
}

void CapKingdom::CreatePauseMenu()
{
	m_pPauseMenu = AddChild(new PauseMenu());

	m_pPauseMenu->SetOnRestartCallback([this]()
		{
			SceneManager::Get()->ReloadScene();
		});

	m_pPauseMenu->SetOnMenuCallback([this]() 
		{
			SceneManager::Get()->PreviousScene();
		});

	m_pPauseMenu->SetOnQuitCallback([]()
		{
			PostQuitMessage(0);
		});
}

void CapKingdom::CreatePostProcessEffect()
{
	m_pPostProcessEffect = MaterialManager::Get()->CreateMaterial<PostVignette>();
	AddPostProcessingEffect(m_pPostProcessEffect);

	m_pPostProcessEffect->SetIsEnabled(false);
	reinterpret_cast<PostVignette*>(m_pPostProcessEffect)->SetRadius(1.1f);
}

void CapKingdom::CreateCollectibles()
{
	// Get location data from file.
	auto coinReader = LocationReader("spawndata/coins.txt");
	auto locations = coinReader.ReadLocations();

	for (auto loc : locations)
	{
		auto coin = AddChild(new Coin());
		coin->GetTransform()->Translate(loc.x, loc.y + 1, loc.z);
	}

	// Get location data from file.
	auto moonReader = LocationReader("spawndata/moons.txt");
	locations = moonReader.ReadLocations();

	for (auto loc : locations)
	{
		auto moon = AddChild(new Moon());
		moon->GetTransform()->Translate(loc.x, loc.y + 1, loc.z);
	}
}

void CapKingdom::CreateSkyBox()
{
	AddChild(new SkyBox());
}

void CapKingdom::ClearAudio()
{
	m_pChannelBackgroundMusic->setPaused(true);
	m_pChannel3DBills->setPaused(true);
	m_pChannel3DExplosion->setPaused(true);
}

void CapKingdom::UpdateHUDText()
{
	m_pHud->SetCoins(m_pMario->GetCoins());
	m_pHud->SetLives(m_pMario->GetLives());
	m_pHud->SetMoons(m_pMario->GetMoons());
}

void CapKingdom::UpdatePostProcess()
{
	// Sin wave will smoothly animate our post-process effect
	m_SinWave += m_SceneContext.pGameTime->GetElapsed() * 2;

	if (m_SinWave >= 3.1459f)
	{
		m_SinWave = 0.f;
	}

	reinterpret_cast<PostVignette*>(m_pPostProcessEffect)->SetTime(m_SinWave);

	// Enable post process only when mario is low
	if (m_pMario->GetLives() <= 1)
	{
		m_pPostProcessEffect->SetIsEnabled(true);
	}
	else
	{
		m_pPostProcessEffect->SetIsEnabled(false);
	}
}

void CapKingdom::OnAllMoonsCollected()
{
	FMOD::Channel* pChannel;
	SoundManager::Get()->GetSystem()->playSound(m_pWonSound, nullptr, false, &pChannel);
	pChannel->setVolume(0.7f);

	// Mute all other sounds while mario prefab plays its own sound
	m_pChannel3DBills->setPaused(true);
	m_pChannel3DExplosion->setPaused(true);
	m_pChannelBackgroundMusic->setPaused(true);

	// Unmute after x seconds;
	TimerManager::Get()->CreateTimer(13.f, [this]() {
		m_pChannelBackgroundMusic->setPaused(false);
		m_pChannel3DBills->setPaused(false);
		m_pChannel3DExplosion->setPaused(false);

		SceneManager::Get()->PreviousScene();
	});
}

void CapKingdom::OnMoonCollected()
{
	// Mute all other sounds while mario prefab plays its own sound
	m_pChannel3DBills->setPaused(true);
	m_pChannel3DExplosion->setPaused(true);
	m_pChannelBackgroundMusic->setPaused(true);

	// Unmute after x seconds;
	TimerManager::Get()->CreateTimer(3.5f, [this]() {
		m_pChannelBackgroundMusic->setPaused(false);
		m_pChannel3DBills->setPaused(false);
		m_pChannel3DExplosion->setPaused(false);
	});
}

#ifdef _DEBUG
void CapKingdom::CreateLocationWriter()
{
	m_LocationWriter = LocationWriter("data.txt");
}
#endif