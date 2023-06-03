#include "stdafx.h"
#include "FirstIsland.h"
#include "Materials/UberMaterial.h"
#include "Materials/Shadow/UberMaterial_Shadow.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

FirstIsland::FirstIsland()
{
}

FirstIsland::~FirstIsland()
{
}

void FirstIsland::Initialize(const SceneContext&)
{

	float normalStrength = 0.4f;

	// Setup textures
	UberMaterial_Shadow* pStoneWallMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial_Shadow>();
	pStoneWallMaterial->SetDiffuseTexture(L"Meshes/WallStone00.004_diffuse.png");
	pStoneWallMaterial->SetNormalTexture(L"Meshes/WallStone00.004_normal.png");
	pStoneWallMaterial->SetSpecularTexture(L"Meshes/WallStone00.004_spec.png");
	pStoneWallMaterial->SetNormalStrength(normalStrength);
	pStoneWallMaterial->SetVariable_Scalar(L"gUseSpecularPhong", false);
	pStoneWallMaterial->SetVariable_Scalar(L"gUseSpecularBlinn", false);

	UberMaterial_Shadow* pBrickWallMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial_Shadow>();
	pBrickWallMaterial->SetDiffuseTexture(L"Meshes/WallBrick00.005_diffuse.png");
	pBrickWallMaterial->SetNormalTexture(L"Meshes/WallBrick00.005_normal.png");
	pBrickWallMaterial->SetSpecularTexture(L"Meshes/WallBrick00.005_spec.png");
	pBrickWallMaterial->SetNormalStrength(normalStrength);
	pBrickWallMaterial->SetVariable_Scalar(L"gUseSpecularPhong", false);
	pBrickWallMaterial->SetVariable_Scalar(L"gUseSpecularBlinn", false);


	DiffuseMaterial_Shadow* pStripes = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pStripes->SetDiffuseTexture(L"Meshes/StripeCamouflageSingleLayer00.004_diffuse.png");

	UberMaterial_Shadow* pMetalFenceMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial_Shadow>();
	pMetalFenceMaterial->SetDiffuseTexture(L"Meshes/MetalFence01.004_diffuse.png");
	pMetalFenceMaterial->SetNormalTexture(L"Meshes/MetalFence01.004_normal.png");
	pMetalFenceMaterial->SetSpecularTexture(L"Meshes/MetalFence01.004_spec.png");

	UberMaterial_Shadow* pLampMetalMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial_Shadow>();
	pLampMetalMaterial->SetDiffuseTexture(L"Meshes/LampMetal00.003_diffuse.png");
	pLampMetalMaterial->SetNormalTexture(L"Meshes/LampMetal00.003_normal.png");
	pLampMetalMaterial->SetSpecularTexture(L"Meshes/LampMetal00.003_spec.png");

	UberMaterial_Shadow* pHatMetalMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial_Shadow>();
	pHatMetalMaterial->SetDiffuseTexture(L"Meshes/HatMetal03.003_diffuse.png");
	pHatMetalMaterial->SetNormalTexture(L"Meshes/HatMetal03.003_normal.png");

	DiffuseMaterial_Shadow* pDazzleMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pDazzleMaterial->SetDiffuseTexture(L"Meshes/DazzleCamouflageSingleLayer.004_diffuse.png");

	// Load model
	const auto pModel = new ModelComponent(L"Meshes/Map/first_island.ovm");
	pModel->SetMaterial(pStoneWallMaterial, 0);
	pModel->SetMaterial(pBrickWallMaterial, 1);
	pModel->SetMaterial(pStripes, 2);
	pModel->SetMaterial(pMetalFenceMaterial, 3);
	pModel->SetMaterial(pLampMetalMaterial, 4);
	pModel->SetMaterial(pHatMetalMaterial, 5);
	pModel->SetMaterial(pDazzleMaterial, 6);

	// Setup physics
	const auto pRigidBodyMesh = new RigidBodyComponent(true);
	auto pDefaultMat = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.8f);
	auto* pTriangleFloorGeo = ContentManager::Load<PxTriangleMesh>(L"Meshes/Map/first_island_collision_floor.ovpt");
	auto* pTriangleWallsGeo = ContentManager::Load<PxTriangleMesh>(L"Meshes/Map/first_island_collision_walls.ovpt");


	pRigidBodyMesh->AddCollider(PxTriangleMeshGeometry{ pTriangleFloorGeo }, *pDefaultMat);
	pRigidBodyMesh->AddCollider(PxTriangleMeshGeometry{ pTriangleWallsGeo }, *pDefaultMat);

	AddComponent(pModel);
	AddComponent(pRigidBodyMesh);

}
