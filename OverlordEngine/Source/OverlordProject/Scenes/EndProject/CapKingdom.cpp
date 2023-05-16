#include "stdafx.h"
#include "CapKingdom.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/ColorMaterial.h"
#include "Materials/UberMaterial.h"
#include "Utils/ClipboardUtil.h"
#include "Prefabs/Mario.h"
#include "Prefabs/Character.h"
#include <array>

#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

void CapKingdom::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawPhysXDebug = false;

	/*CreateFloor();
	CreateWalls();*/
	CreateMap();
	CreatePlayer();

	m_SceneContext.pLights->SetDirectionalLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	if (!m_pSound)
	{

		SoundManager::Get()->GetSystem()->createStream("Resources/Sound/altar_cave.mp3", FMOD_DEFAULT, nullptr, &m_pSound);
		m_pSound->setMode(FMOD_LOOP_NORMAL);
		m_pSound->set3DMinMaxDistance(0.f, 100.f);
	}

	SoundManager::Get()->GetSystem()->playSound(m_pSound, nullptr, true, &m_pBackgroundMusic);
	m_pBackgroundMusic->setVolume(0.1f);

	m_SceneContext.pInput->AddInputAction(InputAction(0, InputState::pressed, VK_DELETE));
}

void CapKingdom::Update()
{
	if (!m_HasStartedLevel)
	{
		SoundManager::Get()->GetSystem()->playSound(m_pSound, nullptr, false, &m_pBackgroundMusic);
		m_HasStartedLevel = true;
	}
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
	m_pMario->DrawImGui();

	ImGui::DragFloat3("direction", &m_LightDirection.x, 0.1f, -1.f, 1.f);
	m_SceneContext.pLights->SetDirectionalLight(m_pMario->GetTransform()->GetWorldPosition(), m_LightDirection);

	ImGui::Checkbox("Draw ShadowMap", &m_DrawShadowMap);
	ImGui::SliderFloat("ShadowMap Scale", &m_ShadowMapScale, 0.f, 1.f);
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


	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	//Character
	CharacterDesc characterDesc{ pDefaultMaterial };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;
	characterDesc.controller.height = 8.5f;
	characterDesc.controller.radius = 3.f;
	characterDesc.JumpSpeed = 80.f;
	characterDesc.maxMoveSpeed = 200.f;
	characterDesc.maxFallSpeed = 120.f;
	characterDesc.fallAccelerationTime = 0.8f;

	m_pMario = AddChild(new Character(characterDesc, new Mario()));

	//Input
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

	m_pMario->GetTransform()->Scale(4, 4, 4);

	//m_pMario->GetTransform()->Scale(5, 5, 5);
}
