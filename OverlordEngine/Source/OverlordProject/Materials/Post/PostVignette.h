#pragma once
//Resharper Disable All

class PostVignette : public PostProcessingMaterial
{
public:
	PostVignette();
	~PostVignette() override = default;
	PostVignette(const PostVignette& other) = delete;
	PostVignette(PostVignette&& other) noexcept = delete;
	PostVignette& operator=(const PostVignette& other) = delete;
	PostVignette& operator=(PostVignette&& other) noexcept = delete;

	void SetTime(float time);
	void SetRadius(float radius);
	void SetColor(DirectX::XMFLOAT3 color);

protected:
	void Initialize(const GameContext& /*gameContext*/) override;

private:
	ID3DX11EffectScalarVariable* m_pTimeVariable{};
	ID3DX11EffectScalarVariable* m_pRadiusVariable{};
	ID3DX11EffectVectorVariable* m_pColorVariable{};
};

