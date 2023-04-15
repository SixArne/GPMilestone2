#include "stdafx.h"
#include "ModelTestScene.h"
#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"

void ModelTestScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawPhysXDebug = true;

	// Create chair gameobject
	auto pGameObject = new GameObject();

	// Setup texture
	DiffuseMaterial* pDiffuseMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	const UINT materialId = pDiffuseMaterial->GetMaterialId();
	pDiffuseMaterial->SetDiffuseTexture(L"Textures/Chair_Dark.dds");

	// Load model
	const auto pModel = new ModelComponent(L"Meshes/chair.ovm");
	pModel->SetMaterial(materialId); // set texture

	// Setup physics
	const auto pRigidBodyMesh = new RigidBodyComponent();
	auto pDefaultMat = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.8f);
	auto* pConvexGeo = ContentManager::Load<PxConvexMesh>(L"Meshes/chair.ovpc");
	pRigidBodyMesh->AddCollider(PxConvexMeshGeometry{ pConvexGeo }, *pDefaultMat);	

	// Add components to chair
	pGameObject->AddComponent(pModel);
	pGameObject->AddComponent(pRigidBodyMesh);

	// offset chair
	pGameObject->GetTransform()->Translate(0.f, 2.f, 0.f);

	// add to scene
	AddChild(pGameObject);

	// create ground plane
	GameSceneExt::CreatePhysXGroundPlane(*this);

	// Setup Camera
	FreeCamera* pCamera = new FreeCamera();
	pCamera->GetTransform()->Translate(0.f, 0.f, -50.f);
	pCamera->GetTransform()->Rotate(0.f, 0.f, 0.f);
	AddChild(pCamera);
	SetActiveCamera(pCamera->GetComponent<CameraComponent>());
}

void ModelTestScene::Update()
{
}

void ModelTestScene::Draw()
{
	
}

void ModelTestScene::OnGUI()
{

}
