#include "stdafx.h"
#include "UberMaterialScene.h"
#include "Materials/UberMaterial.h"

void UberMaterialScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	m_pUberMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();

	m_pUberMaterial->SetDiffuseTexture(L"Textures/Skulls_Diffusemap.tga");
	m_pUberMaterial->SetSpecularTexture(L"Textures/Skulls_Heightmap.tga");
	m_pUberMaterial->SetNormalTexture(L"Textures/Skulls_Normalmap.tga");

	m_pMesh = new GameObject();
	auto* model = m_pMesh->AddComponent(new ModelComponent(L"Meshes/Sphere.ovm"));
	model->SetMaterial(m_pUberMaterial);
	AddChild(m_pMesh);

	m_SceneContext.pCamera->GetTransform()->Translate(0, 3, -5);
}

void UberMaterialScene::Update()
{
}

void UberMaterialScene::Draw()
{
}

void UberMaterialScene::OnGUI()
{
	m_pUberMaterial->DrawImGui();
}
