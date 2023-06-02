#include "stdafx.h"
#include "CapKingdom.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/ColorMaterial.h"
#include "Materials/UberMaterial.h"
#include "Utils/ClipboardUtil.h"
#include "Prefabs/Mario.h"
#include "Prefabs/Character.h"
#include "Prefabs/Enemies/BanzaiBill.h"
#include "Materials/Post/PostVignette.h"
#include "Prefabs/Hud/GameHud.h"
#include <Utils/TimerManager.h>
#include "Utils/LocationWriter.h"
#include "Prefabs/Collectibles/Coin.h"
#include "Prefabs/Collectibles/Moon.h"
#include "Prefabs/SkyBox/SkyBox.h"
#include "Prefabs/PauseMenu/PauseMenu.h"
#include <array>

#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

CapKingdom::~CapKingdom()
{
	if (TimerManager::Get() != nullptr)
	{
		TimerManager::Destroy();
	}
}

void CapKingdom::Initialize()
{
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



#ifdef _DEBUG
	CreateLocationWriter();
#endif // _DEBUG
	CreateCollectibles();


	CreateEnemies();
}

void CapKingdom::Update()
{
	if (m_SceneContext.pInput->IsActionTriggered(PauseMenu::PauseMenuOptions::Open))
	{
		m_pPauseMenu->Toggle();
		m_pHud->Toggle();

		if (m_pPauseMenu->IsActive())
		{
			m_SceneContext.pGameTime->Stop();
			m_pBackgroundMusic->setPaused(true);
			m_pChannel3DBills->setPaused(true);
			m_IsPaused = true;
		}
		else
		{
			m_SceneContext.pGameTime->Start();
			m_pBackgroundMusic->setPaused(false);
			m_pChannel3DBills->setPaused(false);
			m_IsPaused = false;
		}
	}

	if (m_IsPaused) return;

#ifdef _DEBUG
	if (m_SceneContext.pInput->IsActionTriggered(1000))
	{
		std::cout << "writing location data" << std::endl;
		m_LocationWriter.WriteLocation(m_pMarioComponent->GetCharacterController()->GetTransform()->GetWorldPosition());
	}
#endif // _DEBUG

	

	if (!m_HasStartedLevel)
	{
		InitSound();
		m_HasStartedLevel = true;
	}

	UpdateAudioListeners();
	UpdateHUDText();
	UpdatePostProcess();
	TimerManager::Get()->Update(m_SceneContext.pGameTime->GetElapsedReal());
}

void CapKingdom::Draw()
{


}

void CapKingdom::PostDraw()
{
	if (m_IsPaused) return;

	if (m_DrawShadowMap)
	{
		ShadowMapRenderer::Get()->Debug_DrawDepthSRV({ m_SceneContext.windowWidth - 10.f, 10.f }, { m_ShadowMapScale, m_ShadowMapScale }, { 1.f,0.f });
	}
}

void CapKingdom::OnSceneActivated()
{
	if (!m_IsInitialized)
	{
		Initialize();
	}

	m_pMarioComponent->PostInitialize(m_SceneContext);
}

void CapKingdom::OnSceneDeactivated()
{
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
	
	m_DebugMaterial->DrawImGui();
	m_pMarioComponent->DrawImGui();

	ImGui::DragFloat3("direction", &m_LightDirection.x, 0.1f, -1.f, 1.f);
	m_SceneContext.pLights->SetDirectionalLight(m_pMarioComponent->GetTransform()->GetWorldPosition(), m_LightDirection);

	ImGui::Checkbox("Draw ShadowMap", &m_DrawShadowMap);
	ImGui::SliderFloat("ShadowMap Scale", &m_ShadowMapScale, 0.f, 1.f);

#ifdef _DEBUG
	ImGui::InputText("Input", m_FileToSaveTo.data(), ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::Text("using %s", m_FileToSaveTo.data());
	if (ImGui::Button("use file", ImVec2{ 100, 50 }))
	{
		m_LocationWriter.SetFile(m_FileToSaveTo);
		std::cout << "Now writing to: "<< m_FileToSaveTo << std::endl;
	}
#endif
}

void CapKingdom::InitSound()
{

	SoundManager::Get()->GetSystem()->createStream("Resources/Sound/altar_cave.mp3", FMOD_DEFAULT, nullptr, &m_pSound);
	m_pSound->setMode(FMOD_LOOP_NORMAL);
	m_pSound->set3DMinMaxDistance(0.f, 100.f);


	SoundManager::Get()->GetSystem()->playSound(m_pSound, nullptr, false, &m_pBackgroundMusic);
	m_pBackgroundMusic->setVolume(0.1f);


	auto result = SoundManager::Get()->GetSystem()->createStream("Resources/Sound/rocket.mp3", FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &m_pRocketSound);
	result = SoundManager::Get()->GetSystem()->createStream("Resources/Sound/explosion.mp3", FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &m_pExplosionSound);
	result = SoundManager::Get()->GetSystem()->createStream("Resources/Sound/won.mp3", FMOD_DEFAULT, 0, &m_pWonSound);

	result = SoundManager::Get()->GetSystem()->playSound(m_pRocketSound, nullptr, false, &m_pChannel3DBills);
	result = SoundManager::Get()->GetSystem()->playSound(m_pExplosionSound, nullptr, false, &m_pChannel3DExplosion);

	m_pChannel3DBills->setMode(FMOD_LOOP_NORMAL);
	m_pChannel3DBills->setVolume(0.3f);
	m_pChannel3DBills->set3DMinMaxDistance(1.f, 200.f);

	m_pChannel3DExplosion->setMode(FMOD_LOOP_OFF);
	m_pChannel3DExplosion->setVolume(0.3f);
	m_pChannel3DExplosion->set3DMinMaxDistance(1.f, 50.f);
}

void CapKingdom::OnGameOver()
{
	std::cout << "Timer started" << std::endl;
	TimerManager::Get()->CreateTimer(3.5f, []() {
		std::cout << "Mario has died" << std::endl;

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
	auto marioPos = m_pMarioComponent->GetCharacterController();

	auto pos = ToFMod(marioPos->GetTransform()->GetWorldPosition());
	auto forward = ToFMod(marioPos->GetTransform()->GetForward());
	auto up = ToFMod(marioPos->GetTransform()->GetUp());

	FMOD_VECTOR vel{};
	auto dt = m_SceneContext.pGameTime->GetElapsed();
	vel.x = (pos.x - m_PrevListenerPosition.x) / dt;
	vel.y = (pos.y - m_PrevListenerPosition.y) / dt;
	vel.z = (pos.z - m_PrevListenerPosition.z) / dt;
	m_PrevListenerPosition = pos;

	SoundManager::Get()->GetSystem()->set3DListenerAttributes(0, &pos, &vel, &forward, &up);

	// source

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
	CreateFirstIsland();
	CreateSecondIsland();
	CreateBridge();
}

void CapKingdom::CreateFirstIsland()
{
	GameObject* entrance = new GameObject();

	float normalStrength = 0.4f;

	// Setup textures
	UberMaterial* pStoneWallMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pStoneWallMaterial->SetDiffuseTexture(L"Meshes/WallStone00.004_diffuse.png");
	pStoneWallMaterial->SetNormalTexture(L"Meshes/WallStone00.004_normal.png");
	pStoneWallMaterial->SetSpecularTexture(L"Meshes/WallStone00.004_spec.png");
	pStoneWallMaterial->SetNormalStrength(normalStrength);
	pStoneWallMaterial->SetVariable_Scalar(L"gUseSpecularPhong", false);
	pStoneWallMaterial->SetVariable_Scalar(L"gUseSpecularBlinn", false);

	UberMaterial* pBrickWallMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pBrickWallMaterial->SetDiffuseTexture(L"Meshes/WallBrick00.005_diffuse.png");
	pBrickWallMaterial->SetNormalTexture(L"Meshes/WallBrick00.005_normal.png");
	pBrickWallMaterial->SetSpecularTexture(L"Meshes/WallBrick00.005_spec.png");
	pBrickWallMaterial->SetNormalStrength(normalStrength);
	pBrickWallMaterial->SetVariable_Scalar(L"gUseSpecularPhong", false);
	pBrickWallMaterial->SetVariable_Scalar(L"gUseSpecularBlinn", false);

	m_DebugMaterial = pBrickWallMaterial;

	DiffuseMaterial_Shadow* pStripes = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pStripes->SetDiffuseTexture(L"Meshes/StripeCamouflageSingleLayer00.004_diffuse.png");

	UberMaterial* pMetalFenceMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pMetalFenceMaterial->SetDiffuseTexture(L"Meshes/MetalFence01.004_diffuse.png");
	pMetalFenceMaterial->SetNormalTexture(L"Meshes/MetalFence01.004_normal.png");
	pMetalFenceMaterial->SetSpecularTexture(L"Meshes/MetalFence01.004_spec.png");

	UberMaterial* pLampMetalMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pLampMetalMaterial->SetDiffuseTexture(L"Meshes/LampMetal00.003_diffuse.png");
	pLampMetalMaterial->SetNormalTexture(L"Meshes/LampMetal00.003_normal.png");
	pLampMetalMaterial->SetSpecularTexture(L"Meshes/LampMetal00.003_spec.png");

	UberMaterial* pHatMetalMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pHatMetalMaterial->SetDiffuseTexture(L"Meshes/HatMetal03.003_diffuse.png");
	pHatMetalMaterial->SetNormalTexture(L"Meshes/HatMetal03.003_normal.png");

	DiffuseMaterial_Shadow* pDazzleMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pDazzleMaterial->SetDiffuseTexture(L"Meshes/DazzleCamouflageSingleLayer.004_diffuse.png");

	// Load model
	const auto pModel = new ModelComponent(L"Meshes/first_island.ovm");
	pModel->SetMaterial(pStoneWallMaterial, 0);
	pModel->SetMaterial(pBrickWallMaterial, 1);
	pModel->SetMaterial(pStripes, 2);
	pModel->SetMaterial(pMetalFenceMaterial, 3);
	pModel->SetMaterial(pLampMetalMaterial, 4);
	pModel->SetMaterial(pHatMetalMaterial, 5);
	pModel->SetMaterial(pDazzleMaterial, 6);

	// Setup physics
	const auto pRigidBodyMesh = new RigidBodyComponent(true);
	pRigidBodyMesh->SetKinematic(true);
	auto pDefaultMat = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.8f);
	auto* pTriangleFloorGeo = ContentManager::Load<PxTriangleMesh>(L"Meshes/first_island_collision_floor.ovpt");
	auto* pTriangleWallsGeo = ContentManager::Load<PxTriangleMesh>(L"Meshes/first_island_collision_walls.ovpt");


	pRigidBodyMesh->AddCollider(PxTriangleMeshGeometry{ pTriangleFloorGeo }, *pDefaultMat);
	pRigidBodyMesh->AddCollider(PxTriangleMeshGeometry{ pTriangleWallsGeo }, *pDefaultMat);



	entrance->AddComponent(pModel);
	entrance->AddComponent(pRigidBodyMesh);

	entrance->GetTransform()->Rotate(90, 0, 0);

	AddChild(entrance);

	m_CustomObjects.push_back(entrance);
}

void CapKingdom::CreateSecondIsland()
{
	GameObject* entrance = new GameObject();

	float normalStrength = 0.4f;

	// Setup textures
	UberMaterial* pStoneWallMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pStoneWallMaterial->SetDiffuseTexture(L"Meshes/WallStone00.004_diffuse.png");
	pStoneWallMaterial->SetNormalTexture(L"Meshes/WallStone00.004_normal.png");
	pStoneWallMaterial->SetSpecularTexture(L"Meshes/WallStone00.004_spec.png");
	pStoneWallMaterial->SetNormalStrength(normalStrength);
	pStoneWallMaterial->SetVariable_Scalar(L"gUseSpecularPhong", false);
	pStoneWallMaterial->SetVariable_Scalar(L"gUseSpecularBlinn", false);

	UberMaterial* pBrickWallMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pBrickWallMaterial->SetDiffuseTexture(L"Meshes/WallBrick00.005_diffuse.png");
	pBrickWallMaterial->SetNormalTexture(L"Meshes/WallBrick00.005_normal.png");
	pBrickWallMaterial->SetSpecularTexture(L"Meshes/WallBrick00.005_spec.png");
	pBrickWallMaterial->SetNormalStrength(normalStrength);
	pBrickWallMaterial->SetVariable_Scalar(L"gUseSpecularPhong", false);
	pBrickWallMaterial->SetVariable_Scalar(L"gUseSpecularBlinn", false);

	m_DebugMaterial = pBrickWallMaterial;

	UberMaterial* pStripes = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pStripes->SetDiffuseTexture(L"Meshes/StripeCamouflageSingleLayer00.004_diffuse.png");

	UberMaterial* pMetalFenceMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pMetalFenceMaterial->SetDiffuseTexture(L"Meshes/MetalFence01.004_diffuse.png");
	pMetalFenceMaterial->SetNormalTexture(L"Meshes/MetalFence01.004_normal.png");
	pMetalFenceMaterial->SetSpecularTexture(L"Meshes/MetalFence01.004_spec.png");

	UberMaterial* pLampMetalMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pLampMetalMaterial->SetDiffuseTexture(L"Meshes/LampMetal00.003_diffuse.png");
	pLampMetalMaterial->SetNormalTexture(L"Meshes/LampMetal00.003_normal.png");
	pLampMetalMaterial->SetSpecularTexture(L"Meshes/LampMetal00.003_spec.png");

	UberMaterial* pHatMetalMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pHatMetalMaterial->SetDiffuseTexture(L"Meshes/HatMetal03.003_diffuse.png");
	pHatMetalMaterial->SetNormalTexture(L"Meshes/HatMetal03.003_normal.png");

	UberMaterial* pDazzleMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pDazzleMaterial->SetDiffuseTexture(L"Meshes/DazzleCamouflageSingleLayer.004_diffuse.png");

	// Load model
	const auto pModel = new ModelComponent(L"Meshes/second_island.ovm");
	pModel->SetMaterial(pStoneWallMaterial, 0);
	pModel->SetMaterial(pDazzleMaterial, 1);
	pModel->SetMaterial(pBrickWallMaterial, 2);
	pModel->SetMaterial(pStripes, 3);
	pModel->SetMaterial(pDazzleMaterial, 4);
	pModel->SetMaterial(pMetalFenceMaterial, 5);
	pModel->SetMaterial(pStripes, 6);

	// Setup physics
	const auto pRigidBodyMesh = new RigidBodyComponent(true);
	pRigidBodyMesh->SetKinematic(true);
	auto pDefaultMat = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.8f);
	auto* pTriangleFloorGeo = ContentManager::Load<PxTriangleMesh>(L"Meshes/second_island_collision.ovpt");
	//auto* pTriangleWallsGeo = ContentManager::Load<PxTriangleMesh>(L"Meshes/first_island_collision_walls.ovpt");


	pRigidBodyMesh->AddCollider(PxTriangleMeshGeometry{ pTriangleFloorGeo }, *pDefaultMat);
	//pRigidBodyMesh->AddCollider(PxTriangleMeshGeometry{ pTriangleWallsGeo }, *pDefaultMat);

	entrance->AddComponent(pModel);
	entrance->AddComponent(pRigidBodyMesh);

	AddChild(entrance);

	m_CustomObjects.push_back(entrance);

}

void CapKingdom::CreateBridge()
{
	GameObject* entrance = new GameObject();

	float normalStrength = 0.4f;

	// Setup textures
	UberMaterial* pStoneWallMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pStoneWallMaterial->SetDiffuseTexture(L"Meshes/WallStone00.004_diffuse.png");
	pStoneWallMaterial->SetNormalTexture(L"Meshes/WallStone00.004_normal.png");
	pStoneWallMaterial->SetSpecularTexture(L"Meshes/WallStone00.004_spec.png");
	pStoneWallMaterial->SetNormalStrength(normalStrength);
	pStoneWallMaterial->SetVariable_Scalar(L"gUseSpecularPhong", false);
	pStoneWallMaterial->SetVariable_Scalar(L"gUseSpecularBlinn", false);

	UberMaterial* pBrickWallMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pBrickWallMaterial->SetDiffuseTexture(L"Meshes/WallBrick00.005_diffuse.png");
	pBrickWallMaterial->SetNormalTexture(L"Meshes/WallBrick00.005_normal.png");
	pBrickWallMaterial->SetSpecularTexture(L"Meshes/WallBrick00.005_spec.png");
	pBrickWallMaterial->SetNormalStrength(normalStrength);
	pBrickWallMaterial->SetVariable_Scalar(L"gUseSpecularPhong", false);
	pBrickWallMaterial->SetVariable_Scalar(L"gUseSpecularBlinn", false);

	m_DebugMaterial = pBrickWallMaterial;

	UberMaterial* pStripes = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pStripes->SetDiffuseTexture(L"Meshes/StripeCamouflageSingleLayer00.004_diffuse.png");

	UberMaterial* pMetalFenceMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pMetalFenceMaterial->SetDiffuseTexture(L"Meshes/MetalFence01.004_diffuse.png");
	pMetalFenceMaterial->SetNormalTexture(L"Meshes/MetalFence01.004_normal.png");
	pMetalFenceMaterial->SetSpecularTexture(L"Meshes/MetalFence01.004_spec.png");

	UberMaterial* pLampMetalMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pLampMetalMaterial->SetDiffuseTexture(L"Meshes/LampMetal00.003_diffuse.png");
	pLampMetalMaterial->SetNormalTexture(L"Meshes/LampMetal00.003_normal.png");
	pLampMetalMaterial->SetSpecularTexture(L"Meshes/LampMetal00.003_spec.png");

	UberMaterial* pHatMetalMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pHatMetalMaterial->SetDiffuseTexture(L"Meshes/HatMetal03.003_diffuse.png");
	pHatMetalMaterial->SetNormalTexture(L"Meshes/HatMetal03.003_normal.png");

	UberMaterial* pDazzleMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pDazzleMaterial->SetDiffuseTexture(L"Meshes/DazzleCamouflageSingleLayer.004_diffuse.png");

	// Load model
	const auto pModel = new ModelComponent(L"Meshes/second_island_bridge.ovm");
	pModel->SetMaterial(pBrickWallMaterial, 0);
	pModel->SetMaterial(pDazzleMaterial, 1);
	pModel->SetMaterial(pStoneWallMaterial, 2);
	pModel->SetMaterial(pStripes, 3);
	pModel->SetMaterial(pHatMetalMaterial, 4);
	pModel->SetMaterial(pMetalFenceMaterial, 5);

	// Setup physics
	const auto pRigidBodyMesh = new RigidBodyComponent(true);
	pRigidBodyMesh->SetKinematic(true);
	auto pDefaultMat = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.8f);
	auto* pTriangleFloorGeo = ContentManager::Load<PxTriangleMesh>(L"Meshes/second_island_bridge_collision.ovpt");

	pRigidBodyMesh->AddCollider(PxTriangleMeshGeometry{ pTriangleFloorGeo }, *pDefaultMat);

	entrance->GetTransform()->Rotate(90, 0, 0);

	entrance->AddComponent(pModel);
	entrance->AddComponent(pRigidBodyMesh);

	AddChild(entrance);

	m_CustomObjects.push_back(entrance);

}

void CapKingdom::CreatePlayer()
{
	m_pMarioComponent = AddChild(new Mario(m_SceneContext));
	m_pMarioComponent->SetLives(3);

	m_pMarioComponent->SetStartPosition(XMFLOAT3{ -550, 0, -553 });

	m_pMarioComponent->SetOnDieCallback([this]() 
		{
			OnGameOver();
		});
	m_pMarioComponent->SetOnAllMoonsCallback([this]() {

			OnAllMoonsCollected();
		});
	m_pMarioComponent->SetOnMoonCallback([this]() {

		OnMoonCollected();
		
	});

	m_CustomObjects.push_back(m_pMarioComponent);

}

void CapKingdom::CreateEnemies()
{
	auto coinReader = LocationReader("bills.txt");
	auto locations = coinReader.ReadLocations();

	for (auto loc : locations)
	{
		auto bill = AddChild(new BanzaiBill(m_pMarioComponent));
		bill->GetTransform()->Translate(loc.x, loc.y + 1, loc.z);

		bill->SetOnDeathCallback([this, bill]() {
			//Remove from m_Bills
			auto it = std::find(begin(m_Bills), end(m_Bills), bill);
			m_Bills.erase(it);
			});

		m_Bills.push_back(bill);
	}
}

void CapKingdom::CreateHud()
{
	m_pHud = AddChild(new GameHud());
	m_CustomObjects.push_back(m_pHud);
}

void CapKingdom::CreatePauseMenu()
{
	m_pPauseMenu = AddChild(new PauseMenu());
	m_CustomObjects.push_back(m_pPauseMenu);

	m_pPauseMenu->SetOnRestartCallback([this]() {
		// CLEANUP AUDIO

		SceneManager::Get()->ReloadScene();
		});

	m_pPauseMenu->SetOnMenuCallback([this]() {
		// CLEANUP AUDIO

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
	auto coinReader = LocationReader("coins.txt");
	auto locations = coinReader.ReadLocations();

	for (auto loc : locations)
	{
		auto coin = AddChild(new Coin());
		m_CustomObjects.push_back(coin);


		coin->GetTransform()->Translate(loc.x, loc.y + 1, loc.z);
	}

	auto moonReader = LocationReader("moons.txt");
	locations = moonReader.ReadLocations();

	for (auto loc : locations)
	{
		auto moon = AddChild(new Moon());
		m_CustomObjects.push_back(moon);

		moon->GetTransform()->Translate(loc.x, loc.y + 1, loc.z);
	}


}

void CapKingdom::CreateSkyBox()
{
	auto skyBox = AddChild(new SkyBox());

	m_CustomObjects.push_back(skyBox);
}

void CapKingdom::ClearAudio()
{
	m_pBackgroundMusic->setPaused(true);
	m_pChannel3DBills->setPaused(true);
	m_pChannel3DExplosion->setPaused(true);
}

#ifdef _DEBUG
void CapKingdom::CreateLocationWriter()
{
	m_LocationWriter = LocationWriter("data.txt");
}
#endif

void CapKingdom::UpdateHUDText()
{
	int coins = m_pMarioComponent->GetCoins();
	m_pHud->SetCoins(coins);

	m_pHud->SetSpecialCoins(m_pMarioComponent->GetSpecialCoins());

	int lives = m_pMarioComponent->GetLives();
	m_pHud->SetLives(lives);

	m_pHud->SetMoons(m_pMarioComponent->GetMoons());
}

void CapKingdom::UpdatePostProcess()
{
	m_SinWave += m_SceneContext.pGameTime->GetElapsed() * 2;

	if (m_SinWave >= 3.1459f)
	{
		m_SinWave = 0.f;
	}


	reinterpret_cast<PostVignette*>(m_pPostProcessEffect)->SetTime(m_SinWave);

	if (m_pMarioComponent->GetLives() <= 1)
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

	m_pChannel3DBills->setPaused(true);
	m_pChannel3DExplosion->setPaused(true);
	m_pBackgroundMusic->setPaused(true);

	TimerManager::Get()->CreateTimer(13.f, [this]() {
		m_pBackgroundMusic->setPaused(false);
		m_pChannel3DBills->setPaused(false);
		m_pChannel3DExplosion->setPaused(false);

		SceneManager::Get()->PreviousScene();
	});
}

void CapKingdom::OnMoonCollected()
{
	m_pChannel3DBills->setPaused(true);
	m_pChannel3DExplosion->setPaused(true);
	m_pBackgroundMusic->setPaused(true);

	TimerManager::Get()->CreateTimer(3.5f, [this]() {
		m_pBackgroundMusic->setPaused(false);
		m_pChannel3DBills->setPaused(false);
		m_pChannel3DExplosion->setPaused(false);
	});
}

