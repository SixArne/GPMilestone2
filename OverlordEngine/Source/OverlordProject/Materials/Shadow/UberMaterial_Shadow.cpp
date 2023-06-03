#include "stdafx.h"
#include "UberMaterial_Shadow.h"

UberMaterial_Shadow::UberMaterial_Shadow()
	: Material<UberMaterial_Shadow>(L"Effects/Shadow/UberShader_Shadow.fx")
{}

void UberMaterial_Shadow::InitializeEffectVariables()
{
	SetVariable_Vector(L"gLightDirection", { 0.577f, 0.577f, 0.577f });

	// Diffuse
	SetVariable_Scalar(L"gUseTextureDiffuse", true);	// Indicates should render diffuse
	SetVariable_Vector(L"gColorDiffuse", { 1,1,1,1 }); // Backup

	// Specular
	SetVariable_Scalar(L"gUseTextureSpecular", true);	// Indicates should render specular
	SetVariable_Scalar(L"gUseSpecularPhong", true); // Indicates should use phong
	SetVariable_Scalar(L"gUseSpecularBlinn", true); // Indicates should use blinn
	SetVariable_Vector(L"gColorSpecular", { 1,1,1,1 }); // Backup
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

void UberMaterial_Shadow::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureDiffuse", m_pDiffuseTexture);
}

void UberMaterial_Shadow::SetSpecularTexture(const std::wstring& assetFile)
{
	m_pSpecularTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureSpecular", m_pSpecularTexture);
}

void UberMaterial_Shadow::SetNormalTexture(const std::wstring& assetFile)
{
	m_pNormalTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureNormal", m_pNormalTexture);
}

void UberMaterial_Shadow::SetOpacityTexture(const std::wstring& assetFile)
{
	m_pOpacityTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureOpacity", m_pOpacityTexture);
}

void UberMaterial_Shadow::SetCubeMap(const std::wstring& assetFile)
{
	m_pCubeMap = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gCubeEnvironment", m_pCubeMap);
}

void UberMaterial_Shadow::SetNormalStrength(float strength)
{
	SetVariable_Scalar(L"gNormalStrength", strength);
}

void UberMaterial_Shadow::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	/*
	 * TODO_W8
	 * Update The Shader Variables
	 * 1. Update the LightWVP > Used to Transform a vertex into Light clipping space
	 * 	LightWVP = model_world * light_viewprojection
	 * 	(light_viewprojection [LightVP] can be acquired from the ShadowMapRenderer)
	 *
	 * 2. Update the ShadowMap texture
	 *
	 * 3. Update the Light Direction (retrieve the direction from the LightManager > sceneContext)
	*/

	//Update Shadow Variables
	const auto pShadowMapRenderer = ShadowMapRenderer::Get();
	
	XMMATRIX meshWorld = XMLoadFloat4x4(&pModel->GetTransform()->GetWorld());
	XMMATRIX lightVP = XMLoadFloat4x4(&pShadowMapRenderer->GetLightVP());

	auto lightWVP = DirectX::XMMatrixMultiply(meshWorld, lightVP);

	SetVariable_Matrix(L"gWorldViewProj_Light", reinterpret_cast<const float*>(lightWVP.r));

	//2. Update the ShadowMap texture
	SetVariable_Texture(L"gShadowMap", ShadowMapRenderer::Get()->GetShadowMap());

	//3. Update the Light Direction (retrieve the direction from the LightManager > sceneContext)
	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);
}