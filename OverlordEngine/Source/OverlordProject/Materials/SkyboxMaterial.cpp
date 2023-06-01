#include "stdafx.h"
#include "SkyboxMaterial.h"

SkyboxMaterial::SkyboxMaterial()
	: Material<SkyboxMaterial>(L"Effects/Skybox.fx")
{
}

void SkyboxMaterial::SetTexture(const std::wstring& assetFile)
{
	m_pSkyBoxTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"m_CubeMap", m_pSkyBoxTexture);
}

void SkyboxMaterial::InitializeEffectVariables()
{
}
