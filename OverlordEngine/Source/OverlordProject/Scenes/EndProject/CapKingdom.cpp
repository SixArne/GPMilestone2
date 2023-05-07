#include "stdafx.h"
#include "CapKingdom.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/ColorMaterial.h"
#include "Materials/UberMaterial.h"
#include "Utils/ClipboardUtil.h"
#include "Prefabs/Character.h"
#include <array>

void CapKingdom::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawPhysXDebug = false;

	/*CreateFloor();
	CreateWalls();*/
	CreateMap();
	CreatePlayer();

	FMOD::Sound* pSound = nullptr;
	if (!pSound)
	{

		SoundManager::Get()->GetSystem()->createStream("Resources/Sound/altar_cave.mp3", FMOD_DEFAULT, nullptr, &pSound);
		pSound->setMode(FMOD_LOOP_NORMAL);
		pSound->set3DMinMaxDistance(0.f, 100.f);
	}

	SoundManager::Get()->GetSystem()->playSound(pSound, nullptr, false, &m_pBackgroundMusic);
	m_pBackgroundMusic->setVolume(0.1f);
}

void CapKingdom::Update()
{
	// camera
	/*auto camPos = m_pCamera->GetTransform()->GetPosition();

	auto camNewPos = XMFLOAT3
	{
		camPos.x,
		camPos.y,
		camPos.z + (1 * m_SceneContext.pGameTime->GetElapsed())
	};



	m_pCamera->GetTransform()->Translate(camNewPos);*/

	// luneth

	//auto lunethPos = m_pLuneth->GetTransform()->GetPosition();

	////auto lunethNewPos = XMFLOAT3
	////{
	////	lunethPos.x,
	////	lunethPos.y,
	////	lunethPos.z + (1 * m_SceneContext.pGameTime->GetElapsed())
	////};

	//m_pLuneth->GetTransform()->Translate(lunethNewPos);
}

void CapKingdom::Draw()
{


}

void CapKingdom::OnGUI()
{
	GameScene::OnGUI();
	
	m_DebugMaterial->DrawImGui();
	m_pCharacter->DrawImGui();
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

	entrance->GetTransform()->Rotate(90, 0, 0);

	entrance->AddComponent(pModel);
	entrance->AddComponent(pRigidBodyMesh);
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


	entrance->GetTransform()->Rotate(90, 0, 0);

	entrance->AddComponent(pModel);
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


	entrance->GetTransform()->Rotate(90, 0, 0);

	entrance->AddComponent(pModel);
	AddChild(entrance);
}

void CapKingdom::CreatePlayer()
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	//Character
	CharacterDesc characterDesc{ pDefaultMaterial, 1.f, 5.f };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;

	m_pCharacter = AddChild(new Character(characterDesc));
	m_pCharacter->GetTransform()->Translate(0.f, 20.f, 0.f);

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
}
