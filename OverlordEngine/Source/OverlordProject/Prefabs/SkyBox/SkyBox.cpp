#include "stdafx.h"
#include "SkyBox.h"
#include "Materials/SkyboxMaterial.h"

void SkyBox::Initialize(const SceneContext& )
{
	auto skyBox = AddChild(new GameObject());
	auto skyBoxModel = new ModelComponent(L"Meshes/Box.ovm");

	auto mat = MaterialManager::Get()->CreateMaterial<SkyboxMaterial>();
	mat->SetTexture(L"Textures/SkyBoxCustom.dds");

	skyBoxModel->SetMaterial(mat);
	skyBox->AddComponent(skyBoxModel);

	skyBox->GetTransform()->Scale(1000, 1000, 1000);
}

