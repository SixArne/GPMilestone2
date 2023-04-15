#include "stdafx.h"
#include "UberMaterial.h"

UberMaterial::UberMaterial()
	:Material<UberMaterial>(L"Effects/UberShader.fx")
{
}

void UberMaterial::InitializeEffectVariables()
{
	SetVariable_Vector(L"gLightDirection", { 0.577f, 0.577f, 0.577f });
	
	// Diffuse
	SetVariable_Scalar(L"gUseTextureDiffuse", true);	// Indicates should render diffuse
	SetVariable_Vector(L"gColorDiffuse", { 1,1,1,1 }); // Backup

	// Specular
	SetVariable_Scalar(L"gUseTextureSpecular", true);	// Indicates should render specular
	SetVariable_Scalar(L"gUseSpecularPhong", true); // Indicates should use phong
	SetVariable_Scalar(L"gUseSpecularBlinn", true); // Indicates should use blinn
	SetVariable_Vector(L"gColorSpecular", { 1,1,1,1}); // Backup
	SetVariable_Scalar(L"gShininess", 40);

	//ambient
	SetVariable_Vector(L"gColorAmbient", { 1.f, 1.f, 1.f, 1.f });
	SetVariable_Scalar(L"gAmbientIntensity", 0.05f);

	//normals
	SetVariable_Scalar(L"gUseTextureNormal", true);
	SetVariable_Scalar(L"gNormalStrength", 1.f);

	//fresnel
	SetVariable_Scalar(L"gUseFresnelFalloff", true);
	SetVariable_Scalar(L"gFresnelPower", 1.7f);
	SetVariable_Scalar(L"gFresnelHardness", 0.2f);

}

void UberMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureDiffuse", m_pDiffuseTexture);
}

void UberMaterial::SetSpecularTexture(const std::wstring& assetFile)
{
	m_pSpecularTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureSpecular", m_pSpecularTexture);
}

void UberMaterial::SetNormalTexture(const std::wstring& assetFile)
{
	m_pNormalTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureNormal", m_pNormalTexture);
}

void UberMaterial::SetOpacityTexture(const std::wstring& assetFile)
{
	m_pOpacityTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureOpacity", m_pOpacityTexture);
}

void UberMaterial::SetCubeMap(const std::wstring& assetFile)
{
	m_pCubeMap = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gCubeEnvironment", m_pCubeMap);
}

void UberMaterial::SetNormalStrength(float strength)
{
	SetVariable_Scalar(L"gNormalStrength", strength);
}
