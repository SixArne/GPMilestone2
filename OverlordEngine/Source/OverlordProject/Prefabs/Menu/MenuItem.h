#pragma once
class MenuItem : public GameObject
{
public:
	MenuItem(const std::string& name);
	~MenuItem() override = default;
	MenuItem(const MenuItem& other) = delete;
	MenuItem(MenuItem&& other) noexcept = delete;
	MenuItem& operator=(const MenuItem& other) = delete;
	MenuItem& operator=(MenuItem&& other) noexcept = delete;

	void SetIsSelected(bool value);
	void SetPosition(XMFLOAT2 position);
	void SetColor(XMFLOAT4& color);
	void SetText(const std::string& text);
	const std::string& GetText();

	void OnImGui();

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;


private:
	SpriteFont* m_pFont{};

	std::string m_Content{ "My example string" };
	XMFLOAT2 m_ContentPosition{};
	XMFLOAT4 m_ContentColor{ 1.f, 1.f, 1.f, 1.f };

	bool m_IsSelected{ false };
};

