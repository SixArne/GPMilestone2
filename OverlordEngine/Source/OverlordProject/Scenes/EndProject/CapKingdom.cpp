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
#include <array>

#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

void CapKingdom::Initialize()
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


	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.useDeferredRendering = false;


	/*CreateFloor();
	CreateWalls();*/
	CreateMap();
	CreatePlayer();
	CreatePostProcessEffect();
	CreateHud();


	CreateEnemies();

	m_SceneContext.pLights->SetDirectionalLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });
	m_SceneContext.pInput->AddInputAction(InputAction(0, InputState::pressed, VK_DELETE));
}

void CapKingdom::Update()
{

	if (!m_HasStartedLevel)
	{
		InitSound();
		m_HasStartedLevel = true;
	}

	UpdateAudioListeners();
	UpdateHUDText();
	UpdatePostProcess();
}

void CapKingdom::Draw()
{


}

void CapKingdom::PostDraw()
{
	if (m_DrawShadowMap)
	{
		ShadowMapRenderer::Get()->Debug_DrawDepthSRV({ m_SceneContext.windowWidth - 10.f, 10.f }, { m_ShadowMapScale, m_ShadowMapScale }, { 1.f,0.f });
	}
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
}

void CapKingdom::InitSound()
{
	if (!m_pSound)
	{
		SoundManager::Get()->GetSystem()->createStream("Resources/Sound/altar_cave.mp3", FMOD_DEFAULT, nullptr, &m_pSound);
		m_pSound->setMode(FMOD_LOOP_NORMAL);
		m_pSound->set3DMinMaxDistance(0.f, 100.f);
	}

	SoundManager::Get()->GetSystem()->playSound(m_pSound, nullptr, false, &m_pBackgroundMusic);
	m_pBackgroundMusic->setVolume(0.1f);

	FMOD::Sound* pSound2{};
	auto result = SoundManager::Get()->GetSystem()->createStream("Resources/Sound/rocket.mp3", FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &pSound2);


	result = SoundManager::Get()->GetSystem()->playSound(pSound2, nullptr, false, &m_pChannel3D);

	m_pChannel3D->setMode(FMOD_LOOP_NORMAL);
	m_pChannel3D->setVolume(0.3f);
	m_pChannel3D->set3DMinMaxDistance(1.f, 200.f);
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
	auto camera = m_SceneContext.pCamera;
	auto pos = ToFMod(m_pMarioComponent->GetTransform()->GetWorldPosition());
	auto forward = ToFMod(camera->GetTransform()->GetForward());
	auto up = ToFMod(m_pMarioComponent->GetTransform()->GetUp());

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

		m_pChannel3D->set3DAttributes(&spherePos, &sphereVelocity);
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
	pModel->SetMaterial(pStripes, 1);
	pModel->SetMaterial(pBrickWallMaterial, 2);
	pModel->SetMaterial(pDazzleMaterial, 3);
	pModel->SetMaterial(pMetalFenceMaterial, 4);
	pModel->SetMaterial(pStripes, 5);

	// Setup physics
	const auto pRigidBodyMesh = new RigidBodyComponent(true);
	pRigidBodyMesh->SetKinematic(true);
	auto pDefaultMat = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.8f);
	auto* pTriangleFloorGeo = ContentManager::Load<PxTriangleMesh>(L"Meshes/second_island_collision_floor.ovpt");
	//auto* pTriangleWallsGeo = ContentManager::Load<PxTriangleMesh>(L"Meshes/first_island_collision_walls.ovpt");


	pRigidBodyMesh->AddCollider(PxTriangleMeshGeometry{ pTriangleFloorGeo }, *pDefaultMat);
	//pRigidBodyMesh->AddCollider(PxTriangleMeshGeometry{ pTriangleWallsGeo }, *pDefaultMat);


	entrance->GetTransform()->Rotate(90, 0, 0);

	entrance->AddComponent(pModel);
	entrance->AddComponent(pRigidBodyMesh);

	AddChild(entrance);
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
}

void CapKingdom::CreatePlayer()
{
	m_pMarioComponent = AddChild(new Mario());
	m_pMarioComponent->SetTag(L"mario");

	m_pMarioComponent->SetStartPosition(XMFLOAT3{ -550, 0, -553 });
}

void CapKingdom::CreateEnemies()
{
	auto bill = AddChild(new BanzaiBill(m_pMarioComponent));
	bill->GetTransform()->Translate(0, 2, 0);

	m_Bills.push_back(bill);
}

void CapKingdom::CreateHud()
{
	m_pHud = AddChild(new GameHud());
}

void CapKingdom::CreatePostProcessEffect()
{
	m_pPostProcessEffect = MaterialManager::Get()->CreateMaterial<PostVignette>();
	AddPostProcessingEffect(m_pPostProcessEffect);

	m_pPostProcessEffect->SetIsEnabled(false);
	reinterpret_cast<PostVignette*>(m_pPostProcessEffect)->SetRadius(1.1f);

}

void CapKingdom::UpdateHUDText()
{
	m_pHud->SetCoins(m_pMarioComponent->GetCoins());
	m_pHud->SetSpecialCoins(m_pMarioComponent->GetSpecialCoins());
	m_pHud->SetLives(m_pMarioComponent->GetLives());
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
	//reinterpret_cast<PostVignette*>(m_pPostProcessEffect)->SetRadius(m_Radius);

	if (m_pMarioComponent->GetLives() <= 1)
	{
		m_pPostProcessEffect->SetIsEnabled(true);
	}
	else
	{
		m_pPostProcessEffect->SetIsEnabled(false);
	}
}

