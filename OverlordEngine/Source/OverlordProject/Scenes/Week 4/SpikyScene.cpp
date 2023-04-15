#include "stdafx.h"
#include "SpikyScene.h"

#include "Materials/SpikyMaterial.h"

void SpikyScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	m_pMaterial = MaterialManager::Get()->CreateMaterial<SpikyMaterial>();

	m_pSphere = new GameObject();
	auto* model = m_pSphere->AddComponent(new ModelComponent(L"Meshes/OctaSphere.ovm"));
	model->SetMaterial(m_pMaterial);
	AddChild(m_pSphere);

	m_SceneContext.pCamera->GetTransform()->Translate(0, 3, -5);
}

void SpikyScene::OnGUI()
{
	m_pMaterial->DrawImGui();
}

void SpikyScene::Update()
{
	/*auto rotQuat = m_pSphere->GetTransform()->GetRotation();
	auto rotation = MathHelper::QuaternionToEuler(rotQuat);

	rotation.y += 0.01f * m_SceneContext.pGameTime->GetElapsed();

	m_pSphere->GetTransform()->Rotate(rotation, false);*/
}
