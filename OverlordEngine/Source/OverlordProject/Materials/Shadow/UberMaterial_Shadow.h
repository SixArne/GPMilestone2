#pragma once
class UberMaterial_Shadow final : public Material<UberMaterial_Shadow>
{
public:
	UberMaterial_Shadow();
	~UberMaterial_Shadow() override = default;

	UberMaterial_Shadow(const UberMaterial_Shadow& other) = delete;
	UberMaterial_Shadow(UberMaterial_Shadow&& other) noexcept = delete;
	UberMaterial_Shadow& operator=(const UberMaterial_Shadow& other) = delete;
	UberMaterial_Shadow& operator=(UberMaterial_Shadow&& other) noexcept = delete;

	void SetDiffuseTexture(const std::wstring& assetFile);
	void SetSpecularTexture(const std::wstring& assetFile);
	void SetNormalTexture(const std::wstring& assetFile);
	void SetOpacityTexture(const std::wstring& assetFile);
	void SetCubeMap(const std::wstring& assetFile);
	void SetNormalStrength(float strength);

protected:
	void InitializeEffectVariables() override;
	void OnUpdateModelVariables(const SceneContext&, const ModelComponent*) const override;

private:
	TextureData* m_pDiffuseTexture{};
	TextureData* m_pSpecularTexture{};
	TextureData* m_pNormalTexture{};
	TextureData* m_pOpacityTexture{};
	TextureData* m_pCubeMap{};
};